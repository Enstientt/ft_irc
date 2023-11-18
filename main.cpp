#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <poll.h>
#include <fcntl.h>
#include <vector>
#define PORT 6666
#define MAX_CLIENTS 10

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

    // Add more member functions or data members as needed

private:
    int socket;
    sockaddr_in address;
    // Add more data members as needed
};

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
        _clients.emplace_back(newSocket, client_add);
    }

    void handleClient(int index) {
        char buffer[1024];
        // Attempt to receive 1 byte from the client
        int bytesRead = recv(fds[index].fd, buffer, sizeof(buffer), MSG_PEEK);
        if (bytesRead > 0)
        {
            buffer[bytesRead] = '\0';
            recv(fds[index].fd, buffer, bytesRead, 0);
            std::string str(buffer);
            //the logic here
            std::cout << "client "<< index<< " :"<< str;
        }
        else if (bytesRead == 0 || (bytesRead == -1 && (errno == EPIPE || errno == ECONNRESET))) {
            // Client has closed the connection
            std::cout << "Client " << index << " disconnected." << std::endl;
            close(fds[index].fd);
            fds[index].fd = -1;
        }
        else
            perror("recv");

    }
};

int main() {
    // Create an instance of the Server class
    Server ircServer;

    // Run the server
    ircServer.run();

    return 0;
}

