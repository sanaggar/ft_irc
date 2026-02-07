#include "Server.hpp"

/**
 * Validates the nickname from the client's message.
 * Checks if the "NICK" command is present and if the provided nickname is valid and not already in use.
 * Clears buffers and sends appropriate error messages if validation fails.
 * Returns true if the nickname is valid, otherwise returns false.
 */
bool Server::validate_nickname(const string &ret, int client_socket, string &nick)
{
	size_t occ;
	size_t firstOcc;

	if ((occ = ret.find("NICK ")) != string::npos)
	{
		if ((firstOcc = ret.find_first_not_of(" \t\r\n", occ + 5)) == string::npos)
		{
			clear_buffers();
			this->_sendMessage(send_codes(432, this, NULL, nick, ""), client_socket);
		}
		else
		{
			nick = ret.substr(firstOcc, ret.find_first_of("\r\n", firstOcc) - firstOcc);
			nick = nick.substr(0, nick.find_last_not_of(" \t\r\n", nick.size()) + 1);
			if (!nicknameIsValid(nick))
			{
				clear_buffers();
				this->_sendMessage(send_codes(432, this, NULL, nick, ""), client_socket);
			}
			else if (nicknameAlreadyUse(nick))
			{
				this->_sendMessage(send_codes(433, this, NULL, nick, ""), client_socket);
				this->_sendMessage("Please try reconnect with an available nickname.", client_socket);
				close(client_socket);
			}
			else
				return true;
		}
	}
	else 
		this->_sendMessage("You have to enter a nickname\nUsage: NICK [nickname]", client_socket);
	return false;
}

/**
 * Validates the password from the client's message.
 * Checks if the "PASS" command is present and the provided password matches the server's password.
 * Sends appropriate error messages if validation fails.
 * Returns true if the password is correct, otherwise returns false.
 */
bool Server::validate_password(const string &ret, int client_socket)
{
	size_t occ;
	size_t firstOcc;
	string pass;

	if ((occ = ret.find("PASS ")) != string::npos)
	{
		if ((firstOcc = ret.find_first_not_of(" \t\r\n", occ + 5)) == string::npos)
			this->_sendMessage(send_codes(461, this, NULL, "PASS", ""), client_socket);
		else
		{
			string sep = " \t\r\n";
			for (int i = 0; ret[firstOcc + i] && sep.find(ret[firstOcc + i]) == string::npos; i++)
				pass += ret[firstOcc + i];
			
			if (pass.empty())
				this->_sendMessage(send_codes(461, this, NULL, "PASS", ""), client_socket);
			else if (pass.compare(this->_password) != 0)
				this->_sendMessage("WRONG PASSWORD", client_socket);
			else
				return true;
		}
	}
	else
		this->_sendMessage("You need to enter a pass!", client_socket);
	return false;
}

/**
 * Validates the USER command from the client's message.
 * Checks if the "USER" command is present and extracts user details.
 * Sends appropriate error messages if validation fails.
 * Returns true if the user details are valid, otherwise returns false.
 */
bool Server::validate_user(const string &ret, int client_socket,
string &user, string &host, string &serverName, string &realName)
{
	size_t occ;
	size_t firstOcc;
	int i = 0;

	if ((occ = ret.find("USER ")) != string::npos)
	{
		if ((firstOcc = ret.find_first_not_of(" \t\r\n", occ + 5)) == string::npos)
			this->_sendMessage(send_codes(461, this, NULL, "USER", ""), client_socket);
		else
		{
			user = ret.substr(firstOcc, (i = ret.find_first_of(" \t\r\n", firstOcc)) - firstOcc);
			if ((firstOcc = ret.find_first_not_of(" \t\r\n", i)) == string::npos)
				this->_sendMessage(send_codes(461, this, NULL, "USER", ""), client_socket);
			else
			{
				host = ret.substr(firstOcc, (i = ret.find_first_of(" \t\r\n", firstOcc)) - firstOcc);
				if ((firstOcc = ret.find_first_not_of(" \t\r\n", i)) == string::npos)
					this->_sendMessage(send_codes(461, this, NULL, "USER", ""), client_socket);
				else
				{
					serverName = ret.substr(firstOcc, (i = ret.find_first_of(" \t\r\n", firstOcc)) - firstOcc);
					if ((firstOcc = ret.find_first_not_of(" \t\r\n", i)) == string::npos)
						this->_sendMessage(send_codes(461, this, NULL, "USER", ""), client_socket);
					else
					{
						realName = ret.substr(firstOcc, (i = ret.find_first_of(" \t\r\n", firstOcc)) - firstOcc);
						realName = realName.substr(0, realName.find_last_not_of(" \t\r\n", realName.size()) + 1);
						return !(user.empty() || host.empty() || serverName.empty() || realName.empty());
					}
				}
			}
		}
	}
	return false;
}
