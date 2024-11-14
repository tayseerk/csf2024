#include <utility>
#include <sstream>
#include <cassert>
#include <map>
#include "exceptions.h"
#include "message_serialization.h"

// Message --> string
void MessageSerialization::encode( const Message &msg, std::string &encoded_msg )
{
  // TODO: implement
  static std::map<MessageType, std::string> message_to_string = { 
    {MessageType::LOGIN, "LOGIN"}, {MessageType::CREATE, "CREATE"}, 
    {MessageType::PUSH, "PUSH"}, {MessageType::POP, "POP"}, {MessageType::TOP, "TOP"}, 
    {MessageType::SET, "SET"}, {MessageType::GET, "GET"}, {MessageType::ADD, "ADD"}, 
    {MessageType::MUL, "MUL"}, {MessageType::SUB, "SUB"}, {MessageType::DIV, "DIV"}, 
    {MessageType::BEGIN, "BEGIN"}, {MessageType::COMMIT, "COMMIT"}, {MessageType::BYE, "BYE"}, 
    {MessageType::OK, "OK"}, {MessageType::FAILED, "FAILED"}, {MessageType::ERROR, "ERROR"}, 
    {MessageType::DATA, "DATA"}
  };


  encoded_msg.clear();

  MessageType message_type = msg.get_message_type();
  encoded_msg += message_to_string[message_type];

  for (unsigned i = 0; i<msg.get_num_args(); i++){
    encoded_msg += " " + msg.get_arg(i);
  }
  encoded_msg += "\n";

  if (encoded_msg.size() > msg.MAX_ENCODED_LEN) {
    throw InvalidMessage("Message is too long");
  }
}

// string --> Message
void MessageSerialization::decode( const std::string &encoded_msg_, Message &msg )
{
  // TODO: implement
  static std::map<std:: string, MessageType> string_to_message = { 
    {"LOGIN", MessageType::LOGIN}, {"CREATE", MessageType::CREATE}, 
    {"PUSH", MessageType::PUSH}, {"POP", MessageType::POP}, {"TOP", MessageType::TOP}, 
    {"SET", MessageType::SET}, {"GET", MessageType::GET}, {"ADD", MessageType::ADD}, 
    {"MUL", MessageType::MUL}, {"SUB", MessageType::SUB}, {"DIV", MessageType::DIV}, 
    {"BEGIN", MessageType::BEGIN}, {"COMMIT", MessageType::COMMIT}, {"BYE", MessageType::BYE}, 
    {"OK", MessageType::OK}, {"FAILED", MessageType::FAILED}, {"ERROR", MessageType::ERROR}, 
    {"DATA", MessageType::DATA}
  };
  
  if(encoded_msg_.size() > msg.MAX_ENCODED_LEN){
    throw InvalidMessage("Source message is too long");
  }
  if(encoded_msg_.back() != '\n'){
    throw InvalidMessage("Source message lacking terminating newline.");
  }






  //check for quotes beginning and end

}
