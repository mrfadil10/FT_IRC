/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mode.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ibenaait <ibenaait@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/17 14:34:13 by ibenaait          #+#    #+#             */
/*   Updated: 2024/09/14 18:54:40 by ibenaait         ###   ########.fr       */
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
    s = s > LLONG_MAX ? LONG_MAX : s;
	return (s);
}
int    Server::MODE(std::vector<std::string> args, Client &c)
{
    if(c.getState() != REGISTERED)
        return c.reply(ERR_NOTREGISTERED(c.getNickname(),c.getHost())),1;
    if(args.size() <= 2)
        return c.reply(ERR_NEEDMOREPARAMS(c.getNickname(),c.getHost(),"MODE #channel +-iotkl xx")),1;
    std::string s = args[3];
    std::string target = args[1];
    std::string mode = args[2];
    if(!checkIfChannelExist(target) && target[0] != '#')
        return c.reply(ERR_NOSUCHCHANNEL(c.getHost(),c.getNickname(),target)),1;
    bool flag = false;
    Channel &ch = getChannel(target);
    if(ch.checkIfIsClient(c) == 0)
        return c.reply(ERR_NOTONCHANNEL(c.getHost(),c.getNickname())),1;
    std::string nick = c.getNickname();
    std::vector<std::string>::iterator it = args.begin()+3;
    for(size_t i= 0; i < mode.size() ;i++)
    {
        if(mode[i] == '-') flag = false;
        else if(mode[i] == '+') flag = true;
        else if(mode[i] == 'i')
        {
            if(!ch.findClientRole(c))
                c.reply(ERR_CHANOPRIVSNEEDED(c.getHost(),target));
            else if(flag && ch.getInviteOnly())
                c.reply(RPL_ALLINV(nick,c.getHost()));
            else if(!flag && !ch.getInviteOnly())
                c.reply(RPL_NOTINV(nick,c.getHost()));
            else 
            {
                ch.setInviteOnly(flag);
                std::string f = flag == true ? "+":"-";
                c.reply(RPL_CHANNELMODEIS(c.getNickname(),target,f+"k"));
                //ch.sendReplyAll(RPL_MODE(ch.get_list_of_names(),nick,f+"i"),nick);
            }
        }
        else if(mode[i] == 'k')
        {
            if(it == args.end() && flag == true)
                c.reply(ERR_NEEDMOREPARAMS(nick,c.getHost(),"MODE "+target+" +-oitkl KEY"));
            else if(!ch.findClientRole(c))
                c.reply(ERR_CHANOPRIVSNEEDED(c.getHost(),target));
            // else if(flag == true && ch.get_password().size() != 0)
            //     c.reply(ERR_KEYSET(c.getHost(),nick,target));
            else if(flag == false && ch.get_password().size() == 0)
                c.reply(ERR_NOKEYSET(c.getHost(),nick,target));
            else if(!flag)
            {
                ch.setKey(flag);
                c.reply(RPL_CHANNELMODEIS(c.getNickname(),target,"-k"));
            }
            else 
            {
                ch.setPassword(del_break(*it));
                ch.setKey(flag);
                c.reply(RPL_CHANNELMODEIS(c.getNickname(),target,"+k"));
                it++;
            }

        }else if(mode[i]== 't')
        {
            if(!ch.findClientRole(c))
                c.reply(ERR_CHANOPRIVSNEEDED(c.getHost(),target));
        }
        else if(mode[i] == 'l')
        {
            if(it == args.end() && flag)
                c.reply(ERR_NEEDMOREPARAMS(nick,c.getHost(),"MODE "+target+" +-oitkl limit"));
            if(!ch.findClientRole(c))
                c.reply(ERR_CHANOPRIVSNEEDED(c.getHost(),target+"1"));
            else if(!flag)
            {
                ch.setLimit(flag);
                c.reply(RPL_CHANNELMODEIS(c.getNickname(),target,"-l"));
            }
            else
            {
                ch.setLimit(flag);
                ch.setMaxClient(aatoi(del_break(*it).c_str()));
                c.reply(RPL_CHANNELMODEIS(c.getNickname(),target,"+l"));
                it++;
            }  
        }
        else if(mode[i] ==  'o')
        {
            if(it == args.end())
                c.reply(ERR_NEEDMOREPARAMS(nick,c.getHost(),"MODE "+target+" +-oitkl operator"));
            else if(ch.findClientRole(c) != 1)
                c.reply(ERR_CHANOPRIVSNEEDED(c.getHost(),target));
            else if(ch.checkIfIsClientNickName(del_break(*it)) == 0)
                c.reply(ERR_NOTONCHANNEL(c.getHost(),del_break(*it)));
            else
            {
                Client &client = ch.getClientByNickName(del_break(*it));
                if(ch.findClientRole(client) && flag)
                    continue;
                if(!ch.findClientRole(client) && !flag)
                    continue;
                std::string f = flag == true ? "+":"-";
                ch.setClientRole(client,flag);
                // ch.allClientInChannel();
                c.reply(RPL_MODE(c.getNickname(),c.getUsername(),c.getHost(),target,f+"o",client.getNickname()));
                ch.sendReplyAll(RPL_MODE(c.getNickname(),c.getUsername(),c.getHost(),target,f+"o",client.getNickname()),c.getNickname());
                c.reply(RPL_CHANNELMODEIS(c.getNickname(),target,f+"o"));
                c.reply(RPL_UMODEIS(client.getNickname(),f+"o"));
                it++;
            }
        }else if(mode[i] == 'n' || mode[i] == 's' || mode[i] == '\r')
            continue;
        else 
            c.reply(ERR_UMODEUNKNOWNFLAG(c.getNickname()));
    }
    return 0;
}
