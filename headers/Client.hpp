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
    bool auth()
    {
        return is_authenticate;
    }
    void setAuth(bool at)
    {
        this->is_authenticate = at;
    }
    int getSocket()
    {
        return this->socket;
    }
    void set_nickName(std::string nick)
    {
        this->nickName = nick;
    }
    std::string get_nickname()
    {
        return this->nickName;
    }
    void set_user(std::string user)
    {
        this->userName = user;
    }
    std::string get_user()
    {
        return this->userName;
    }
    std::string get_pwd()
    {
        return this->pwd;
    }

    void set_pwd(std::string pwd)
    {
        this->pwd = pwd;
    }
    // Add more member functions or data members as needed

private:
    int socket;
    sockaddr_in address;
    std::string message;
    std::string nickName;
    std::string userName;
    bool is_authenticate;
    std::string pwd;
    // Add more data members as needed
};

#endif