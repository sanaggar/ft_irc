#include "Server.hpp"

map<int, TempUserInfo> tempUserInfo;

Server::Server(const string& port, const string& password) : _password(password),_port(port) {}

Server::~Server()
{
	for (map<string, Channel*>::iterator it = _channels.begin(); it != _channels.end(); ++it)
		delete it->second;
	if (!_channels.empty()) {_channels.clear();}

	for (map<int, User*>::iterator it = _users.begin(); it != _users.end(); ++it)
		delete it->second;
	if (!_users.empty()) {_users.clear();}
	if (!_buffer_sd.empty()) {_buffer_sd.clear();}
	close(this->_serverSocket);
}

void    Server::error(const char *msg)
{perror(msg); exit(EXIT_FAILURE);}

bool    Server::isThereanUser(int fd)
{
	if (this->_users.find(fd) == this->_users.end())
		return (false);
	return (true);
}

User*	Server::find_user(int fd) {return (this->get_users()[this->clientfd[fd].fd]);}

User*	Server::find_user_byfd(int fd) {return (this->get_users()[fd]);}

/**
 * Checks if a nickname is already in use.
 * Returns true if the nickname is found, false otherwise.
 */
bool Server::nicknameAlreadyUse(string nick)
{
	for (map<int, User *>::iterator it = this->get_users().begin(); it != this->get_users().end(); it++)
	{
		if (nick.compare(it->second->get_nickname()) == 0)
			return (true);
	}
	return (false);
}

/**
 * Clears the server's buffers.
 * Resets the buffers for incoming client data to prepare for the next operation.
 * Clears internal buffer strings and maps associated with the new client socket.
 */
void Server::clear_buffers()
{
	this->get_bufferstr().clear();
	this->getBufferSd().find(this->_newClientSocket)->second.clear();
	_newbuff.clear();
	_strbuffer.clear();
}


/**
 * Creates and adds a user to the server.
 * Initializes a new User object with provided details and adds it to the server's collection.
 * Sends welcome messages to the client upon successful creation.
 */
void Server::create_and_add_user
(const string &nick,
const string &user,
const string &host,
const string &realName,
int client_socket)
{
	this->_newbuff.erase(_newbuff.begin(), _newbuff.end());
	User *newUser = new User(nick, user, host, realName);
	this->setUsers(client_socket, newUser);
	cout << "Number of user connected on the server: " << this->_users.size() << endl;
	this->_sendMessage(send_codes(001, this, newUser, "", ""), client_socket);
	this->_sendMessage(send_codes(002, this, newUser, "", ""), client_socket);
	this->_sendMessage(send_codes(003, this, newUser, "", ""), client_socket);
	this->_sendMessage(send_codes(004, this, newUser, "", ""), client_socket);
}

/**
 * Checks the connection and validates the client's credentials.
 * Validates the password, nickname, and user details from the client's message.
 * Creates and adds a new user to the server if all validations are successful.
 * Sends appropriate error messages and clears buffers if validations fail.
 */
void Server::check_connection()
{
    string ret(this->_newbuff + this->_strbuffer);
    string nick = "", user = "", host = "", serverName = "", realName = "";
    TempUserInfo &info = tempUserInfo[this->_newClientSocket];

    bool isPassGood = !info.pass.empty();
    cout << "check_connection: isPassGood: " << isPassGood << endl;
    bool isNickGood = !info.nick.empty();
    cout << "check_connection: isNickGood: " << isNickGood << endl;
    bool isUserGood = !info.user.empty() && !info.host.empty() && !info.realname.empty();
    cout << "check_connection: isUserGood: " << isUserGood << endl;

    if (isPassGood && isNickGood && isUserGood && running_status()) {
        createUserFromTempInfo(this->_newClientSocket, info);
    } else {
        cout << "check_connection: Validation failed - clearing buffers" << endl;
        clear_buffers();
    }
}

