#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <poll.h>
#define PORT 6666
#define MAX_CLIENTS 10

int main() {
    int ret = 0;

    // Create a socket
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cout << "Error in creating the socket" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Bind the socket to a specific IP and port
    sockaddr_in sock_add, client_add;
    socklen_t client_len = sizeof(client_add);
    sock_add.sin_family = AF_INET;
    sock_add.sin_addr.s_addr = INADDR_ANY;
    sock_add.sin_port = htons(PORT);
    ret = bind(serverSocket, (sockaddr *)&sock_add, sizeof(sockaddr));
    if (ret < 0) {
        std::cout << "Error in binding" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    ret = listen(serverSocket, MAX_CLIENTS);
    if (ret == -1) {
        std::cout << "Error while listening" << std::endl;
        exit(EXIT_FAILURE);
    }

    struct pollfd fds[MAX_CLIENTS + 1];
    int nfds = 1;
    fds[0].fd = serverSocket;
    fds[0].events = POLLIN;

    int clientSockets[MAX_CLIENTS];
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        clientSockets[i] = -1;  // Initialize with an invalid value
    }

    while (1) {
        ret = poll(fds, nfds, -1);
        if (ret == -1) {
            perror("poll");
            exit(EXIT_FAILURE);
        }

        // Check for incoming connection requests
        if (fds[0].revents & POLLIN) {
            int newSocket = accept(serverSocket, (sockaddr *)&client_add, &client_len);
            if (newSocket == -1) {
                perror("accept");
                exit(EXIT_FAILURE);
            }

            int i;
            for (i = 0; i < MAX_CLIENTS; ++i) {
                if (clientSockets[i] == -1) {
                    clientSockets[i] = newSocket;
                    std::cout << "Client " << i + 1 << " connected from " << inet_ntoa(client_add.sin_addr)
                              << ":" << ntohs(client_add.sin_port) << std::endl;
                    break;
                }
            }

            if (i == MAX_CLIENTS) {
                // No available slot for the new client
                std::cout << "Connection limit reached. Rejecting new connection." << std::endl;
                close(newSocket);
            } else {
                // Add the new client socket to the poll array
                fds[nfds].fd = newSocket;
                fds[nfds].events = POLLIN | POLLHUP | POLLERR;
                nfds++;
            }
        }

        // Check for data to read and disconnects on existing client sockets
        for (int i = 1; i < nfds; ++i) {
            if (fds[i].revents & (POLLIN | POLLHUP | POLLERR)) {
                char buffer[1];
                // Attempt to receive 1 byte from the client
                int bytesRead = recv(fds[i].fd, buffer, sizeof(buffer), MSG_PEEK);
                if (bytesRead == 0 || (bytesRead == -1 && (errno == EPIPE || errno == ECONNRESET))) {
                    // Client has closed the connection
                    std::cout << "Client " << i << " disconnected." << std::endl;
                    close(fds[i].fd);
                    fds[i].fd = -1;
                }
            }
        }
    }

    close(serverSocket);

    return 0;
}
