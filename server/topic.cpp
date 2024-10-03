#include "../includes/irc.hpp"

int    Server::TOPIC(std::string cmd, Client &c)
{
    if(c.getState() != REGISTERED)
        return c.reply(ERR_NOTREGISTERED(c.getNickname(),c.getHost())),1;
    std::vector<std::string> args = splitCommands(cmd);
    if(args.size() < 2)
        return c.reply(ERR_NEEDMOREPARAMS(c.getNickname(),c.getHost(),"TOPIC")),1;
    std::string target = args[1];
    Channel *ch = getChannel(target);
    if(!ch)
        return c.reply(ERR_NOSUCHCHANNEL(c.getHost(),c.getNickname(),target)),1;
    if(ch->checkIfIsClient(c.getNickname()) == 0)
        return c.reply(ERR_NOTONCHANNEL(c.getHost(),target)),1;
    if(args.size() == 2)
    {
        if(!ch->getIsTopic())
            c.reply(RPL_NOTOPIC(c.getHost(),c.getNickname(),target));
        else
        {
            c.reply(RPL_TOPICDISPLAY(c.getHost(),c.getNickname(),target,ch->getTopic()));
            c.reply(REPLY_TOPICWHOTIME(c.getUsername(),ch->getTimeTop(),c.getNickname(),c.getHost(),target));
        }
    }else
    {
        if(!ch->findClientRole(c.getNickname()) && ch->getChTopOp())
            c.reply(ERR_CHANOPRIVSNEEDED(c.getHost(),target));
        else
        {
            ch->setTopic(args[2]);
            ch->setIsTopic(true);
            ch->setTimeTop(getTimeSc());
            c.reply(RPL_TOPIC(c.getNickname(),c.getUsername(),c.getHost(),target,ch->getTopic()));
            ch->sendReplyAll(RPL_TOPIC(c.getNickname(),c.getUsername(),c.getHost(),target,ch->getTopic()),c.getNickname());
        }
    }
    return 0;
}