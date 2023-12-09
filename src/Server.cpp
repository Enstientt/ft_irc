#include "../headers/Server.hpp"
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

/****************************** server set UP *******************************************/
Server::Server()
{
	server_name = "Irc-Server";
}
Server::Server(std::string port, std::string password) : port(port), password(password)
{
	// initialize client and channel con
	server_name = "Irc-Server";
	client_note_found.set_nickName("NOT__FOUND");
	channel_note_found.set_name("NOT_FOUND");
	// Create a socket
	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocket == -1)
	{
		std::cout << "Error in creating the socket" << std::endl;
		exit(EXIT_FAILURE);
	}
	// set socket opt
	int opt = 1;
	if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
	{
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}
	// non-block fd
	if (fcntl(serverSocket, F_SETFL, O_NONBLOCK) == -1)
	{
		perror("fcntl");
		exit(EXIT_FAILURE);
	}
	// Bind the socket to a specific IP and port
	char hostname[128];
	sockaddr_in sock_add;
	int port_1 = atoi(port.c_str());
	sock_add.sin_family = AF_INET;
	sock_add.sin_addr.s_addr = INADDR_ANY;
	sock_add.sin_port = htons(port_1);
	if (bind(serverSocket, (sockaddr *)&sock_add, sizeof(sockaddr)) == -1)
	{
		std::cout << "Error in binding" << std::endl;
		exit(EXIT_FAILURE);
	}
	gethostname(hostname, sizeof(hostname));
	struct hostent *h;
	h = gethostbyname(hostname);
	char *ip = inet_ntoa(*((struct in_addr *)h->h_addr_list[0]));
	host_ip = ip;
	// Listen for incoming connections
	if (listen(serverSocket, MAX_CLIENTS) == -1)
	{
		std::cout << "Error while listening" << std::endl;
		exit(EXIT_FAILURE);
	}

	// Initialize poll structure
	fds[0].fd = serverSocket;
	fds[0].events = POLLIN;
	for (int i = 1; i < MAX_CLIENTS + 1; ++i)
	{
		fds[i].fd = -1; // Initialize with an invalid value
	}
}

Server::~Server()
{
	close(serverSocket);
}
void Server::run()
{
	std::cout << "Server listening on port " << port << "..." << std::endl;

	while (true)
	{
		int ret = poll(fds, MAX_CLIENTS + 1, -1);
		if (ret == -1)
		{
			perror("poll");
			exit(EXIT_FAILURE);
		}

		// Check for incoming connection requests
		if (fds[0].revents & POLLIN)
		{
			acceptConnection();
		}

		// Check for data to read and disconnects on existing client sockets
		for (int i = 1; i < MAX_CLIENTS + 1; ++i)
		{
			if (fds[i].revents & (POLLIN | POLLHUP | POLLERR))
				handleClient(i);
		}
	}
};
void Server::acceptConnection()
{
	sockaddr_in client_add;
	socklen_t client_len = sizeof(client_add);
	int newSocket = accept(serverSocket, (sockaddr *)&client_add, &client_len);
	if (newSocket == -1)
	{
		perror("accept");
		exit(EXIT_FAILURE);
	}

	int i;
	for (i = 1; i < MAX_CLIENTS + 1; ++i)
	{
		if (fds[i].fd == -1)
		{
			fds[i].fd = newSocket;
			fds[i].events = POLLIN | POLLHUP | POLLERR;
			std::cout << "Client " << i << " connected from " << inet_ntoa(client_add.sin_addr)
					  << ":" << ntohs(client_add.sin_port) << std::endl;
			break;
		}
	}

	if (i == MAX_CLIENTS + 1)
	{
		// No available slot for the new client
		std::cout << "Connection limit reached. Rejecting new connection." << std::endl;
		close(newSocket);
	}
	Client newClient(newSocket, client_add);
	_clients.push_back(newClient);
}
void Server::handleClient(int index)
{
	std::vector<Client>::iterator it;
	char buffer[512];
	bzero(buffer, 512);
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
			for (; it != _clients.end(); it++)
			{
				if (it->getSocket() == fds[index].fd)
				{
					it->addtosetMessage(str);
					if (it->getMessage().length() > 512)
					{
						it->setMessage("");
						continue;
					}
					if (it->getMessage().find('\n') != std::string::npos)
					{
						if (isMultipleWords(it->getMessage(), '\n') > 1)
							handleMulti(*it);
						else
						{
							execute_command(*it);
						}
						std::cout << "client " << index << " :" << it->getMessage();
						it->setMessage("");
					}
				}
			}
		}
	}
	else if (bytesRead == 0 || bytesRead == -1)
	{
		// Client has closed the connection
		std::cout << "Client " << index << " disconnected." << std::endl;
		std::vector<Client>::iterator it = _clients.begin();
		for (; it != _clients.end(); it++)
		{
			if (it->getSocket() == fds[index].fd)
			{
				clearChannels(*it);
				_clients.erase(it);
				break;
			}
		}
		if (it != _clients.end())
			close(fds[index].fd);
		fds[index].fd = -1;
	}
	cleanServer();
}
/****************************** commands *******************************************/
void Server::pass(std::string password, Client &client)
{
	if (client.auth() == false)
	{
		if (password.empty())
		{
			send(client.getSocket(), ERR_NEEDMOREPARAMS(client.get_nickname(), "PASS", host_ip).c_str(), ERR_NEEDMOREPARAMS(client.get_nickname(), "PASS", host_ip).length(), 0);
			return;
		}
		if (password != this->password)
		{

			send(client.getSocket(), ERR_PASSWDMISMATCH(client.get_nickname(), host_ip).c_str(), ERR_PASSWDMISMATCH(client.get_nickname(), host_ip).length(), 0);
		}
		else
		{
			client.set_pwd(password);
		}
	}
	else
	{
		std::string message = ERR_ALREADYREGISTRED(server_name, client.get_nickname());
		send(client.getSocket(), message.c_str(), message.length(), 0);
	}
};

