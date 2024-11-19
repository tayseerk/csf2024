#include <utility>
#include <sstream>
#include <cassert>
#include <map>
#include "exceptions.h"
#include "message_serialization.h"

void MessageSerialization::encode( const Message &msg, std::string &encoded_msg )
{
  // Message --> string map
  static std::map<MessageType, std::string> message_to_string = { 
    {MessageType::LOGIN, "LOGIN"}, {MessageType::CREATE, "CREATE"}, 
    {MessageType::PUSH, "PUSH"}, {MessageType::POP, "POP"}, {MessageType::TOP, "TOP"}, 
    {MessageType::SET, "SET"}, {MessageType::GET, "GET"}, {MessageType::ADD, "ADD"}, 
    {MessageType::MUL, "MUL"}, {MessageType::SUB, "SUB"}, {MessageType::DIV, "DIV"}, 
    {MessageType::BEGIN, "BEGIN"}, {MessageType::COMMIT, "COMMIT"}, {MessageType::BYE, "BYE"}, 
    {MessageType::OK, "OK"}, {MessageType::FAILED, "FAILED"}, {MessageType::ERROR, "ERROR"}, 
    {MessageType::DATA, "DATA"}
  };
  encoded_msg.clear(); // clear previous messages

  MessageType message_type = msg.get_message_type(); // get message type
  encoded_msg += message_to_string[message_type]; // add message type to string
  // add arguments
  for (unsigned i = 0; i<msg.get_num_args(); i++){ 
    encoded_msg += " " + msg.get_arg(i);
  }
  encoded_msg += "\n"; // add null terminator
  // check if it's over max length
  if (encoded_msg.size() > msg.MAX_ENCODED_LEN) {
    throw InvalidMessage("Message is too long");
  }
}

void MessageSerialization::decode( const std::string &encoded_msg_, Message &msg )
{
  // string --> Message
  static std::map<std:: string, MessageType> string_to_message = { 
    {"LOGIN", MessageType::LOGIN}, {"CREATE", MessageType::CREATE}, 
    {"PUSH", MessageType::PUSH}, {"POP", MessageType::POP}, {"TOP", MessageType::TOP}, 
    {"SET", MessageType::SET}, {"GET", MessageType::GET}, {"ADD", MessageType::ADD}, 
    {"MUL", MessageType::MUL}, {"SUB", MessageType::SUB}, {"DIV", MessageType::DIV}, 
    {"BEGIN", MessageType::BEGIN}, {"COMMIT", MessageType::COMMIT}, {"BYE", MessageType::BYE}, 
    {"OK", MessageType::OK}, {"FAILED", MessageType::FAILED}, {"ERROR", MessageType::ERROR}, 
    {"DATA", MessageType::DATA}
  };

  msg = Message();

  check_exceptions(encoded_msg_, msg);

  unsigned index = 0;
  std::string msg_type_str = extract_string(encoded_msg_, index);
  remove_null_char(msg_type_str);
  msg.set_message_type(string_to_message[msg_type_str]);

  while (index < encoded_msg_.length()){
    std::string arg_str = extract_string(encoded_msg_, index);
    if(arg_str.empty() || arg_str == "\n"){
      break;
    }
    remove_null_char(arg_str);
    msg.push_arg(arg_str);
  }

  if(!msg.is_valid()){
    throw InvalidMessage("Resulting message is invalid.");
  }
}

 void MessageSerialization::check_exceptions(const std::string &encoded_msg_, Message &msg)
 {
  // check for max size
  if(encoded_msg_.size() > msg.MAX_ENCODED_LEN){
    throw InvalidMessage("Source message is too long");
  }
  // check for null terminator at the end
  if(encoded_msg_.back() != '\n'){
    throw InvalidMessage("Source message lacking terminating newline.");
  }
 }

 std::string MessageSerialization::extract_string(const std::string &encoded_msg_, unsigned &index)
 {

  unsigned start = index;

  while(encoded_msg_[start] == ' '){
    start++;
  }

  unsigned end = start;
  if(encoded_msg_[start] == '\"'){
    start++;
    end++;
    while(encoded_msg_[end] != '\"' && end < encoded_msg_.length()){
      end++;
    }
  } else {
    while(encoded_msg_[end] != ' ' && end < encoded_msg_.length()){
      end++;
    }
  }

  index = end + 1;

  return encoded_msg_.substr(start, end - start);
 }

 void MessageSerialization::remove_null_char(std::string &string)
 {
    size_t null_pos = string.find('\n');
    if(null_pos!= std::string::npos){
      string.erase(null_pos);
    }
 }
