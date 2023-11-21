#include "headers/Server.hpp"

int main(int arc, char** argv){
    // Create an instance of the Server class
    if (arc == 3)
    {
        std::string port = argv[1];
        std::string password = argv[2];
        int port_1 = atoi(argv[1]);
        if(port.find_first_not_of("0123456789") != std::string::npos || (port_1 < 1024 || port_1 > 65535))
        {
            std::cout << "the port must be an intger betwen 1024 to 65535."<<std::endl;
            return (1);
        }
        Server ircServer(port, password);
        // Run the server
        ircServer.run();
    }

    return 0;
}

