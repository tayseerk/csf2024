#include <iostream>
#include <cassert>
#include "csapp.h"
#include "message.h"
#include "message_serialization.h"
#include "server.h"
#include "exceptions.h"
#include "client_connection.h"
#include "value_stack.h"

ClientConnection::ClientConnection( Server *server, int client_fd )
  : m_server( server )
  , m_client_fd( client_fd )
  , login_status(false)
  , mode_status(0)
{
  rio_readinitb( &m_fdbuf, m_client_fd );
  m_stack = new ValueStack();
}

ClientConnection::~ClientConnection()
{
  Close(m_client_fd);
}

void ClientConnection::chat_with_client()
{
  try{ // try-catch for unrecoverable exceptions
    char buffer[MAXLINE];
    while(true) // keep accepting requests
    {
      try{ // try-catch for recoverable exceptions
        // read message
        ssize_t input = rio_readlineb(&m_fdbuf, buffer, MAXLINE);
        if (input == 0) break;

        // decode message
        std::string client_msg_str(buffer, input); 
        Message client_msg;
        MessageSerialization::decode(client_msg_str, client_msg);
        Message reply_msg = process_handling(client_msg); // process handling
        respond(reply_msg); // send response
        if(client_msg.get_message_type() == MessageType::BYE){
          break; // stop chatting
        }
      } catch (OperationException &ex) { // recoverable
        handle_error(ex.what(), MessageType::FAILED);
      } catch (FailedTransaction &ex) { // recoverable
        handle_error(ex.what(), MessageType::FAILED);
      } catch (...) {
        throw;
        break;
      }
    }
  } catch (InvalidMessage &ex) { //unrecoverable
    handle_error(ex.what(), MessageType::ERROR);
  } catch (CommException &ex) { // unrecoverable
    handle_error(ex.what(), MessageType::ERROR);
  } catch (...) {
    std::cerr << "Error: unexpected error.\n";
    Close(m_client_fd);
  }
}

// TODO: additional member functions
Message ClientConnection::process_handling(Message msg)
{
  MessageType type = msg.get_message_type();
  //everything but logged in first checks if client is logged in
  if(type == MessageType::LOGIN){
    return login(msg); 
  } else if (type == MessageType::CREATE){
    check_has_logged_in();
    return create(msg);
  } else if(type == MessageType::PUSH){
    check_has_logged_in();
    return push(msg); 
  } else if(type == MessageType::POP){
    check_has_logged_in();
    return pop(); 
  } else if(type == MessageType::TOP){
    check_has_logged_in();
    return top(); 
  } else if (type == MessageType::SET){
    check_has_logged_in();
    return set(msg);
  } else if (type == MessageType::GET){
    check_has_logged_in();
    return get(msg);
  } else if (type == MessageType::ADD || type == MessageType::MUL 
  ||type == MessageType::SUB ||type == MessageType::DIV){
    check_has_logged_in();
    return handle_arithmetic(type);
  } else if (type == MessageType::BEGIN){
    check_has_logged_in();
    return begin();
  } else if (type == MessageType::COMMIT){
    check_has_logged_in();
    return commit();
  } else if (type == MessageType::BYE){
    check_has_logged_in();
    return bye();
  } else { // anything else
    throw InvalidMessage("\"Invalid request\"");
  }
}

Message ClientConnection::login(Message msg)
{
  if(login_status){ // already logged in (not the first message)
    throw InvalidMessage("\"LOGIN may only be the first message\"");
  } else { // mark logged in
    login_status = true;
    return reply_ok();
  }
}

Message ClientConnection::create(Message msg)
{
  std::string table_name = msg.get_table();
  if(m_server->find_table(table_name) != nullptr){ // table already in server
    throw OperationException("\"Can't create a table that already exists.\"");
  }
  m_server->create_table(table_name);
  return reply_ok();
}

