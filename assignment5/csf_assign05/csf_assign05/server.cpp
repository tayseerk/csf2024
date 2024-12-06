#include <iostream>
#include <cassert>
#include <memory>
#include "csapp.h"
#include "exceptions.h"
#include "guard.h"
#include "server.h"


Server::Server()
  : mode( 0 ) // autocommit is default mode
  , is_logged_in (false)
  // TODO: initialize member variables
{
  // TODO: implement
  pthread_mutex_init(&mutex, NULL);
}

Server::~Server()
{
  // TODO: implement
  Close(socket_fd);
  pthread_mutex_destroy(&mutex);
}

void Server::listen( const std::string &port )
{
  // TODO: implement
    std::cerr<< "listening\n"; // debugging
  try{
    int port_int = std::stoi(port);
    if(port_int < 1024 || port_int > 65535) {
      throw std::out_of_range("Port out of range.");
    }
  } catch(...){
    fatal("Invalid port.");
  }

  socket_fd = open_listenfd(port.c_str());
  if(socket_fd < 0) {
    fatal("Failed to listen.");
  }
}

void Server::server_loop()
{
  // TODO: implement

    std::cerr<< "in server loop\n"; // debugging

  // Note that your code to start a worker thread for a newly-connected
  // client might look something like this:
  while(true){
    struct sockaddr_in clientaddr;
    int client_fd = accept_connection(socket_fd, &clientaddr);
    ClientConnection *client = new ClientConnection( this, client_fd );
    pthread_t thr_id;
    if ( pthread_create( &thr_id, nullptr, client_worker, client ) != 0 ){
      log_error( "Could not create client thread" );
      delete client;
      continue;
    } 
    pthread_detach( pthread_self() ); // (thr_id)?
    
  }
  std::cerr<< "outside server loop\n"; // debugging
}


void *Server::client_worker( void *arg )
{
  // TODO: implement
    std::cerr<< "calling client worker\n"; // debugging

  // Assuming that your ClientConnection class has a member function
  // called chat_with_client(), your implementation might look something
  // like this:

  std::unique_ptr<ClientConnection> client( static_cast<ClientConnection *>( arg ) );
  client->chat_with_client();
  return nullptr;

}

void Server::log_error( const std::string &what )
{
  std::cerr << "Error: " << what << "\n";
}

// TODO: implement member functions

bool Server::is_autocommit()
{
  return mode == 0;
}

void Server::change_mode()
{
  if(mode == 0){
    mode = 1;
  } else {
    mode = 0;
  }
}

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
  std::cerr<< "creating table\n"; // debugging
  Table *table = new Table(name);
  tables.emplace(name, table);
}

Table* Server::find_table( const std::string &name )
{
    std::cerr<< "finding table\n"; // debugging
  if (tables.find(name)!= tables.end())
  {
    std::cerr<< "table found\n"; // debugging
    return tables[name];
  }
  std::cerr<< "no table\n"; // debugging
  return nullptr;
}

void Server::fatal (std::string err_message)
{
  log_error(err_message);
  std::exit(EXIT_FAILURE);
}

void Server::log_in()
{
    std::cerr<< "logging in\n"; // debugging
  is_logged_in = true;
}

void Server::log_out()
{
    std::cerr<< "logging out\n"; // debugging
  is_logged_in = false;
}

bool Server::get_is_logged_in()
{
    std::cerr<< "checking login status\n"; // debugging
  return is_logged_in;
}
