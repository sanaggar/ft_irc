#include "Server.hpp"


/**
 * Constructs a formatted message string based on the provided code and user information.
 * This function uses different codes to format and return various server responses and error messages.
 * It also includes user-specific information such as nickname, username, and hostname if available.
 * Returns the formatted message string.
 */
string send_codes(int code, Server *serv, User *usr, string buf1, string buf2)
{
    (void) buf2;
    stringstream ss;
    ss << code;
    string codestr = ss.str();
    if (code < 10)
        codestr.insert(0, 2, '0');
    else if (code < 100)
        codestr.insert(0, 1, '0');

    string ret;

    if (usr)
        ret += ":" + serv->getServername() + " " + codestr + " " + usr->get_nickname() + " ";
    else
        ret += ":" + serv->getServername() + " " + codestr + " " + "*" + " ";
    
    switch (code)
    {
        case 001:
            ret += RPL_WELCOME(usr->get_nickname(), usr->get_username(), usr->get_hostname());			
		    	break;
		case 002:
            ret += RPL_YOURHOST(usr->get_hostname());
		    	break;
        case 003:
            ret += RPL_CREATED(serv->getServername());
            break;
        case 004:
            ret += RPL_MYINFO(usr->get_hostname());
		    	break;
        case 005:
            ret += RPL_BOUNCE(serv->getServername(), serv->getPort());
                break;
        case 461:
            ret += ERR_NEEDMOREPARAMS(buf1);
			    break;        
        case 403:
            ret += ERR_NOSUCHCHANNEL(buf1);
                break;
        case 405:
            ret += ERR_TOOMANYCHANNELS(buf1);
                break;
        case 441:
            ret += ERR_USERNOTINCHANNEL(buf1, buf2);
                break;
        case 442:
            ret += ERR_NOTONCHANNEL(buf1);
				break;
        case 431:
            ret += ERR_NONICKNAMEGIVEN;
		    	break;
        case 432:
            ret += ERR_ERRONEUSNICKNAME(buf1);
			    break;
        case 433:
            ret += ERR_NICKNAMEINUSE(buf1);
			    break;
        case 331:
            ret += RPL_NOTOPIC(buf1);
		    	break;
        case 353:
            ret += RPL_NAMREPLY(buf1, buf2);
			    break;
        case 366:
            ret += RPL_ENDOFNAMES(buf1);
			    break;   
        case 401:
            ret += ERR_NOSUCHNICK(buf1);
			    break;
        case 381:
            ret += RPL_YOUREOPER;
			    break;
        case 464:
            ret += ERR_PASSWDMISMATCH;
		    	break;
        case 482:
            ret += ERR_CHANOPRIVSNEEDED(buf1);
		    	break;
        case 484:
            ret += ERR_RESTRICTED;
			    break;
        case 324:
            ret += RPL_CHANNELMODEIS(buf1, buf2);
                break;
        case 332:
            ret += RPL_TOPIC(buf1, buf2);
			    break;
        case 471:
            ret += ERR_CHANNELISFULL(buf1);
			    break;
        case 475:
            ret += ERR_BADCHANNELKEY(buf1);
			    break;
        case 467:
            ret += ERR_KEYSET(buf1);
                break;
        case 472:
            ret += ERR_UNKNOWNMODE(buf1, buf2);
                break;
        case 341:
            ret += RPL_INVITING(buf1, buf2);
                break;
        case 443:
            ret += ERR_USERONCHANNEL(buf1, buf2);
                break;
        case 473:
            ret += ERR_INVITEONLYCHAN(buf1);
				break;
    }

    return ret;
}

/**
 * Constructs and returns a string representing the user's full identifier.
 * The format is :nickname!username@hostname, which is used for identifying users in IRC messages.
 * This is a standard format in IRC for specifying the full identity of a user.
 */
string print_user(User *usr)
{
    return (":" + usr->get_nickname() + "!" + usr->get_username() + "@" + usr->get_hostname() + " ");
}

/**
 * Sends a message to all users in a channel except for the user with the specified socket descriptor.
 * This function iterates through both regular users and channel operators in the channel.
 * For each user, if their socket descriptor does not match the specified one, the message is sent to them.
 */
void Server::sendinchanexceptuser(string message, Channel *chan, int sd)
{
    for (map<int, User *>::iterator it = chan->getUsers().begin(); it != chan->getUsers().end(); it++)
    {
        if (sd != it->first)
            this->_sendMessage(message, it->first);
    }
    for (map<int, User *>::iterator it = chan->getChanOps().begin(); it != chan->getChanOps().end(); it++)
    {
        if (sd != it->first)
            this->_sendMessage(message, it->first);
    }

}

/**
 * Sends a message to all users and channel operators in a channel.
 * This function iterates through both regular users and channel operators in the channel.
 * For each user and operator, the message is sent to them.
 * This is useful for broadcasting messages to the entire channel.
 */
void Server::sendtoeveryone(const string &message, Channel *chan)
{
    for (map<int, User *>::iterator it = chan->getUsers().begin(); it != chan->getUsers().end(); ++it)
        this->_sendMessage(message, it->first);
    for (map<int, User *>::iterator it = chan->getChanOps().begin(); it != chan->getChanOps().end(); ++it)
        this->_sendMessage(message, it->first);
}

string getCurrentDate()
{
    time_t t = time(0);
    tm* now = localtime(&t);
    ostringstream oss;
    oss << (now->tm_mday < 10 ? "0" : "") << now->tm_mday << "/"
        << ((now->tm_mon + 1) < 10 ? "0" : "") << (now->tm_mon + 1) << "/"
        << (now->tm_year + 1900) % 100;
    return oss.str();
}
