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
private:
    int socket;
    sockaddr_in address;
    std::string message;
    std::string nickName;
    std::string realname;
    std::string hostName;
    std::string userName;
    bool is_authenticate;
    std::string pwd;
public:
    Client(int socket, const sockaddr_in& address);
    int getSocket() const;
    const sockaddr_in& getAddress() const;
    void setMessage(std::string message);
    const std::string getMessage();
    bool auth();
    void setAuth(bool at);
    int getSocket();
    void set_nickName(std::string nick);
    std::string get_nickname();
    void set_user(std::string realName);
    std::string get_user();
    std::string get_pwd();
    void set_pwd(std::string pwd);
};

#endif