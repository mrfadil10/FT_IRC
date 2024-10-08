#ifndef IRC_HPP
# define IRC_HPP
#include <ctime>
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


#define RED			"\033[1;91m"
#define GREEN		"\033[32m"
#define YELLOW		"\033[1;93m"
#define BLUE		"\033[1;94m"
#define B_CYAN		"\033[1;96m"
#define B_BLACK		"\033[1;90m"
#define I_GREEN		"\033[1;92m"
#define RESET		"\033[0m"

#define ERR_USERSDONTMATCH(server, nick) ":" + server + " 502 " + nick + " :Cannot change mode for other users\r\n"
#define ERR_NOTEXTTOSEND(host, nick) ":" + host + " 412 " + nick + " :No text to send\r\n"
#define RPL_PRIVMSG(nick,username,hostname,target,comment) ":"+nick+"!~"+username+"@"+hostname+" PRIVMSG "+target+" :"+comment+"\r\n"
#define ERR_NORECIPIENT(host, nick, command) ":" + host + " 411 " + nick + " :No recipient given (" + command + ")\r\n"
#define ERR_CHANOPRIVSNEEDED(hostname, channel) ":" + hostname + " 482 " + channel + " :You're not channel operator\r\n"
#define RPL_MODE(nick, username, host,target,mode,target_nick) ":" + nick + "!~" + username + "@" + host + " MODE " + target + " " + mode + " " + target_nick + "\r\n"
#define RPL_INVITE(nick, username, host,target,target_nick) ":" + nick + "!~" + username + "@" + host + " INVITE " + target_nick + " " + target + "\r\n"
#define RPL_KICK(nick, username, host,target,target_nick,message) ":" + nick + "!~" + username + "@" + host + " KICK " + target + " " + target_nick + " " + message + "\r\n"
#define RPL_TOPIC(nick, username, host,target,topic) ":" + nick + "!~" + username + "@" + host + " TOPIC " + target +" "+ topic + "\r\n"
#define RPL_NOTINV(nickname,hostname) (":" + hostname + " 664 " + nickname + ": Channel already not in invite-only mode\r\n")
#define ERR_UNKNOWNCOMMAND(hostname, nick, arg) ":" + hostname + " 421 " + nick + " " + arg + " :Unknown command\r\n"
#define ERR_NEEDMOREPARAMS(nick, hostname, cmd) ":" + hostname + " 461 " + nick + " " + cmd + " :Not enough parameters\r\n"
#define ERR_NOSUCHCHANNEL(hostname, nick, channel) ":" + hostname + " 403 " + nick + " " + channel + " :No such channel\r\n"
#define ERR_USERNOTINCHANNEL(hostname, nick, nick2, chann) ":" + hostname + " 441 " + nick + " " + nick2 + " " + chann + " :They aren't on that channel\r\n"
// #define (hostname, nick, nick2) ":" + hostname + " 401 " + nick + " " + nick2 + " :No such nick\r\n"
#define ERR_KEYSET(hostname, nick, chann) ":" + hostname + " 467 " + nick + " " + chann + " :Channel key already set\r\n"
#define ERR_NOKEYSET(hostname, nick, chann) ":" + hostname + " 467 " + nick + " " + chann + " :Cannot remove channel key (no key set)\r\n"
// #define ERR_UNKNOWNMODE(hostname, nick, c) ":" + hostname + " 472 " + nick + " " + c + " :is unknown mode char to me\r\n"
#define ERR_NOTREGISTERED(nick, hostname) ":" + hostname + " 451 " + nick + " :You have not registered\r\n"
#define ERR_USERONCHANNEL(hostname, nick, chann) ":" + hostname + " 443 " + nick + " " + chann + " :is already on channel\r\n"
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
#define RPL_JOIN_OP(nick, username, channelname, ipaddress) ":@" + nick + "!~" + username + "@" + ipaddress + " JOIN " + channelname + "\r\n"
#define RPL_TOPICDISPLAY(hostname, nick, channel, topic) ":" + hostname + " 332 " + nick + " " + channel + " " + topic + "\r\n"
// #define ERR_USERSDONTMATCH(nickname,hostname) ":"+hostname+" "+" 502 "+nickname+" :Cant change mode for other users\r\n"
// #define RPL_CHANNELMODEIS(hostname, nick, channel, modes) ":" + hostname + " 324 " + nick + " " + channel + " " + modes + "\r\n"
#define RPL_MODEISLIMIT(channel, hostname, mode, newlimit) ":" + hostname + " MODE " + channel + " " + mode + " " + newlimit + "\r\n"
#define RPL_ALLINV(nickname,hostname) ":"+ hostname +" 665 " + nickname + ": Channel already set in invite-only mode\r\n"
#define RPL_WHOISOPERATOR (host,nick) ":" + hostname + " 313 "+host+" "+nick+" :is an IRC operator\r\n"
#define RPL_CHANNELMODEIS(hostname,client, channel, modes) ":" + hostname + " 324 " + client + " " + channel + " " + modes + "\r\n"
#define RPL_NICKCHANGE(oldNick, nick,username,hostname) ":" + oldNick + "!~" + username + "@" + hostname + " NICK :" + nick + "\r\n"
#define NICKNAME_RPLY(nickname, username, hostname, newNickName)  ":" + nickname + "!~" + username + "@" + hostname + " NICK :" + newNickName  + "\r\n"
#define RPL_UMODEIS(hostname,client, user_modes) ":" + hostname + " 221 " + client + " " + user_modes + "\r\n"

