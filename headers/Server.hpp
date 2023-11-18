#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <vector>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

class Server {
    
private:
    const int MAX_CLIENTS = 10;
    int serverSocket;
    int port;

    int acceptConnection() {
        sockaddr_in clientAddr;
        socklen_t clientLen = sizeof(clientAddr);

        int newSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientLen);
        if (newSocket == -1) {
            perror("Error accepting connection");
            exit(EXIT_FAILURE);
        }

        std::cout << "New client connected from " << inet_ntoa(clientAddr.sin_addr)
                  << ":" << ntohs(clientAddr.sin_port) << std::endl;

        return newSocket;
    }

    void handleClient(int clientSocket) {
        // Implement your logic for handling the connected client
        // For an IRC server, you'd process IRC commands, manage channels, etc.
        // Example: recv, send, process commands, etc.

        // Close the client socket when done
        close(clientSocket);
        std::cout << "Client disconnected." << std::endl;
    }
public:
    Server(int port) : port(port) {
        // Create a socket
        serverSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (serverSocket == -1) {
            perror("Error creating socket");
            exit(EXIT_FAILURE);
        }

        // Bind the socket to a specific IP and port
        sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = INADDR_ANY;
        serverAddr.sin_port = htons(port);

        if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
            perror("Error binding socket");
            exit(EXIT_FAILURE);
        }

        // Listen for incoming connections
        if (listen(serverSocket, MAX_CLIENTS) == -1) {
            perror("Error listening on socket");
            exit(EXIT_FAILURE);
        }
    }

    ~Server() {
        close(serverSocket);
    }

    void run() {
        std::cout << "Server listening on port " << port << "..." << std::endl;

        while (true) {
            int newSocket = acceptConnection();
            handleClient(newSocket);
        }
    }
};

#endif

// int main() {
//     // Create an instance of the Server class
//     Server ircServer(6666);

//     // Run the server
//     ircServer.run();

//     return 0;
// }