Message ClientConnection::push(Message msg)
{
  std::string value = msg.get_value();
  m_stack->push(value);
  return reply_ok();
}

Message ClientConnection::pop()
{
  check_empty_stack("\"Can't pop an empty stack.\""); //can't pop empty stack
   
  m_stack->pop();
  return reply_ok();
}

Message ClientConnection::top()
{
  check_empty_stack("\"Can't get top of an empty stack.\"");
  
  //get top value from stack
  std::string value = m_stack->get_top(); 
  return reply_data(value);
}

Message ClientConnection::set(Message msg)
{
  // retrieve the table and lock it
  Table *table = get_server_table(msg.get_table()); 
  lock_table(table);

  // make sure the stack is not empty
  check_empty_stack("\"no value to set since stack is empty.\"");
  // get the top value from the stack and then pop that value
  std::string val = m_stack->get_top();
  m_stack->pop();
  std::string key = msg.get_key();
  table->set(key, val); // set the value in the table
  unlock_table(table); // releases the lock only in autocommit mode (stays locked for trans)
  return reply_ok();
}

Message ClientConnection::get(Message msg)
{
  // retrieve the table and lock it 
  Table *table = get_server_table(msg.get_table());
  lock_table(table);
  std::string key = msg.get_key(); // get the key
  // if the key doesn't exist, throw an error
  if (!table->has_key(key)) {
    unlock_table(table); // only unlock for autocommit mode
    throw OperationException("\"key doesn't exist in the table.\"");
  }
  // get the value associated with the key and push onto stack
  std::string val = table->get(key);
  m_stack->push(val);
  // unlock only for autocommit mode
  unlock_table(table);
  return reply_ok();
}

Message ClientConnection::handle_arithmetic(MessageType type)
{
  check_empty_stack("\"No values in stack. Cannot calculate.\""); // check first operator is not empty
  std::string right_val = m_stack->get_top(); // right operator string
  m_stack->pop();
  check_empty_stack("\"Only one operator. Cannot calculate.\"");// check second operator is not empty
  std::string left_val = m_stack->get_top(); // left operator string
  m_stack->pop();

  //make sure strings are integers
  if(!(string_is_digit(right_val) && string_is_digit(left_val))){
    throw OperationException("\"Two top value aren't numeric\"");
  } 
  // integer operators
  unsigned right = std::stoi(right_val); 
  unsigned left = std::stoi(left_val);
  std::string value = do_arithmetic(type, left, right); // do specified math
  m_stack->push(value); // push back onto stack

  return reply_ok();
}

Message ClientConnection::begin()
{
  if (mode_status == 1) {
    throw OperationException("\"Cannot begin a transaction while already in one.\"");
  }
  mode_status = 1; // switch from autocommit to trans (0 is autocommit, 1 is trans)
  return reply_ok();
}

Message ClientConnection::commit()
{
  if (mode_status == 0) {
    throw OperationException("\"no transaction has started\"");
  }
  // we want to commit for all locked tables then unlock them when finished 
  for (auto &table_name : locked_tables) {
    Table *t = get_server_table(table_name);
    t->commit_changes();
    t->unlock();
  }
  // clear the locked tables then exit trans mode
  locked_tables.clear();
  mode_status = 0;
  return reply_ok();
}

Message ClientConnection::bye()
{
  login_status = false; // logout
  return reply_ok();
}

Message ClientConnection::reply_ok()
{
  return Message(MessageType::OK); // create ok message
}

Message ClientConnection::reply_data(std::string value)
{
  return Message(MessageType::DATA, {value}); //create data message
}

void ClientConnection::autocommit_lock(Table* table_ptr)
{
  if(table_ptr != nullptr){
    if(mode_status == 0){ // do only in autocommit mode
      table_ptr->lock();
    }
  } 
}

void ClientConnection::autocommit_unlock(Table* table_ptr)
{
  if(table_ptr != nullptr){
    if(mode_status == 0){ //do only in autocommit mode
      table_ptr->unlock();
    }
  }
}