#define RPL_CREATIONTIME(hostname,nickname,channelName,creationTime) ":" + hostname + " 329 " + nickname + " " + channelName + " " + creationTime+"\r\n"
/// join 
#define RPL_INVITING(hostname,nick,target_nick,channel) ":"+hostname+" 341 "+nick+" "+target_nick+" "+ channel +"\r\n"
#define RPL_NOTOPIC(hostname,nick,channelname)":"+hostname+" 331 " +nick+ " " + channelname + " :no topic is set\r\n"
#define RPL_ENDOFNAMES(hostname, nick, channelname) ":" + hostname + " 366 " + nick + " " + channelname + " :END of /NAMES list\r\n"
#define RPL_NAMREPLY(hostname, clients, channelname, nick) ":" + hostname + " 353 " + nick + " = " + channelname + " :" + clients + "\r\n"
#define RPL_JOIN(nick, username, channelname, ipaddress) ":" + nick + "!~" + username + "@" + ipaddress + " JOIN " + channelname + "\r\n"


#define REPLY_WELCOME(nick,username, hostname) ":" + hostname + " 001 "+nick +" :Welcome to the ExampleNet IRC Network, "+nick+"!"+username+"@"+hostname+"\r\n"
#define REPLY_YOURHOST(nick, hostname) ":" + hostname + " 002 " + nick + " :Your host is " + hostname + " running version 1.0 !\r\n"
#define REPLY_CREATED(nick, hostname, formatted_time) ":" + hostname + " 003 " + nick + " :This server was created " + formatted_time + " !\r\n"

// #define ERROR_PASSWDMISMATCH(nick, hostname) ":" + hostname + " 464 " + nick + " :Password incorrect !\r\n"
// #define ERROR_ALREADYREGISTERED(nick, hostname) ":" + hostname + " 462 " + nick + " :You are already registered !\r\n"
// #define ERROR_NOTREGISTERED(nick, hostname) ":" + hostname + " 451 " + nick + " :You have not registered !\r\n"

// #define ERROR_ERRONEUSNICKNAME(nick, hostname) ":" + hostname + " 432 " + nick + " :Erroneus nickname !\r\n"
// #define ERROR_NONICKNAMEGIVEN(nick, hostname) ":" + hostname + " 431 " + nick + " :No nickname given !\r\n"
// #define ERROR_NICKNAMEINUSE(nick, hostname) ":" + hostname + " 433 " + nick + " :Nickname is already in use !\r\n"
extern bool g_interrupt;
// #define REPLY_NICKCHANGE(oldNick, nick, hostname) ":" + oldNick + "!~u@" + hostname + " NICK " + nick + "\r\n"
// #define ERROR_NEEDMOREPARAMS(nick, hostname) (std::string (":") + hostname + std::string(" 461 ") + nick + " :Not enough parameters !\r\n")

