#include "Channel.hpp"

Channel::Channel(string name) :_name(name) {}

Channel::~Channel()
{
    _users.clear();
    _chanUserOps.clear();
    _invited.clear();
}


void Channel::addChanops(int fd, User *usr) {this->_chanUserOps.insert(make_pair(fd, usr));}
void Channel::addUser(int fd, User *usr) {this->_users.insert(make_pair(fd, usr));}

string Channel::get_userlistinchan()
{
    string output;
    for (map<int, User *>::iterator it = this->_chanUserOps.begin(); it != this->_chanUserOps.end(); it++)
    {
        if (!output.empty())
            output += " ";
        output += "@";
        output += it->second->get_nickname();
    }
    for (map<int, User *>::iterator it = this->_users.begin(); it != this->_users.end(); it++)
    {
        if (!output.empty())
            output += " ";
        output += it->second->get_nickname();
    }
    return (output);
}

bool    Channel::isChanop(int fd)
{
    if (this->_chanUserOps.find(fd) != this->_chanUserOps.end())
        return (true);
    return (false);
}

void    Channel::rm_user(int fd)
{
    map<int, User*>::iterator it;

    if ((it = this->_users.find(fd)) != this->_users.end())
        this->_users.erase(it);
    else if ((it = this->_chanUserOps.find(fd)) != this->_chanUserOps.end())
        this->_chanUserOps.erase(it);
}

int Channel::searchuserbyname(string nickname)
{
    for (map<int, User*>::iterator it = this->_users.begin(); it != this->_users.end(); it++)
    {
        if (nickname.compare(it->second->get_nickname()) == 0)
            return it->first;
    }
    for (map<int, User*>::iterator it = this->_chanUserOps.begin(); it != this->_chanUserOps.end(); it++)
    {
        if (nickname.compare(it->second->get_nickname()) == 0)
            return it->first;
    }
    return (-1);
}


map<int, User*> & Channel::getChanOps() {return this->_chanUserOps;}
map<int, User*> & Channel::getUsers() {return this->_users;}
int Channel::getUsersNb() {return (this->_users.size() + this->_chanUserOps.size());}
string Channel::get_channelname() {return (_name);}
vector<int> Channel::get_invited() {return (this->_invited);}
string Channel::get_key() {return this->_key;}
int Channel::get_maxUser() {return this->_maxUser;}
string Channel::get_mode() {return this->_mode;}
string Channel::get_topic() {return this->_topic;}

void    Channel::set_key(string key)
{
    this->_key = key;
    cout << "Set key in channel" << this->get_channelname() << "to : " << key << endl;
}
void    Channel::set_invited(int fd) {this->_invited.push_back(fd);}
void    Channel::set_maxUser(int maxUser)
{
    this->_maxUser = maxUser;
    cout << "Max user set to " << this->_maxUser << endl;
}
void    Channel::set_mode(string mode) {this->_mode = mode;}
void    Channel::set_topic(string topic) {this->_topic = topic;}