void Server::nick(std::string nick, Client &client)
{
	std::string message;
	// if not authenticate just processd with authentication
	if (nick.empty())
	{
		message = ERR_NONICKNAMEGIVEN;
		send(client.getSocket(), message.c_str(), message.length(), 0);
		return;
	}
	if (!client.get_pwd().empty() && client.auth() == false)
	{
		if (!isValidNick(nick))
		{
			message = server_name + "432 " + nick + " :Erroneous Nickname\r\n";
			send(client.getSocket(), message.c_str(), message.length(), 0);
		}
		else if (nick_already_exist(nick) == false)
		{
			client.set_nickName(nick);
		}
		else
		{
			message = ": 433 " + nick + " :Nickname is already in use.\r\n";
			send(client.getSocket(), message.c_str(), message.length(), 0);
		}
	}
	// changing the nickname
	else if (client.auth() == true)
	{
		if (!isValidNick(nick))
		{
			message = server_name + "432 " + client.get_nickname() + nick + " :Erroneous Nickname\r\n";
			send(client.getSocket(), message.c_str(), message.length(), 0);
		}
		else if (nick_already_exist(nick) == false)
		{
			message = ":" + client.get_nickname() + "!" + client.get_username() + "@" + host_ip + " NICK :" + nick + "\r\n";
			client.set_nickName(nick);
			std::vector<std::string>::iterator it = client.get_channels().begin();
			for (; it != client.get_channels().end(); it++)
			{
				Channel &chan = find_channel(*it);
				chan.broadcast_message(client, message, 0);
			}
			send(client.getSocket(), message.c_str(), message.length(), 0);
		}
		else
		{
			message = ": 433 " + nick + " :Nickname is already in use.\r\n";
			send(client.getSocket(), message.c_str(), message.length(), 0);
		}
	}
	if (!client.get_pwd().empty() && !client.get_username().empty() && !client.get_nickname().empty() && client.auth() == false)
		welcomeClient(client);
}
void Server::user(std::string username, std::string mode, std::string hostName, std::string realName, Client &client)
{
	(void)mode;
	if (!checkForma(username, mode, hostName, realName))
	{
		std::string errorMessage = ERR_NEEDMOREPARAMS(client.get_nickname(), "USER", host_ip);
		send(client.getSocket(), errorMessage.c_str(), errorMessage.length(), 0);
		return;
	}
	else if (!client.get_pwd().empty() && client.auth() == false)
	{
		client.set_username(username);
		client.set_realname(realName);
	}
	else if (client.auth())
	{
		std::string message = ERR_ALREADYREGISTRED(server_name, client.get_nickname());
		send(client.getSocket(), message.c_str(), message.length(), 0);
		return;
	}
	if (!client.get_pwd().empty() && !client.get_username().empty() && !client.get_nickname().empty() && client.auth() == false)
		welcomeClient(client);
}
void Server::privmsg(Client &client, std::string command)
{
	std::istringstream iss(command);
	std::string cmd, target, msg;
	std::string message;
	iss >> cmd >> target >> std::ws;
	std::getline(iss, msg);
	if (target.empty() || msg.empty())
	{
		message = ERR_NEEDMOREPARAMS(client.get_nickname(), cmd, host_ip);
		send(client.getSocket(), message.c_str(), message.length(), 0);
	}
	// handle channel
	else if (target[0] == '#')
	{
		Channel &chan = find_channel(target);
		if (chan.get_name() == channel_note_found.get_name())
		{
			message = ERR_NOSUCHCHANNEL(client.get_nickname(), target, host_ip);
			send(client.getSocket(), message.c_str(), message.length(), 0);
		}
		else if (chan.in_channel(client))
		{
			message = RPL_PRIVMSG(client.get_nickname(), client.get_username(), chan.get_name(), msg, host_ip);
			chan.broadcast_message(client, message, 1);
		}
		else
		{
			message = ERR_NOTONCHANNEL(client.get_nickname(), target, host_ip);
			send(client.getSocket(), message.c_str(), message.length(), 0);
		}
	}
	// handle users
	else
	{
		Client &cl = find_client(target);
		if (cl.get_nickname() != client_note_found.get_nickname())
		{
			message = RPL_PRIVMSG(client.get_nickname(), cl.get_username(), target, msg, host_ip);
			send(cl.getSocket(), message.c_str(), message.length(), 0);
		}
		else
		{
			message = ERR_NOSUCHNICK(client.get_nickname(), target, host_ip);
			send(client.getSocket(), message.c_str(), message.length(), 0);
		}
	}
};

