#ifndef CLIENT_CONNECTION_H
#define CLIENT_CONNECTION_H

#include <set>
#include "message.h"
#include "csapp.h"

class Server; // forward declaration
class Table; // forward declaration
class ValueStack; //forward declare

class ClientConnection {
private:
  Server *m_server;
  int m_client_fd;
  rio_t m_fdbuf;
  ValueStack *m_stack; 
  std::set<std::string> locked_tables;
  bool login_status;
  int mode_status; // mode = 0 when autocommit and mode = 1 when in transaction
  // copy constructor and assignment operator are prohibited
  ClientConnection( const ClientConnection & );
  ClientConnection &operator=( const ClientConnection & );

public:
  ClientConnection( Server *server, int client_fd );
  ~ClientConnection();

  void chat_with_client();

  // TODO: additional member functions
  Message process_handling(Message msg);
  //process handling
  Message login(Message msg);
  Message create(Message msg);
  Message push(Message msg);
  Message pop();
  Message top();
  Message set(Message msg);
  Message get(Message msg);
  Message handle_arithmetic(MessageType type);
  Message begin();
  Message commit();
  Message bye();
  //success replies
  Message reply_ok();
  Message reply_data(std::string value);
  //autocommit
  void autocommit_lock(Table* table_ptr);
  void autocommit_unlock(Table* table_ptr);
  //more helper
  Table* get_server_table(std::string table_name);
  bool string_is_digit(std::string& str);
  std::string do_arithmetic(MessageType type, unsigned left, unsigned right);
  //error handling
  void handle_error(const std::string error_msg, MessageType error_type);
  Message reply_error(const std::string error_msg);
  Message reply_failed(const std::string error_msg);
  // send back
  void respond(Message reply);
  //checks
  void check_has_logged_in();
  void check_empty_stack(const std::string error_msg);
  // more helper functions
  void rollback_trans(); // rollback a transaction 
  void lock_table(Table *table); // locks table right away in autocommit mode, uses trylock for trans mode
  void unlock_table(Table *table); // unlocks when in autocommit mode, doesn't do anything in trans mode
    
};

#endif // CLIENT_CONNECTION_H
