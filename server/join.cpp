
#include "../includes/irc.hpp"
std::vector<std::string> splitChannelandKey(std::string msg)
{
	// int					i = 0;
	std::vector<std::string>	cmd;
    std::cout << msg << std::endl;
	std::stringstream	str(msg);
	std::string				tm;
	if (msg == "\n")
		return (cmd);
    while (std::getline(str, tm, ','))
	{
        std::cout <<"'"<< tm <<"'"<<std::endl;
		cmd.push_back(del_break(tm));
	}
	return (cmd);
}

std::string getChannelTime()
{
        // Check if creatchannelTime is valid
		time_t		creatchannelTime;
		std::stringstream		ss;
		time(&creatchannelTime);
        if (creatchannelTime == -1) {
            return "Error: Invalid time value";
        }

        // Convert to UTC using gmtime()
		// std::cout <<"ok ss  "<< creatchannelTime<<std::endl;
        struct tm* gmTime = gmtime(&creatchannelTime);
        if (gmTime == NULL) {
            return "Error: Failed to convert to UTC";
        }
        // Format the time to YYYY-MM-DD HH:MM:SS
        char buffer[30];
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", gmTime);

        // Append milliseconds as ".000"
        std::string result(buffer);
    	result += ".000";
		// std::stringstream ss;
		// ss << creatchannelTime;

    return result;

}

// void REPLY_A(int a,int b,Client &c,Channel &ch,std::string channels)
// {
//     if(b)
//     {
//         c.reply(RPL_TOPICDISPLAY(c.getHost(),c.getNickname(),channels,ch.getTopic()));
//     }else
//         c.reply(RPL_NOTOPIC(c.getHost(),c.getNickname(),channels));
// 	c.reply(RPL_JOIN(c.getNickname(),c.getUsername(),channels,c.getHost(),""));
//     if(a)
//     {
//         ch.sendReplyAll(":" + c.getNickname() + "!" + c.getUsername() + "@" + c.getHost() + " JOIN " + channels + "\r\n", c.getNickname());
//     }
// 	c.reply(RPL_NAMREPLY(c.getHost(),ch.get_list_of_names(),channels,c.getNickname()));
// 	c.reply(RPL_ENDOFNAMES(c.getHost(),c.getNickname(),channels));
//     // c.reply(RPL_CHANNELMODEIS(c.getNickname(),channels,"+ns"));
// }
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
            ch.addClient(c.getFd(),c.getNickname(),true);
            ch.setMode('t');
            ch.setTime(getChannelTime());
            _channels.push_back(ch);
            c.reply(RPL_NOTOPIC(c.getHost(),c.getNickname(),channels[i]));
            c.reply(RPL_JOIN(c.getNickname(),c.getUsername(),channels[i],c.getHost(),""));
            c.reply(RPL_NAMREPLY(c.getHost(),ch.get_list_of_names(),channels[i],c.getNickname()));
            c.reply(RPL_ENDOFNAMES(c.getHost(),c.getNickname(),channels[i]));
        }else
        {
            Channel &cn = getChannel(channels[i]);
            if(cn.checkIfIsClientNickName(c.getNickname()))
                c.reply(":is Member\r\n");
            else if(cn.getInviteOnly() == true && !cn.checkIfInviteToChannel(c))
                c.reply(ERR_INVITEONLY(c.getHost(),c.getNickname(),channels[i]));
            else if(cn.get_max_client() != -1 && cn.get_max_client() > cn.get_nbr_client())
                c.reply(ERR_CHANNELISFULL(c.getHost(),c.getNickname(),channels[i]));
            else if(cn.getKey() == false && cn.get_password().compare(keys))
                c.reply(ERR_BADCHANNELKEY(c.getNickname(),c.getHost(),channels[i]));
            else 
            {
                cn.addClient(c.getFd(),c.getNickname(),false);
                c.reply(RPL_TOPICDISPLAY(c.getHost(),c.getNickname(),channels[i],cn.getTopic()));
                c.reply(RPL_JOIN(c.getNickname(),c.getUsername(),channels[i],c.getHost(),""));
                cn.sendReplyAll(":" + c.getNickname() + "!" + c.getUsername() + "@" + c.getHost() + " JOIN " + channels[i] + "\r\n", c.getNickname());
                c.reply(RPL_NAMREPLY(c.getHost(),cn.get_list_of_names(),channels[i],c.getNickname()));
                c.reply(RPL_ENDOFNAMES(c.getHost(),c.getNickname(),channels[i]));
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