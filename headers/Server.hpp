#ifndef SERVER_HPP
#define SERVER_HPP

#include "Client.hpp"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <poll.h>
#include <fcntl.h>
#include <vector>
#include "head.hpp"
#include <sstream>
#define PORT 6666
#define MAX_CLIENTS 10

class Server {
private:
    std::string port;
    std::string password;
    int serverSocket;
    std::vector<Client> _clients;
    struct pollfd fds[MAX_CLIENTS + 1];
    void acceptConnection();
    void handleClient(int index);
public:
    Server() ;
    Server(std::string port, std::string password);
    ~Server();
    void run();
    void pass(std::string password, std::string command, Client &client);
    bool nick_already_exist(std::string nick, Client &client);
    void nick(std::string nick , Client &client, int flag );
    void user(std::string nick, std::string mode, std::string hostName, std::string realName , Client &client);
    void privmsg(Client &client, std::string command);
    void execute_command(Client &client);
};
#endif
