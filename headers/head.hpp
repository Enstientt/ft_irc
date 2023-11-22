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
#define IRC_JOIN_MSG(server, nick, channel)( ":" + server+ " 331 " + nick + " " + channel + " :No topic is set\r\n" \
        + ":" + server + " 353 " +  nick + " = " + channel + " :" + nick + "\r\n" \
        + ":" + server  +" 366 "+ nick + " " + channel + " :End of /NAMES list.\r\n")
#define IRC_PRIVMSG_MSG(nick, channel, message) (":" + nick  + " PRIVMSG " + channel + " :" + message + "\r\n")
#define IRC_RPL_NOTOPIC(server, nick, channel) \
    ":" server " 331 " nick " " channel " :No topic is set\r\n"

#define IRC_RPL_TOPIC(server, nick, channel, topic) \
    ":" server " 332 " nick " " channel " :" topic "\r\n"

#define IRC_RPL_NAMREPLY(server, nick, channel, users) \
    ":" server " 353 " nick " = " channel " :" users "\r\n"

#define IRC_RPL_ENDOFNAMES(server, nick, channel) \
    ":" server " 366 " nick " " channel " :End of /NAMES list.\r\n"
#endif