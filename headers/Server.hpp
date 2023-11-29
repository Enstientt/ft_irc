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
#include "Channel.hpp"
#include <algorithm>

#define MAX_CLIENTS 10

class Channel;
class Server {
private:
    std::string port;
    std::string server_name;
    std::string password;
    int serverSocket;
    std::vector<Client> _clients;
    std::vector<Channel> _channels;
    struct pollfd fds[MAX_CLIENTS + 1];
    void acceptConnection();
    void handleClient(int index);
    Channel channel_note_found;
    Client client_note_found;
public:
    Server() ;
    Server(std::string port, std::string password);
    ~Server();
    Client & find_client(std::string nick);
    Channel & find_channel(std::string chan);
    void run();
    void cleanServer();
    std::string filterString(const std::string &str);
    void pass(std::string password, std::string command, Client &client);
    bool nick_already_exist(std::string nick, Client &client);
    bool isValidNick(const std::string& nick);
    bool isMultipleWords(std::string str);
    void nick(std::string nick , Client &client, int flag );
    void user(std::string nick, std::string mode, std::string hostName, std::string realName , Client &client);
    void privmsg(Client &client, std::string command);
    void execute_command(Client &client);
    void join(Client &client, std::string target, std::string &password);
    void handle_mode(Client &client, std::string &command);
    void invite(Client &client, std::string nickname, std::string channel);
    void kick(Client &client, std::string channel, std::string user, std::string message);
    void topic(Client &client,std::string channel, std::string topic);
};
#endif
