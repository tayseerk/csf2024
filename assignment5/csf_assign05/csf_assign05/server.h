#ifndef SERVER_H
#define SERVER_H

#include <map>
#include <string>
#include <pthread.h>
#include "table.h"
#include "client_connection.h"

class Server {
private:
  // TODO: add member variables
  pthread_mutex_t mutex; // mutex for server
  int mode; //0 = autocommit, 1 = transaction
  int socket_fd;
  std::map<std::string, Table*> tables; // map of tables (key is table name, value is table object)
  //bool is_logged_in;
  // copy constructor and assignment operator are prohibited
  Server( const Server & );
  Server &operator=( const Server & );

public:
  Server();
  ~Server();

  void listen( const std::string &port );
  void server_loop();

  static void *client_worker( void *arg );

  void log_error( const std::string &what );

  // TODO: add member functions
  bool is_autocommit(); // checks mode
  void change_mode(); // changes mode
  int accept_connection(int socket_fd, struct sockaddr_in *clientaddr); 
  void create_table( const std::string &name ); // suggested function
  Table *find_table( const std::string &name ); // suggested function
  void fatal (std::string err_message); 
  bool get_is_logged_in();


};


#endif // SERVER_H
