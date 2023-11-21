#include "../headers/Server.hpp"

Server::Server(){
}

Server:: Server(std::string port, std::string password): port(port) , password(password){
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
		int port_1 = atoi(port.c_str());
		sock_add.sin_family = AF_INET;
		sock_add.sin_addr.s_addr = INADDR_ANY;
		sock_add.sin_port = htons(port_1);
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

Server::~Server() {
	close(serverSocket);
	}
void Server::run() {
		std::cout << "Server listening on port " << port << "..." << std::endl;

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
	};
void Server::pass(std::string password, std::string command, Client &client)
{
	if (client.auth() == false && client.get_pwd().empty())
	{
		if (password.empty())
		{
			send(client.getSocket(), ERR_NEEDMOREPARAMS(client.get_nickname(), "PASS").c_str(), ERR_NEEDMOREPARAMS(client.get_nickname(), "PASS").length(), 0);
		}
		else if (password != this->password)
			send(client.getSocket(), ERR_PASSWDMISMATCH(client.get_nickname()).c_str(), ERR_PASSWDMISMATCH(client.get_nickname()).length(), 0);
		else
		{
			client.set_pwd(password);
		}
	}
};

void Server::nick(std::string nick , Client &client, int flag )
{
	std::string message;
	//if not authenticate just processd with authentication
	if (!client.get_pwd().empty() && client.auth() == false)
	{
		if (nick_already_exist(nick, client) == false)
		{
			client.set_nickName(nick);
		}
		else
		{
			message = ": 433 "+ nick + " :Nickname is already in use.\r\n";
			send(client.getSocket(), message.c_str(), message.length(), 0);
		}
	}
	//changing the nickname
	else if ( client.auth() == true)
	{
		if (nick_already_exist(nick, client) == false)
		{
			message = ": nickname "+ client.get_nickname() + " change to  " + nick +"\r\n";
			client.set_nickName(nick);
			send(client.getSocket(), message.c_str(), message.length(), 0);
		}
		else{
			message = ": 433 "+ nick + " :Nickname is already in use.\r\n";
			send(client.getSocket(), message.c_str(), message.length(), 0);
		}
	}
}
void Server::user(std::string nick, std::string mode, std::string hostName, std::string realName , Client &client)
{
	if (!client.get_pwd().empty() && !client.get_nickname().empty() && client.auth() == false)
	{

		if (nick != client.get_nickname())
		{
			std::string message = "error :the  nickName in USER command mismatch the correct one\r\n";
			send(client.getSocket(), message.c_str(), message.length(), 0);
			return;
		}
		else
		{
			std::string client_add = inet_ntoa(client.getAddress().sin_addr);
			std::string welcomeMessage = ": 001 " + nick + " :Welcome " + nick + " to the Internet Relay Chat " + user_forma(nick, realName,client_add)+ "\r\n";
			client.set_user(realName);
			client.setAuth(true);
			send(client.getSocket(), welcomeMessage.c_str(), welcomeMessage.length(), 0);
		}
	}
}
void Server::privmsg(Client &client, std::string command){
	std::vector<Client>::iterator it = _clients.begin();
	std::istringstream iss(command);
	std::string cmd , target, msg;
	std::string message;
	iss>>cmd>>target>>std::ws;
	std::getline(iss, msg);
	//handle channels
	if (target[0] =='#')
	{
		std::cout << "it s for channel " << target<<std::endl;
	}
	//handle users
	else
	{
		for (; it!=_clients.end();it++)
		{
			if (target == it->get_nickname())
				break;
		}
		if (it !=_clients.end())
		{
			message = user_forma(client.get_nickname(), client.get_user() , inet_ntoa(client.getAddress().sin_addr));
			message += " " + msg + "\r\n";
			send(it->getSocket(), message.c_str(), message.length(), 0);
		}
		else
		{
			message =":401 " + client.get_nickname() + " " + target + " :No such nick\r\n";
			send(client.getSocket(), message.c_str(), message.length(), 0);
		}       
	}
};

void Server::execute_command(Client &client)
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
		{
			std::string mode;
			std::string hostName;
			std::string realName;
			iss>>mode>>hostName>>realName;
			user(value, mode, hostName, realName , client);
		}
		else if (cmd == "PRIVMSG")
		{
			privmsg(client,command);
		}
	}

void Server::acceptConnection() {
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
void Server::handleClient(int index) {
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
            std::vector<Client>::iterator it = _clients.begin();
            for(;it!=_clients.end();it++)
            {
                if (it->getSocket() == fds[index].fd)
                {
                    _clients.erase(it);
                    break;
                }
            }
            close(fds[index].fd);
            fds[index].fd = -1;
        }
        else
            perror("recv");
    }
//tools
bool Server::nick_already_exist(std::string nick, Client &client)
{
	std::vector<Client>::iterator it = _clients.begin();
	if (it == _clients.end())
		return false;
	else{
		for (; it!=_clients.end() ;it++)
		{
			if (it->get_nickname() == nick)
			{
				return true;
			}
		}
	}
	return false;
};


