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
  std::string table_name = msg.get_table(); 
  Table *table = get_server_table(table_name); // acquire table ptr
  autocommit_lock(table); 

  check_empty_stack("\"Stack is empty, no value to set.\"");
  std::string value = m_stack->get_top();
  m_stack->pop(); // take value off of stack
  std::string key = msg.get_key();

  table->set(key, value); // set value
  autocommit_unlock(table);
  return reply_ok();
}

Message ClientConnection::get(Message msg)
{
  std::string table_name = msg.get_table();
  Table *table = get_server_table(table_name); //acquire table ptr
  autocommit_lock(table);

  std::string key = msg.get_key();
  std::string value;

  if (table->has_key(key)){ //check key exists in table
    // get value from key
    value = table->get(key);
    m_stack->push(value);
    autocommit_unlock(table);
    return reply_ok();
  } else { // key does not exist in table
    autocommit_unlock(table);
    throw OperationException("\"Key does not exist in this table. Can't get value.\"");
  }
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
  if (m_server->is_autocommit()){ // curently in autocommit
    m_server->change_mode();
    return reply_ok();
  } else { // in the middle of a transaction
    throw OperationException("\"Cannot begin transaction in the middle of another transaction.\"");
  }
}

Message ClientConnection::commit()
{
  // check if it is in transaction mode
  if(m_server->is_autocommit()){
    throw OperationException("\"No transaction has begun.\"");
  }
  //for each table in server, commit changes
  for(auto table_name : locked_tables){
    Table *table = get_server_table(table_name);
    table->commit_changes();
  }
  // change mode back to default (autocommit)
  m_server->change_mode();
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
    if(m_server->is_autocommit()){ // do only in autocommit mode
      table_ptr->lock();
    }
  } 
}

void ClientConnection::autocommit_unlock(Table* table_ptr)
{
  if(table_ptr != nullptr){
    if(m_server->is_autocommit()){ //do only in autocommit mode
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
  Message reply_msg;
  if(error_type == MessageType::ERROR){ // unrecoverable
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