/**
 * Determines if the client is making a first connection.
 * Checks for the presence of initial connection commands (CAP LS, PASS, NICK, USER).
 * Returns true if any of these commands are present, indicating a first connection.
 */
bool	Server::firstConnection(int i)
{

	if (isThereanUser(this->clientfd[i].fd) == true)
		return false;

	string buffer_str(_strbuffer);

	bool hasCapLS = buffer_str.find("CAP LS") != string::npos;
	bool hasPASS = buffer_str.find("PASS ") != string::npos;
	bool hasNick = buffer_str.find("NICK ") != string::npos;
	bool hasUser = buffer_str.find("USER ") != string::npos;

	return (hasCapLS == true || hasNick == true || hasUser == true || hasPASS == true);
}

/**
 * Searches for a user by their nickname.
 * Iterates through the user map to find and return the file descriptor of the user with the given nickname.
 * Returns -1 if the user is not found.
 */
int Server::searchUserby_nickname(const string& nickname)
{
	for (map<int, User*>::iterator it = this->_users.begin(); it != this->_users.end(); it++)
	{
		if (nickname.compare(it->second->get_nickname()) == 0)
			return it->first;
	}
	return (-1);
}

/**
 * Sends a message to a client.
 * Appends CRLF to the message and sends it to the client specified by the socket descriptor.
 * Throws a runtime error if sending the message fails.
 */
void Server::_sendMessage(string message, int sd)
{
    if (running_status() == true)
    {
        message += "\r\n";
        if (send(sd, message.c_str(), message.length(), 0) < 0)
            throw runtime_error("Error sending message.");

        if (message.find("PONG") == string::npos)
            cout << "Server: " << message;
    }
}

/**
 * Sets up the server socket.
 * Initializes and configures the server's listening socket.
 * Binds the socket to the specified port and sets it to non-blocking mode.
 * Prepares the server to accept incoming client connections.
 */
void	Server::setserversocket()
{
	this->_serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (this->_serverSocket < 0)
		error("Error opening socket");

	this->_portnb = atoi(this->av[1]);

	memset(&this->_servAddr, 0, sizeof(this->_servAddr));
	this->_servAddr.sin_family = AF_INET;
	this->_servAddr.sin_addr.s_addr = INADDR_ANY;
	this->_servAddr.sin_port = htons(this->_portnb);

	this->_cliLen = sizeof(this->_cliAddr);
	if (bind(this->_serverSocket, (struct sockaddr *) &this->_servAddr, sizeof(this->_servAddr)) < 0)
		error("Binding failed");

	listen(this->_serverSocket, 100); // 100 is the number max of clients can connect simultanately

	memset(this->clientfd, 0, sizeof(this->clientfd));

	this->clientfd[0].fd = this->_serverSocket;
	this->clientfd[0].events = POLLIN;

	int flags = fcntl(this->_serverSocket, F_GETFL);
	fcntl(this->_serverSocket, F_SETFL, flags | O_NONBLOCK);
}

/**
 * Handles the disconnection of a user.
 * Clears the user's channels, deletes the user from the server's user map, and closes the socket.
 * Ensures proper cleanup of resources when a user disconnects.
 */
void Server::user_disconnect(int sd)
{
	if (isThereanUser(sd) == false)
	{
		close(sd);
		return ;
	}
	this->get_users().find(sd)->second->get_channels().clear();
	delete this->get_users().find(sd)->second;
	this->get_users().erase(sd);
	close(sd);
}

/**
 * Accepts a new client connection.
 * Handles incoming client connections and sets up necessary structures.
 * Configures the new client socket to non-blocking mode and adds it to the pollfd array.
 */
