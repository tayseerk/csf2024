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
{
  rio_readinitb( &m_fdbuf, m_client_fd );
  m_stack = new ValueStack();
}

ClientConnection::~ClientConnection()
{
  // TODO: implement
  Close(m_client_fd);
}

void ClientConnection::chat_with_client()
{

    std::cerr<< "chatting with client\n"; // debugging
  // TODO: implement
  try{
    char buffer[MAXLINE];
    while(true)
    {
      try{
        // read message
        std::cerr<< "reading message\n"; // debugging
        ssize_t input = rio_readlineb(&m_fdbuf, buffer, MAXLINE);
        if (input == 0) break;
      
        std::cerr<< "message read\n"; // debugging
        // decode message
        std::string client_msg_str(buffer, input);
        Message client_msg;
        MessageSerialization::decode(client_msg_str, client_msg);
        if(!msg.is_valid()){
          std::cerr<< "throwing invalid 3\n"; // debugging
          throw InvalidMessage("Invalid message format.");
        } // MOVED FROM PROCESS HANDLING
        // process handling
        Message reply_msg = process_handling(client_msg);
        respond(reply_msg);
        if(client_msg.get_message_type() == MessageType::BYE){
          break;
        }
      } catch (OperationException &ex) { // recoverable
        handle_error(ex.what(), MessageType::FAILED);
        //continue;
      } catch (FailedTransaction &ex) { // recoverable
        handle_error(ex.what(), MessageType::FAILED);
        //continue;
      } catch (...) {
        break;
      }
    }
  } catch (InvalidMessage &ex) { //unrecoverable
    std::cerr<< "handle invalid message\n"; // debugging
    handle_error(ex.what(), MessageType::ERROR);
  } catch (CommException &ex) { // unrecoverable
    std::cerr<< "handle comm exception\n"; // debugging
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

  if(type == MessageType::LOGIN){
    return login(msg); //implement
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
  } else { //MessageType is BYE
    check_has_logged_in();
    return bye();
  }
}

Message ClientConnection::login(Message msg)
{
    std::cerr<< "handle login\n"; // debugging
  if(m_server->get_is_logged_in()){
    std::cerr<< "throwing invalid 1\n"; // debugging
    throw InvalidMessage("Already logged in.");
  }
  m_server->log_in();
  return reply_ok();
}

Message ClientConnection::create(Message msg)
{
    std::cerr<< "handle create\n"; // debugging
  std::string table_name = msg.get_table();
  if(m_server->find_table(table_name) != nullptr){
    throw OperationException("Can't create a table that already exists.");
  }
  m_server->create_table(table_name);
  return reply_ok();
}

Message ClientConnection::push(Message msg)
{
    std::cerr<< "handle push\n"; // debugging
  std::string value = msg.get_value();
  m_stack->push(value);
  return reply_ok();
}

Message ClientConnection::pop()
{
    std::cerr<< "handle pop\n"; // debugging
  check_empty_stack("Can't pop from an empty stack.");
   
  m_stack->pop();
  return reply_ok();
}

Message ClientConnection::top()
{
    std::cerr<< "handle top\n"; // debugging
  check_empty_stack("Can't get top of an empty stack.");
  
  //get top value from stack
  std::string value = m_stack->get_top(); 
  return reply_data(value);
}

Message ClientConnection::set(Message msg)
{
    std::cerr<< "handle set\n"; // debugging
  std::string table_name = msg.get_table();
  Table *table = get_server_table(table_name);
  autocommit_lock(table);

  check_empty_stack("Stack is empty, no value to set.");
  std::string value = m_stack->get_top();
  m_stack->pop();
  std::string key = msg.get_key();
  table->set(key, value);

  autocommit_unlock(table);
  return reply_ok();

}

Message ClientConnection::get(Message msg)
{
    std::cerr<< "handle get\n"; // debugging
  std::string table_name = msg.get_table();
  Table *table = get_server_table(table_name);

  autocommit_lock(table);
  std::string key = msg.get_key();
  std::string value;
  if (table->has_key(key)){
    value = table->get(key);
    m_stack->push(value);
    return reply_ok();
  } else {
    throw OperationException("Key does not exist in this table. Can't get value.");
  }
}

Message ClientConnection::handle_arithmetic(MessageType type)
{
  std::cerr<< "handle arithmetic\n"; // debugging
  check_empty_stack("No values in stack. Cannot calculate.");
  std::string right_val = m_stack->get_top();
  m_stack->pop();
  check_empty_stack("Only one operator. Cannot calculate.");
  std::cerr<< "stack not empty\n"; // debugging
  std::string left_val = m_stack->get_top();
  m_stack->pop();

std::cerr<< "checking if string is digit\n"; // debugging
if(!(string_is_digit(right_val) && string_is_digit(left_val))){
  std::cerr<< "not digit\n"; // debugging
  throw OperationException("Operator is not a digit.");
} std::cerr<< "is digit\n"; // debugging

  unsigned right = std::stoi(right_val); 
  unsigned left = std::stoi(left_val);
  std::cerr<< "converted string to int\n"; // debugging
  std::string value = do_arithmetic(type, left, right);

  m_stack->push(value);

  return reply_ok();
}

Message ClientConnection::begin()
{
    std::cerr<< "handle begin\n"; // debugging
  if (m_server->is_autocommit()){
    m_server->change_mode();
    return reply_ok();
  } else {
    throw OperationException("Cannot begin transaction in the middle of another transaction.");
  }
}

Message ClientConnection::commit()
{
    std::cerr<< "handle commit\n"; // debugging
  if(m_server->is_autocommit()){
    throw OperationException("No transaction has begun.");
  }
  for(auto table_name : locked_tables){
    Table *table = get_server_table(table_name);
    table->commit_changes();
    //transaction_unlock();
  }
  m_server->change_mode();
  return reply_ok();
}

Message ClientConnection::bye()
{
  m_server->log_out();
  return reply_ok();
}

Message ClientConnection::reply_ok()
{
  return Message(MessageType::OK);
}

Message ClientConnection::reply_data(std::string value)
{
  return Message(MessageType::DATA, {value});
}

void ClientConnection::autocommit_lock(Table* table_ptr)
{
  if(table_ptr != nullptr){
    if(m_server->is_autocommit()){ //autocommit mode
      table_ptr->lock();
    }
  }
}

void ClientConnection::autocommit_unlock(Table* table_ptr)
{
  if(table_ptr != nullptr){
    if(m_server->is_autocommit()){ //autocommit mode
      table_ptr->unlock();
    }
  }
}

Table* ClientConnection::get_server_table(std::string table_name)
{
  std::cerr<< "get_server_table\n"; // debugging
  Table *table = m_server->find_table(table_name);
  std::cerr<< "HERE\n"; // debugging
  if(table == nullptr){
    throw OperationException("Table does not exist.");
  } else {
    std::cerr<< "returning table\n"; // debugging
    return table;
  }
}

bool ClientConnection::string_is_digit(std::string& str) 
{
  std::cerr<< "in check\n"; // debugging
  for(long unsigned i = 0; i<str.size(); i++ ){
    std::cerr<< "in loop\n"; // debugging
    if(!isdigit(str[i])){
      std::cerr<< "not digit\n"; // debugging
      return false;
    }
  }
  std::cerr<< "finished check\n"; // debugging
  return true;
}

std::string ClientConnection::do_arithmetic(MessageType type, unsigned left, unsigned right)
{
  std::string value;
  if(type == MessageType::ADD){
    value = std::to_string(left + right);
  } else if(type == MessageType::MUL) {
    value = std::to_string(left * right);
  } else if(type == MessageType::SUB){
    value = std::to_string(left - right);
  } else {
    if(right == 0){
      throw OperationException("Cannot divide by zero.");
    }
    value = std::to_string(left / right);
  }
  return value;
}

void ClientConnection::handle_error(const std::string error_msg, MessageType error_type)
{
  std::cerr<< "handle error\n"; // debugging
  Message reply_msg;
  if(error_type == MessageType::ERROR){
    reply_msg = reply_error(error_msg);
  } else {
    reply_msg = reply_failed(error_msg);
  }
  respond(reply_msg);
}

Message ClientConnection::reply_error(const std::string error_msg)
{
  std::cerr<< "reply error\n"; // debugging
  m_server->log_out(); //logs out when cannot continue due to error
  return Message(MessageType::ERROR, {error_msg});
}

Message ClientConnection::reply_failed(const std::string error_msg)
{
  return Message(MessageType::FAILED, {error_msg});
}

void ClientConnection::respond(Message reply)
{
  std::cerr<< "responding...\n"; // debugging
  std::string reply_str;
  MessageSerialization::encode(reply, reply_str);

  rio_writen(m_client_fd, reply_str.c_str(), reply_str.length());
}

void ClientConnection::check_has_logged_in()
{
  if(!m_server->get_is_logged_in()){
    std::cerr<< "throwing invalid 2\n"; // debugging
    throw InvalidMessage("Has not logged in.");
  }
}

void ClientConnection::check_empty_stack(const std::string error_msg)
{
  std::cerr<< "checking if stack is empty\n"; // debugging
  if(m_stack->is_empty()){
    std::cerr<< "empty stack\n"; // debugging
    throw OperationException(error_msg);
  }
}

