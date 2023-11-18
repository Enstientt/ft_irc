#ifndef CLIENT_HPP
#define CLIENT_HPP
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <poll.h>
#include <fcntl.h>
#include <vector>
#include "head.hpp"


class Client {
public:
    Client(int socket, const sockaddr_in& address) : socket(socket), address(address) {
        // Perform any additional initialization for a new client
    }
    int getSocket() const {
        return socket;
    }

    const sockaddr_in& getAddress() const {
        return address;
    }

    void setMessage(std::string message){
        this->message = message;
    }
    const std::string getMessage(){
        return this->message;
    }

    // Add more member functions or data members as needed

private:
    int socket;
    sockaddr_in address;
    std::string message;
    // Add more data members as needed
};

#endif