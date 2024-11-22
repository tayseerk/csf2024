#include <iostream>
#include <string>
#include "csapp.h"
#include "message.h"
#include "message_serialization.h"
#include "exceptions.h"

int main(int argc, char **argv) {
  if ( argc != 6 && (argc != 7 || std::string(argv[1]) != "-t") ) {
    std::cerr << "Usage: ./incr_value [-t] <hostname> <port> <username> <table> <key>\n";
    std::cerr << "Options:\n";
    std::cerr << "  -t      execute the increment as a transaction\n";
    return 1;
  }

  int count = 1;

  bool use_transaction = false;
  if ( argc == 7 ) {
    use_transaction = true;
    count = 2;
  }

  std::string hostname = argv[count++];
  std::string port = argv[count++];
  std::string username = argv[count++];
  std::string table = argv[count++];
  std::string key = argv[count++];

  // TODO: implement
  
  int clientfd;
  rio_t rio;

  // open the server
  clientfd = open_clientfd(hostname.c_str(), port.c_str());
  if (clientfd < 0) {
    std::cerr << "Error: can't establish server connection\n";
    return 1;
  }
  rio_readinitb(&rio, clientfd);

  // send the message for LOGIN
  try {
    Message login_msg(MessageType::LOGIN, {username});
    std::string encoded_message;
    MessageSerialization::encode(login_msg, encoded_message);
    rio_writen(clientfd, encoded_message.c_str(), encoded_message.length());

    // read the response from server 
    char buf[Message::MAX_ENCODED_LEN];
    ssize_t length = rio_readlineb(&rio, buf, Message::MAX_ENCODED_LEN);
    if (length <= 0) {
      std::cerr << "Error: message could not be read from the server\n";
      Close(clientfd);
      return 1;
    }

    // decode the response from server 
    Message response;
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
    
    // send the begin message
    if (use_transaction) {
      Message begin_msg(MessageType::BEGIN);
      MessageSerialization::encode(begin_msg, encoded_message);
      rio_writen(clientfd, encoded_message.c_str(), encoded_message.length());

      // read the response
      length = rio_readlineb(&rio, buf, Message::MAX_ENCODED_LEN);
      if (length <= 0) {
        std::cerr << "Error: couldn't read response from server\n";
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
    }

    // send message for GET
    Message get_msg(MessageType::GET, {table, key});
    MessageSerialization::encode(get_msg, encoded_msg);
    rio_writen(clientfd, encoded_msg.c_str(), encoded_msg.length());

    // read the response from the server 
    length = rio_readlineb(&rio, buf, Message::MAX_ENCODED_LEN);
    if (length <= 0) {
      std::cerr << "Error: could not read response from server\n";
      Close(clientfd);
      return 1;
    }

    MessageSerialization::decode(std::string(buf, length), response);

    if (response.get_message_type() == MessageType::ERROR ||
        response.get_message_type() == MessageType::FAILED) {
      std::cerr << "Error: " << response.get_quoted_text()
                << "\n";
      Close(clientfd);
      return 1;
    } else if (response.get_message_type()
               != MessageType::OK) {
      std::cerr << "Error: Unexpected response from server\n";
      Close(clientfd);
      return 1;
    }

    // Send PUSH 1 message
    Message push_msg(MessageType::PUSH, {"1"});
    MessageSerialization::encode(push_msg, encoded_message);
    rio_writen(clientfd, encoded_message.c_str(), encoded_message.length());

    // Read response
    length = rio_readlineb(&rio, buf, Message::MAX_ENCODED_LEN);
    if (length <= 0) {
      std::cerr << "Error: Failed to read from server\n";
      Close(clientfd);
      return 1;
    }

    MessageSerialization::decode(std::string(buf, length), response);

    if (response.get_message_type() == MessageType::ERROR ||
        response.get_message_type() == MessageType::FAILED) {
      std::cerr << "Error: " << response.get_quoted_text()
                << "\n";
      Close(clientfd);
      return 1;
    } else if (response.get_message_type()
               != MessageType::OK) {
      std::cerr << "Error: bad server response\n";
      Close(clientfd);
      return 1;
    }

    // send the message for ADD 
    Message add_msg(MessageType::ADD);
    MessageSerialization::encode(add_msg, encoded_msg);
    rio_writen(clientfd, encoded_msg.c_str(),
               encoded_msg.length());

    // read the response from server 
    length = rio_readlineb(&rio, buf, Message::MAX_ENCODED_LEN);
    if (length <= 0) {
      std::cerr << "Error: Failed to read from server\n";
      Close(clientfd);
      return 1;
    }

    MessageSerialization::decode(std::string(buf, length), response);

    if (response.get_message_type() == MessageType::ERROR ||
        response.get_message_type() == MessageType::FAILED) {
      std::cerr << "Error: " << response.get_quoted_text()
                << "\n";
      Close(clientfd);
      return 1;
    } else if (response.get_message_type()
               != MessageType::OK) {
      std::cerr << "Error: bad server response\n";
      Close(clientfd);
      return 1;
    }

    // send the message for SET  
    Message set_msg(MessageType::SET, {table, key});
    MessageSerialization::encode(set_msg, encoded_message);
    rio_writen(clientfd, encoded_message.c_str(), encoded_message.length());

    // Read response
    n = rio_readlineb(&rio, buf, Message::MAX_ENCODED_LEN);
    if (n <= 0) {
      std::cerr << "Error: Failed to read from server\n";
      Close(clientfd);
      return 1;
    }

    MessageSerialization::decode(std::string(buf, n), response);
    if (response.get_message_type() == MessageType::ERROR || response.get_message_type() == MessageType::FAILED) {
      std::cerr << "Error: " << response.get_quoted_text() << "\n";
      Close(clientfd);
      return 1;
    } else if (response.get_message_type()
               != MessageType::OK) {
      std::cerr << "Error: Unexpected response from server\n";
      Close(clientfd);
      return 1;
    }

    // send message for COMMIT
    if (use_transaction) {
      Message commit_msg(MessageType::COMMIT);
      MessageSerialization::encode(commit_msg, encoded_message);
      rio_writen(clientfd, encoded_message.c_str(), encoded_message.length());

      // Read response
      length = rio_readlineb(&rio, buf, Message::MAX_ENCODED_LEN);
      if (length <= 0) {
        std::cerr << "Error: Failed to read from server\n";
        Close(clientfd);
        return 1;
      }

      MessageSerialization::decode(std::string(buf, length), response);
      if (response.get_message_type() == MessageType::ERROR ||
          response.get_message_type() == MessageType::FAILED) {
        std::cerr << "Error: " << response.get_quoted_text()
                  << "\n";
        Close(clientfd);
        return 1;
      } else if (response.get_message_type()
                 != MessageType::OK) {
        std::cerr << "Error: bad server response\n";
        Close(clientfd);
        return 1;
      }
    }

    // send the message for BYE 
    Message bye_msg(MessageType::BYE);
    MessageSerialization::encode(bye_msg, encoded_message);
    rio_writen(clientfd, encoded_message.c_str(), encoded_message.length());

    // read the response from the server
    length = rio_readlineb(&rio, buf, Message::MAX_ENCODED_LEN);
    if (length <= 0) {
      std::cerr << "Error: could not read response from server\n";
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

