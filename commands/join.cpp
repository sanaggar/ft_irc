#include "../Server.hpp"

void Server::join_command(const string& buffer, int fd)
{
    string buf(buffer);
    string channelsList = extract_channels_list(buf);
    if (channelsList.empty())
	{
        send_error_message(fd, "JOIN");
        return;
    }

    string keysList = extract_keys_list(buf);
    int numberOfChannels = count_channels(channelsList);

    for (int i = 0; i < numberOfChannels; i++)
	{
        string channelName = extract_next_channel(channelsList);
        string key = extract_next_key(keysList);
        
        if (!is_channel_valid(channelName, fd)) continue;
        if (!is_user_allowed_in_channel(channelName, key, fd)) continue;

        if (is_channel_empty(channelName))
            create_channel(channelName);

        if (is_user_already_in_channel(channelName, fd)) continue;
        
        add_user_to_channel(channelName, fd);
        notify_channel_join(channelName, fd);
        send_topic_to_user(channelName, fd);
        send_user_list_to_user(channelName, fd);
        send_channel_modes_to_user(channelName, fd);
    }
}

string Server::extract_channels_list(const string &buf)
{
    size_t start = buf.find_first_not_of(" \t\r\n", 5);
    if (start != string::npos)
	{
        size_t end = buf.find_first_of(" \t\r\n", start);
        return buf.substr(start, end - start);
    }
    return "";
}

string Server::extract_keys_list(const string &buf)
{
    size_t start = buf.find_first_of(" \t\r\n", buf.find_first_not_of(" \t\r\n", 5));
    start = buf.find_first_not_of(" \t\r\n", start);
    if (start != string::npos)
	{
        size_t end = buf.find_first_of(" \t\r\n", start);
        return buf.substr(start, end - start);
    }
    return "";
}

int Server::count_channels(const string &channelsList)
{
    return 1 + count(channelsList.begin(), channelsList.end(), ',');
}

string Server::extract_next_channel(string &channelsList)
{
    size_t pos = channelsList.find(",");
    string channelName = channelsList.substr(0, pos);
    channelsList.erase(0, pos + 1);
    return channelName;
}

string Server::extract_next_key(string &keysList)
{
    size_t pos = keysList.find(",");
    string key = keysList.substr(0, pos);
    keysList.erase(0, pos + 1);
    return key;
}

bool Server::is_channel_valid(const string &channelName, int fd)
{
	cout << __FUNCTION__ << ": Entree dans la fonction" << endl;
    if (!channelNameInvalid(channelName))
    {
		cout << __FUNCTION__ << ":Entree dans le premier if" << endl;
        this->_sendMessage(channelName + ":Erroneous Channel Name",
            this->clientfd[fd].fd);
        return false;
    }
    User* user = find_user(fd);
    if (user == NULL)
    {
		cout << __FUNCTION__ << ":Entree dans le deuxieme if" << endl;

        cerr << "Error: User not found for fd " << fd << endl;
        return false;
    }
    if (user->ls_channel() > 10)
    {
		cout << __FUNCTION__ << ":Entree dans le troisieme if" << endl;

        this->_sendMessage(send_codes(405, this, user, channelName, ""),
            this->clientfd[fd].fd);
        return false;
    }
    return true;
}

bool Server::is_user_allowed_in_channel
(const string &channelName, const string &key, int fd)
{
    map<string, Channel*>::iterator channelIt = this->getChannels().find(channelName);
    if (channelIt == this->getChannels().end())
        return true;
    Channel *channel = channelIt->second;
    if ((channel->get_mode().find("k") != string::npos && !is_key_valid(channel, key, fd))
        || (channel->get_mode().find("l") != string::npos && is_channel_full(channel, fd))
        || (channel->get_mode().find("i") != string::npos && !is_user_invited(channel, fd)))
        return false;
    return true;
}

bool Server::is_key_valid(Channel *channel, const string &key, int fd)
{
    if (key.empty() && channel->get_key().compare("") != 0)
	{
        this->_sendMessage(send_codes(461, this, find_user(fd),
            channel->get_channelname(), ""), this->clientfd[fd].fd);
        return false;
    }
    if (channel->get_key().compare(key) != 0 && channel->get_key().compare("") != 0)
	{
        this->_sendMessage(send_codes(475, this, find_user(fd),
            channel->get_channelname(), ""), this->clientfd[fd].fd);
        return false;
    }
    return true;
}

bool Server::is_channel_full(Channel *channel, int fd)
{
    if (channel->get_maxUser() <= channel->getUsersNb())
	{
        this->_sendMessage(send_codes(471, this, find_user(fd),
            channel->get_channelname(), ""), this->clientfd[fd].fd);
        return true;
    }
    return false;
}

bool Server::is_user_invited(Channel *channel, int fd)
{
    vector<int> invitedList = channel->get_invited();
    if (find(invitedList.begin(), invitedList.end(), this->clientfd[fd].fd) ==
        invitedList.end())
	{
        this->_sendMessage(send_codes(473, this, find_user(fd),
            channel->get_channelname(), ""), this->clientfd[fd].fd);
        return false;
    }
    return true;
}

bool Server::is_channel_empty(const string &channelName)
{
    return this->getChannels().find(channelName) == this->getChannels().end();
}

void Server::create_channel(const string &channelName)
{
    Channel *newChannel = new Channel(channelName);
    this->setChannels(channelName, newChannel);
}

bool Server::is_user_already_in_channel(const string &channelName, int fd)
{
    return this->getChannels().find(channelName)->second->
        searchuserbyname(find_user(fd)->get_nickname()) != -1;
}

void Server::add_user_to_channel(const string &channelName, int fd)
{
    Channel *channel = this->getChannels().find(channelName)->second;
    if (channel->getUsersNb() == 0)
        channel->addChanops(this->clientfd[fd].fd, find_user(fd));
    else
        channel->addUser(this->clientfd[fd].fd, find_user(fd));
    find_user(fd)->addchannel(channelName);
}

void Server::notify_channel_join(const string &channelName, int fd)
{
    string userAnswer = print_user(find_user(fd));
    userAnswer += "JOIN " + channelName;
    if (this->getChannels().find(channelName)->second->get_mode().find("a") ==
        string::npos)
        sendtoeveryone(userAnswer, this->getChannels().find(channelName)->second);
}

void Server::send_topic_to_user(const string &channelName, int fd)
{
    if (this->getChannels().find(channelName)->second->get_topic().empty())
        this->_sendMessage(send_codes(331, this, find_user(fd), channelName, ""),
            this->clientfd[fd].fd);
    else
	{
        this->_sendMessage(send_codes(332, this, find_user(fd), channelName,
            this->getChannels().find(channelName)->second->get_topic()),
            this->clientfd[fd].fd);
    }
}

void Server::send_user_list_to_user(const string &channelName, int fd)
{
	string listOfUser = this->getChannels().find(channelName)->
        second->get_userlistinchan();
	if (this->getChannels().find(channelName)->second->get_mode().find("a") ==
        string::npos)
	{
		this->_sendMessage(send_codes(353, this, find_user(fd),
            channelName, listOfUser), this->clientfd[fd].fd);
		this->_sendMessage(send_codes(366, this, find_user(fd),
            channelName, ""), this->clientfd[fd].fd);
	}
}

void Server::send_channel_modes_to_user(const string &channelName, int fd)
{
	if (!this->getChannels().find(channelName)->second->get_mode().empty())
	{
		this->_sendMessage(send_codes(324, this, find_user(fd), channelName,
            this->getChannels().find(channelName)->second->get_mode()),
            this->clientfd[fd].fd);
	}
}
