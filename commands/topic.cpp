#include "../Server.hpp"

void Server::topic_command(string buffer, int fd)
{
	string channelName = extract_channel_name(buffer);
	if (channelName.empty())
	{
		this->_sendMessage(send_codes(461, this, find_user(fd), "TOPIC", ""), this->clientfd[fd].fd);
		return;
	}

	if (!validate_channel_and_user(fd, channelName))
	{
		return;
	}

	string topic = extract_topic(buffer);
	if (topic.empty())
	{
		this->_sendMessage(send_codes(332, this, find_user(fd), channelName, this->getChannels().find(channelName)->second->get_topic()), this->clientfd[fd].fd);
	} else
	{
		set_and_announce_topic(fd, channelName, topic);
	}
}

string Server::extract_channel_name(const string &buffer)
{
	size_t i, j = 0;
	string channelName = "";
	if ((i = buffer.find_first_not_of(" \t\r\n", 6)) != string::npos)
	{
		channelName = buffer.substr(i, ((j = buffer.find_first_of(" \t\r\n", i)) - i));
	}
	return channelName;
}

bool Server::validate_channel_and_user(int fd, const string &channelName)
{
	if (this->getChannels().find(channelName) == this->getChannels().end())
	{
		this->_sendMessage(send_codes(403, this, find_user(fd), channelName, ""), this->clientfd[fd].fd);
		return false;
	}
	if (find_user(fd)->get_channels().find(channelName) == find_user(fd)->get_channels().end())
	{
		this->_sendMessage(send_codes(442, this, find_user(fd), channelName, ""), this->clientfd[fd].fd);
		return false;
	}
	if ((this->getChannels().find(channelName)->second->get_mode().find("t") != string::npos) &&
		(this->getChannels().find(channelName)->second->getChanOps().find(this->clientfd[fd].fd) == this->getChannels().find(channelName)->second->getChanOps().end()))
		{
		this->_sendMessage(send_codes(482, this, find_user(fd), channelName, ""), this->clientfd[fd].fd);
		return false;
	}
	return true;
}

string Server::extract_topic(const string &buffer)
{
	string topic = "";
	size_t j = buffer.find_first_of(" \t\r\n", 6); // Trouver la fin du nom du canal
	if (j != string::npos)
	{
		j = buffer.find_first_not_of(" \t\r\n", j); // Trouver le début du sujet
		if (j != string::npos)
		{
			if (buffer[j] == ':')
			{
				j++;
			}
			topic = buffer.substr(j); // Extraire tout le texte restant comme sujet
			topic = topic.substr(0, topic.find_last_not_of("\r\n") + 1); // Enlever les caractères de fin de ligne
		}
	}
	return topic;
}

void Server::set_and_announce_topic(int fd, const string &channelName, const string &topic)
{
	this->getChannels().find(channelName)->second->set_topic(topic);
	this->_sendMessage(send_codes(332, this, find_user(fd), channelName, topic), this->clientfd[fd].fd);
	string userMessage = print_user(find_user(fd));
	userMessage += "TOPIC " + channelName + " :" + topic; // Inclure le nom du canal et le sujet
	sendtoeveryone(userMessage, this->getChannels().find(channelName)->second);
}
