/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mode.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ibenaait <ibenaait@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/17 14:34:13 by ibenaait          #+#    #+#             */
/*   Updated: 2024/09/23 01:15:53 by ibenaait         ###   ########.fr       */
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
			return s;
		i++;
	}
	while (str[i])
	{
		if (std::isdigit(str[i]))
			break;
		s = (s * 10) + (str[i] - '0');
		i++;
	}
    s = s > LONG_MAX ? LONG_MAX : s;
	return (s);
}
int    Server::MODE(std::vector<std::string> args, Client &c)
{
    if(c.getState() != REGISTERED)
        return c.reply(ERR_NOTREGISTERED(c.getNickname(),c.getHost())),1;
    if(args.size() == 1)
        return c.reply(ERR_NEEDMOREPARAMS(c.getNickname(),c.getHost(),"MODE #channel +-iotkl xx")),1;
    std::string target = del_break(args[1]);
    if(checkIfChannelExist(del_break(target)) == 0 )
    {
        if(c.getNickname().compare(target))
            return c.reply(RPL_UMODEIS(c.getHost(),c.getNickname(),"+Zi")),1;
        if(target[0] != '#')
            return c.reply(ERR_NOSUCHCHANNEL(c.getHost(),c.getNickname(),target)),1;
    }
    Channel *ch = getChannel(target);
    if(args.size() == 2)
    {
        ch->setMode('t');
        c.reply(RPL_CHANNELMODEIS(c.getHost(),c.getNickname(),target,ch->getMode()));
        c.reply(RPL_CREATIONTIME(c.getHost(),c.getNickname(),target,ch->getTime()));
        return 1;
    }
    std::string s = args[3];
    std::string mode = del_break(args[2]);
    // if(checkIfChannelExist(del_break(target)) == 0 || target[0] != '#')
    //     return c.reply(ERR_NOSUCHCHANNEL(c.getHost(),c.getNickname(),target)),1;
    bool flag ;
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
                c.reply(RPL_ALLINV(nick,c.getHost()));
            else if(!flag && !ch->getInviteOnly())
                c.reply(RPL_NOTINV(nick,c.getHost()));
            else 
            {
                ch->setInviteOnly(flag);
                std::string f = flag == true ? "+":"-";
                c.reply(RPL_CHANNELMODEIS(c.getHost(),c.getNickname(),target,f+"i"));
                if(flag)
                    ch->setMode('i');
                else
                    ch->eraseMode('i');
                //ch.sendReplyAll(RPL_MODE(ch.get_list_of_names(),nick,f+"i"),nick);
            }
        }
        else if(mode[i] == 'k')
        {
            if(it == args.end() && flag)
                c.reply(ERR_NEEDMOREPARAMS(nick,c.getHost(),"MODE "+target+" +-oitkl KEY"));
            else if(!ch->findClientRole(c.getNickname()))
                c.reply(ERR_CHANOPRIVSNEEDED(c.getHost(),target));
            else if(flag == false && ch->get_password().size() == 0)
                c.reply(ERR_NOKEYSET(c.getHost(),nick,target));
            else if(!flag)
            {
                ch->setKey(flag);
                c.reply(RPL_CHANNELMODEIS(c.getHost(),c.getNickname(),target,"-k"));
                ch->eraseMode('k');
            }
            else 
            {
                ch->setPassword(del_break(*it));
                ch->setKey(flag);
                c.reply(RPL_CHANNELMODEIS(c.getHost(),c.getNickname(),target,"+k"));
                ch->setMode('k');
                it++;
            }

        }else if(mode[i]== 't')
        {
            if(!ch->findClientRole(c.getNickname()))
                c.reply(ERR_CHANOPRIVSNEEDED(c.getHost(),target));
        }
        else if(mode[i] == 'l')
        {
            if(it == args.end() && flag)
                c.reply(ERR_NEEDMOREPARAMS(nick,c.getHost(),"MODE "+target+" +-oitkl limit"));
            if(!ch->findClientRole(c.getNickname()))
                c.reply(ERR_CHANOPRIVSNEEDED(c.getHost(),target+"1"));
            else if(!flag)
            {
                ch->setLimit(flag);
                ch->setMaxClient(-1);
                c.reply(RPL_CHANNELMODEIS(c.getHost(),c.getNickname(),target,"-l"));
                ch->eraseMode('l');
            }
            else
            {
                ch->setLimit(flag);
                ch->setMaxClient(aatoi(del_break(*it).c_str()));
                 c.reply(RPL_CHANNELMODEIS(c.getHost(),c.getNickname(),target,"+l"));
                ch->setMode('l');
                it++;
            }
        }
        else if(mode[i] ==  'o')
        {
            if(it == args.end())
                c.reply(ERR_NEEDMOREPARAMS(nick,c.getHost(),"MODE "+target+" +-oitkl operator"));
            else if(ch->findClientRole(c.getNickname()) != 1)
                c.reply(ERR_CHANOPRIVSNEEDED(c.getHost(),target));
            else if(ch->checkIfIsClientNickName(del_break(*it)) == 0)
                c.reply(ERR_NOSUCHNICK(c.getHost(),c.getNickname(),del_break(*it)));
            else
            {
                
                //Client &client = ch->getClientByNickName(del_break(*it));
                if(ch->findClientRole(del_break(*it)) && flag)
                    continue;
                if(!ch->findClientRole(del_break(*it)) && !flag)
                    continue;
                std::string f = flag == true ? "+":"-";
                ch->setClientRole(del_break(*it),flag);
                c.reply(RPL_MODE(c.getNickname(),c.getUsername(),c.getHost(),target,f+"o",del_break(*it)));
                ch->sendReplyAll(RPL_MODE(c.getNickname(),c.getUsername(),c.getHost(),target,f+"o",del_break(*it)),c.getNickname());
                c.reply(RPL_CHANNELMODEIS(c.getHost(),c.getNickname(),target,f+"o"));
                c.reply(RPL_UMODEIS(c.getHost(),del_break(*it),f+"o"));
                if(flag)
                    ch->setMode('o');
                else
                    ch->eraseMode('o');
                it++;
            }
        }else if(mode[i] == 'n' || mode[i] == 's' || mode[i] == '\r')
            continue;
        else 
            c.reply(ERR_UMODEUNKNOWNFLAG(c.getNickname()));
    }
    return 0;
}
