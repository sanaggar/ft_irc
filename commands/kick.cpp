#include "../Server.hpp"

void Server::kick_command(const string& buffer, int fd)
{
	string buf = buffer;
	int kickCount = count(buf.begin(), buf.end(), '\n');
	
	string channelsName = get_channels_name(buf);
	if (channelsName.empty())
	{
		this->_sendMessage(send_codes(461, this, find_user(fd), "KICK", ""), this->clientfd[fd].fd);
		return;
	}

	int nbOfChannels = 1 + count(channelsName.begin(), channelsName.end(), ',');
	size_t k = buf.find_first_not_of(" \t\r\n", buf.find_first_of(" \t\r\n", 5));
	
	for (int l = 0; l < kickCount; l++)
	{
		string usersNick = get_users_nick(buf, k);
		int nbUsers = 1 + count(usersNick.begin(), usersNick.end(), ',');
		
		if (usersNick.empty())
		{
			this->_sendMessage(send_codes(461, this, find_user(fd), "KICK", ""), this->clientfd[fd].fd);
			return;
		}
		
		process_kick_command(buf, usersNick, nbUsers, channelsName, nbOfChannels, fd);
		
		buf.erase(0, buf.find("\n") + 1);
	}
}

string Server::get_channels_name(const string &buf)
{
	size_t i = buf.find_first_not_of(" \t\r\n", 5);
	if (i != string::npos)
		return buf.substr(i, buf.find_first_of(" \t\r\n", i) - i);
	return "";
}

string Server::get_users_nick(const string &buf, size_t &k)
{
	return buf.substr(k, buf.find_first_of(" \t\r\n", k) - k);
}

void Server::process_kick_command
(string &buf, string &usersNick, int nbUsers, string &channelsName, int nbOfChannels, int fd)
{
	(void)buf;
	for (int j = 0; j < nbUsers; j++)
	{
		string userNick = usersNick.substr(0, usersNick.find(","));
		usersNick.erase(0, usersNick.find(",") + 1);
		
		for (int i = 0; i < nbOfChannels; i++)
		{
			string tmp = channelsName;
			string channelName = tmp.substr(0, tmp.find(","));
			tmp.erase(0, tmp.find(",") + 1);
			handle_kick_user_from_channel(userNick, channelName, fd);
		}
	}
}

void Server::handle_kick_user_from_channel(const string &userNick, const string &channelName, int fd)
{
	if (this->getChannels().find(channelName) == this->getChannels().end())
	{
		this->_sendMessage(send_codes(403, this, find_user(fd), channelName, ""), this->clientfd[fd].fd);
		return;
	}
	if (this->getChannels().find(channelName)->second->searchuserbyname(find_user(fd)->get_nickname()) == -1)
	{
		this->_sendMessage(send_codes(442, this, find_user(fd), channelName, ""), this->clientfd[fd].fd);
		return;
	}
	if (!this->getChannels().find(channelName)->second->isChanop(this->clientfd[fd].fd))
	{
		this->_sendMessage(send_codes(482, this, find_user(fd), channelName, ""), this->clientfd[fd].fd);
		return;
	}
	int userToKickSd;
	if ((userToKickSd = this->getChannels().find(channelName)->second->searchuserbyname(userNick)) == -1)
	{
		this->_sendMessage(send_codes(441, this, find_user(fd), userNick, channelName), this->clientfd[fd].fd);
		return;
	}
	string userAnswer = print_user(find_user_byfd(userToKickSd));
	userAnswer += "PART " + channelName;
	sendtoeveryone(userAnswer, this->getChannels().find(channelName)->second);
	this->getChannels().find(channelName)->second->rm_user(userToKickSd);
	if (this->getChannels().find(channelName)->second->getUsersNb() == 0)
	{
		delete this->getChannels().find(channelName)->second;
		this->getChannels().erase(channelName);
	}
	find_user_byfd(userToKickSd)->get_channels().erase(channelName);
}