#define PART_REPLY(nickname, username, hostname, channel, reason) ":" + nickname + "!~" + username + "@" + hostname + " PART " + channel +" "+ reason + "\r\n"

// #define RPL_AWAY(sender , username, hostname, message , nick_resever) ":" + std::string(sender) + "!~" + std::string(username) + "@" + std::string(hostname) + " PRIVMSG " + nick_resever + " :" + message + "\r\n"

// #define ERROR_CHANNELISFULL(nick, channelName) (std::string(":") + nick + " 471 " + channelName + " :Cannot join channel (+l)\r\n")
// #define ERROR_BADCHANNELKEY(nick, hostname, channelName) (std::string(":") + hostname + " 475 " + nick + " " + channelName + " :Cannot join channel (+K) - bad key\r\n")
// #define ERROR_INVITEONLY(nick, channelName) ":" + nick + " 473 " + channelName + " :Cannot join channel (+i)\r\n"

// #define REPLY_JOIN(nick, username, channelname, ipaddress) ":" + nick + "!~" + username + "@" + ipaddress + " JOIN " + channelname + "\r\n"
// #define REPLY_TOPICDISPLAY(hostname, nick, channel, topic) ":" + hostname + " 332 " + nick + " " + channel + " :" + topic + "\r\n"


#define REPLY_TOPICWHOTIME(username, topic_time, nick, hostname, channelName) ":" + hostname + " 333 " + nick + " " + channelName + " " + nick + "!~" + username + "@" + hostname + " " + topic_time + "\r\n"
// #define REPLY_NAMREPLY(hostname, clients, channelname, nick) ":" + hostname + " 353 " + nick + " = " + channelname + " :" + clients + "\r\n"
// #define REPLY_ENDOFNAMES(hostname, nick, channelname) ":" + hostname + " 366 " + nick + " " + channelname + " :END of /NAMES list\r\n"

#define ERROR_INVALIDMODEPARAM_LIMIT(channel, hostname,arg) ":" + hostname + " 696 " + channel + " l "+arg+" You must specify a parameter for the limit mode. Syntax: <limit>.\r\n"
// #define ERROR_INVALIDMODEPARAM_LIMIT(channel, hostname) ":" + hostname + " 696 " + channel + " l "+a+" You must specify a parameter for the limit mode. Syntax: <limit>.\r\n"
#define ERROR_INVALIDMODEPARAM_OP(channel, hostname) ":" + hostname + " 696 " + channel + " o * you must specifiy a parameter for the op mode. Syntax: <nick>.\r\n"
// #define ERROR_INVALIDKEY(channel, hostname, key) ":" + hostname + " 525 " + channel + " :Key is not well-formed :`" + key + "`\r\n"
#define ERROR_INVALIDMODEPARAM__KEY(nick,channel, hostname, param) ":"+hostname+" 696 "+nick+" "+channel+" k * :Invalid mode k parameter: '"+param+"'\r\n"
#define ERROR_USERNOTINCHANNEL(hostname, channel,target_nick) ":" + hostname + " 441 " + channel + " " +target_nick +" :they aren't on that channel\r\n"
// #define ERROR_NOPRIVILEGES(hostname, channel) ":" + hostname + " 482 " + channel + " " + ":You're not a channel operator\r\n"
// #define ERROR_NOPRIVILEGES__(hostname, channel, flag) ":" + hostname + " 482 " + channel + " " + ":You're not a channel operator to set channel mode " + flag + "\r\n"

// #define REPLY_CREATIONTIME(hostname, channelName, nick, channel_time) ":" + hostname + " 329 " + nick + " " + channelName + " " + channel_time + "\r\n"
// #define REPLY_CHANNELMODES(hostname, channelName, nick, channelmode) ":" + hostname + " 324 " + nick + " " + channelName + " " + channelmode + "\r\n"
// #define REPLY_CHANNELMODES__(username, channelName, nick, channelmode) ":" + nick + "!~" + username + " MODE " + " " + channelName + " " + channelmode + "\r\n"

