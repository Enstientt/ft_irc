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
    bool is_registered;
    std::vector<std::string> _channels;
    std::string pwd;
public:
    Client();
    Client(int socket, const sockaddr_in& address);
    int getSocket() const;
    bool get_reg() const;
    void set_reg(bool reg);
    void set_username(std::string username);
    std::string get_realname() const;
    std::string get_username() const;
    void set_realname(std::string realName);
    const sockaddr_in& getAddress() const;
    void setMessage(std::string message);
    const std::string getMessage();
    bool auth();
    void setAuth(bool at);
    int getSocket();
    void set_nickName(std::string nick);
    std::string get_nickname();
    std::string get_pwd();
    void set_pwd(std::string pwd);
    void addtosetMessage(std::string message);
    void set_channels(std::string channel);
    std::vector<std::string > &get_channels();
};
#endif