void	Server::acceptClientconnexion()
{
	if (DEBUG) {cout << "[DEBUG] " << __FUNCTION__ << ": Entry" << endl;}

	this->_newClientSocket = accept(this->_serverSocket, (struct sockaddr*)&this->_cliAddr, &this->_cliLen);
		if (this->_newClientSocket == -1)
			perror("Error accepting a connection");
		else
		{
			cout << "Server has detected a connection !" << endl;
			int clientFlags = fcntl(this->_newClientSocket, F_GETFL, 0);
			fcntl(this->_newClientSocket, F_SETFL, clientFlags | O_NONBLOCK);

			this->setBufferSd(this->_newClientSocket, "");

			for (int i = 1; i <= MAX_CLIENTS; i++)
			{
				if (this->clientfd[i].fd == 0)
				{
					this->clientfd[i].fd = this->_newClientSocket;
					this->clientfd[i].events = POLLIN;
					break;
				}
			}
		}
}

/**
 * Receives and processes a message from a client.
 * Determines if the client is making a first connection or is an existing user.
 * Handles various IRC commands (PING, PART, JOIN, PRIVMSG, NICK, OPER, TOPIC, KICK, MODE, INVITE) based on the client's message.
 * Ensures appropriate actions and responses for each command.
 */
void Server::recvClientMsg(int i)
{
    bool firstconnection = firstConnection(i);

    if (this->clientfd[i].fd == 0)
        return;

    cout << "recvClientMsg: Client " << i << " - firstConnection: " << firstconnection << endl;

    string tmp(_strbuffer);
    TempUserInfo &info = tempUserInfo[this->clientfd[i].fd];

    if (firstconnection == true && running_status() == true) {
        if (tmp.find("PASS ") == 0) {
            handlePassCommand(info, tmp);
        } else if (tmp.find("NICK ") == 0) {
            handleNickCommand(info, tmp);
        } else if (tmp.find("USER ") == 0) {
            handleUserCommand(info, tmp);
        } else {
            this->_sendMessage("You need to enter a pass!", this->clientfd[i].fd);
        }
        check_connection();
    } else if (firstconnection == false && running_status() == true) {
        cout << "recvClientMsg: Processing command: " << tmp << endl;

        if (tmp.find("PING ") != string::npos) {
            pong_command(tmp, i);
        } else if (tmp.find("PART ") != string::npos) {
            part_command(tmp, i);
        } else if (tmp.find("JOIN ") != string::npos) {
            join_command(tmp, i);
        } else if (tmp.find("PRIVMSG ") != string::npos) {
            priv_msg(tmp, i);
        } else if (tmp.find("NICK ") != string::npos) {
            nick_command(tmp, i);
        } else if (tmp.find("OPER ") != string::npos) {
            oper_command(tmp, i);
        } else if (tmp.find("TOPIC ") != string::npos) {
            topic_command(tmp, i);
        } else if (tmp.find("KICK ") != string::npos) {
            kick_command(tmp, i);
        } else if (tmp.find("MODE ") != string::npos) {
            mode_channel_handler(tmp, i);
        } else if (tmp.find("INVITE ") != string::npos) {
            invite_command(tmp, i);
        }
    }

}

int	Server::get_newClientSocket() {return (this->_newClientSocket);}
struct pollfd	Server::get_clientfd() {return (*this->clientfd);}
map<int, string> & Server::getBufferSd() {return this->_buffer_sd;}
map<int, User*> & Server::get_users() {return this->_users;}
map<string, Channel*> & Server::getChannels() {return this->_channels;}
string& Server::get_bufferstr() {return this->_strbuffer;}
const string&	Server::getServername() {return this->_serverName;}
const string& Server::getPort() {return this->_port;}

void	Server::setBufferSd(int sd, string buffer)
{this->_buffer_sd.insert(make_pair(sd, buffer));}
void	Server::setChannels(string name, Channel *channel)
{this->_channels.insert(make_pair(name, channel));}
void    Server::set_bufferstr(string str) {this->_strbuffer = str;}
void	Server::setServername(string servername) {this->_serverName = servername;}
void Server::setUsers(int fd, User *user) {this->_users.insert(make_pair(fd, user));}


