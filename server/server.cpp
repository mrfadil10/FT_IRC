/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mfadil <mfadil@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/10 17:28:00 by mfadil            #+#    #+#             */
/*   Updated: 2024/08/14 22:50:51 by mfadil           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/irc.hpp"

Server::Server(int port, std::string password) :_port(port), _password(password)
{
	_sock = createSocket();
}

Server::~Server() {}

// getters

int		Client::getFd() const {
	return (sockfd);
}

std::string	Client::getNickname() const {
	return (nick);
}

std::string	Client::getUsername() const {
	return (username);
}

std::string	Client::getFullname() const {
	return (fullname);
}

std::string	Client::getMsg() const {
	return (msg);
}

std::string	Client::getHost() const {
	return (host);
}

State	Client::getState() const {
	return (state);
}
bool	Client::getIsOper() const {
	return (_isoper);
}

// setters
void	Client::setUsername(std::string _new) {
	username = _new;
}

void	Client::setNickname(std::string _new) {
	nick = _new;
}

void	Client::setFullname(std::string _new) {
	fullname = _new;
}

void	Client::setMsg(std::string Newmsg) {
	msg = Newmsg;
}

void	Client::setState(State Newstate) {
	state = Newstate;
}

void	Client::setIsoper(bool Newstate) {
	_isoper = Newstate;
}

void	Client::addMsg(std::string message) {
	msg += message;
}
void	Client::setHost(std::string _new) {
	host = _new;
}

int		Server::createSocket()
{
	int	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		throw std::runtime_error("\033[1;91mError while opening socket.\033[0m");

	int	val = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)))
		throw std::runtime_error("\033[1;91mError while setting socket options.\033[0m");

	if (fcntl(sockfd, F_SETFL, O_NONBLOCK) == -1)
		throw std::runtime_error("\033[1;91mError while setting socket to NON-BLOCKING.\033[0m");

	sockaddr_in serv_address = {};
	bzero((char *) &serv_address, sizeof(serv_address));
	serv_address.sin_port = htons(_port);
	serv_address.sin_family = AF_INET;
	serv_address.sin_addr.s_addr = INADDR_ANY;

	if (bind(sockfd, (struct sockaddr *) &serv_address, sizeof(serv_address)) < 0)
		throw std::runtime_error("\033[1;91mError while binding socket.\033[0m");
	if (listen(sockfd, 1000) < 0)
		throw std::runtime_error("\033[1;91mError while listening on socket.\033[0m");
	return (sockfd);
}

void	Server::newCl()
{
	int	new_fd;
	char	buf[2048];
	sockaddr_in s_adress = {};
	socklen_t s_size = sizeof(s_adress);

	new_fd = accept(_sock, (sockaddr *)&s_adress, &s_size);
	if (new_fd < 0)
		throw std::runtime_error("\033[1;91mError while accepting new client.\033[0m");
	if (getnameinfo((struct sockaddr *) &s_adress, sizeof(s_adress), buf, NI_MAXHOST, NULL, 0, NI_NUMERICSERV) != 0)
		throw std::runtime_error("\033[1;91mError while getting hostname on new client.\033[0m");
	_clients.push_back(Client(new_fd, buf));
	pollfd pollfd = {new_fd, POLLIN, 0};
	_pollfds.push_back(pollfd);
}

void	Server::displayClient()
{
	std::string state;
	size_t i = _clients.size();
	std::cout << GREEN << "Clients connected: " << i << RESET << std::endl;
	for (size_t j = 0; j < i; j++)
	{
		state = (_clients.at(j).getState() == REGISTERED) ? "yes" : "no";
		std::cout << BLUE << "client[" << j << "]:" << " registered:" << state
		<< "   nick:" << _clients.at(j).getNickname()
		<< "   user:" << _clients.at(j).getUsername()
		<< "   realname:" << _clients.at(j).getFullname() << RESET << std::endl;
	}
	std::cout << std::endl;
	return ;
}

void	Server::eraseClient(int fd)
{
	std::vector<Client>::iterator it = _clients.begin();

	for (; it != _clients.end(); ++it)
	{
		if (it->getFd() == fd)
		{
			_clients.erase(it);
			return ;
		}
	}
}

void	Server::clientDisconnect(int fd)
{
	std::vector<pollfd>::iterator it = _pollfds.begin();
	eraseClient(fd);
	while (it != _pollfds.end())
	{
		if (it->fd == fd)
		{
			_pollfds.erase(it);
			break ;
		}
		it++;
	}
	close(fd);
	std::cout << RED << "A client has been disconnected." << RESET << std::endl;
}

std::vector<std::string> Server::splitCommands(std::string msg)
{
	int					i = 0;
	std::vector<std::string>	cmd;
	std::stringstream	str(msg);
	std::string				tm;
	if (msg == "\n")
		return (cmd);
	while (std::getline(str, tm, '\n'))
	{
		cmd.push_back(tm);
		std::cout << cmd.at(i++) << std::endl;
	}
	return (cmd);
}

std::vector<Client>::iterator	Server::findClientIt(int fd)
{
	std::vector<Client>::iterator ret = _clients.begin();
	std::vector<Client>::iterator end = _clients.end();
	while (ret != end)
	{
		if (ret->getFd() == fd)
			return (ret);
		ret++;
	}
	throw (std::out_of_range("\033[1;91mError while searching for user\033[0m"));
}

std::string	Server::readMessage(int fd)
{
	std::string	msg;
	char	buff[256];
	bzero(buff, 256);
	std::vector<Client>::iterator cl = findClientIt(fd);
	msg = cl->getMsg();
	std::cout << "msg : <" << msg << ">"<< std::endl;

	while (!(std::strstr(buff, "\n")))
	{
		int	k = 0;
		bzero(buff, 256);
		if ((k = recv(fd, buff, 256, MSG_DONTWAIT)) < 0)
		{
			if (errno != EWOULDBLOCK)
				throw std::runtime_error("\033[1;91merror in reciving message\033[0m");
			return ("");
		}
		else if (k == 0)
			throw (std::out_of_range("\033[1;91mTEST\033[0m"));
		cl->addMsg(buff);
		msg += buff;
	}
	cl->setMsg("");
	return (msg);
}

void	Server::handleMessage(int fd)
{
	try
	{
		this->_cmd = splitCommands(readMessage(fd));
		std::cout << this->_cmd.size() << std::endl;
	}
	catch (const std::exception& e)
	{
		clientDisconnect(fd);
		std::cerr << e.what() << '\n';
		return ;
	}
	for (std::vector<std::string>::iterator it = this->_cmd.begin(); it != this->_cmd.end(); it++)
		parseCmd(*it, findClient(fd));
	displayClient();
}

void	Server::parseCmd(std::string str, Client &cl)
{
	std::string	tmp;
	std::vector<std::string> args;
	std::stringstream	ss(str);
	std::getline(ss, tmp, ' ');

	args.push_back(tmp);
	std::cout << "Parse command : [" << tmp << "]" << std::endl;

	std::string cmds[15] = {"PASS", "NICK", "USER", "OPERATOR", "PRIVATE MESSAGE", "JOIN", "KILL", "PING", "PART", "LIST", "NAMES", "TOPIC", "KICK", "MODE", "NOTICE"};

	int		(Server::*ptr[15])(std::vector<std::string> args, Client &cl) = {
			&Server::cmdPass,
			&Server::cmdNick,
			&Server::cmdUser,
			// &Server::OperCmd,
			// &Server::PrivMsgCmd,
			// &Server::JoinCmd,
			// &Server::KillCmd,
			// &Server::PingCmd,
			// &Server::PartCmd,
			// &Server::ListCmd,
			// &Server::NamesCmd,
			// &Server::TopicCmd,
			// &Server::KickCmd,
			// &Server::ModeCmd,
			// &Server::NoticeCmd,
	};
	for (int i = 0; i <= 14; ++i)
	{
		if (tmp == cmds[i])
		{
			while (std::getline(ss, tmp, ' '))
				args.push_back(tmp);
			(this->*ptr[i])(args, cl);
			return ;
		}
	}
}

Client		&Server::findClient(std::string nickname)
{
	for (unsigned int i = 0; i < _clients.size(); i++)
	{
		if (_clients[i].getNickname() == nickname)
			return (_clients[i]);
	}
	throw (std::out_of_range("\033[1;91mError while searching for user\033[0m"));
}

Client		&Server::findClient(int fd)
{
	for (unsigned int i = 0; i < _clients.size(); i++)
	{
		if (_clients[i].getFd() == fd)
			return (_clients[i]);
	}
	throw (std::out_of_range("\033[1;91mError while searching for user\033[0m"));
}

Client::Client(int sockfd, std::string hostname) : sockfd(sockfd), host(hostname), _isoper(false)
{
	state = HANDSHAKE;
	msg = "";
}

Client::~Client() {}

std::string Client::getPrefix()
{
	std::string prefix = ":" + nick + (username.empty() ? "" : "!" + username) + (host.empty() ? "" : "@" + host);
	return (prefix);
}

void	Client::reply(std::string msg)
{
	std::string prefix = nick + (username.empty() ? "" : "!" + username) + (host.empty() ? "" : "@" + host);
	std::string pack = prefix + " " + msg + "\r\n";
	std::cout << I_GREEN << "----> " << pack << RESET << std::endl;
	if (send(sockfd, pack.c_str(), pack.length(), 0) < 0)
		throw (std::runtime_error("\033[1;91mError while sending message\033[0m"));
}

void	Server::launch()
{
	pollfd fd_server = {_sock, POLLIN, 0};
	_pollfds.push_back(fd_server);

	std::cout << I_GREEN << "Server is started on port " << _port << RESET << std::endl;
	while (g_interrupt == false)
	{
		if (poll(_pollfds.begin().base(), _pollfds.size(), -1) < 0)
			break ;
		for (unsigned int i = 0; i < _pollfds.size(); i++)
		{
			if (_pollfds[i].revents == 0)
				continue ;
			if ((_pollfds[i].revents & POLLIN) == POLLIN)
			{
				if (_pollfds[i].fd == _sock)
				{
					newCl();
					displayClient();
					break ;
				}
			}
			handleMessage(_pollfds[i].fd);
		}
	}
	for (unsigned int i = 0; i < _pollfds.size(); i++)
		close(_pollfds[i].fd);
}

void	Client::welcome()
{
	if (state != LOGIN || nick.empty() || username.empty())
	{
		std::cout << YELLOW << "Waiting registration... " << nick << std::endl;
		return ;
	}
	state = REGISTERED;
	reply("001 " + nick + " :Welcome " + nick +  " into our irc network");
	std::cout << B_CYAN << nick << " is registered" << RESET << std::endl;
}
