#include "../Server.hpp"

void Server::part_command(string tmp, int i)
{
    string channelname = "";
    size_t q;

    if ((q = tmp.find_first_not_of(" \t\r\n", 5)) != string::npos)
        channelname = tmp.substr(q, (tmp.find_first_of(" \t\r\n", q) - q));
    if (channelname.empty())
    {
        _sendMessage(send_codes(461, this, find_user(i), "PART", ""), this->clientfd[i].fd);
        return;
    }

    string msg = "";
    size_t j = tmp.find_first_not_of(" \t\r\n", tmp.find_first_of(" \t\r\n", q));
    if (j != string::npos)
    {
        msg = tmp.substr(j, (tmp.find_first_of("\r\n", j) - j));
    }

    handle_part_channels(channelname, msg, i);
}

void Server::handle_part_channels(string &channelname, const string &msg, int i)
{
    int nbchannels = 1 + count(channelname.begin(), channelname.end(), ',');

    for (int t = 0; t < nbchannels; t++)
    {
        string chan_name = channelname.substr(0, channelname.find(","));
        channelname.erase(0, channelname.find(",") + 1);
        removeSpaces(chan_name);

        if (this->getChannels().find(chan_name) == this->getChannels().end())
            _sendMessage(send_codes(403, this, find_user(i), chan_name, ""), this->clientfd[i].fd);
        else if (find_user(i)->get_channels().find(chan_name) == find_user(i)->get_channels().end())
            _sendMessage(send_codes(442, this, find_user(i), chan_name, ""), this->clientfd[i].fd);
        else
        {
            process_user_part(chan_name, msg, i);
        }
    }
}

void Server::process_user_part(const string &chan_name, const string &msg, int i)
{
    string response = print_user(find_user(i));
    response += "PART " + chan_name;
    if (!msg.empty())
        response += " :" + msg;

    cout << "response :" << response << endl;
    
    // Send part message to all users in the channel, including the one who is leaving
    this->sendtoeveryone(response, this->getChannels().find(chan_name)->second);

    this->getChannels().find(chan_name)->second->rm_user(this->clientfd[i].fd);

    if (this->getChannels().find(chan_name)->second->getUsersNb() == 0)
    {
        delete this->getChannels().find(chan_name)->second;
        this->getChannels().erase(chan_name);
    }

    find_user(i)->get_channels().erase(chan_name);
}