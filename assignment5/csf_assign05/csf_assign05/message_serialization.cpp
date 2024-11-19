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
  // std::string args_str = extract_string(encoded_msg_, index);

  msg.set_message_type(string_to_message[msg_type_str]);

  while (index < encoded_msg_.length()){
    std::string arg_str = extract_string(encoded_msg_, index);
    if(arg_str == "\n"){
      break;
    }
    size_t null_pos = arg_str.find('\n');
    if(null_pos!= std::string::npos){
      arg_str.erase(null_pos);
    }
    msg.push_arg(arg_str);
  }

  // std::tuple<std::string, std::string> spliced_command = splice_string(encoded_msg_);

  //check for quotes beginning and end

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


  // while(encoded_msg_[index] == ' '){
  //   index++;
  // }

  // unsigned start = index;
  // while(encoded_msg_[index] != ' '){
  //   index++;
  // }
  unsigned start = index;

  while(encoded_msg_[start] == ' '){
    start++;
  }

  unsigned end = start;
  while(encoded_msg_[end] != ' ' && end < encoded_msg_.length()){
    end++;
  }

  index = end;

  return encoded_msg_.substr(start, end - start);
  // return encoded_msg_.substr(start, index - start);
 }

//  std::tuple<std::string, std::string> MessageSerialization::splice_string(const std::string &encoded_msg_)
//  {
//   std::tuple<std::string, std::string> spliced;
//   long unsigned int itr; //keep track of end of string
//   std::string message_type_str = find_string(encoded_msg_, itr);
//   std::string args = find_args(encoded_msg_.substr(itr, encoded_msg_.length() - itr), itr);
  
//   size_t null_pos = args.find('\n');
//   if(null_pos!= std::string::npos){
//     args.erase(null_pos);
//   }
  
//   spliced = make_tuple(message_type_str, args);

//   return spliced;
//  }

//  std::string MessageSerialization::find_string(const std::string &encoded_msg_, long unsigned int &itr)
//  {
//   long unsigned int start_index = 0; // for message type word
//   while(encoded_msg_[start_index] == ' '){
//     start_index++;
//   }

//   long unsigned int end_index = start_index; // for message type word
//   while(encoded_msg_[end_index] != ' ' && end_index < encoded_msg_.length()){
//     end_index++;
//   }

//   itr = end_index;

//   return encoded_msg_.substr(start_index, end_index - start_index);
//  }

//  std::string MessageSerialization::find_args(std::string &sub_string, long unsigned int &itr )
//  {
//   std::string args;
//   // if (sub_string.length()==1){
//   //   args+=sub_string;
//   // } else {
//   //   args += find_string(sub_string, itr);
//   //   find_args(sub_string.substr(args.length(), sub_string.length()), itr);
//   // }
//   // return args;
//   while (itr < sub_string.length() && sub_string[itr] != '\n') {
//     if (!args.empty()) {
//       args += " "; 
//     }
//     args += find_string(sub_string, itr); 
//     std::string new_string = sub_string.substr(itr, sub_string.length() - itr);
//     sub_string.erase();
//     sub_string += new_string;
//   }
//   return args;
//  }