Table* ClientConnection::get_server_table(std::string table_name)
{
  // find table by name
  Table *table = m_server->find_table(table_name);
  if(table == nullptr){ // not found
    throw OperationException("\"Table does not exist.\"");
  } else { // return pointer
    return table;
  }
}

bool ClientConnection::string_is_digit(std::string& str) 
{
  // for every char in string
  for(long unsigned i = 0; i<str.size(); i++ ){
    if(!isdigit(str[i])){ // false if any string isn't a digit
      return false;
    }
  }
  return true; // no strings weren't a digit
}

std::string ClientConnection::do_arithmetic(MessageType type, unsigned left, unsigned right)
{
  //do specified math
  std::string value;
  if(type == MessageType::ADD){
    value = std::to_string(left + right);
  } else if(type == MessageType::MUL) {
    value = std::to_string(left * right);
  } else if(type == MessageType::SUB){
    value = std::to_string(left - right);
  } else {
    if(right == 0){ // cannot divide by 0
      throw OperationException("\"Cannot divide by zero.\"");
    }
    value = std::to_string(left / right);
  }
  return value;
}

void ClientConnection::handle_error(const std::string error_msg, MessageType error_type)
{
  if (error_type == MessageType::FAILED && mode_status == 1) {
    rollback_trans(); // ADDED FOR TRANSACTION
  }

  Message reply_msg;
  if(error_type == MessageType::ERROR){ // unrecoverable
    login_status = false; // added this to end session since its unrecoverable but this could be wrong so I will double check
    reply_msg = reply_error(error_msg);
  } else { // recoverable
    reply_msg = reply_failed(error_msg);
  }
  respond(reply_msg); // send message
}

Message ClientConnection::reply_error(const std::string error_msg)
{
  login_status = false; //logs out when cannot continue due to error
  return Message(MessageType::ERROR, {error_msg}); // create error message
}

Message ClientConnection::reply_failed(const std::string error_msg)
{
  return Message(MessageType::FAILED, {error_msg}); // create failed message
}

void ClientConnection::respond(Message reply)
{
  std::string reply_str;
  MessageSerialization::encode(reply, reply_str); // encode message to string

  rio_writen(m_client_fd, reply_str.c_str(), reply_str.length()); // write to client
}

void ClientConnection::check_has_logged_in()
{
  // cannot perform request if client has not logged in
  if(!login_status){
    throw InvalidMessage("\"First request must be LOGIN\"");
  }
}

void ClientConnection::check_empty_stack(const std::string error_msg)
{
  if(m_stack->is_empty()){
    throw OperationException(error_msg);
  }
}

// added for transaction
void ClientConnection::rollback_trans() {
  // go thru all the locked tables in this transaction
  for (auto &tname : locked_tables) {
    Table *t = get_server_table(tname);
    t->rollback_changes(); // revert the table's state (prior to transaction)
    t->unlock(); // release the lock
  }
  // clear the locked tables and exit (returns back to autocommit mode)
  locked_tables.clear();
  mode_status = 0;
}

void ClientConnection::lock_table(Table *table) {
  if (mode_status == 0) {
    table->lock(); // autocommit mode so we just lock
  } else {
    // transaction mode: if it isn't alr locked, trylock
    std::string table_name = table->get_name();
    if (locked_tables.find(table_name) == locked_tables.end()) {
      // if trylock doesnt work
      if (!table->trylock()) {
        throw FailedTransaction("\"couldn't get a lock on the table\"");
      }
      locked_tables.insert(table_name); // success so we log this table as 'locked'
    }
  }
}

void ClientConnection::unlock_table(Table *table) {
  if (mode_status == 0) {
    table->unlock(); // unlock if alr in autocommit mode
  }
  // transaction mode won't do nothing here so we should just unlock at commit/rollback
}

