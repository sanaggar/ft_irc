#include "../Server.hpp"

void Server::pong_command(string tmp, int i)
{
    size_t spacepos = tmp.find(" ");
    if (spacepos != string::npos) 
    {
        string pongMessage = "PONG " + tmp.substr(spacepos + 1);
        this->_sendMessage(pongMessage, this->clientfd[i].fd);
        cout << "\033[1;34mPing sent by client, Pong received by server\033[0m\n";
    }
}