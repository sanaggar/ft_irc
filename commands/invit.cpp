#include "../Server.hpp"

void Server::invite_command(const string& buffer, int fd)
{
	string buf(buffer);
	string userNickname = extract_user_nickname(buf);
	string channelName = extract_channel_name1(buf);

	string userAnswer = print_user(find_user(fd));
	userAnswer += "INVITE " + userNickname + " " + channelName;

	removeSpaces(channelName);
	removeSpaces(userNickname);

	if (channelName.empty())
	{
		send_error_message(fd, "INVITE");
		return;
	}

	if (is_channel_non_existent(channelName))
	{
		this->_sendMessage(send_codes(403, this, find_user(fd), channelName, ""),
			this->clientfd[fd].fd);
		return;
	}

	int userToSendSd = this->searchUserby_nickname(userNickname);
	if (userToSendSd == -1)
	{
		this->_sendMessage(send_codes(401, this, find_user(fd), userAnswer, ""),
			this->clientfd[fd].fd);
	}
	else if (is_user_in_channel(userToSendSd, channelName))
	{
		this->_sendMessage(send_codes(443, this, find_user(fd), userNickname, channelName),
			this->clientfd[fd].fd);
	}
	else if (is_user_not_in_channel(fd, channelName))
	{
		this->_sendMessage(send_codes(442, this, find_user(fd), channelName, ""),
			this->clientfd[fd].fd);
	}
	else if (is_invite_only_channel(channelName, fd))
	{
		this->_sendMessage(send_codes(482, this, find_user(fd), channelName, ""),
			this->clientfd[fd].fd);
	}
	else
		invite_user_to_channel(userToSendSd, channelName, fd, userNickname);
}

string Server::extract_user_nickname(const string &buf)
{
	size_t i = buf.find_first_not_of(" \t\r\n", 7);
	if (i != string::npos)
		return buf.substr(i, buf.find_first_of(" \t\r\n", i) - i);
	return "";
}

string Server::extract_channel_name1(const string &buf)
{
	size_t j = buf.find_first_of(" \t\r\n", buf.find_first_not_of(" \t\r\n", 7));
	j = buf.find_first_not_of(" \t\r\n", j);
	if (j != string::npos)
		return buf.substr(j, buf.find_first_of("\r\n", j) - j);
	return "";
}

void Server::send_error_message(int fd, const string &command)
{
	this->_sendMessage(send_codes(461, this, find_user(fd), command, ""), this->clientfd[fd].fd);
}

bool Server::is_user_in_channel(int userSd, const string &channelName)
{
	return this->getChannels().find(channelName)->second->getUsers().find(userSd) !=
		this->getChannels().find(channelName)->second->getUsers().end();
}

bool Server::is_channel_non_existent(const string &channelName)
{
	return this->getChannels().find(channelName) == this->getChannels().end();
}

bool Server::is_user_not_in_channel(int fd, const string &channelName)
{
	return find_user(fd)->get_channels().find(channelName) ==
		find_user(fd)->get_channels().end();
}

bool Server::is_invite_only_channel(const string &channelName, int fd)
{
	return (this->getChannels().find(channelName)->second->get_mode().find("i") != string::npos) &&
		(this->getChannels().find(channelName)->second->getChanOps().find(this->clientfd[fd].fd) ==
			this->getChannels().find(channelName)->second->getChanOps().end());
}

void Server::invite_user_to_channel
(int userToSendSd, const string &channelName, int fd, const string &userNickname)
{
	this->getChannels().find(channelName)->second->set_invited(userToSendSd);
	this->_sendMessage(send_codes(341, this, find_user(fd), channelName, userNickname), userToSendSd);
}
