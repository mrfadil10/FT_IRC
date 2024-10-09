#include "../includes/irc.hpp"

// std::string	ERR_PASSWDMISMATCH(Client &client)
// {
// 	return ("\033[1;91m" + client.getNickname() + " :Password incorrect\r\n");
// }

int		teststring(std::string test)
{
	const char *tst = test.c_str();
	int		i = -1;
	while (tst[++i])
	{
		if (tst[i] == '\r')
		{
			// std::cout << "character number : " << i << " Line Break\n";
			return (1);
		}
	}
	return (0);
}

int Server::cmdPass(std::string arg, Client &cl)
{
	std::vector<std::string> pswrd = splitCommands(del_break(arg));
	if (pswrd.size() < 2)
	{
		cl.reply(ERR_NEEDMOREPARAMS(cl.getNickname(),cl.getHost(),"PASS"));
		return (-1);
	}
	std::string mdp = del_break(pswrd[1]);
	if (mdp != _password)
	{
		std::cout <<"password[1] : " << "[" << mdp << "]"<< std::endl;
		std::cout << "password : " << "[" << _password << "]" << std::endl;
		teststring(pswrd[1]);
		cl.reply(ERR_PASSWDMISMATCH(cl.getNickname(),cl.getHost()));
		return (-1);
	}
	cl.setState(LOGIN);
	displayClient();
	cl.welcome();
	return (0);
}
