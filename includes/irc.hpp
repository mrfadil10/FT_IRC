#ifndef IRC_HPP
# define IRC_HPP

# include <iostream>
# include <vector>
# include <sstream>
# include <sys/socket.h>
# include <algorithm>
# include <sys/types.h>
# include <arpa/inet.h>
# include <netdb.h>
# include <fcntl.h>
# include <map>
# include <poll.h>
# include <unistd.h>
# include <cstdlib>
# include <string>
# include <strings.h>

# include <string.h>
# include <cstring>
# include <cerrno>
# include <signal.h>
#include <set>
#include <algorithm> 

#include <stdlib.h>
// #include "Channel.hpp"

// #define MODE_CLIENT_LIMIT 1		//+l
// #define MODE_INVITE_ONLY 2		//+i
// #define MODE_KEY 4				//+k
// #define MODE_PROTECTED_TOPIC 8	//+t
// #define	MODE_OPERATOR_PRIV 16		//+o

#define RED			"\033[1;91m"
#define GREEN		"\033[32m"
#define YELLOW		"\033[1;93m"
#define BLUE		"\033[1;94m"
#define B_CYAN		"\033[1;96m"
#define B_BLACK		"\033[1;90m"
#define I_GREEN		"\033[1;92m"
#define RESET		"\033[0m"

#define ERR_CHANOPRIVSNEEDED(hostname, channel) ":" + hostname + " 482 " + channel + " :You're not channel operator\r\n"
#define RPL_MODE(nick, username, host,target,mode,target_nick) ":" + nick + "!" + username + "@" + host + " MODE " + target + " " + mode + " " + target_nick + "\r\n"
#define RPL_NOTINV(nickname,hostname) (":" + hostname + " 664 " + nickname + ": Channel already not in invite-only mode\r\n")
#define ERR_UNKNOWNCOMMAND(hostname, nick, arg) ":" + hostname + " 421 " + nick + " " + arg + " :Unknown command\r\n"
#define ERR_NEEDMOREPARAMS(nick, hostname, cmd) ":" + hostname + " 461 " + nick + " " + cmd + " :Not enough parameters\r\n"
#define ERR_NOSUCHCHANNEL(hostname, nick, channel) ":" + hostname + " 403 " + nick + " " + channel + " :No such channel\r\n"
#define ERR_USERNOTINCHANNEL(hostname, nick, nick2, chann) ":" + hostname + " 441 " + nick + " " + nick2 + " " + chann + " :They aren't on that channel\r\n"
#define ERR_NOSUCHNICK(hostname, nick, nick2) ":" + hostname + " 401 " + nick + " " + nick2 + " :No such nick\r\n"
#define ERR_KEYSET(hostname, nick, chann) ":" + hostname + " 467 " + nick + " " + chann + " :Channel key already set\r\n"
#define ERR_NOKEYSET(hostname, nick, chann) ":" + hostname + " 467 " + nick + " " + chann + " :Cannot remove channel key (no key set)\r\n"
#define ERR_UNKNOWNMODE(hostname, nick, c) ":" + hostname + " 472 " + nick + " " + c + " :is unknown mode char to me\r\n"
#define ERR_NOTREGISTERED(nick, hostname) ":" + hostname + " 451 " + nick + " :You have not registered\r\n"
#define ERR_USERONCHANNEL(hostname, nick, nick2, chann) ":" + hostname + " 443 " + nick + " " + nick2 + " " + chann + " :is already on channel\r\n"
#define ERR_NOTONCHANNEL(hostname, chann) ":" + hostname + " 442 " + chann + " " + ":You're not on that channel\r\n"
#define ERR_CANNOTSENDTOCHAN(hostname, nick, channel) ":" + hostname + " 404 " + nick + " " + channel + " :Cannot send to channel\r\n"
//#define ERR_PASSWDMISMATCH(nick, hostname) ":" + hostname + " 464 " + nick + " :Password incorrect !\r\n"
#define ERR_ERRONEUSNICKNAME(nick, hostname) ":" + hostname + " 432 * " + nick + " :Erroneus nickname !\r\n"
#define ERR_NONICKNAMEGIVEN(nick, hostname) ":" + hostname + " 431 " + nick + " :No nickname given !\r\n"
#define ERR_NICKNAMEINUSE(nick, hostname) ":" + hostname + " 433 " + nick + " :Nickname is already in use !\r\n"
#define ERR_ALREADYREGISTERED(nick, hostname) ":" + hostname + " 462 " + nick + " :You may not reregister !\r\n"
#define ERR_BADCHANNELNAME(nick, hostname, channelname) ":" + hostname + " 476 " + nick + " " + channelname + " :Invalid channel name." + "\r\n"
#define ERR_CHANNELISFULL(hostname, nick, channelName) ":" + hostname + " 471 " + nick + " " + channelName + " :Cannot join channel, Channel is full (+l)\r\n"
#define ERR_BADCHANNELKEY(nick, hostname, channelName) ":" + hostname + " 475 " + nick + " " + channelName + " :Cannot join channel (+K) - bad key\r\n"
#define ERR_INVITEONLY(hostname, nick, channelName) ":" + hostname + " 473 " + nick + " " + channelName + " :Cannot join channel, you must be invited (+i)\r\n"
#define ERR_INVITEONLY(hostname, nick, channelName) ":" + hostname + " 473 " + nick + " " + channelName + " :Cannot join channel, you must be invited (+i)\r\n"
#define RPL_JOIN_OP(nick, username, channelname, ipaddress) ":@" + nick + "!~" + username + "@" + ipaddress + " JOIN " + channelname + "\r\n"
#define RPL_TOPICDISPLAY(hostname, nick, channel, topic) ":" + hostname + " 332 " + nick + " " + channel + " :" + topic + "\r\n"
#define ERR_USERSDONTMATCH(nickname,hostname) ":"+hostname+" "+" 502 "+nickname+" :Cant change mode for other users\r\n"
// #define RPL_CHANNELMODEIS(hostname, nick, channel, modes) ":" + hostname + " 324 " + nick + " " + channel + " " + modes + "\r\n"
#define RPL_MODEISLIMIT(channel, hostname, mode, newlimit) ":" + hostname + " MODE " + channel + " " + mode + " " + newlimit + "\r\n"
#define RPL_ALLINV(nickname,hostname) ":"+ hostname +" 665 " + nickname + ": Channel already set in invite-only mode\r\n"
#define RPL_WHOISOPERATOR (host,nick) ":irc.iben.chat 313 "+host+" "+nick+" :is an IRC operator\r\n"
#define RPL_CHANNELMODEIS(client, channel, modes) ":irc.iben.chat 324 " + client + " " + channel + " " + modes + "\r\n"
#define RPL_NICKCHANGE(oldNick, nick,username,hostname) ":" + oldNick + "!~" + username + "@" + hostname + " NICK :" + nick + "\r\n"
#define NICKNAME_RPLY(nickname, username, hostname, newNickName)  ":" + nickname + "!~" + username + "@" + hostname + " NICK :" + newNickName  + "\r\n"
extern bool g_interrupt;
#define RPL_UMODEIS(client, user_modes) ":irc.iben.chat 221 " + client + " " + user_modes + "\r\n"
#define ERR_UMODEUNKNOWNFLAG(nickname) ": 501 " + nickname + " Unknown MODE flag\r\n"

