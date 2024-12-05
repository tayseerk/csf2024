#include <iostream>
#include <cassert>
#include "csapp.h"
#include "message.h"
#include "message_serialization.h"
#include "server.h"
#include "exceptions.h"
#include "client_connection.h"

ClientConnection::ClientConnection( Server *server, int client_fd )
  : m_server( server )
  , m_client_fd( client_fd )
{
  rio_readinitb( &m_fdbuf, m_client_fd );
}

ClientConnection::~ClientConnection()
{
  // TODO: implement
  Close(m_client_fd);
}

void ClientConnection::chat_with_client()
{
  // TODO: implement
  char buffer[MAXLINE];
  while(true)
  {
    // read message
    ssize_t input = rio_readlineb(&m_fdbuf, buffer, MAXLINE);
    if (input == 0) break;
  }
    // decode message
    std::string client_msg_str;
    Message client_msg;
    MessageSerialization::decode(client_msg_str, client_msg);
    // process handling
    Message reply_msg = process_handling(client_msg);
    // encode reply
   // respond(reply_msg);

  
}

// TODO: additional member functions
Message ClientConnection::process_handling(Message msg)
{
  MessageType type = msg.get_message_type();
  if(type == MessageType::LOGIN){
    //something
  }
}
