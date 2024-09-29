#include "../includes/irc.hpp"
// std::string joinArg(std::vector<std::string> v)
// {
//     std::string all;
//     for (size_t i = 2; i < v.size(); i++)
//     {
//         all += del_break(v.at(i)) + " ";
//     }
//     return all;
// }
// std::vector<std::string> splitTopicCommand(const std::string& input) {
//     std::vector<std::string> cmd;
//     std::istringstream iss(input);
//     std::string part;
//     std::string command, channel;

//     if (std::getline(iss, part, ' ')) {
//         command = part;
//     }
//     if (std::getline(iss, part, ' ')) {
//         channel = part;
//     }

//     std::string topic;
//     std::getline(iss, topic);

//     size_t colonPos = topic.find(':');
//     if (colonPos != std::string::npos) {
//         topic = topic.substr(colonPos + 1);
//     }
//     if(!command.empty())
//         cmd.push_back(command);
//     if(!channel.empty())
//         cmd.push_back(channel);
//     if(!topic.empty())
//         cmd.push_back(topic);

//     return cmd;
// }

int    Server::TOPIC(std::string cmd, Client &c)
{
    if(c.getState() != REGISTERED)
        return c.reply(ERR_NOTREGISTERED(c.getNickname(),c.getHost())),1;
    std::vector<std::string> args = splitCommands(del_break(cmd));
    if(args.size() < 2)
        return c.reply(ERR_NEEDMOREPARAMS(c.getNickname(),c.getHost(),"TOPIC")),1;
    std::string target = del_break(args[1]);
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
            c.reply(RPL_TOPICDISPLAY(c.getHost(),c.getNickname(),target,ch->getTopic()));
        c.reply(REPLY_TOPICWHOTIME(c.getUsername(),ch->getTimeTop(),c.getNickname(),c.getHost(),target));
    }else
    {
        if(!ch->findClientRole(c.getNickname()) && ch->getChTopOp())
            c.reply(ERR_CHANOPRIVSNEEDED(c.getHost(),target));
        else
        {
            ch->setTopic(del_break(args[2]));
            ch->setIsTopic(true);
            ch->setTimeTop(getTimeSc());
            c.reply(RPL_TOPIC(c.getNickname(),c.getUsername(),c.getHost(),target,ch->getTopic()));
            ch->sendReplyAll(RPL_TOPIC(c.getNickname(),c.getUsername(),c.getHost(),target,ch->getTopic()),c.getNickname());
        }
    }
    return 0;
}