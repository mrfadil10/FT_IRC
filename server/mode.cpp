/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mode.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ibenaait <ibenaait@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/17 14:34:13 by ibenaait          #+#    #+#             */
/*   Updated: 2024/09/28 23:25:17 by ibenaait         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// #include "Channel.hpp"
#include "../includes/irc.hpp"

long long	aatoi(const char * str)
{
	size_t	i;
	long long	s;

	i = 0;
	s = 0;
	if (str[i] == '-' || str[i] == '+')
	{
		if (str[i] == '-')
			return -1;
		i++;
	}
	while (str[i])
	{
		if (std::isalpha(str[i]))
			return (-1);
		s = (s * 10) + (str[i] - '0');
		i++;
	}
    s = s > LONG_MAX ? LONG_MAX : s;
	return (s);
}
int     invalidMode(std::string mode)
{
    bool flag = false;
    bool flag1 = false;
    for (size_t i = 0; i < mode.size(); i++)
    {
        if(mode[i] == '-' || mode[i] == '+')
            flag = true;
        else if(mode[i] == 'i' || mode[i]=='k' || mode[i] == 't' || mode[i] == 'l'|| mode[i] == 'o')
            flag1 = true;
    }
    if(flag == true && flag1 == false)
        return 1;
    return 0;
}
std::string makeMode(std::string modeStr)
{
    std::set<char> addedModes;
    std::set<char> removedModes;
    char currentSign = '\0';
    for (size_t i = 0; i < modeStr.size(); ++i) {
        if (modeStr[i] == '+' || modeStr[i] == '-') {
            currentSign = modeStr[i];
        } else if (isalpha(modeStr[i]))
        {
            if (currentSign == '+') 
            {
                addedModes.insert(modeStr[i]);
                removedModes.erase(modeStr[i]);
            }
            else if (currentSign == '-') 
            {
                removedModes.insert(modeStr[i]);
                addedModes.erase(modeStr[i]);  
            }
        }
    }
    std::stringstream result;
    std::stringstream addResult, removeResult;

     if (!removedModes.empty()) 
     {
        removeResult << "-";
        for (std::set<char>::iterator it = removedModes.begin(); it != removedModes.end(); ++it) 
        {
            removeResult << *it;
        }
    }

    if (!addedModes.empty()) {
        addResult << "+";
        for (std::set<char>::iterator it = addedModes.begin(); it != addedModes.end(); ++it) {
            addResult << *it;
        }
    }   
    result << removeResult.str() << addResult.str();

    return result.str();
    
}
int    Server::MODE(std::string cmd, Client &c)
{
    if(c.getState() != REGISTERED)
        return c.reply(ERR_NOTREGISTERED(c.getNickname(),c.getHost())),1;
    std::vector<std::string> args = splitCommands(cmd);
    if(args.size() == 1)
        return c.reply(ERR_NEEDMOREPARAMS(c.getNickname(),c.getHost(),"MODE #channel +-iotkl xx")),1;
    std::string target = del_break(args[1]);
    Channel *   ch = getChannel(target);
    if(!ch)
    {
        if(target[0] == '#')
            return c.reply(ERR_NOSUCHCHANNEL(c.getHost(),c.getNickname(),target)),1;
        if(c.getNickname().compare(target) == 0)
            return c.reply(RPL_UMODEIS(c.getHost(),c.getNickname(),"+Zi")),1;
        else
            return c.reply(ERR_NOSUCHNICK(c.getHost(),target)),1;
    }
    if(args.size() == 2 || (args.size() == 3 && invalidMode(args[2]) == 1))
    {
        if(ch->checkIfIsClient(c.getNickname()))
            c.reply(RPL_CHANNELMODEIS(c.getHost(),c.getNickname(),ch->get_name(),ch->getMode()));
        else
            c.reply(RPL_CHANNELMODEIS(c.getHost(),c.getNickname(),ch->get_name(),"+t"));
        c.reply(RPL_CREATIONTIME(c.getHost(),c.getNickname(),ch->get_name(),ch->getTime()));
        return 1;
    }
    std::string mode = del_break(args[2]);
    std::string as,m;
    bool flag  = true;
    if(ch->checkIfIsClientNickName(c.getNickname()) == 0)
        return c.reply(ERR_NOTONCHANNEL(c.getHost(),c.getNickname())),1;
    std::string nick = c.getNickname();
    std::vector<std::string>::iterator it = args.begin()+3;
    for(size_t i= 0; i < mode.size() ;i++)
    {
        if(mode[i] == '-') flag = false;
        else if(mode[i] == '+') flag = true;
        else if(mode[i] == 'i')
        {
            if(!ch->findClientRole(c.getNickname()))
                c.reply(ERR_CHANOPRIVSNEEDED(c.getHost(),target));
            else if(flag && ch->getInviteOnly())
                continue;// c.reply(RPL_ALLINV(nick,c.getHost()));
            else if(!flag && !ch->getInviteOnly())
                continue;//c.reply(RPL_NOTINV(nick,c.getHost()));
            else 
            {
                std::string f = flag == true ? "+":"-";
                as+=f+"i";
                ch->setInviteOnly(flag);
                if(flag)
                    ch->setMode('i');
                else
                    ch->eraseMode('i');
            }
        }
        else if(mode[i] == 'k')
        {
            if(it == args.end() && flag)
                c.reply("Reply(696): "+target+" k * You must specify a parameter for the key mode. Syntax: <key>.\r\n");
            else if(!ch->findClientRole(c.getNickname()))
                c.reply(ERR_CHANOPRIVSNEEDED(c.getHost(),target));
            else if(!flag && !ch->getKey())
                c.reply(ERR_NOKEYSET(c.getHost(),nick,target));
            else if(flag && ch->getKey())
                continue;
            else 
            {
                if(!flag)
                {
                    ch->setKey(flag);
                    ch->eraseMode('k');
                }
                else 
                {
                    if(del_break(*it).find(' ') != std::string::npos)
                    {
                        c.reply(ERROR_INVALIDMODEPARAM__KEY(c.getNickname(),target,c.getHost(),del_break(*it)));
                        continue;
                    }
                    ch->setPassword(del_break(*it));
                    ch->setKey(flag);
                    m += del_break(*it)+" ";
                    ch->setMode('k');
                    it++;
                }
                std::string f = flag == true ? "+":"-";
                as+=f+"k";
            } 
        }else if(mode[i]== 't')
        {
            if(!ch->findClientRole(c.getNickname()))
                c.reply(ERR_CHANOPRIVSNEEDED(c.getHost(),target));
            else if(ch->getChTopOp() && flag)
                continue;
            else if(!ch->getChTopOp() && !flag)
                continue;
            else
            {
                std::string f = flag == true ? "+":"-";
                as+=f+"t";
                ch->setChTopOp(flag);
            }
        }
        else if(mode[i] == 'l')
        {
            if(it == args.end() && flag)
                c.reply(ERROR_INVALIDMODEPARAM_LIMIT(target,c.getHost(),"*"));
            else if(!ch->findClientRole(c.getNickname()))
                c.reply(ERR_CHANOPRIVSNEEDED(c.getHost(),target));
            else if(!ch->getLimit() && !flag)
                continue;
            else 
            {
                if(!flag)
                {
                    ch->setLimit(flag);
                    ch->setMaxClient(-1);
                    ch->eraseMode('l');
                }
                else
                {
                    if(aatoi(del_break(*it).c_str()) == -1)
                    {
                        c.reply(ERROR_INVALIDMODEPARAM_LIMIT(target,c.getHost(),del_break(*it)));
                        continue;
                    }
                    // if(aatoi(del_break(*it).c_str()) == 0)
                    // {
                    //     ch->setLimit(false);
                    //     ch->setMaxClient(0);
                    //     ch->eraseMode('l');
                    // }
                    else
                    {
                        ch->setLimit(flag);
                        ch->setMaxClient(aatoi(del_break(*it).c_str()));
                        ch->setMode('l');
                    }
                    m += del_break(*it)+" ";
                    it++;
                }
                std::string f = flag == true ? "+":"-";
                as+=f+"l";
            }
            
        }
        else if(mode[i] ==  'o')
        {
            if(it == args.end())
                c.reply(ERROR_INVALIDMODEPARAM_OP(target,c.getHost()));
            else if(ch->findClientRole(c.getNickname()) != 1)
                c.reply(ERR_CHANOPRIVSNEEDED(c.getHost(),target));
            else if(ch->checkIfIsClientNickName(del_break(*it)) == 0)
                c.reply(ERR_NOSUCHNICK(c.getHost(),del_break(*it)));
            else
            {
                //Client &client = ch->getClientByNickName(del_break(*it));
                if(ch->findClientRole(del_break(*it)) && flag)
                    continue;
                if(!ch->findClientRole(del_break(*it)) && !flag)
                    continue;
                std::string f = flag == true ? "+":"-";
                as+=f+"o";
                m += del_break(*it)+" ";
                ch->setClientRole(del_break(*it),flag);
                if(flag)
                    ch->setMode('o');
                else
                    ch->eraseMode('o');
                it++;
            }
            
        }else
            c.reply(ERR_UNKNOWNMODE(c.getNickname(),c.getHost(),target,mode[i]));
    }
    if(!as.empty())
    {
        c.reply(RPL_MODE(c.getNickname(),c.getUsername(),c.getHost(),target,makeMode(as),m));
        ch->sendReplyAll(RPL_MODE(c.getNickname(),c.getUsername(),c.getHost(),target,makeMode(as),m),c.getNickname());
    }
    return 0;
}
