#ifndef head_hpp
#define head_hpp
#define OPER_PASS "@irc_server"
#define BOTE_ID "BOTE@gamil.com"
#define user_forma(nickname, username, adress) (":" + nickname + "!" + username + "@" + adress)
#define ERR_UNKNOWNCOMMAND(command) (": 421 " + command + " :Unknown command\r\n")
#define ERR_NONICKNAMEGIVEN ": 431 :There is no nickname.\r\n"
#define ERR_ERRONEUSNICKNAME(client, nickname) (": 432 " + client + " " + nickname + " :Erroneus nickname\r\n")
#define RPL_NICK(oclient, uclient, client) (": " + oclient + "!" + uclient + "@" + adress +" NICK " + client + "\r\n")
#define ERR_NICKNAMEINUSE(client, nickname) (": 433 " + client + " " + nickname + " :Nickname is already in use.\r\n")
#define RPL_WELCOME(user_forma, nickname) (": 001 " + nickname + " :Welcome " + nickname + " to the Internet Relay Chat " + user_forma + "\r\n")
#define RPL_YOURHOST(servername)(": 002 Your host is "+ servername+", available Commands MODE((+/-)itklo) PRIVMSG PASS NICK USER JOIN KICK INVITE OPER BOTE \r\n")
#define RPL_CREATED(client, datetime) (": 003 " + client + " :This server was created " + datetime + "\r\n")
#define ERR_NEEDMOREPARAMS(client, command, adress)(":"+adress+" 461 " + client + " " + command + " :Not enough parameters.\r\n")
#define ERR_PASSWDMISMATCH(client, adress) (":"+adress+" 464 " + client + " :Password incorrect\r\n")
#define IRC_JOIN_MSG(nick, channel,users, topic)(": 353 " +  nick + " = " + channel + " :" + users + "\r\n" \
        +": 366 "+ nick + " " + channel + " :End of /NAMES list.\r\n")
#define RPL_PRIVMSG(nick, username, target, message, adress) (":" + nick + "!" + username + "@" + adress+" PRIVMSG " + target + " " + message + "\r\n")
#define IRC_RPL_NOTOPIC(server, nick, channel) \
    ":" server " 331 " nick " " channel " :No topic is set\r\n"
#define ERR_CHANNELISFULL(client, channel, adress) (":"+adress+" 471 " + client + " " + channel + " :Cannot join channel (+l)\r\n")
#define ERR_INVITEONLYCHAN(client, channel, adress) (":"+ adress+ " 473 " + client + " " + channel + " :Cannot join channel (+i)\r\n")
#define IRC_RPL_TOPIC(server, nick, channel, topic)(":"+server+ " 332 " +nick +" "+ channel+ " " +topic +"\r\n")
#define ERR_USERONCHANNEL(client, nick, channel, adress) (":"+adress+" 443 " + client + " " + nick + " " + channel + " is already on channel\r\n")
#define ERR_CHANNELISFULL(client, channel, adress) (":"+adress+" 471 " + client + " " + channel + " :Cannot join channel (+l)\r\n")
#define RPL_JOIN(user_forma, client, channel) (user_forma + " JOIN :" + channel + "\r\n")
#define ERR_BADCHANNELKEY(client, channel,adress) (":"+adress+" 475 " + client + " " + channel + " :Cannot join channel (+k)\r\n")
#define ERR_NOSUCHNICK(client, nickname, adress) (":"+adress+" 401 " + client + " " + nickname + " :No such nick/channel\r\n")
#define ERR_NOSUCHCHANNEL(client, channel,adress) (":"+adress+" 403 " + client + " " + channel + " :No such channel\r\n")
#define ERR_NOTONCHANNEL(client, channel,adress) (":"+adress+" 442 " + client + " " + channel + " :You're not on that channel.\r\n")
#define RPL_NAMREPLY(nick, channel, users) ": 353 " +  nick + " = " + channel + " :" + users + "\r\n"
#define RPL_ENDOFNAMES(nickname, channel) (": 366 "+ nick + " " + channel + " :End of /NAMES list.\r\n")
#define RPL_MODESET(operator, channel, mode, nick) (":" + operator + " MODE " + channel + " " + mode + "  " + nick + "\r\n")
#define RPL_TOPICSET(operator, channel, mode, nick) (":" + operator + " TOPIC " + channel + " " + mode + "  " + nick + "\r\n")
#define RPL_INVITING(server, nick, channel) (":"+ server +" 341 "+ channel +" "+ nick+ "\r\n")
#define ERR_CHANOPRIVSNEEDED(server, nick, channel) ( ":"+ server+ " 482 "+ nick+ " " +channel+" :You're not channel operator\r\n")
#define RPL_INVITATION(nick, inviter, channel)(":" + inviter +" PRIVMSG "+ nick +" :\001INVITE " +channel +"\001\r\n")
#define RPL_KICKED(server, nick, channel, kicker, comment) ":" + server + " KICK "+ channel+ " " +nick +" :" + kicker +" "+ comment + "\r\n"
#define ERR_USERNOTINCHANNEL(server, nick, user, channel) ":" +server+ " 441 "+ nick+ " "+ user +" " +channel +" :They aren't on that channel\r\n"
#define ERR_WRONG_COMMAND(servername, client, command)(":" + servername +  " 421 " + client +" " + command+  " :Unknown command\r\n" )
#define ERR_ALREADYREGISTRED(servername, client) (": "+ servername+  "462 " + client + ":You may not reregister\r\n")
#define ERR_UNKNOWNMODE(nickname, mode)(": 472 " + nickname + " :is unknown mode" + mode +" to me\r\n")
#define ERR_NOTAUTHENTICATED(serverName, client)( ":" + serverName + " 451 " + client + " :You have not registered\r\n")
#endif