/***************************************************************/

void Server::handlePassCommand(TempUserInfo &info, const string &command)
{
    istringstream iss(command);
    string cmd, pass;
    iss >> cmd >> pass;

    if (pass == this->_password) {
        info.pass = pass;
        info.hasPass = true;
    } else {
        this->_sendMessage("WRONG PASSWORD", this->_newClientSocket);
    }
}

void Server::handleNickCommand(TempUserInfo &info, const string &command)
{
    istringstream iss(command);
    string cmd, nick;
    iss >> cmd >> nick;

    if (nicknameIsValid(nick) && !nicknameAlreadyUse(nick)) {
        info.nick = nick;
        info.hasNick = true;
    } else {
        this->_sendMessage(send_codes(432, this, NULL, nick, ""), this->_newClientSocket);
    }
}

void Server::handleUserCommand(TempUserInfo &info, const string &command)
{
    istringstream iss(command);
    string cmd, user, mode, unused, realname;
    iss >> cmd >> user >> mode >> unused;
    getline(iss, realname);
    if (!realname.empty() && realname[0] == ':') {
        realname.erase(0, 1);
    }

    info.user = user;
    info.host = mode; // Assuming mode is host
    info.realname = realname;
    info.hasUser = true;
}

void Server::createUserFromTempInfo(int fd, TempUserInfo &info)
{
    User *newUser = new User(info.nick, info.user, info.host, info.realname);
    newUser->authenticate();
    this->setUsers(fd, newUser);
    tempUserInfo.erase(fd);

    cout << "Number of user connected on the server: " << this->_users.size() << endl;
    this->_sendMessage(send_codes(001, this, newUser, "", ""), fd);
    this->_sendMessage(send_codes(002, this, newUser, "", ""), fd);
    this->_sendMessage(send_codes(003, this, newUser, "", ""), fd);
    this->_sendMessage(send_codes(004, this, newUser, "", ""), fd);
}

void Server::handling_server_msg(int i)
{
    if (DEBUG) {cout << __FUNCTION__ << ": Entree" << endl;}

    string commandBuffer;
    char buffer[1024];
    int bytesRead;

    if (this->clientfd[i].fd <= 0) {
        cerr << "Invalid file descriptor for client " << i << endl;
        return;
    }

    bytesRead = recv(this->clientfd[i].fd, buffer, sizeof(buffer), 0);

    if (bytesRead > 0) {
        buffer[bytesRead] = '\0';
        cout << "Received data: " << buffer << endl;
    } else if (bytesRead == 0) {
        cout << "Client " << i << " has been disconnected" << endl;
        this->user_disconnect(this->clientfd[i].fd);
        memset(&this->clientfd[i], 0, sizeof(this->clientfd[i]));
        return;
    } else {
        cerr << "Error receiving data from client " << i << ": " << strerror(errno) << endl;
        this->user_disconnect(this->clientfd[i].fd);
        memset(&this->clientfd[i], 0, sizeof(this->clientfd[i]));
        return;
    }

    commandBuffer = buffer;
    if (!commandBuffer.empty() && commandBuffer.find('\n') == string::npos && running_status())
    {
        this->getBufferSd().find(this->clientfd[i].fd)->second += buffer;
    }
    else if (commandBuffer.find('\n') != string::npos && running_status() == true)
    {
        if (!this->getBufferSd().find(this->clientfd[i].fd)->second.empty())
        {
            this->set_bufferstr(this->getBufferSd().find(this->clientfd[i].fd)->second + commandBuffer);
            this->recvClientMsg(i);
            this->get_bufferstr().clear();
            this->getBufferSd().find(this->clientfd[i].fd)->second.clear();
        }
        else
        {
            this->set_bufferstr(commandBuffer);
            this->recvClientMsg(i);
            this->get_bufferstr().clear();
        }
    }
}
