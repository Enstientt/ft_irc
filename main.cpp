#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <poll.h>
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
    }

    void handleClient(int index) {
        char buffer[1];
        // Attempt to receive 1 byte from the client
        int bytesRead = recv(fds[index].fd, buffer, sizeof(buffer), MSG_PEEK);
        if (bytesRead == 0 || (bytesRead == -1 && (errno == EPIPE || errno == ECONNRESET))) {
            // Client has closed the connection
            std::cout << "Client " << index << " disconnected." << std::endl;
            close(fds[index].fd);
            fds[index].fd = -1;
        }
    }
};

int main() {
    // Create an instance of the Server class
    Server ircServer;

    // Run the server
    ircServer.run();

    return 0;
}