// #define REPLY_INVITING(hostname, inviting, invited, channel) ":" + hostname + " 341 " + inviting + " " + invited + " " + channel + " :Inviting " + invited + " to " + channel + "\r\n"
// #define REPLY_INVITE(nick, username, clienthostname, invited, channel) ":" + nick + "!" + username + "@" + clienthostname + " INVITE " + invited + " :" + channel + "\r\n"
// #define ERROR_NOTONCHANNEL(hostname, channel) ":" + hostname + " 442 " + channel + " " + ":You're not on that channel\r\n"
// #define ERROR_NOSUCHCHANNEL(hostname, channel, nick) ":" + hostname + " 403 " + nick + " " + channel + " :No such channel\r\n"
// #define ERROR_NOSUCHNICK(hostname, channel, argument) ":" + hostname + " 401 " + channel + " " + argument + " :No such nickname\r\n"
// #define ERROR_USERONCHANNEL(hostname, channel, nick) ":" + hostname + " 443 " + nick + " " + channel + "  :is already on channel\r\n"

// #define REPLY_SETTOPIC(nick, uname, hostname, channel, topic) ":" + nick + "!~" + uname + "@" + hostname + " TOPIC " + channel + " :" + topic + "\r\n"

#define ERR_UNKNOWNMODE(nick, hostname, channel, character) ":" + hostname + " 472 " + nick + " " + channel + " " + character + " :is unknown mode character to me\r\n"
// #define REPLY_YOUREOPER(hostname, nick) ":" + hostname + " 381 " + nick + ":You are now an IRC operator\r\n"
// #define REPLY_KICK(kicker, username, host, channel, targetuser, reason) ":" + kicker + "!" + username + "@" + host + " KICK " + channel + " " + targetuser + " :" + reason + "\r\n"
// #define PRIVMSG_FORMAT(senderNick, senderUsername, senderHostname, receiver, message) ":" + senderNick + "!~" + senderUsername + "@" + senderHostname + " PRIVMSG " + receiver + " :" + message + "\r\n"
// #define ERROR_UNKNOWNCOMMAND(nick, hostname, command) ":" + hostname + " 421 " + nick + " " + command + " :Unknown command\r\n"

// #define REPLY_YOUREOPER(hostname, nick) ":" + hostname + " 381 " + nick + ":You are now an IRC operator\r\n"
// #define REPLY_KICK(kicker, username, host, channel, targetuser, reason) ":" + kicker + "!" + username + "@" + host + " KICK " + channel + " " + targetuser + " :" + reason + "\r\n"
// #define PRIVMSG_FORMAT(senderNick, senderUsername, senderHostname, receiver, message) ":" + senderNick + "!~" + senderUsername + "@" + senderHostname + " PRIVMSG " + receiver + " :" + message + "\r\n"
// #define ERROR_UNKNOWNCOMMAND(nick, hostname, command) ":" + hostname + " 421 " + nick + " " + command + " :Unknown command\r\n"

// #define RPL_NOTOPIC(hostname, channel) ":" + hostname + " 331 " + channel + " :No topic is set\r\n"
// #define RPL_TOPIC(hostname, channel, topic) ":" + hostname + " 332 " + channel + " :" + topic + "\r\n"
// #define ERR_NOTONCHANNEL(hostname, channel, nick) ":" + hostname + " 442 " + nick + " " + channel + " :You're not on that channel\r\n"
// #define ERR_NOTEXTTOSEND(hostname) ":" + hostname + " 412 " + ":No text to send\r\n"
#define ERR_NOSUCHNICK(hostname, nick,nick_target) ":" + hostname + " 401 " + nick +" "+nick_target+" :No such nick\r\n"

// #define ERROR_NEEDTOREGISTER(nick, hostname, command) ":" + hostname + " 422 " + nick + " " + command + " :You need to register before you can use that command\r\n"
// #define ERROR_REALNAME(nick, hostname) ":" + hostname + " 423 " + nick + " :Error in realename !\r\n"

