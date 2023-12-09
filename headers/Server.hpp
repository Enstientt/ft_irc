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

#define MAX_CLIENTS 100

class Channel;
class Server {
private:
    std::string port;
    std::string server_name;
    std::string password;
    std::string host_ip;
    int serverSocket;
    std::vector<Client> _clients;
    std::vector<Channel> _channels;
    std::vector<Client> _server_operators;
    struct pollfd fds[MAX_CLIENTS + 1];
    void acceptConnection();
    void handleClient(int index);
    Channel channel_note_found;
    Client client_note_found;
public:
    /********** Server set up **************/
    Server() ;
    Server(std::string port, std::string password);
    void run();
    ~Server();
    /************** commands ***************/
    void execute_command(Client &client);
    void pass(std::string password, Client &client);
    void nick(std::string nick , Client &client);
    void user(std::string nick, std::string mode, std::string hostName, std::string realName , Client &client);
    void privmsg(Client &client, std::string command);
    // void ultraJoin(Client &client , std::string &targets, std::string &keys);
    void join(Client &client, std::string target, std::string &password);
    void handle_mode(Client &client, std::string &command);
    void invite(Client &client, std::string nickname, std::string channel);
    void kick(Client &client, std::string channel, std::string user, std::string message);
    void topic(Client &client,std::string channel, std::string topic);
    void handle_bote(Client &client);
    /************** Tools ****************/
    Client & find_client(std::string nick);
    Channel & find_channel(std::string chan);
    void cleanServer();
    void clearChannels(Client & client);
    std::string filterString(const std::string &str);
    bool checkForma(const std::string &username, std::string &mode, std::string hostName, std::string realName);
    bool stringExistsInArray(std::string target, std::string array[], int arraySize);
    bool nick_already_exist(std::string nick);
    bool isValidNick(const std::string& nick);
    int isMultipleWords(std::string str, char c);   
    void welcomeClient(Client & client);
    void handleMulti(Client &client);
};
#endif
