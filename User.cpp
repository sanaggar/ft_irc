#include "User.hpp"

User::User(string nick, string user, string host, string realname):
_host(host), _nick(nick), _realname(realname), _user(user) {}

User::~User() {}

/**
 * Adds a channel to the user's list of channels.
 * If the channel is not already in the user's list, it inserts the channel name.
 */
void User::addchannel(string name)
{
    if (this->_channels.find(name) == this->_channels.end())
        this->_channels.insert(name);
}

size_t User::ls_channel() {return this->_channels.size();}

string User::get_nickname() {return (this->_nick);}
string User::get_username() {return (this->_user);}
string User::get_hostname() {return (this->_host);}
string User::get_realname() {return (this->_realname);}
set<string> &User::get_channels() {return (this->_channels);}
string User::get_mode() {return this->_mode;}

void	User::set_mode(string mode) {this->_mode = mode;}
void    User::setnickname(string name) {this->_nick = name;}
void    User::setuser(string name) {this->_user = name;}
void    User::sethost(string name) {this->_host = name;}
void    User::setrealname(string name) {this->_realname = name;}

void User::authenticate() {authenticated = true;}

bool User::isAuthenticated() const {return authenticated;}