// #define ERR_KEYSET(hostname, channelname) ":" + hostname  + " 467 "+ channelname + " :Channel key already set\r\n"

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
		
		// bool			receives_wallops;
		// bool			receives_server_notices;
		
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
		// std::string getStartTimestp() const;
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
        std::map<std::string,std::pair<bool,int> > client;////member
		std::map<std::string,int> invite;
		std::set<char> mode;
		std::string timeScTo;
		std::string timeScCh;
		std::string arg;
		bool isKey;
		bool islimit;
		bool isInviteOnly;
		bool isTopic;
		bool chTopicOp;
    public:
        Channel(std::string _name,std::string password);
        // ~Channel();
		void clearTopic();
        Channel(const Channel &c);
		void	setFdClien(int fd);
        Channel &operator=(const Channel &c);
		int getModeSize();
    	// const std::map<std::string, bool>& getClients() const { return client; }
        std::string get_name() const;
        std::string get_password() const;
        std::string getTopic() const;
		void clearInvite();
        long long get_nbr_client() const;
        long long get_max_client() const;
		std::string getTopicTimestp() const;
		void	eraseClient(std::string nickname);
		bool getLimit()const;
		bool getInviteOnly() const;
		bool getKey()const;
		void setMode(char s);
		void eraseMode(char s);
		std::string getMode(int i);
		bool hasMode(char mode) const;
		void setTime(std::string const &time);
		void setTimeTop(std::string const &time);
		std::string getTime();
		std::string getTimeTop();
		// std::string get_mode_string() const;
		void allClientInChannel();
		void getMemberOp();
		void setInviteOnly(bool in);
		void setLimit(bool limit);
        void setMaxClient(long long m);
        void setName(std::string &_name);
        void setPassword(std::string &_passord);
        void setTopic(std::string &_topic);
        void setClientRole(std::string const &nickname, bool role);
		void setKey(bool key);
		void setIsTopic(bool istopic);
		bool getIsTopic();
		void setChTopOp(bool istopic);
		bool getChTopOp();
		void	setClient(std::string const &nickname,bool role);
		void	addClient(int fd, const std::string& nickname, bool isOperator);
		void	addIviteClient(int fd, const std::string& nickname);
		std::string get_list_of_names();
        int		checkIfIsClientNickName(std::string name);
		Client	&getClientByNickName(std::string name);
        int		findClientRole(std::string nikname);
        int		checkIfIsClient(std::string const &nickname);
		int		checkIfIsInvite(const Client &c);
        void    removeClientNickName(std::string const &nickname);
		int		checkIfInviteToChannel(Client &c);
		void    inviteChannel(Client &c,std::string pass);
		void	addMember(std::string const &name);
		void	sendReplyAll(const std::string &msg, std::string nickname);
};

class Server
{
	private:
		std::string					_cmd;
		int							_port;
		int							_sock;
		std::map<int,Client*>			_clients;
		std::map<std::string,Channel*>	_channels;
		std::vector<pollfd>			_pollfds;
		std::string					_password;
		
	public:
		Server(int port, std::string password);
		~Server();

	int		createSocket();
	void	launch();

	void	handleMessage(int fd);
	void	displayClient();
	void							replys(Client &c,std::string msg);
	void								parseCmd(std::string cmd, Client &cl,int fd);
	std::vector<std::string>			splitCommands(std::string msg);
	std::vector<std::string> 			splitChannel(std::string msg);
	std::string							readMessage(int fd);
	Client								*getClientByNickNameS(std::string Nickname);
	void								set_Channel(Channel const &ch);
	int									checkIfChannelExist(std::string &target);
	Channel								*getChannel(std::string &target);
	void							joinZero(Client &c);
	void							clientDisconnect(int fd);
	void							eraseClient(int fd);
	void							newCl();

	bool							is_used(Client cl, std::string name);

	Client							*findClient(int fd);
	int								findClientInS(std::string nickname);
	std::map<int,Client*>::iterator	findClientIt(int fd);

	// IRC Commands...
	int		MODE(std::string args, Client &c);
	int		JOIN(std::string args, Client &c);
	int		TOPIC(std::string args, Client &c);
	int		KICK(std::string args, Client &c);
	int		INVITE(std::string args, Client &c);
	int		PART(std::string cmd, Client &c);
	int 	PRIVMSG(std::string args, Client& client);
	int cmdUser(std::string args, Client &cl);
	int cmdNick(std::string args, Client &cl);
	int cmdPass(std::string args, Client &cl);
	
};

// utils
long long	aatoi(const char * str);
std::string	del_break(std::string str);
std::string	ERROR_NEED_MORE_PARAMETERS(Client &client, std::string cmd);
std::string getTimeSc();
#endif
