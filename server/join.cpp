
#include "../includes/irc.hpp"
std::vector<std::string> splitChannelandKey(std::string msg)
{
	// int					i = 0;
	std::vector<std::string>	cmd;

	std::stringstream	str(msg);
	std::string				tm;
	if (msg == "\n")
		return (cmd);
    char s = msg.find(',') == std::string::npos ? ' ': ',';
    while (std::getline(str, tm, s))
	{
		cmd.push_back(del_break(tm));
	}
	return (cmd);
}

void REPLY_A(int a,int b,Client &c,Channel &ch,std::string channels)
{
    (void)b;
    c.reply(RPL_NOTOPIC(c.getNickname(),channels,c.getHost()));
	c.reply(RPL_JOIN(c.getNickname(),c.getUsername(),channels,c.getHost(),""));
    if(a)
        ch.sendReplyAll(":" + c.getNickname() + "!" + c.getUsername() + "@" + c.getHost() + " JOIN " +channels + "\r\n", c.getNickname());
	c.reply(RPL_NAMREPLY(c.getHost(),ch.get_list_of_names(),channels,c.getNickname()));
	c.reply(RPL_ENDOFNAMES(c.getHost(),c.getNickname(),channels));
}
int Server::JOIN(std::vector<std::string> args, Client &c)
{
    if(c.getState() != REGISTERED)
        return c.reply(ERR_NOTREGISTERED(c.getNickname(),c.getHost())),1;
    if (args.size() < 2) 
        return c.reply(ERR_NEEDMOREPARAMS(c.getNickname(),c.getHost(),"JOIN")),1;
    std::vector<std::string> channels = splitChannelandKey(del_break(args[1]));
    std::vector<std::string> key;
    if(args.size() == 3)
        key = splitChannelandKey(del_break(args[2]));
    std::string keys = "";
    for (size_t i = 0;i < channels.size();i++)
    {
        keys = i < key.size() && !key.empty() ? key[i]:"";
        if (channels[i][0] != '#' || channels[i].find(' ') != std::string::npos)
        {
            c.reply(ERR_BADCHANNELNAME(c.getHost(),c.getNickname(),channels[i]));
            continue;
        }
        if (checkIfChannelExist(channels[i]) != 1)
        {
            
            Channel ch(channels[i], keys);
            ch.ft_add_Member(c);
            set_Channel(ch);
            std::cout << "---1-->" << ch.getKey()<<std::endl;
            REPLY_A(0,0,c,ch,channels[i]);
        }else
        {
            Channel &cn = getChannel(channels[i]);
            // cn.ft_invite_channel(c, keys);
            if(cn.checkIfIsClient(c))
                c.reply(":is Member\r\n");
            else if(cn.getInviteOnly() == true && !cn.checkIfInviteToChannel(c))
                c.reply(ERR_INVITEONLY(c.getHost(),c.getNickname(),channels[i]));
            else if(cn.get_max_client() != -1 && cn.get_max_client() > cn.get_nbr_client())
                c.reply(ERR_CHANNELISFULL(c.getHost(),c.getNickname(),channels[i]));
            else if(cn.getKey() == false && cn.get_password().compare(keys))
                c.reply(ERR_BADCHANNELKEY(c.getNickname(),c.getHost(),channels[i]));
            else 
            {
                cn.setClient(c);
                REPLY_A(1,1,c,cn,channels[i]);
            }
        }
    }
    return 0;
}
                /////reply 1
                // c.reply(RPL_NOTOPIC(c.getNickname(),channels[i],c.getHost()));
			    // c.reply(RPL_JOIN(c.getNickname(),c.getUsername(),channels[i],c.getHost(),"+k"));
			    // c.reply(RPL_NAMREPLY(c.getHost(),ch.get_list_of_names(),channels[i],c.getNickname()));
			    // c.reply(RPL_ENDOFNAMES(c.getHost(),c.getNickname(),channels[i]));
                //reply2
                // c.reply(RPL_NOTOPIC(c.getNickname(),channels[i],c.getHost()));
                // c.reply(RPL_JOIN(c.getNickname(),c.getUsername(),channels[i],c.getHost(),"+k"));
                // cn.sendReplyAll(":" + c.getNickname() + "!~" + c.getUsername() + "@" + c.getHost() + " JOIN " +channels[i] + "\r\n", c.getNickname());
                // c.reply(RPL_NAMREPLY(c.getHost(),cn.get_list_of_names(),channels[i],c.getNickname()));
                // c.reply(RPL_ENDOFNAMES(c.getHost(),c.getNickname(),channels[i]));