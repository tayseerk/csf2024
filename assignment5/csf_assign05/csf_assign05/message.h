#ifndef MESSAGE_H
#define MESSAGE_H

#include <vector>
#include <string>

enum class MessageType {
  // Used only for uninitialized Message objects
  NONE,

  // Requests
  LOGIN,
  CREATE,
  PUSH,
  POP,
  TOP,
  SET,
  GET,
  ADD,
  SUB,
  MUL,
  DIV,
  BEGIN,
  COMMIT,
  BYE,

  // Responses
  OK,
  FAILED,
  ERROR,
  DATA,
};

class Message {
private:
  MessageType m_message_type;
  std::vector<std::string> m_args;

public:
  // Maximum encoded message length (including terminator newline character)
  static const unsigned MAX_ENCODED_LEN = 1024;

  Message();
  Message( MessageType message_type, std::initializer_list<std::string> args = std::initializer_list<std::string>() );
  Message( const Message &other );
  ~Message();

  Message &operator=( const Message &rhs );

  MessageType get_message_type() const;
  void set_message_type( MessageType message_type );

  std::string get_username() const;
  std::string get_table() const;
  std::string get_key() const;
  std::string get_value() const;
  std::string get_quoted_text() const;

  void push_arg( const std::string &arg );

  bool is_valid() const;

  unsigned get_num_args() const { return m_args.size(); }
  std::string get_arg( unsigned i ) const { return m_args.at( i ); }

  // student implemented:

  bool valid_num_args(unsigned int expected_num_args) const;
  bool validity(const unsigned cmd_len, const unsigned arg_len, bool arg_valid) const;
  bool identifier_is_valid(std::string arg) const;
  bool both_identifiers_are_valid(std::string arg1, std::string arg2) const;
  bool value_is_valid(std::string arg) const;
  bool quoted_text_is_valid(std::string arg) const;
};

#endif // MESSAGE_H