int Server::handle_mode(Client &client, Channel &chan, std::string mode, std::string parameter, std::string &modes)
{
	// std::istringstream iss(command);
	// iss >> cmd >> name >> mode;
	// Channel & chan = find_channel(name);
	std::string msg;
	if (chan.in_channel(client) && (chan.is_operator(client) /*|| isServerOperator())*/))
	{
		if (mode == "+k")
		{
			chan.set_pass(parameter);
			chan.set_rest(true);
			modes +=mode;
			return 1;
		}
		else if (mode == "-k")
		{
			chan.set_rest(false);
			modes +=mode;
			return 0;
		}
		else if (mode == "+t")
		{
			chan.set_topic_protected(true);
			modes +=mode;
			return 0;
		}
		else if (mode == "-t")
		{
			chan.set_topic_protected(false);
			modes +=mode;
			return 0;
		}
		else if (mode == "+o" || mode == "-o")
		{
			Client &cl = find_client(parameter);
			if (cl.get_nickname() == "NOT__FOUND")
			{
				msg = ERR_NOSUCHNICK(client.get_nickname(), parameter, host_ip);
				send(client.getSocket(), msg.c_str(), msg.length(), 0);
			}
			else if (!chan.in_channel(cl))
			{
				msg = ERR_USERNOTINCHANNEL(server_name, client.get_nickname(), parameter, chan.get_name());
				send(client.getSocket(), msg.c_str(), msg.length(), 0);
			}
			else
			{
				if (mode == "+o" && !chan.is_operator(cl))
					chan.add_operator(cl);
				else if (mode == "-o" && chan.is_operator(cl))
					chan.remove_operator(cl);
				modes +=mode;
			}
			return 1;
		}
		else if (mode == "+l")
		{
			if (parameter.empty())
			{
				msg = ERR_NEEDMOREPARAMS(client.get_nickname(), "MODE", host_ip);
				send(client.getSocket(), msg.c_str(), msg.length(), 0);
				return 0;
			}
			if (parameter.find_first_not_of("0123456789") != std::string::npos){
				msg = ERR_UNKNOWNMODE(client.get_nickname(), mode);
				send(client.getSocket(), msg.c_str(), msg.length(), 0);
				return 0;
			}
			chan.set_limit(std::stoi(parameter));
			chan.set_lim_state(true);
			modes +=mode;
			return 1;
		}
		else if (mode == "-l")
		{
			chan.set_limit(MAX_CLIENTS);
			chan.set_lim_state(false);
			modes +=mode;
			return 0;
		}

		else if (mode == "+i")
		{
			chan.set_invite_only(true);
			modes +=mode;
			return 0;
		}
		else if (mode == "-i")
		{
			chan.set_invite_only(false);
			modes +=mode;
			return 0;
		}
		else
		{
			msg = ERR_UNKNOWNMODE(client.get_nickname(), mode);
			send(client.getSocket(), msg.c_str(), msg.length(), 0);
			return 0;
		}
	}
	else
	{
		msg = ERR_CHANOPRIVSNEEDED(server_name, client.get_nickname(), chan.get_name());
		send(client.getSocket(), msg.c_str(), msg.length(), 0);
		return -1;
	}
}

