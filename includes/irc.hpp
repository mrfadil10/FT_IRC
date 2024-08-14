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

#define RED			"\033[1;91m"
#define GREEN		"\033[32m"
#define YELLOW		"\033[1;93m"
#define BLUE		"\033[1;94m"
#define B_CYAN		"\033[1;96m"
#define B_BLACK		"\033[1;90m"
#define I_GREEN		"\033[1;92m"
#define RESET		"\033[0m"

extern bool g_interrupt;

enum State
{
	HANDSHAKE,
	LOGIN,
	REGISTERED
};

// class Channel;

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

	public:
		Client(int fd, std::string host);
		~Client();

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

	// Setters

		void		setMsg(std::string msg);
		void		setNickname(std::string nickname);
		void		setUsername(std::string username);
		void		setFullname(std::string realname);
		void		setHost(std::string hostname);
		void		setState(State newState);
		void		setIsoper(bool isoper);
		void		addMsg(std::string msg);
};

class Server
{
	private:
		std::vector<std::string>	_cmd;
		int							_port;
		int							_sock;
		std::vector<Client>			_clients;
		std::vector<pollfd>			_pollfds;
		std::string					_password;

	public:
		Server(int port, std::string password);
		~Server();

	int		createSocket();
	void	launch();

	void	handleMessage(int fd);
	void	displayClient();

	void								parseCmd(std::string cmd, Client &cl);
	std::vector<std::string>			splitCommands(std::string msg);
	std::string							readMessage(int fd);

	// Manage Clients

	void							clientDisconnect(int fd);
	void							eraseClient(int fd);
	void							newCl();

	bool							is_used(Client cl, std::string name);

	Client							&findClient(int fd);
	Client							&findClient(std::string nickname);
	std::vector<Client>::iterator	findClientIt(int fd);

	// IRC Commands...

	int cmdUser(std::vector<std::string> args, Client &cl);
	int cmdNick(std::vector<std::string> args, Client &cl);
	int cmdPass(std::vector<std::string> args, Client &cl);
};

// utils

std::string	del_break(std::string str);
std::string	ERROR_NEED_MORE_PARAMETERS(Client &client, std::string cmd);

#endif
