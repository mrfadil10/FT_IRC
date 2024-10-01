#include "../includes/irc.hpp"

bool	Server::is_used(Client cl, std::string name)
{
	for (std::map<int,Client*>::iterator it = this->_clients.begin(); it != this->_clients.end(); ++it)
	{
		if (it->second->getNickname() == name && it->second->getFd() != cl.getFd())
			return (true);
	}
	return (false);
}

bool	isSpecial(char c)
{
	if (c >= 91 && c <= 96)
		return (true);
	else if (c >= 123 && c <= 125)
		return (true);
	else
		return (false);
}

bool	is_valid_nick(std::string name)
{
	if (!(isSpecial(name[0]) || isalpha(name[0])))
		return (false);
	for (size_t i = 1; i < name.size(); ++i)
	{
		if (!(isalnum(name[i]) || isSpecial(name[i]) || name[i] == '-'))
			return (false);
	}
	return (true);
}

std::string	ERRMSG_EMPTY(Client cl)
{
	return ("\033[1;91m" + cl.getNickname() + " :No nickname was given\033[0m");
}

std::string	NICK_ERR(Client cl, std::string newNick)
{
	return ("\033[1;91m" + cl.getNickname() + " " + newNick + " :Error in nickname\033[0m\r\n");
}

std::string	ERRMSG_ALREADYUSED(Client cl, std::string newNick)
{
	return ("\033[1;91m" + cl.getNickname() + " " + newNick + " :Nickname already used\033[0m\r\n");
}

std::string NICK_SUCCESS(std::string newNick)
{
	return ("\033[1;92mNICK " + newNick + "\033[0m");
}

int Server::cmdNick(std::string cmd, Client& client)
{
	std::vector<std::string> args = splitCommands(cmd);
	if (args.size() < 2) return (client.reply(ERRMSG_EMPTY(client)), -1);
	std::string requestedNick = args[1];
	std::string oldnick = client.getNickname().empty() ? client.getNickname() : "";
	if (!is_valid_nick(requestedNick))
		return (client.reply(NICK_ERR(client, requestedNick)), -1);
	if (is_used(client, requestedNick))
		return (client.reply(ERRMSG_ALREADYUSED(client, requestedNick)), -1);
	client.reply(NICK_SUCCESS(requestedNick));
	client.setNickname(requestedNick);
	client.welcome();
	return (0);
}
