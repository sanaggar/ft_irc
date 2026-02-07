#pragma once

#include <arpa/inet.h>
#include <cerrno>
#include <csignal>
#include <cstdlib>
#include <ctime>
#include <fcntl.h>
#include <iomanip>
#include <iostream>
#include <map>
#include <netinet/in.h>
#include <poll.h>
#include <set>
#include <sstream>
#include <stdexcept>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>
#include <algorithm>

using namespace std;

#define MAX_CLIENTS 10
#define ver "1.0"
#define USER_MODE ""
#define CHANNEL_MODE "itkol"
#define OPERPWD "9289"
#define RPL_INVITING(channel, nick) (channel + " " + nick)
#define RPL_NAMREPLY(channel, listOfUsers) ("= " + channel + " :" + listOfUsers)
#define RPL_ENDOFNAMES(channel) (channel + " :End of NAMES list")
#define RPL_WELCOME(nick, user, host) (":Welcome to the Internet Relay Network " + nick + "!" + user + "@" + host)
#define RPL_YOUREOPER (":You are now an IRC operator")
#define RPL_NOTOPIC(channel) (channel + " :No topic is set")
#define RPL_YOURHOST(hostname) ("Your host is " + hostname + ", running version " ver)
#define RPL_TOPIC(channel, topic) (channel + " :" + topic)
#define RPL_CREATED(serverName) (string("This server was created ") + getCurrentDate())
#define RPL_MYINFO(hostname) (hostname + " " ver " " USER_MODE " " CHANNEL_MODE)
#define RPL_BOUNCE(servername, port) ("Try server " + servername + ", port " + port)
#define RPL_CHANNELMODEIS(channel, modes) (channel + " " + modes)
#define ERR_NEEDMOREPARAMS(command) (command + " :Not enough parameters")
#define ERR_NOTONCHANNEL(channel) (channel + " :You're not on that channel")
#define ERR_TOOMANYCHANNELS(channel) (channel + " :You have joined too many channels")
#define ERR_NOSUCHCHANNEL(channel) (channel + " :No such channel")
#define ERR_USERNOTINCHANNEL(nick, channel) (nick + " " + channel + " :They aren't on that channel")
#define ERR_NONICKNAMEGIVEN (":No nickname given")
#define ERR_ERRONEUSNICKNAME(nick) (nick + " :Erroneous nickname")
#define ERR_NICKNAMEINUSE(nick) (nick + " :Nickname is already in use")
#define ERR_NOSUCHNICK(nick) (nick + " :No such nick/channel")
#define ERR_CHANOPRIVSNEEDED(channel) (channel + " :You're not channel operator")
#define ERR_RESTRICTED (":Your connection is restricted!")
#define ERR_BADCHANNELKEY(channel) (channel + " :Cannot join channel (+k)")
#define ERR_CHANNELISFULL(channel) (channel + " :Cannot join channel (+l)")
#define ERR_INVITEONLYCHAN(channel) (channel + " :Cannot join channel (+i)")
#define ERR_PASSWDMISMATCH (":Password incorrect")
#define ERR_KEYSET(channel) (channel + " :Channel key already set")
#define ERR_UNKNOWNMODE(c, channel) (c + ":is unknown mode char to me for " + channel)
#define ERR_USERONCHANNEL(user, channel) (user + " " + channel + ":is already on channel")
