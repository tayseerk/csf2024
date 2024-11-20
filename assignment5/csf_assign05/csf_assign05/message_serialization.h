#ifndef MESSAGE_SERIALIZATION_H
#define MESSAGE_SERIALIZATION_H

#include "message.h"

namespace MessageSerialization {
  void encode(const Message &msg, std::string &encoded_msg);
  void decode(const std::string &encoded_msg, Message &msg);

  // helper functions:
  void check_exceptions(const std::string &encoded_msg, Message &msg);
  std::string extract_string(const std::string &encoded_msg_, unsigned &index);
  void remove_null_char(std::string &string);
};

#endif // MESSAGE_SERIALIZATION_H
