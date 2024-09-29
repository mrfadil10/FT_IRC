
#include "../includes/irc.hpp"

std::vector<std::string> splitNickName(std::string msg)
{
	// int					i = 0;
	std::vector<std::string>	cmd;
    // std::cout << msg << std::endl;
	std::stringstream	str(msg);
	std::string				tm;
	if (msg == "\n")
		return (cmd);
    while (std::getline(str, tm, ','))
	{
        if (tm.empty())
		    return (cmd);
		cmd.push_back(del_break(tm));
	}
	return (cmd);
}
int    Server::KICK(std::string cmd, Client &c)
{
    if(c.getState() != REGISTERED)
        return c.reply(ERR_NOTREGISTERED(c.getNickname(),c.getHost())),1;
    std::vector<std::string> args = splitCommands(del_break(cmd));
    if(args.size() < 3)
        return c.reply(ERR_NEEDMOREPARAMS(c.getNickname(),c.getHost(),"KCIK")),1;
    std::string target = del_break(args[1]);
    Channel *ch = getChannel(target);
    if(!ch)
        return c.reply(ERR_NOSUCHCHANNEL(c.getHost(),c.getNickname(),target)),1;
    if(ch->checkIfIsClient(c.getNickname()) == 0)
        return c.reply(ERR_NOTONCHANNEL(c.getHost(),target)),1;
    if(!ch->findClientRole(c.getNickname()))
        return c.reply(ERR_CHANOPRIVSNEEDED(c.getHost(),target)),1;
    std::vector<std::string> vec = splitNickName(del_break(args[2]));
    std::string reason = args.size() >= 4 ? del_break(args[3]) : "";
    for(size_t i = 0;i < vec.size();i++)
    {
        if(ch->checkIfIsClient(c.getNickname()) == 0)
            return c.reply(ERR_NOTONCHANNEL(c.getHost(),target)),1;
        if(ch->checkIfIsClient(vec.at(i)) == 0)
        {
            c.reply(ERROR_USERNOTINCHANNEL(c.getHost(),target,vec.at(i)));
            continue;
        }
        ch->removeClientNickName(vec.at(i));
        c.reply(RPL_KICK(c.getNickname(),c.getUsername(),c.getHost(),target,vec.at(i),reason));
        ch->sendReplyAll(RPL_KICK(c.getNickname(),c.getUsername(),c.getHost(),target,vec.at(i),reason),c.getNickname());
    }
    return 0;
}