/// join 

#define RPL_NOTOPIC(nick,channelname,ipaddress)":irc.iben.chat 331 " +nick+ " " + channelname + " :no topic is set\r\n"
#define RPL_ENDOFNAMES(hostname, nick, channelname) ":irc.iben.chat 366 " + nick + " " + channelname + " :End of NAMES list\r\n"
#define RPL_NAMREPLY(hostname, clients, channelname, nick) ":irc.iben.chat 353 " + nick + " = " + channelname + " :" + clients + "\r\n"
#define RPL_JOIN(nick, username, channelname, ipaddress,mode) ":" + nick + "!~" + username + "@" + ipaddress + " JOIN " + mode + channelname +"\r\n"
enum State
{
	HANDSHAKE,
	LOGIN,
	REGISTERED
};

// class Channel;



//
class Client
{
	private:
		int				sockfd;
		std::string		msg;
		std::string		nick;
		std::string		host;
		std::string		username;
		std::string		fullname;
		State			state;
		bool			_isoper;
		bool			is_invisible;
		bool			receives_wallops;
		bool			receives_server_notices;

	public:
		Client(int fd, std::string host);
		~Client();
		Client(const Client &c);
		Client &operator=(const Client &c);
		bool operator==(const Client &c)const;
		bool operator<=(const Client &c)const;
		bool operator>=(const Client &c)const;
		bool operator<(const Client &c)const;
		bool operator!=(const Client& c)const;
	// Client functions

		void	reply(std::string msg);
		void	welcome();

	// Getters

