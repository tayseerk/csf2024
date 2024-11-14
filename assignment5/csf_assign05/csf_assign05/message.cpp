#include <set>
#include <map>
#include <regex>
#include <cassert>
#include "message.h"

Message::Message()
  : m_message_type(MessageType::NONE)
{
}

Message::Message( MessageType message_type, std::initializer_list<std::string> args )
  : m_message_type( message_type )
  , m_args( args )
{
}

Message::Message( const Message &other )
  : m_message_type( other.m_message_type )
  , m_args( other.m_args )
{
}

Message::~Message()
{
}

Message &Message::operator=( const Message &rhs )
{
  // TODO: implement
  this->m_message_type = rhs.m_message_type;
  this->m_args = rhs.m_args;

  return *this;
}

MessageType Message::get_message_type() const
{
  return m_message_type;
}

void Message::set_message_type(MessageType message_type)
{
  m_message_type = message_type;
}

std::string Message::get_username() const
{
  // TODO: implement
  return get_arg(0);
}

std::string Message::get_table() const
{
  // TODO: implement
  return get_arg(0);
}

std::string Message::get_key() const
{
  // TODO: implement
  return get_arg(1);
}

std::string Message::get_value() const
{
  // TODO: implement
  return get_arg(0);
}

std::string Message::get_quoted_text() const
{
  // TODO: implement
  return get_arg(0);
}

void Message::push_arg( const std::string &arg )
{
  m_args.push_back( arg );
}

bool Message::is_valid() const
{
  if (m_message_type == MessageType::LOGIN){
    return valid_num_args(1) && validity(5, get_username().size(), identifier_is_valid(get_username()));
  } else if (m_message_type == MessageType::CREATE){
    return valid_num_args(1) && validity(6, get_table().size(), identifier_is_valid(get_table()));
  } else if (m_message_type == MessageType::PUSH || m_message_type == MessageType::DATA){
    return valid_num_args(1) && validity(4, get_value().size(), value_is_valid(get_value()));
  } else if (m_message_type == MessageType::SET || m_message_type == MessageType::GET){
    return valid_num_args(2) && validity(3, get_table().size() + get_key().size(), both_identifiers_are_valid(get_table(), get_key()));
  } else if (m_message_type == MessageType::FAILED){
    return valid_num_args(1) && validity(6, get_quoted_text().size(), quoted_text_is_valid(get_quoted_text()));
  } else if (m_message_type == MessageType::ERROR){
    return valid_num_args(1) && validity(5, get_quoted_text().size(), quoted_text_is_valid(get_quoted_text()));
  } else { // PUSH, POP, TOP, ADD, SUB, MUL, DIV, BEGIN, COMMIT, BYE, OK
    return valid_num_args(0);
  }

}

bool Message::valid_num_args(unsigned int expected_num_args) const
{
  return get_num_args() == expected_num_args;
}

bool Message::validity(const unsigned cmd_len, const unsigned arg_len, bool arg_valid) const
{
  return !(cmd_len + arg_len + 1 > MAX_ENCODED_LEN) && arg_valid;
}

bool Message::identifier_is_valid(std::string arg) const
{
  // first char is A-Z or a-z
  if (!std::isalpha(arg[0])){
    return false;
  }
  for (long unsigned int i = 1; i<arg.size(); i++){
    if (!(std::isalpha(arg[i]) || arg[i]== '_' || arg[i]==' ' || isdigit(arg[i]))){
      return false;
    }
  } 
  
  return true;
}

bool Message::both_identifiers_are_valid(std::string arg1, std::string arg2) const
{
  return identifier_is_valid(arg1) && identifier_is_valid(arg2);
}

bool Message::value_is_valid(std::string arg) const
{
  return arg.find(' ') == std::string::npos;
}

bool Message::quoted_text_is_valid(std::string arg) const
{
  for (long unsigned int i = 1; i<arg.size()-1; i++){
    if(arg[i] == 34){
      return false;
    }
  }
  return true;
}
