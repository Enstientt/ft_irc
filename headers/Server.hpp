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
public:
    Server() {
        // Create a socket
        serverSocket = socket(AF_INET, SOCK_STREAM, 0);
      if (serverSocket == -1) {
            std::cout << "Error in creating the socket" << std::endl;
            exit(EXIT_FAILURE);
        }
        //set socket opt
        int opt = 1;
        if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))==-1)
        {
            perror("setsockopt");
            exit(EXIT_FAILURE);
        }
        //non-block fd
        if (fcntl(serverSocket, F_SETFD, O_NONBLOCK) ==-1)
        {
            perror("fcntl");
            exit(EXIT_FAILURE);
        }
        // Bind the socket to a specific IP and port
        sockaddr_in sock_add;
        sock_add.sin_family = AF_INET;
        sock_add.sin_addr.s_addr = INADDR_ANY;
        sock_add.sin_port = htons(PORT);
        if (bind(serverSocket, (sockaddr *)&sock_add, sizeof(sockaddr)) == -1) {
            std::cout << "Error in binding" << std::endl;
            exit(EXIT_FAILURE);
        }

        // Listen for incoming connections
        if (listen(serverSocket, MAX_CLIENTS) == -1) {
            std::cout << "Error while listening" << std::endl;
            exit(EXIT_FAILURE);
        }

        // Initialize poll structure
        fds[0].fd = serverSocket;
        fds[0].events = POLLIN;
        for (int i = 1; i < MAX_CLIENTS + 1; ++i) {
            fds[i].fd = -1;  // Initialize with an invalid value
        }
    }
    

    ~Server() {
        close(serverSocket);
    }

    void run() {
        std::cout << "Server listening on port " << PORT << "..." << std::endl;

        while (true) {
            int ret = poll(fds, MAX_CLIENTS + 1, -1);
            if (ret == -1) {
                perror("poll");
                exit(EXIT_FAILURE);
            }

            // Check for incoming connection requests
            if (fds[0].revents & POLLIN) {
                acceptConnection();
            }

            // Check for data to read and disconnects on existing client sockets
            for (int i = 1; i < MAX_CLIENTS + 1; ++i) {
                if (fds[i].revents & (POLLIN | POLLHUP | POLLERR)) {
                    handleClient(i);
                }
            }
        }
    }

    void pass(std::string password, std::string command, Client &client)
{
    if (client.auth() == false && client.get_pwd().empty())
    {
        if (password.empty())
        {
            send(client.getSocket(), ERR_NEEDMOREPARAMS(client.get_nickname(), "PASS").c_str(), ERR_NEEDMOREPARAMS(client.get_nickname(), "PASS").length(), 0);
        }
        else if (password != "pass")
            send(client.getSocket(), ERR_PASSWDMISMATCH(client.get_nickname()).c_str(), ERR_PASSWDMISMATCH(client.get_nickname()).length(), 0);
        else
        {
            client.set_pwd(password);
        }
    }
}

bool nick_already_exist(std::string nick)
{
    std::vector<Client>::iterator it = _clients.begin();
    if (it == _clients.end())
        return false;
    else{
        for (; it!=_clients.end() ;it++)
        {
            if (it->get_nickname() == nick)
            {
                std::string message = ": 433 "+ nick + " :Nickname is already in use.\r\n";
                send(it->getSocket(), message.c_str(), message.length(), 0);
                return true;
            }
        }
    }
    return false;
}

void nick(std::string nick , Client &client, int flag )
{
    //if not authenticate just processd with authentication
    if (!client.get_pwd().empty())
    {
        if (client.auth() == false && nick_already_exist(nick) == false)
        {
            client.set_nickName(nick);
        }
    }
    //changing the nickname
    else if ( client.auth() == true)
    {
        
    }
}

void user(std::string user, Client &client)
{
    std::cout<<user;
}
void execute_command(Client &client)
    {

        std::string command = client.getMessage();
        std::string cmd;
        std::string value;
        std::istringstream iss(command);
        iss >> cmd >> value ;

    if (cmd == "PASS")
        pass(value, command, client);
    else if (cmd == "NICK")
        nick(value, client, 0);
    else if (cmd == "USER")
        user(value , client);
    }

private:
    int serverSocket;
    std::vector<Client> _clients;
    struct pollfd fds[MAX_CLIENTS + 1];


    void acceptConnection() {
        sockaddr_in client_add;
        socklen_t client_len = sizeof(client_add);
        int newSocket = accept(serverSocket, (sockaddr *)&client_add, &client_len);
        if (newSocket == -1) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        int i;
        for (i = 1; i < MAX_CLIENTS + 1; ++i) {
            if (fds[i].fd == -1) {
                fds[i].fd = newSocket;
                fds[i].events = POLLIN | POLLHUP | POLLERR;
                std::cout << "Client " << i << " connected from " << inet_ntoa(client_add.sin_addr)
                          << ":" << ntohs(client_add.sin_port) << std::endl;
                break;
            }
        }

        if (i == MAX_CLIENTS + 1) {
            // No available slot for the new client
            std::cout << "Connection limit reached. Rejecting new connection." << std::endl;
            close(newSocket);
        }
        Client newClient(newSocket, client_add);
        _clients.push_back(newClient);
    }

    void handleClient(int index) {
        std::vector<Client>::iterator it;
        char buffer[1024];
        bzero(buffer, 1024);
        // Attempt to receive 1 byte from the client
        int bytesRead = recv(fds[index].fd, buffer, sizeof(buffer), MSG_PEEK);
        if (bytesRead > 0)
        {
            std::string str(buffer);
            buffer[bytesRead] = '\0';
            recv(fds[index].fd, buffer, bytesRead, 0);
            it = _clients.begin();
            if (it != _clients.end())
            {
                for ( ; it != _clients.end(); it++)
                {
                    if (it->getSocket() == fds[index].fd)
                    {
                        it->setMessage(str);
                        execute_command(*it);
                    }
                }
                
            }
            //the logic here
            std::cout << "client "<< index<< " :"<< str;
        }
        else if (bytesRead == 0 || bytesRead == -1) {
            // Client has closed the connection
            std::cout << "Client " << index << " disconnected." << std::endl;
            close(fds[index].fd);
            fds[index].fd = -1;
        }
        else
            perror("recv");
    }
};

#endif
