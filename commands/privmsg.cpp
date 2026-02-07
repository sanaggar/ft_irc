#include "../Server.hpp"

void Server::priv_msg(string buffer, int fd)
{
    int command_offset = buffer.compare(0, 6, "NOTICE") == 0 ? 7 : 8;
    size_t target_start = buffer.find_first_not_of(" \t\r\n", command_offset);

    if (target_start == string::npos)
	{
        return; // Invalid message format
    }

    string msgtarget = buffer.substr(target_start, buffer.find_first_of(" \t\r\n", target_start) - target_start);
    size_t message_start = buffer.find_first_of(" \t\r\n", target_start);
    message_start = buffer.find_first_not_of(" \t\r\n", message_start);

    string message;
    if (message_start != string::npos)
	{
        message = buffer.substr(message_start, buffer.find_first_of("\r\n", message_start) - message_start);
    }

    string id_of_channel_prefix = "#&+";
    string sender_message = print_user(find_user(fd)) + "PRIVMSG " + msgtarget + " " + message;
    removeSpaces(msgtarget);

    if (!msgtarget.empty() && id_of_channel_prefix.find(msgtarget[0]) != string::npos)
	{
        handle_channel_message(msgtarget, sender_message, fd);
    } else
	{
        handle_private_message(msgtarget, sender_message, fd);
    }
}

void Server::handle_channel_message(const string& channel_name, const string& sender_message, int sender_fd)
{
    if (this->getChannels().find(channel_name) == this->getChannels().end())
	{
        this->_sendMessage(send_codes(401, this, find_user(sender_fd), channel_name, ""), this->clientfd[sender_fd].fd);
    } else
	{
        this->sendinchanexceptuser(sender_message, this->getChannels().find(channel_name)->second, this->clientfd[sender_fd].fd);
    }
}

void Server::handle_private_message(const string& recipient_nickname, const string& sender_message, int sender_fd)
{
    int recipient_fd = this->searchUserby_nickname(recipient_nickname);
    if (recipient_fd == -1)
	{
        this->_sendMessage(send_codes(401, this, find_user(sender_fd), recipient_nickname, ""), this->clientfd[sender_fd].fd);
    } else
	{
        this->_sendMessage(sender_message, recipient_fd);
    }
}
