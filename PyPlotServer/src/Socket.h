#ifndef __PY_SOCKET_H__
#define __PY_SOCKET_H__

#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#include <string>
#include <iostream>

namespace NatPlotLib {

  //! \brief The integer signal type.
  typedef uint64_t sig; 


  class Socket {
  public:
    Socket(unsigned port) : port(port) {
      create();
    };

    //! \brief Create a socket, return the success or failure of this operation.
    bool create() {
      socket_id = socket(AF_INET, SOCK_STREAM, 0);
      if (socket_id < 0) return false;
      return true;
    }

    //! \brief Connect to another socket.
    bool connect() {
      serv_addr.sin_family = AF_INET;
      serv_addr.sin_port = htons(port);

      // Convert IPv4 and IPv6 addresses from text to binary form
      if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) {
      	printf("invalid_address_error()");
      	return false;
      }

      if (::connect(socket_id, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
      	printf("\nConnection Failed \n");
      	return false;
      }

      return true;
    }

    template<typename T>
    void send(const T& data) {
      ::send(socket_id, static_cast<const void*>(&data), sizeof(T), 0);
    }

    template<>
    void send(const std::string& str) {
      const sig size = str.size();
      ::send(socket_id, static_cast<const void*>(&size), sizeof(sig), 0);
      ::send(socket_id, static_cast<const void*>(&str[0]), size * sizeof(char), 0);
    }

    template<typename T>
    void send(const T* data, size_t size) {
      ::send(socket_id, data, size * sizeof(T), 0);
    }
    
    void read_string(std::string& buffer) {
      std::cout << "Buffer length: " << buffer.size() << std::endl;
      ::read(socket_id, static_cast<void*>(&buffer[0]), buffer.size() * sizeof(char));
    }

  private:
    unsigned port;
    unsigned socket_id = 0;

    struct sockaddr_in serv_addr;
  };
}
#endif // __PY_SOCKET_H__