		std::string		getMsg() const;
		std::string		getNickname() const;
		std::string		getHost() const;
		std::string		getUsername() const;
		std::string		getFullname() const;
		std::string		getPrefix();
		State			getState() const;
		bool			getIsOper() const;
		int				getFd() const;
		bool			getReceivesWallops() const;
		bool			getIsInvisible() const;
		bool			getReceivesServerNotices() const;
		bool			IsOperator() const;
	// Setters


		void		setMsg(std::string msg);
		void		setNickname(std::string nickname);
		void		setUsername(std::string username);
		void		setFullname(std::string realname);
		void		setHost(std::string hostname);
		void		setState(State newState);
		void		setIsoper(bool isoper);
		void		addMsg(std::string msg);
		void		setIsInvisible(bool ischeck);
		void		setReceivesServerNotices(bool ischeck);
		void		setReceivesWallops(bool ischeck);
		void		setIsOperator(bool ischeck);
};


class Channel
{
    private:
        std::string name;
        std::string password;
        std::string topic;
        long long max_client;
        long long nbr_client;
        std::map<Client*,bool> client;////member
		std::vector<Client> invite;
		// std::set<char> mode;
		bool isKey;
		bool islimit;
		bool isInviteOnly;
		bool isPrivatChannel;
    public:
        Channel(std::string _name,std::string _password);
        ~Channel();
        Channel(const Channel &c);
        Channel &operator=(const Channel &c);
        std::string get_name() const;
        std::string get_password() const;
        std::string getTopic() const;
        long long get_nbr_client() const;
        long long get_max_client() const;
		bool getLimit()const;
		bool getInviteOnly() const;
		bool getKey()const;
	
		// std::string get_mode_string() const;
		void allClientInChannel();
		void getMemberOp();
		void setInviteOnly(bool in);
		void setLimit(bool limit);
        void setMaxClient(long long m);
        void setName(std::string &_name);
        void setPassword(std::string &_passord);
        void setTopic(std::string &_topic);
        void setClientRole(Client &c, bool role);
		void setKey(bool key);
		void setClient(Client &c);
		// void setMode(char mode);
		// void disable_mode(mode_t mode);
		std::string get_list_of_names();
        int		checkIfIsClientNickName(std::string name);
		Client	&getClientByNickName(std::string name);
		// void	sendReplyAllCl(const std::string &msg);
        int		findClientRole(Client &c);
        int		checkIfIsClient(Client &c);
		int		checkIfIsInvite(const Client &c);
        // void    ft_add_to_channel(Client &c,std::string pass);
        void    ft_rm_client(Client &c);
		int		checkIfInviteToChannel(Client &c);
		void    inviteChannel(Client &c,std::string pass);
		void	ft_add_Member(Client &c);
		void sendReplyAll(const std::string &msg, std::string nick_name);
};

class Server
{
	private:
		std::vector<std::string>	_cmd;
		int							_port;
		int							_sock;
		std::vector<Client>			_clients;
		std::vector<Channel>		_channels;
		std::vector<pollfd>			_pollfds;
		std::string					_password;

	public:
		Server(int port, std::string password);
		~Server();

	int		createSocket();
	void	launch();

	void	handleMessage(int fd);
	void	displayClient();
	void							replys(const Client &c, const std::string &msg);
	void								parseCmd(std::string cmd, Client &cl);
	std::vector<std::string>			splitCommands(std::string msg);
	std::vector<std::string> 			splitChannel(std::string msg);
	std::string							readMessage(int fd);
	// void							getAllChannelAddByClient(Client &c,std::string oldnick,std::string newnick);
	//manger Channel 
	void								set_Channel(Channel &ch);
	int									checkIfChannelExist(std::string &target);
	Channel								&getChannel(std::string &target);
	// Manage Clients

	void							clientDisconnect(int fd);
	void							eraseClient(int fd);
	void							newCl();

	bool							is_used(Client cl, std::string name);

	Client							&findClient(int fd);
	Client							&findClient(std::string nickname);
	int								findClientIn(std::string nickname);
	std::vector<Client>::iterator	findClientIt(int fd);

	// IRC Commands...
	int    MODE(std::vector<std::string> args, Client &c);
	int JOIN(std::vector<std::string> args, Client &c);
	int cmdUser(std::vector<std::string> args, Client &cl);
	int cmdNick(std::vector<std::string> args, Client &cl);
	int cmdPass(std::vector<std::string> args, Client &cl);
	
};

// utils

std::string	&del_break(std::string &str);
std::string	ERROR_NEED_MORE_PARAMETERS(Client &client, std::string cmd);

#endif
