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
		return cl.reply(ERR_NEEDMOREPARAMS(cl.getNickname(),cl.getHost(),"PASS")),-1;
	if(cl.getState() == REGISTERED)
        return cl.reply(ERR_ALREADYREGISTERED(cl.getNickname(),cl.getHost())),-1;
	std::string mdp = del_break(pswrd[1]);
	if (mdp != _password)
		return cl.reply(ERR_PASSWDMISMATCH(cl.getNickname(),cl.getHost())),-1;
	cl.setState(LOGIN);
	cl.setLogin(true);
	displayClient();
	cl.welcome();
	return (0);
}
