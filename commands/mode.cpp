#include "../Server.hpp"

bool    find_mode(char c, string buf) {return (buf.find(c) != string::npos);}

void Server::add_mode(Channel *channel, string mode, int fd, string buffer)
{
	string channel_mode = channel->get_mode();
	string addmode;

	for (size_t i = (mode[0] != '+' ? 0 : 1); i < mode.size(); i++)
	{
		if (!find_mode(mode[i], CHANNEL_MODE))
		{
			string stringMode(1, mode[i]);
			this->_sendMessage(send_codes(472, this, find_user(fd),
				stringMode, channel->get_channelname()), this->clientfd[fd].fd);
		}
		else if (find_mode(mode[i], "itkol"))
		{
			if (mode[i] == 'k') {mode_k_command(channel, mode, buffer, fd);}
			if (mode[i] == 'l') {mode_l_command(channel, mode, buffer, fd);}
			if (mode[i] == 'o') {mode_o_command(channel, mode, buffer, fd);}
			if ((mode[i] == 'k' && channel->get_key() != "")
				|| mode[i] == 'l' || mode[i] == 't' || mode[i] == 'i')
				addmode += mode[i];
		}
		else if (channel_mode.find(mode[i]) == string::npos)
			addmode += mode[i];
	}
	channel->set_mode(channel_mode + addmode);
	string userAnswer = print_user(find_user(fd));
	if (!addmode.empty())
		userAnswer += "MODE " + channel->get_channelname() + " +" + addmode;
	if (userAnswer.find("MODE") != string::npos)
		this->sendtoeveryone(userAnswer, channel);
}

void Server::remove_modes(Channel *channel, string mode, int fd, string buffer)
{
	string channel_mode = channel->get_mode();
	string deletedmode;

	for (size_t i = 1; i < mode.size(); i++)
	{
		if (!find_mode(mode[i], CHANNEL_MODE))
		{
			string strmode(1, mode[i]);
			this->_sendMessage(send_codes(472, this, find_user(fd),
				strmode, channel->get_channelname()), this->clientfd[fd].fd);
		}
		else if (find_mode(mode[i], "itkol"))
		{
			if (mode[i] == 'k') {mode_k_command(channel, mode, buffer, fd);}
			if (mode[i] == 'l') {mode_l_command(channel, mode, buffer, fd);}
			if (mode[i] == 'o') {mode_o_command(channel, mode, buffer, fd);}
			if (mode[i] == 'k') {deletedmode += mode[i];}
			if (mode[i] == 't' || mode[i] == 'l' || mode[i] == 'i')
			{
				deletedmode += mode[i];
				channel_mode.erase(channel_mode.find(mode[i]), 1);
			}
		}
		else if (channel_mode.find(mode[i]) != string::npos)
		{
			deletedmode += mode[i];
			channel_mode.erase(channel_mode.find(mode[i]), 1);
		}
	}
	channel->set_mode(channel_mode);
	string userAnswer = print_user(find_user(fd));
	if (!deletedmode.empty())
		userAnswer += "MODE " + channel->get_channelname() + " -" + deletedmode;
	if (userAnswer.find("MODE") != string::npos)
		this->sendtoeveryone(userAnswer, channel);
}

void Server::channel_mode(Channel *channel, string mode, int fd, string buffer)
{
	if (mode[0] == '-') {remove_modes(channel, mode, fd, buffer);}
	else {add_mode(channel, mode, fd, buffer);}
}


void    Server::mode_channel_handler(string buffer, int fd)
{
	string tmp(buffer);
	int j = 0;
	size_t i;

	string targetmsg = "";

	if ((i = tmp.find_first_not_of(" \t\r\n", 5)) != string::npos)
		targetmsg = tmp.substr(i, ((j = tmp.find_first_of(" \t\r\n", i)) - i));
	if (targetmsg.empty())
	{
		this->_sendMessage(send_codes(461, this, find_user(fd), "MODE", ""),
			this->clientfd[fd].fd);
		return ;
	}
	string idOfChannel = "#&+";
	j = tmp.find_first_not_of(" \t\r\n", j);
	string mode = "";
	if (tmp.find_first_of(" \t\r\n", j) != string::npos)
		mode = tmp.substr(j, (tmp.find_first_of(" \t\r\n", j) - j));
	if (!targetmsg.empty() && idOfChannel.find(targetmsg[0]) != string::npos)
	{
		if (this->getChannels().find(targetmsg) == this->getChannels().end())
		{
			this->_sendMessage(send_codes(403, this, find_user(fd), targetmsg, ""),
			this->clientfd[fd].fd);
		}
		else if (this->getChannels().find(targetmsg)->
			second->getChanOps().find(this->clientfd[fd].fd)
			== this->getChannels().find(targetmsg)->second->getChanOps().end())
		{
			this->_sendMessage(send_codes(482, this, find_user(fd), targetmsg, ""),
				this->clientfd[fd].fd);
		}
		else
			channel_mode(this->getChannels().find(targetmsg)->second, mode, fd, buffer);
	}
}