void Server::join(Client &client, std::string target, std::string &pass)
{
	std::string msg;
	int toggler = 0;

	Channel &chan = find_channel(target);
	// check if the channel exist
	if (chan.get_name() != channel_note_found.get_name())
	{
		std::string ps = chan.get_pass();
		if (chan.in_channel(client))
		{
			msg = ERR_USERONCHANNEL(client.get_nickname(), client.get_nickname(), chan.get_name(), host_ip);
			send(client.getSocket(), msg.c_str(), msg.length(), 0);
			return;
		}
		if (chan.is_invite_only())
		{
			if (!chan.is_invited(client))
			{
				msg = ERR_INVITEONLYCHAN(client.get_nickname(), chan.get_name(), host_ip);
				send(client.getSocket(), msg.c_str(), msg.length(), 0);
				toggler = 1;
			}
		}
		if (chan.get_lim_state())
		{
			if (chan.is_full())
			{
				msg = ERR_CHANNELISFULL(client.get_nickname(), chan.get_name(), host_ip);
				send(client.getSocket(), msg.c_str(), msg.length(), 0);
				toggler = 1;
			}
		}
		if (chan.get_rest())
		{
			if (pass != ps)
			{
				msg = ERR_BADCHANNELKEY(client.get_nickname(), chan.get_name(), host_ip);
				send(client.getSocket(), msg.c_str(), msg.length(), 0);
				toggler = 1;
			}
		}
		if (toggler == 0)
		{
			chan.add_client_to_channnel(client);
			client.set_channels(chan.get_name());
			msg = RPL_JOIN(user_forma(client.get_nickname(), client.get_username(), host_ip), client.get_nickname(), chan.get_name());
			chan.broadcast_message(client, msg, 0);
			std::cout << "*****" << chan.get_topic() << std::endl;
			msg = IRC_JOIN_MSG(client.get_nickname(), chan.get_name(), chan.get_list_of_users(), chan.get_topic());
			send(client.getSocket(), msg.c_str(), msg.length(), 0);
		}
	}
	else
	{
		// Create the channel and add the client to it
		Channel newChannel(target, pass);
		newChannel.add_client_to_channnel(client);
		newChannel.add_operator(client);
		_channels.push_back(newChannel);
		client.set_channels(target);
		msg = RPL_JOIN(user_forma(client.get_nickname(), client.get_username(), host_ip), client.get_nickname(), target);
		newChannel.broadcast_message(client, msg, 0);
		msg = IRC_JOIN_MSG(client.get_nickname(), newChannel.get_name(), newChannel.get_list_of_users(), newChannel.get_topic());
		send(client.getSocket(), msg.c_str(), msg.length(), 0);
	}
}

