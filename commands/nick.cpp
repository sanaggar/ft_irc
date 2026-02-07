#include "../Server.hpp"

void Server::nick_command(string buffer, int clientFd)
{
    size_t startPos = 0;
    string newNickname = "";
    string trimmedBuffer(buffer);

    if ((startPos = trimmedBuffer.find_first_not_of(" \t\r\n", 5)) != string::npos)
        newNickname = trimmedBuffer.substr(startPos, trimmedBuffer.find_first_of(" \t\r\n", startPos) - startPos);

    if (newNickname.empty())
    {
        this->_sendMessage(send_codes(431, this, find_user(clientFd), "", ""), this->clientfd[clientFd].fd);
        return;
    }

    if (!nicknameIsValid(newNickname))
    {
        this->_sendMessage(send_codes(432, this, find_user(clientFd), newNickname, ""), this->clientfd[clientFd].fd);
        return;
    }

    if (this->nicknameAlreadyUse(newNickname))
    {
        this->_sendMessage(send_codes(433, this, find_user(clientFd), newNickname, ""), this->clientfd[clientFd].fd);
        return;
    }

    string notificationMessage = print_user(find_user(clientFd));
    notificationMessage += "NICK " + newNickname;
    this->_sendMessage(notificationMessage, this->clientfd[clientFd].fd);

    find_user(clientFd)->setnickname(newNickname);
}