void Server::mode_o_command(Channel *channel, string mode, string buffer, int fd)
{
	int i = 0;
	for (int j = 0; buffer[i] && j < 3; i++)
	{
		if (buffer[i] == ' ' || buffer[i] == '\t')
		{
			while (buffer[i] == ' ' || buffer[i] == '\t')
				i++;
			j++;
			i--;
		}
	}
	string name = buffer.substr(i, (buffer.find_first_of(" \t\r\n", i) - i));
	int userSd = channel->searchuserbyname(name);
	if (userSd == -1)
	{
		this->_sendMessage(send_codes(441, this, find_user(fd),
			name, channel->get_channelname()), this->clientfd[fd].fd);
		return;
	}
	channel->rm_user(userSd);
	if (mode[0] == '-')
		channel->addUser(userSd, find_user_byfd(userSd));
	else
		channel->addChanops(userSd, find_user_byfd(userSd));

	string userAnswer = print_user(find_user(fd));
	userAnswer += "MODE " + channel->get_channelname() + " " + mode + " " + name;

	this->sendtoeveryone(userAnswer, channel);
}


void Server::mode_l_command(Channel *channel, string mode, string buffer, int fd)
{
	if (mode[0] == '-')
	{
		channel->set_maxUser(-1);
		return;
	}

	int i = 0;
	for (int j = 0; buffer[i] && j < 3; i++)
	{
		if (buffer[i] == ' ' || buffer[i] == '\t')
		{
			while (buffer[i] == ' ' || buffer[i] == '\t')
				i++;
			j++;
			i--;
		}
	}

	string name = buffer.substr(i, buffer.find('\r') != string::npos ?
		buffer.length() - 2 - i : buffer.length() - 1 - i);
	int maxUser = strtoul(name.c_str(), NULL, 0);
	if (maxUser < 0)
	{
		this->_sendMessage("Max user key must be superior to 0.",
			this->clientfd[fd].fd);
		return;
	}
	channel->set_maxUser(maxUser);
}

void    Server::mode_k_command(Channel *channel, string mode, string buffer, int fd)
{
	if (mode[0] == '-')
	{
		channel->set_key("");
		return ;
	}
	int i = 0;
	for (int j = 0; buffer[i] && j < 3; i++)
	{
		if (buffer[i] == ' ' || buffer[i] == '\t')
		{
			while (buffer[i] == ' ' || buffer[i] == '\t')
				i++;
			j++;
			i--;
		}
	}
	string key = buffer.substr(i, (buffer.find_first_of(" \t\r\n", i) - i));
	if (key.compare("x") == 0)
	{
		this->_sendMessage(send_codes(467, this, find_user(fd),
			channel->get_key(), ""), this->clientfd[fd].fd);
	}
	else
	{
		if (!key.empty()) {channel->set_key(key);}
		else
		{
			string keycmp = print_user(find_user(fd));
			this->_sendMessage(keycmp + ": Wrong key!", this->clientfd[fd].fd);
		}
	}   
}

void Server::oper_command(string buffer, int fd)
{
	string buf(buffer);
	int j = 0;
	size_t i;
	string user;
	if ((i = buf.find_first_not_of(" \t\r\n", 5)) != string::npos)
		user = buf.substr(i, ((j = buf.find_first_of(" \t\r\n", i)) - i));
	if (user.empty())
	{
		this->_sendMessage(send_codes(461, this, find_user(fd), "OPER", ""),
			this->clientfd[fd].fd);
		return;
	}
	if (this->searchUserby_nickname(user) == -1)
	{
		this->_sendMessage(send_codes(401, this, find_user(fd), user, ""),
			this->clientfd[fd].fd);
		return;
	}
	j = buf.find_first_not_of(" \t\r\n", j);
	string password = buf.substr(j, (buf.find_first_of(" \t\r\n", j) - j));
	if (password.empty())
	{
		this->_sendMessage(send_codes(461, this, find_user(fd), "OPER", ""),
			this->clientfd[fd].fd);
		return;
	}
	if (password.compare(OPERPWD) == 0)
	{
		string userAnswer = print_user(find_user_byfd(this->
			searchUserby_nickname(user)));
		userAnswer += "MODE " + find_user_byfd(this->
			searchUserby_nickname(user))->get_nickname() + " +o";
		find_user_byfd(this->searchUserby_nickname(user))->
			set_mode(find_user_byfd(this->searchUserby_nickname(user))->
			get_mode() + "o");
		this->_sendMessage(userAnswer, this->searchUserby_nickname(user));
		this->_sendMessage(send_codes(381, this, find_user_byfd(this->
			searchUserby_nickname(user)), "", ""),
			this->searchUserby_nickname(user));
	}
	else
	{
		this->_sendMessage(send_codes(464, this,
			find_user_byfd(this->searchUserby_nickname(user)), "", ""),
			this->clientfd[fd].fd);
	}
}
