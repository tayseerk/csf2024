#include <iostream>
#include <string>
#include "csapp.h"
#include "message.h"
#include "message_serialization.h"
#include "exceptions.h"

int main(int argc, char **argv)
{
  if ( argc != 6 ) {
    std::cerr << "Usage: ./get_value <hostname> <port> <username> <table> <key>\n";
    return 1;
  }

  std::string hostname = argv[1];
  std::string port = argv[2];
  std::string username = argv[3];
  std::string table = argv[4];
  std::string key = argv[5];

  int clientfd;
  rio_t rio;

  clientfd = open_clientfd(hostname.c_str(), port.c_str());
  if (clientfd < 0) {
    std::cerr << "Error: cannot establish connection to server\n";
    return 1;
  }

  rio_readinitb(&rio, clientfd);

  // send message for LOGIN 
  try {
    Message login_msg(MessageType::LOGIN, {username});
    std::string encoded_message;
    MessageSerialization::encode(login_msg, encoded_message);
    rio_writen(clientfd, encoded_message.c_str(), encoded_message.length());

    // read response
    char buf[Message::MAX_ENCODED_LEN];
    ssize_t length = rio_readlineb(&rio, buf, Message::MAX_ENCODED_LEN);
    if (length <= 0) {
      std::cerr << "Error: could not read response from server\n";
      Close(clientfd);
      return 1;
    }

    Message response;
    MessageSerialization::decode(std::string(buf, length),response);

    if (response.get_message_type() == MessageType::ERROR || response.get_message_type() == MessageType::FAILED) {
      std::cerr << "Error: " << response.get_quoted_text() << "\n";
      Close(clientfd);
      return 1;
    } else if (response.get_message_type() != MessageType::OK) {
      std::cerr << "Error: bad server response\n";
      Close(clientfd);
      return 1;
    }

    // send message for GET
    Message get_msg(MessageType::GET, {table, key});
    MessageSerialization::encode(get_msg, encoded_message);
    rio_writen(clientfd, encoded_message.c_str(), encoded_message.length());

    // read
    length = rio_readlineb(&rio, buf, Message::MAX_ENCODED_LEN);
    if (length <= 0) {
      std::cerr << "Error: could not read from server\n";
      Close(clientfd);
      return 1;
    }

    MessageSerialization::decode(std::string(buf, length), response);
    if (response.get_message_type() == MessageType::ERROR || response.get_message_type() == MessageType::FAILED) {
      std::cerr << "Error: " << response.get_quoted_text() << "\n";
      Close(clientfd);
      return 1;
    } else if (response.get_message_type() != MessageType::OK) {
      std::cerr << "Error: bad server response\n";
      Close(clientfd);
      return 1;
    }

    // send message for TOP
    Message top_msg(MessageType::TOP);
    MessageSerialization::encode(top_msg, encoded_message);
    rio_writen(clientfd, encoded_message.c_str(), encoded_message.length());

    // read from server
    length = rio_readlineb(&rio, buf, Message::MAX_ENCODED_LEN);
    if (length  <= 0) {
      std::cerr << "Error: could not read from server\n";
      Close(clientfd);
      return 1;
    }

    MessageSerialization::decode(std::string(buf, length), response);
    if (response.get_message_type() == MessageType::ERROR || response.get_message_type() == MessageType::FAILED) {
      std::cerr << "Error: " << response.get_quoted_text() << "\n";
      Close(clientfd);
      return 1;
    } else if (response.get_message_type() == MessageType::DATA) {
      std::cout << response.get_value() << "\n";
    } else {
      std::cerr << "Error: bad server response\n";
      Close(clientfd);
      return 1;
    }

    // send message for BYE 
    Message bye_msg(MessageType::BYE);
    MessageSerialization::encode(bye_msg, encoded_message);
    rio_writen(clientfd, encoded_message.c_str(), encoded_message.length());

    // read from server 
    length = rio_readlineb(&rio, buf, Message::MAX_ENCODED_LEN);
    if (length <= 0) {
      std::cerr << "Error: could not read from the server\n";
      Close(clientfd);
      return 1;
    }

    MessageSerialization::decode(std::string(buf, length), response);
    if (response.get_message_type() != MessageType::OK) {
      std::cerr << "Error: bad server response\n";
      Close(clientfd);
      return 1;
    }

    Close(clientfd);
    return 0;

  } catch (InvalidMessage &ex) {
    std::cerr << "Error: " << ex.what() << "\n";
    Close(clientfd);
    return 1;
  } catch (...) {
    std::cerr << "Error: unexpected error\n";
    Close(clientfd);
    return 1;
  }
}