void Server::invite(Client &client, std::string nickname, std::string channel)
{
	std::string msg;
	if (nickname.empty() || channel.empty())
	{
		msg = ERR_NEEDMOREPARAMS(client.get_nickname(), "INVITE", host_ip);
		send(client.getSocket(), msg.c_str(), msg.length(), 0);
		return;
	}
	Channel &chan = find_channel(channel);
	Client &cl = find_client(nickname);
	if (cl.get_nickname() == "NOT__FOUND")
	{
		msg = ERR_NOSUCHNICK(client.get_nickname(), nickname, host_ip);
		send(client.getSocket(), msg.c_str(), msg.length(), 0);
		return;
	}
	if (chan.get_name() == "NOT_FOUND")
	{
		msg = ERR_NOSUCHCHANNEL(client.get_nickname(), channel, host_ip);
		send(client.getSocket(), msg.c_str(), msg.length(), 0);
		return;
	}
	if (chan.is_operator(client))
	{
		if (chan.in_channel(cl))
		{
			msg = ERR_USERONCHANNEL(client.get_nickname(), nickname, channel, host_ip);
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
		msg = ERR_NEEDMOREPARAMS(client.get_nickname(), "KICK", host_ip);
		send(client.getSocket(), msg.c_str(), msg.length(), 0);
		return;
	}
	Channel &chan = find_channel(channel);
	Client &cl = find_client(user);
	if (chan.get_name() == "NOT_FOUND")
	{
		msg = ERR_NOSUCHCHANNEL(client.get_nickname(), channel, host_ip);
		send(client.getSocket(), msg.c_str(), msg.length(), 0);
		return;
	}
	if (cl.get_nickname() == "NOT__FOUND")
	{
		msg = ERR_NOSUCHNICK(client.get_nickname(), user, host_ip);
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
			chan.broadcast_message(client, msg, 0);
			chan.remove_client_from_channel(cl);
			chan.is_operator(cl) ? chan.remove_operator(cl) : (void)1;
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
	if (channel.empty() || channel[0] != '#')
	{
		msg = ERR_NEEDMOREPARAMS(client.get_nickname(), "TOPIC", host_ip);
		send(client.getSocket(), msg.c_str(), msg.length(), 0);
		return;
	}
	Channel &chan = find_channel(channel);
	if (chan.get_name() == "NOT_FOUND")
	{
		msg = ERR_NOSUCHCHANNEL(client.get_nickname(), channel, host_ip);
		send(client.getSocket(), msg.c_str(), msg.length(), 0);
		return;
	}
	if (!chan.in_channel(client))
	{
		msg = ERR_NOTONCHANNEL(client.get_nickname(), channel, host_ip);
		send(client.getSocket(), msg.c_str(), msg.length(), 0);
		return;
	}
	if (chan.get_topic_state() && !topic.empty())
	{
		if (chan.is_operator(client))
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
	else if (!chan.get_topic_state() && !topic.empty())
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
void Server::execute_command(Client &client)
{

	std::string command = client.getMessage();
	std::string cmd;
	std::string value;
	command = filterString(command);
	std::string array[] = {"PASS", "NICK", "USER", "PRIVMSG", "JOIN", "MODE", "INVITE", "KICK", "TOPIC", "BOTE", "QUIT"};
	std::istringstream iss(command);
	iss >> cmd >> value;
	if (cmd == "PASS")
	{
		pass(value, client);
		return;
	}
	else if (cmd == "NICK")
	{
		nick(value, client);
		return;
	}
	else if (cmd == "USER")
	{
		std::string mode;
		std::string hostName;
		std::string realName;
		iss >> mode >> hostName >> realName;
		user(value, mode, hostName, realName, client);
		return;
	}
	if (client.auth())
	{

		if (cmd == "PRIVMSG")
			privmsg(client, command);
		else if (cmd == "JOIN")
		{
			std::string pass;
			getline(iss, pass);
			if (!pass.empty())
				isMultipleWords(pass, ' ') >= 1 ? pass = pass.substr(2) : pass = pass.substr(1);
			std::cout << "this is the password" << pass << "*" << std::endl;
			join(client, value, pass);
		}
		else if (cmd == "MODE")
		{
			mode(client, command);
		}
		else if (cmd == "INVITE")
		{
			std::string channel;
			iss >> channel;
			invite(client, value, channel);
		}
		else if (cmd == "KICK")
		{
			std::string nickname, message;
			iss >> nickname;
			getline(iss, message);
			kick(client, value, nickname, message);
		}
		else if (cmd == "TOPIC")
		{
			std::string top;
			getline(iss, top);
			topic(client, value, top);
		}
		else if (cmd == "BOTE")
			handle_bote(client);
		else if (!stringExistsInArray(cmd, array, 11))
		{
			std::string msg;
			msg = ERR_WRONG_COMMAND(server_name, client.get_nickname(), cmd);
			send(client.getSocket(), msg.c_str(), msg.length(), 0);
		}
	}
	else
	{
		// need to authenticate;
		std::string msg;
		msg = ERR_NOTAUTHENTICATED(server_name, client.get_nickname());
		send(client.getSocket(), msg.c_str(), msg.length(), 0);
	}
}

/****************************** TOOLS *******************************************/
bool Server::nick_already_exist(std::string nick)
{
	std::vector<Client>::iterator it = _clients.begin();
	if (it == _clients.end())
		return false;
	else
	{
		for (; it != _clients.end(); it++)
		{
			if (it->get_nickname() == nick)
			{
				return true;
			}
		}
	}
	return false;
};
Channel &Server::find_channel(std::string channel)
{
	std::vector<Channel>::iterator it = _channels.begin();
	if (it != _channels.end())
	{
		for (; it != _channels.end(); it++)
		{
			if (it->get_name() == channel)
			{
				return *it;
			}
		}
	}
	return channel_note_found;
}

void Server::cleanServer()
{
	std::vector<Channel>::iterator it = _channels.begin();
	std::vector<Channel>::iterator tmp;
	if (it != _channels.end())
	{
		for (; it != _channels.end(); it++)
		{
			if (it->channel_size() == 0)
			{
				tmp = _channels.erase(it);
				if (tmp == _channels.end())
					break;
			}
		}
	}
}

int Server::isMultipleWords(std::string str, char c)
{
	int ret = std::count(str.begin(), str.end(), c);
	if (ret > 1)
		return ret;
	return 0;
}

std::string Server::filterString(const std::string &str)
{
	std::string result;
	for (size_t i = 0; i < str.size(); ++i)
	{
		if (str[i] != '\r' && str[i] != '\n')
		{
			result += str[i];
		}
	}
	return result;
}

bool Server::isValidNick(const std::string &nick)
{
	if (nick.size() > 9)
	{
		return false;
	}

	std::string validChars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789[]\\`_^{|}";

	for (size_t i = 0; i < nick.size(); ++i)
	{
		if (validChars.find(nick[i]) == std::string::npos)
		{
			return false;
		}
	}

	return true;
}

void Server::handle_bote(Client &client)
{

	std::string trigger("KNOCK");
	Client &boteClient = find_client("BOTE");
	if (boteClient.get_nickname() == "NOT__FOUND")
	{
		std::cout << "the bot is not available" << std::endl;
		return;
	}
	send(boteClient.getSocket(), trigger.c_str(), trigger.length(), 0);
	char buffer[256];
	memset(buffer, 0, sizeof(buffer));
	int bytes_received = recv(boteClient.getSocket(), buffer, sizeof(buffer), 0);
	if (bytes_received == -1 || bytes_received == 0)
		std::cout << "Error" << std::endl;
	else
	{
		std::string buff(buffer);
		std::string message = RPL_PRIVMSG(boteClient.get_nickname(), user_forma(boteClient.get_nickname(), boteClient.get_username(), host_ip), client.get_nickname(), buff, host_ip);
		send(client.getSocket(), message.c_str(), message.length(), 0);
	}
}

void Server::handleMulti(Client &client)
{
	std::string ruby = client.getMessage();
	std::string buffer = client.getMessage();
	std::string ret;
	int i = isMultipleWords(buffer, '\n');
	for (int j = 0; j < i; j++)
	{
		ret = buffer.substr(ret.length(), buffer.find_first_of('\n') + 1);
		client.setMessage(ret);
		execute_command(client);
	}
}

void Server::clearChannels(Client &client)
{
	std::vector<Channel>::iterator it = _channels.begin();
	for (; it != _channels.end(); it++)
	{
		if (it->in_channel(client))
			it->remove_client_from_channel(client);
	}
}

void Server::welcomeClient(Client &client)
{
	std::string client_add = inet_ntoa(client.getAddress().sin_addr);
	std::string welcomeMessage = ": 001 " + client.get_nickname() + " :Welcome " + client.get_nickname() + " to the Internet Relay Chat " + user_forma(client.get_nickname(), client.get_username(), host_ip) + "\r\n";
	client.setAuth(true);
	send(client.getSocket(), welcomeMessage.c_str(), welcomeMessage.length(), 0);
}

Client &Server::find_client(std::string nick)
{
	std::vector<Client>::iterator it = _clients.begin();
	if (it != _clients.end())
	{
		for (; it != _clients.end(); it++)
		{
			if (nick == it->get_nickname())
			{
				return *it;
			}
		}
	}
	return client_note_found;
}

bool Server::stringExistsInArray(std::string target, std::string array[], int arraySize)
{
	for (int i = 0; i < arraySize; i++)
	{
		if (array[i] == target)
		{
			return true;
		}
	}
	return false;
}

bool Server::checkForma(const std::string &username, std::string &mode, std::string hostName, std::string realName)
{
	if (username.empty() || mode.empty() || hostName.empty() || realName.empty())
		return false;
	if (mode != "0" || hostName != "*")
		return false;
	return true;
}
std::vector<std::string> split(const std::string &s, char delim)
{
	std::vector<std::string> elems;
	std::stringstream ss(s);
	std::string item;
	while (getline(ss, item, delim))
	{
		elems.push_back(item);
	}
	return elems;
}

void Server::mode(Client &client, std::string &command)
{
	(void)client;
	std::string cmd, sMode, name, mode, params,modes, signe, msg;
	std::istringstream iss(command);
	iss >> cmd >> name >> mode;
	getline(iss, params);
	!params.empty() ? params = params.substr(1) : "";
	std::vector<std::string> parameters = split(params, ' ');
	std::vector<std::string>::iterator it = parameters.begin();
	Channel &chan = find_channel(name);
	if (chan.get_name() == channel_note_found.get_name())
	{
		msg = ERR_NOSUCHCHANNEL(client.get_nickname(), chan.get_name(), host_ip);
		send(client.getSocket(), msg.c_str(), msg.length(), 0);
		return;
	}
	if (!chan.in_channel(client))
	{
		msg = ERR_NOTONCHANNEL(client.get_nickname(), chan.get_name(), host_ip);
		send(client.getSocket(), msg.c_str(), msg.length(), 0);
		return;
	}
	if (mode[0] != '-' && mode[0] != '+')
		return;
	signe += mode[0];
	int i = 1;
	// for(std::vector<std::string>::iterator it = parameters.begin();it!=parameters.end();it++)
	for (; i < (int)mode.length(); i++)
	{
		if (mode[i] == '-' || mode[i] == '+')
		{
			signe = mode[i];
			continue;
		}
		else
		{
			sMode = signe + mode[i];
			if ((signe == "-"  || mode[i] == 'i' || mode[i]=='t') && mode[i] !='o')
				handle_mode(client, chan, sMode, "", modes);
			else
			{
				if (it!=parameters.end())
				{
					handle_mode(client, chan, sMode, *it,modes);
					it++;
				}
				else
					handle_mode(client, chan, sMode, "", modes);
			}
		}
	}
	// Channel & chan = find_channel(name);
	msg = RPL_MODESET(client.get_nickname(), chan.get_name(), modes," ");
	chan.broadcast_message(client, msg, 0);
}