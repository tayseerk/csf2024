#include <iostream>
#include <string>
#include "csapp.h"
#include "message.h"
#include "message_serialization.h"
#include "exceptions.h"

int main(int argc, char **argv)
{
  if (argc != 7) {
    std::cerr << "Usage: ./set_value <hostname> <port> <username> <table> <key> <value>\n";
    return 1;
  }

  std::string hostname = argv[1];
  std::string port = argv[2];
  std::string username = argv[3];
  std::string table = argv[4];
  std::string key = argv[5];
  std::string value = argv[6];

  // TODO: implement
    int clientfd;
  rio_t rio;

  clientfd = open_clientfd(hostname.c_str(), port.c_str());
  if (clientfd < 0) {
    std::cerr << "Error: Unable to connect to server\n";
    return 1;
  }

  rio_readinitb(&rio, clientfd);

  try {
    // Send LOGIN message
    Message login_msg(MessageType::LOGIN, {username});
    std::string encoded_msg;
    MessageSerialization::encode(login_msg, encoded_msg);
    rio_writen(clientfd, encoded_msg.c_str(), encoded_msg.length());

    // Read response
    char buf[Message::MAX_ENCODED_LEN];
    ssize_t n = rio_readlineb(&rio, buf,
                              Message::MAX_ENCODED_LEN);
    if (n <= 0) {
      std::cerr << "Error: Failed to read from server\n";
      Close(clientfd);
      return 1;
    }

    Message response;
    MessageSerialization::decode(std::string(buf, n), response);

    if (response.get_message_type() == MessageType::ERROR || response.get_message_type() == MessageType::FAILED) {
      std::cerr << "Error: " << response.get_quoted_text() << "\n";
      Close(clientfd);
      return 1;
    } else if (response.get_message_type() != MessageType::OK) {
      std::cerr << "Error: Unexpected response from server\n";
      Close(clientfd);
      return 1;
    }

    // Send PUSH message with value
    Message push_msg(MessageType::PUSH, {value});
    MessageSerialization::encode(push_msg, encoded_msg);
    rio_writen(clientfd, encoded_msg.c_str(), encoded_msg.length());

    // Read response
    n = rio_readlineb(&rio, buf, Message::MAX_ENCODED_LEN);
    if (n <= 0) {
      std::cerr << "Error: could not read response from server\n";
      Close(clientfd);
      return 1;
    }

    MessageSerialization::decode(std::string(buf, n), response);

    if (response.get_message_type() == MessageType::ERROR || response.get_message_type() == MessageType::FAILED) {
      std::cerr << "Error: " << response.get_quoted_text() << "\n";
      Close(clientfd);
      return 1;
    } else if (response.get_message_type() != MessageType::OK) {
      std::cerr << "Error: bad server response\n";
      Close(clientfd);
      return 1;
    }

    // Send SET message
    Message set_msg(MessageType::SET, {table, key});
    MessageSerialization::encode(set_msg, encoded_msg);
    rio_writen(clientfd, encoded_msg.c_str(), encoded_msg.length());

    // Read response
    n = rio_readlineb(&rio, buf, Message::MAX_ENCODED_LEN);
    if (n <= 0) {
      std::cerr << "Error: could not read response from server\n";
      Close(clientfd);
      return 1;
    }

    MessageSerialization::decode(std::string(buf, n), response);

    if (response.get_message_type() == MessageType::ERROR || response.get_message_type() == MessageType::FAILED) {
      std::cerr << "Error: " << response.get_quoted_text() << "\n";
      Close(clientfd);
      return 1;
    } else if (response.get_message_type() != MessageType::OK) {
      std::cerr << "Error: bad server response\n";
      Close(clientfd);
      return 1;
    }

    // Send BYE message
    Message bye_msg(MessageType::BYE);
    MessageSerialization::encode(bye_msg, encoded_msg);
    rio_writen(clientfd, encoded_msg.c_str(), encoded_msg.length());

    // Read response
    n = rio_readlineb(&rio, buf, Message::MAX_ENCODED_LEN);
    if (n <= 0) {
      std::cerr << "Error: could not read response from server\n";
      Close(clientfd);
      return 1;
    }

    MessageSerialization::decode(std::string(buf, n), response);

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

