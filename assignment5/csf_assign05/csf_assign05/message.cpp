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
  return get_arg(0);
}

std::string Message::get_table() const
{
  return get_arg(0);
}

std::string Message::get_key() const
{
  return get_arg(1);
}

std::string Message::get_value() const
{
  return get_arg(0);
}

std::string Message::get_quoted_text() const
{
  return get_arg(0);
}

void Message::push_arg( const std::string &arg )
{
  m_args.push_back( arg );
}

bool Message::is_valid() const
{
  // different message type cases 
  // each checks the number of arguments is correct
  // checks length, and checks that the arguments are valid (depending on the type of arg)
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
  return get_num_args() == expected_num_args; // number of actual args matches expected num args
}

bool Message::validity(const unsigned cmd_len, const unsigned arg_len, bool arg_valid) const
{
  // checks message length is less than max length
  // checks that argument is valid is true
  return !(cmd_len + arg_len + 1 > MAX_ENCODED_LEN) && arg_valid;
}

bool Message::identifier_is_valid(std::string arg) const
{
  // first char is A-Z or a-z
  if (!std::isalpha(arg[0])){
    return false;
  }
  // identifier has to only contain letters, underscores, whitespaces, or digits
  for (long unsigned int i = 1; i<arg.size(); i++){
    if (!(std::isalpha(arg[i]) || arg[i]== '_' || arg[i]==' ' || isdigit(arg[i]))){
      return false;
    }
  } 
  // every char in identifier is valid
  return true;
}

bool Message::both_identifiers_are_valid(std::string arg1, std::string arg2) const
{
  return identifier_is_valid(arg1) && identifier_is_valid(arg2); // check two identifiers
}

bool Message::value_is_valid(std::string arg) const
{
  return arg.find(' ') == std::string::npos; // no whitespaces
}

bool Message::quoted_text_is_valid(std::string arg) const
{ // there are no quotation marks in the middle of the text
  for (long unsigned int i = 1; i<arg.size()-1; i++){
    if(arg[i] == 34){
      return false;
    }
  }
  return true;
}
