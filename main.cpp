#include "Server.hpp"

bool isRunning = true;

void    handler(int signum)
{
	(void)signum;
	isRunning = false;
}

bool running_status() {return isRunning;}

int passwordParsing(const string& str)
{
	if (str.size() != 4) {return 1;}
	for (int i = 0;str[i]; i++)
		if (isdigit(str[i]) == 0) {return 1;}
	return 0;
}

int portParsing(char *av1)
{
	return 0;
	if (strlen(av1) != 4)
		return 1;
	int port = static_cast<int>(strtoul(av1, NULL, 0));
	if ((port >= 6660 && port <= 6669) || port == 6679 || port == 6697)
		return 0;
	return 1;
}


int main(int ac, char **av)
{
	try
	{
		if (ac != 3) {throw runtime_error(": ./ircserv <port> <password>");}
		if (portParsing(av[1]) == 1) {throw runtime_error(" Wrong port.");}
		if (passwordParsing(av[2]) == 1) {throw runtime_error(" Wrong password.");}

		Server server(av[1], av[2]);
		server.av = av;
		server.setserversocket();
		while (isRunning == true)
		{
    		signal(SIGINT, handler);
			for (int i = 1; i <= MAX_CLIENTS; i++)
			{
				int numReady = poll(server.clientfd, MAX_CLIENTS + 1, -1);
				if (numReady == -1) {break;}

				if (server.clientfd[0].revents & POLLIN)
					server.acceptClientconnexion();

				if (server.clientfd[i].revents & POLLIN)
					server.handling_server_msg(i);
			}
		}
	}
	catch(const exception& e) {cerr << e.what() << endl;}

	return (EXIT_SUCCESS);
}

//oui