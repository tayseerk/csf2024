#include <iostream>
#include <cassert>
#include <memory>
#include "csapp.h"
#include "exceptions.h"
#include "guard.h"
#include "server.h"


Server::Server()
  : mode( 0 ) // autocommit is default mode
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
  struct sockaddr_in serveraddr = {0};
  int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_fd < 0) 
  {
    fatal("socket failed");
  }

  serveraddr.sin_family = AF_INET;
  serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
  serveraddr.sin_port = htons(static_cast<unsigned short>(std::stoi(port)));

  if (bind(socket_fd, (struct sockaddr *) &serveraddr, sizeof(serveraddr)) < 0) 
  {  
    fatal("bind failed");
  }
  if (::listen(socket_fd, 5) < 0)
  {
    fatal("listen failed");
  } 

}

void Server::server_loop()
{
  // TODO: implement

  // Note that your code to start a worker thread for a newly-connected
  // client might look something like this:
  while(true){
    int client_fd = accept_connection();
    if(client_fd >= 0){
      ClientConnection *client = new ClientConnection( this, client_fd );
      pthread_t thr_id;
      if ( pthread_create( &thr_id, nullptr, client_worker, client ) != 0 )
        log_error( "Could not create client thread" );
    }
  }
}


void *Server::client_worker( void *arg )
{
  // TODO: implement

  // Assuming that your ClientConnection class has a member function
  // called chat_with_client(), your implementation might look something
  // like this:
/*
  std::unique_ptr<ClientConnection> client( static_cast<ClientConnection *>( arg ) );
  client->chat_with_client();
  return nullptr;
*/
return nullptr;
}

void Server::log_error( const std::string &what )
{
  std::cerr << "Error: " << what << "\n";
}

// TODO: implement member functions

int Server::accept_connection() 
{
  struct sockaddr_in  client_addr;
  socklen_t client_len = sizeof(client_addr);
  int client_fd = accept(socket_fd, (struct sockaddr *) &client_addr, &client_len);
  if (client_fd < 0) 
    fatal("accept failed");
  return client_fd;

}

void Server::create_table( const std::string &name )
{
  Table *table = new Table(name);
  tables.emplace(name, table);
}

Table* Server::find_table( const std::string &name )
{
  if (tables.find(name)!= tables.end())
  {
    return tables[name];
  }

  return nullptr;
}

void Server::fatal (std::string err_message)
{
  log_error(err_message);
  std::exit(EXIT_FAILURE);
}
