#include <iostream>
#include <cassert>
#include <memory>
#include "csapp.h"
#include "exceptions.h"
#include "guard.h"
#include "server.h"


Server::Server()
{
  pthread_mutex_init(&mutex, NULL);
  pthread_mutex_init(&mutex_for_tables, NULL);  
}

Server::~Server()
{
  Close(socket_fd);
  pthread_mutex_destroy(&mutex);
  pthread_mutex_destroy(&mutex_for_tables);
}

void Server::listen( const std::string &port )
{
  try{
    int port_int = std::stoi(port);
    if(port_int < 1024 || port_int > 65535) {
      throw std::out_of_range("Port out of range.");
    }
  } catch(...){
    fatal("Invalid port.");
  }
  // open socket
  socket_fd = open_listenfd(port.c_str());
  if(socket_fd < 0) {
    fatal("Failed to listen.");
  }
}

void Server::server_loop()
{
  while(true) { // continuously accept new connections
    struct sockaddr_in clientaddr;
    int client_fd = accept_connection(socket_fd, &clientaddr); // accept
    ClientConnection *client = new ClientConnection( this, client_fd ); // create client
    // create thread
    pthread_t thr_id;
    if ( pthread_create( &thr_id, nullptr, client_worker, client ) != 0 ){
      log_error( "Could not create client thread" );
      delete client;
      pthread_detach( pthread_self() ); // detach
      continue;
    } 
  }
}


void *Server::client_worker( void *arg )
{
  // start chat
  std::unique_ptr<ClientConnection> client( static_cast<ClientConnection *>( arg ) );
  client->chat_with_client();
  return nullptr;
}

void Server::log_error( const std::string &what )
{
  std::cerr << "Error: " << what << "\n";
}

// TODO: implement member functions

int Server::accept_connection(int socket_fd, struct sockaddr_in *clientaddr) 
{
  unsigned clientlen = sizeof(*clientaddr);
  int client_fd = accept(socket_fd, (struct sockaddr *) clientaddr, &clientlen);
  if(client_fd<0){
    if (errno == EINTR) {
      return -1;
    } else {
      log_error("Failed to accept: " +std::string(strerror(errno))); // errno?
      return -1;
    }
  }
  return client_fd;
}

void Server::create_table( const std::string &name )
{
  pthread_mutex_lock(&mutex_for_tables);
  if (tables.find(name) != tables.end()) {
    pthread_mutex_unlock(&mutex_for_tables);
    throw OperationException("\"already created\"");
  }
  Table *table = new Table(name);
  tables[name] = table;
  pthread_mutex_unlock(&mutex_for_tables);
}

Table* Server::find_table( const std::string &name )
{
  pthread_mutex_lock(&mutex_for_tables);
  Table *t = nullptr;
  if (tables.find(name) != tables.end()) {
    t = tables[name];
  }
  pthread_mutex_unlock(&mutex_for_tables);
  return t;
}

void Server::fatal (std::string err_message)
{
  log_error(err_message);
  std::exit(EXIT_FAILURE);
}
