#include <cstddef>
#include <iostream>
#include <cstring>
#include <vector>
#include <algorithm>

using namespace std;

void removeSpaces(string &str) {
    str.erase(remove_if(str.begin(), str.end(), ::isspace), str.end());
}

bool nicknameIsValid(string nick)
{
    string letter = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    string special = "[]\\`_^{|}";
    string digit = "0123456789";
    string minus = "-";
    string firstChar = letter + special;
    string all = letter + special + digit + minus;
    if (nick.size() > 9)
        return (false);
    if (firstChar.find(nick[0]) == string::npos)
        return (false);
    for (int i = 1; nick[i] && i < 9; i++)
    {
        if (all.find(nick[i]) == string::npos)
            return (false);
    }
    return (true);
}


bool checkInvalidCharacter(char c)
{
    return (c == '\0' || c == '\a' || c == '\r' || c == '\n' || c == ' ' || c == ',' || c == ':');
}

bool channelNameInvalid(string name)
{
    if (name.size() > 50)
        return false;
    string::iterator it = name.begin();
    string channelId = "#&+";
    if (!name.empty() && channelId.find(name[0]) == string::npos)
        return false;
    for (; it != name.end() && !checkInvalidCharacter(*it); it++);
    return (it == name.end());
}
