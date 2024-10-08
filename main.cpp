#include "includes/irc.hpp"

bool	g_interrupt = false;

std::string	del_break(std::string str)
{
	if (str.empty())
		return (str);
	while (1)
	{
		if (str[str.size() - 1] == '\r')
			str = str.substr(0, (str.size() - 1));
		else if(str[str.size() - 1] == '\n')
			str = str.substr(0, (str.size() - 1));
		else if(str[str.size() - 1] == '\v')
			str = str.substr(0, (str.size() - 1));
		else if(str[str.size() - 1] == '\t')
			str = str.substr(0, (str.size() - 1));
		else if(str[str.size() - 1] == '\f')
			str = str.substr(0, (str.size() - 1));
		else
			break;
	}
	return (str);
}

std::string	ERROR_NEED_MORE_PARAMETERS(Client &client, std::string cmd)
{
	return ("461 " + client.getNickname() + " " + cmd + " :Not enough parameters\r\n");
}

void	signalHandler(int const signal)
{
	(void)signal;
	g_interrupt = true;
}

bool	getport(char *ac, int &port)
{
	char	*buffer;

	if (*ac == '\0')
		return (false);
	port = strtol(ac, &buffer, 10);
	if (*buffer != '\0')
	{
		std::cout << "Error input: port must contain only digits\r\n" << std::endl;
		return (false);
	}
	if (port < 0 || port > 65535)
	{
		std::cout << "Error input: port is out of range [0; 65535]\r\n" << std::endl;
		return (false);
	}
	return (true);
}
void f()
{
	system("leaks ircserv");
}
int main(int ac, char **av)
{
	int port;

	signal(SIGINT, signalHandler);
	signal(SIGQUIT, signalHandler);
	if (ac != 3)
	{
		std::cout << "Usage: ./ircserv <port> <password>\r\n" << std::endl;
		return (1);
	}
	if (getport(av[1], port) == false)
		return (1);
	try
	{
		Server server(port, av[2]);
		server.launch();
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}
	atexit(f);
	return (0);
}
