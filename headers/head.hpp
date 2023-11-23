#ifndef head_hpp
#define head_hpp

#define user_forma(nickname, username, adress) (":" + nickname + "!" + username + "@" + adress)
#define ERR_UNKNOWNCOMMAND(command) (": 421 " + command + " :Unknown command\r\n")
#define ERR_NONICKNAMEGIVEN ": 431 :There is no nickname.\r\n"
#define ERR_ERRONEUSNICKNAME(client, nickname) (": 432 " + client + " " + nickname + " :Erroneus nickname\r\n")
#define RPL_NICK(oclient, uclient, client) (": " + oclient + "!" + uclient + "@localhost NICK " + client + "\r\n")
#define ERR_NICKNAMEINUSE(client, nickname) (": 433 " + client + " " + nickname + " :Nickname is already in use.\r\n")
#define RPL_WELCOME(user_forma, nickname) (": 001 " + nickname + " :Welcome " + nickname + " to the Internet Relay Chat " + user_forma + "\r\n")
#define RPL_YOURHOST(client, servername) (": 002 " + client + " :Your host is " + servername + "\r\n")
#define RPL_CREATED(client, datetime) (": 003 " + client + " :This server was created " + datetime + "\r\n")
#define ERR_NEEDMOREPARAMS(client, command) (":localhost 461 " + client + " " + command + " :Not enough parameters.\r\n")
#define ERR_PASSWDMISMATCH(client) (":localhost 464 " + client + " :Password incorrect\r\n")
#define IRC_PRIVMSG_MSG(nick, channel, message) (":" + nick  + " PRIVMSG " + channel + " :" + message + "\r\n")
#define RPL_PRIVMSG(nick, username, target, message) (":" + nick + "!" + username + "@localhost PRIVMSG " + target + "  :" + message + "\r\n")
#define IRC_RPL_NOTOPIC(server, nick, channel) \
    ":" server " 331 " nick " " channel " :No topic is set\r\n"
#define ERR_CHANNELISFULL(client, channel) (":localhost 471 " + client + " " + channel + " :Cannot join channel (+l)\r\n")
#define ERR_INVITEONLYCHAN(client, channel) (":localhost 473 " + client + " " + channel + " :Cannot join channel (+i)\r\n")
#define IRC_RPL_TOPIC(server, nick, channel, topic) \
    ":" server " 332 " nick " " channel " :" topic "\r\n"
#define ERR_USERONCHANNEL(client, nick, channel) (":localhost 443 " + client + " " + nick + " " + channel + " is already on channel\r\n")
#define ERR_CHANNELISFULL(client, channel) (":localhost 471 " + client + " " + channel + " :Cannot join channel (+l)\r\n")
#define RPL_JOIN(user_forma, client, channel) (user_forma + " JOIN :" + channel + "\r\n")
#define ERR_BADCHANNELKEY(client, channel) (":localhost 475 " + client + " " + channel + " :Cannot join channel (+k)\r\n")
#define ERR_NOSUCHNICK(client, nickname) (":localhost 401 " + client + " " + nickname + " :No such nick/channel\r\n")
#define ERR_NOSUCHCHANNEL(client, channel) (":localhost 403 " + client + " " + channel + " :No such channel\r\n")
#define ERR_NOTONCHANNEL(client, channel) (":localhost 442 " + client + " " + channel + " :You're not on that channel.\r\n")
#define RPL_NAMREPLY(nickname, channel, users) (":" + " 353 " +  nick + " = " + channel + " :" + nick + "\r\n")
#define RPL_ENDOFNAMES(nickname, channel) ( ":"+" 366 "+ nick + " " + channel + " :End of /NAMES list.\r\n");
#endif