#include "../headers/Server.hpp"

Server::Server(){
	server_name = "Irc-Server";
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
Client &  Server::find_client(std::string nick)
{
	std::vector<Client>::iterator it = _clients.begin();
	if (it!= _clients.end()){
		for(;it!=_clients.end();it++)
		{
			if (nick == it->get_nickname())
			{
				return *it;
			}
		}
	}
	client_note_found.set_nickName("NOT_FOUND");
	return client_note_found;
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
    std::istringstream iss(command);
    std::string cmd , target, msg;
    std::string message;
    iss >> cmd >> target >> std::ws;
    std::getline(iss, msg);

	if (target.empty() || msg.empty())
	{
		message = ERR_NEEDMOREPARAMS(client.get_nickname(), cmd);
		send(client.getSocket(), message.c_str(), message.length(), 0);
	}
    //handle channels
    else if (target[0] =='#')
    {
        std::vector<Channel>::iterator it = _channels.begin();
        for(; it != _channels.end(); ++it)
        {
            if (it->get_name() == target)
                break;
        }
        if (it == _channels.end())
        {
            message = ERR_NOSUCHCHANNEL(client.get_nickname(), target);
			send(client.getSocket(), message.c_str(), message.length(), 0);
        }
        else if(it->in_channel(client))
        {
			message = RPL_PRIVMSG(client.get_nickname(), client.get_user(), it->get_name(), msg);
            it->broadcast_message(client, message, 1);
        }
        else
        {
			message = ERR_NOTONCHANNEL(client.get_nickname(), target);
            send(client.getSocket(), message.c_str(), message.length(),0);
        }
    }
    //handle users
    else
    {
        std::vector<Client>::iterator it = _clients.begin();
        for (; it != _clients.end(); ++it)
        {
            if (it->get_nickname() == target)
                break;
        }
        if (it != _clients.end())
        {
            message = IRC_PRIVMSG_MSG(client.get_nickname(), it->get_nickname(), msg);
            send(it->getSocket(), message.c_str(), message.length(), 0);
        }
        else
        {
            message = ERR_NOSUCHNICK(client.get_nickname(), target);
            send(client.getSocket(), message.c_str(), message.length(), 0);
        }       
    }
};

void Server::handle_mode(Client &client, std::string& command)
{
    std::istringstream iss(command);
    std::string msg, cmd, name, mode, parameter;
    iss >> cmd>>name >> mode >> parameter;

    std::vector<Channel>::iterator it = _channels.begin();
    for (; it != _channels.end(); ++it)
    {
        if (it->get_name() == name && it->is_operator(client))
        {
            if (mode == "+k")
			{
				std::cout<<"pass set "<<std::endl;
				it->set_pass(parameter);
				it->set_rest(true);
			}
            else if (mode == "-k")
			{
				std::cout<<"the pass removed"<<std::endl;
				it->set_pass("");
				it->set_rest(false);
			}
            else if (mode == "+t") it->set_topic_protected(true);
            else if (mode == "-t") it->set_topic_protected(false);
            else if (mode == "+o" || mode =="-o")
			{
				Client & cl = find_client(parameter);
				if (cl.get_nickname()=="NOT_FOUND")
					return;
				if (it->in_channel(cl))
				{	
					if (mode == "+o")
					{
						it->add_operator(cl);
						msg = RPL_MODESET(client.get_nickname(), it->get_name(), mode, cl.get_nickname());
						it->broadcast_message(client,msg,0);
					}
					else if (mode=="-o" && it->is_operator(cl))
					{
						it->remove_operator(cl);
						msg = RPL_MODESET(client.get_nickname(), it->get_name(), mode, cl.get_nickname());
						it->broadcast_message(client,msg,0);
					}
				}
			}
            else if (mode == "+l")
			{
				it->set_limit(std::stoi(parameter));
				it->set_lim_state(true);
				msg = RPL_MODESET(client.get_nickname(), it->get_name(), mode,parameter);
				it->broadcast_message(client, msg, 0);
			}
            else if (mode == "-l")
			{
				std::cout<<"limit removed"<<std::endl;
				it->set_limit(MAX_CLIENTS);
				it->set_lim_state(false);
				msg = RPL_MODESET(client.get_nickname(), it->get_name(), mode,parameter);
				it->broadcast_message(client, msg, 0);
			}
				
            else if (mode == "+i") it->set_invite_only(true);
            else if (mode == "-i") it->set_invite_only(false);
            break;
        }
    }
}
void Server::execute_command(Client &client)
	{

		std::string command = client.getMessage();
		std::string cmd;
		std::string value;
		std::istringstream iss(command);
		iss >> cmd >> value;
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
		else if (cmd == "JOIN")
		{

			std::string pass;
			iss>>pass;
			join(client, value ,pass);
		}
		else if(cmd == "MODE")
			handle_mode(client, command);
		else if (cmd == "INVITE")
		{
			std::string channel;
			iss>>channel;
			invite(client, value, channel);
		}
		else if (cmd == "KICK")
		{
			std::string nickname, message;
			iss>>nickname;
			getline(iss, message);
			kick(client, value, nickname, message);
		}
		else if (cmd == "TOPIC")
		{
			std::string top;
			getline(iss, top);
			topic(client, value, top);
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

void Server::join(Client &client, std::string target, std::string pass)
{
    std::vector<Channel>::iterator it = _channels.begin();
    std::string msg;
	int toggler = 0;

    for(; it!=_channels.end();it++)
    {
        if (it->get_name() == target)
            break;
    }
    //check if the channel exist
    if (it != _channels.end())
    {
        if (it->in_channel(client))
        {
			msg = ERR_USERONCHANNEL(client.get_nickname(), client.get_nickname(), it->get_name());
            send(client.getSocket(), msg.c_str(), msg.length(), 0);
			return;
        }
        if (it->is_invite_only())
        {
			if (!it->is_invited(client))
			{
				msg = ERR_INVITEONLYCHAN(client.get_nickname(), it->get_name());
				send(client.getSocket(), msg.c_str(), msg.length(), 0);
				toggler = 1;
			}
        }
        if (it->get_lim_state())
        {
			if (it->is_full())
			{
				msg = ERR_CHANNELISFULL(client.get_nickname(), it->get_name());
				send(client.getSocket(), msg.c_str(), msg.length(), 0);
				toggler = 1;
			}
        }
        if (it->get_rest())
        {
			if (pass != it->get_pass())
			{
				std::cout<< "this is the channel pass"<<it->get_pass()<<std::endl;
				std::cout<< "this is the pass"<<pass<<std::endl;
				msg =ERR_BADCHANNELKEY(client.get_nickname(), it->get_name());
				send(client.getSocket(), msg.c_str(),msg.length(), 0);
				toggler = 1;
			}
        }
		if (toggler == 0)
		{
			it->add_client_to_channnel(client);
			msg = RPL_JOIN(user_forma(client.get_nickname(),client.get_user(),inet_ntoa(client.getAddress().sin_addr)), client.get_nickname(), it->get_name());
			it->broadcast_message(client, msg, 0);
			msg = IRC_JOIN_MSG(client.get_nickname(), it->get_name(), it->get_list_of_users());
			send(client.getSocket(), msg.c_str(),msg.length(),0);
		}
    }
    else
    {
        // Create the channel and add the client to it
        Channel newChannel(target, pass);
		newChannel.add_client_to_channnel(client);
		newChannel.add_operator(client);
        _channels.push_back(newChannel);
		msg = RPL_JOIN(user_forma(client.get_nickname(),client.get_user(),inet_ntoa(client.getAddress().sin_addr)), client.get_nickname(), target);
		newChannel.broadcast_message(client, msg,0);
		msg = IRC_JOIN_MSG(client.get_nickname(), newChannel.get_name(), newChannel.get_list_of_users());
		send(client.getSocket(), msg.c_str(),msg.length(),0);
    }
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
Channel & Server::find_channel(std::string channel)
{
	std::vector<Channel>::iterator it = _channels.begin();
	if (it != _channels.end())
	{
		for(;it!=_channels.end();it++)
		{
			if (it->get_name()==channel)
			{
				return *it;
			}
		}
	}
	channel_note_found.set_name("NOT_FOUND");
	return channel_note_found;
}
void Server::invite(Client & client, std::string nickname, std::string channel)
{
	std::string msg;
	if (nickname.empty() || channel.empty())
	{
		msg = ERR_NEEDMOREPARAMS(client.get_nickname(), "INVITE");
		send(client.getSocket(), msg.c_str(), msg.length(), 0);
		return;
	}
	Channel &chan = find_channel(channel);
	Client  &cl = find_client(nickname);
	if (cl.get_nickname()=="NOT_FOUND")
	{
		msg = ERR_NOSUCHNICK(client.get_nickname(), nickname);
		send(client.getSocket(), msg.c_str(), msg.length(), 0);
		return;
	}
	if (chan.get_name()=="NOT_FOUND")
	{
		msg = ERR_NOSUCHCHANNEL(client.get_nickname(), channel);
		send(client.getSocket(), msg.c_str(), msg.length(), 0);
		return;
	}
	if (chan.is_operator(client))
	{
		if (chan.in_channel(cl))
		{
			msg = ERR_USERONCHANNEL(client.get_nickname(), nickname, channel);
			send(client.getSocket(), msg.c_str(), msg.length(), 0);
		}
		else
		{
			chan.add_invited(cl);
			msg = RPL_INVITING(server_name, nickname, channel);
			send(client.getSocket(), msg.c_str(), msg.length(), 0);
			msg = RPL_INVITATION(cl.get_nickname(), client.get_nickname(), channel);
			send(cl.getSocket(), msg.c_str(), msg.length(), 0);
		}
	}
	else
	{
		msg = ERR_CHANOPRIVSNEEDED(server_name, client.get_nickname(), channel);
		send(client.getSocket(), msg.c_str(), msg.length(), 0);
	}
}

void Server::kick(Client &client, std::string channel, std::string user, std::string message)
{
	std::string msg;
	if (user.empty() || channel.empty())
	{
		msg = ERR_NEEDMOREPARAMS(client.get_nickname(), "INVITE");
		send(client.getSocket(), msg.c_str(), msg.length(), 0);
		return;
	}
	Channel &chan = find_channel(channel);
	Client  &cl = find_client(user);
	if (chan.get_name()=="NOT_FOUND")
	{
		msg = ERR_NOSUCHCHANNEL(client.get_nickname(), channel);
		send(client.getSocket(), msg.c_str(), msg.length(), 0);
		return;
	}
	if (cl.get_nickname()=="NOT_FOUND")
	{
		msg = ERR_NOSUCHNICK(client.get_nickname(), user);
		send(client.getSocket(), msg.c_str(), msg.length(), 0);
		return;
	}
	if (chan.is_operator(client))
	{
		if (!chan.in_channel(cl))
		{
			msg = ERR_USERNOTINCHANNEL(server_name, client.get_nickname(), user, channel);
			send(client.getSocket(), msg.c_str(), msg.length(), 0);
		}
		else
		{
			msg = RPL_KICKED(server_name, user, channel, client.get_nickname(), message);
			chan.broadcast_message(client,msg ,0);
			chan.remove_client_from_channel(cl);
			chan.is_operator(cl)? chan.remove_operator(cl) : (void)1;
		}
	}
	else
	{
		msg = ERR_CHANOPRIVSNEEDED(server_name, client.get_nickname(), channel);
		send(client.getSocket(), msg.c_str(), msg.length(), 0);
	}
}

void Server::topic(Client &client, std::string channel, std::string topic)
{
		std::string msg;
	if (channel.empty())
	{
		msg = ERR_NEEDMOREPARAMS(client.get_nickname(), "TOPIC");
		send(client.getSocket(), msg.c_str(), msg.length(), 0);
		return;
	}
	Channel &chan = find_channel(channel);
	if (chan.get_name()=="NOT_FOUND")
	{
		msg = ERR_NOSUCHCHANNEL(client.get_nickname(), channel);
		send(client.getSocket(), msg.c_str(), msg.length(), 0);
		return;
	}
	if (!chan.in_channel(client))
	{
		msg = ERR_NOTONCHANNEL(client.get_nickname(),channel);
		send(client.getSocket(), msg.c_str(), msg.length(), 0);
		return;
	}
	if (chan.get_topic_state() && !topic.empty())
	{
		if(chan.is_operator(client))
		{
			chan.set_topic(topic);
			msg = IRC_RPL_TOPIC(server_name, client.get_nickname(), channel, topic);
			chan.broadcast_message(client, msg, 0);
		}
		else
		{
			msg = ERR_CHANOPRIVSNEEDED(server_name, client.get_nickname(), channel);
			send(client.getSocket(), msg.c_str(), msg.length(), 0);
		}
	}
	else if (chan.get_topic_state() && !topic.empty())
	{
		chan.set_topic(topic);
		msg = IRC_RPL_TOPIC(server_name, client.get_nickname(), channel, topic);
		chan.broadcast_message(client, msg, 0);		
	}
	else if (topic.empty())
	{
		msg = IRC_RPL_TOPIC(server_name, client.get_nickname(), channel, chan.get_topic());
		send(client.getSocket(), msg.c_str(), msg.length(), 0);
	}
}