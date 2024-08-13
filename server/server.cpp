/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mfadil <mfadil@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/10 17:28:00 by mfadil            #+#    #+#             */
/*   Updated: 2024/08/13 16:38:39 by mfadil           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/irc.hpp"

Server::Server(int port, std::string password) :_port(port), _host("127.0.0.1"), _password(password), _operPassword("operator's password")
{
	_sock = createSocket();
}

Server::~Server() {}

// getters

int		Client::getFd() const {
	return (_sockfd);
}

std::string	Client::getNickname() const {
	return (_nickname);
}

std::string	Client::getUsername() const {
	return (_username);
}

std::string	Client::getRealname() const {
	return (_realname);
}

std::string	Client::getMsg() const {
	return (_msg);
}

std::string	Client::getHostname() const {
	return (_hostname);
}

State	Client::getState() const {
	return (_state);
}
bool	Client::getIsOper() const {
	return (_isoper);
}

// setters
void	Client::setUsername(std::string _new) {
	_username = _new;
}

void	Client::setNickname(std::string _new) {
	_nickname = _new;
}

void	Client::setRealname(std::string _new) {
	_realname = _new;
}

void	Client::setMsg(std::string Newmsg) {
	_msg = Newmsg;
}

void	Client::setState(State Newstate) {
	_state = Newstate;
}

void	Client::setIsoper(bool Newstate) {
	_isoper = Newstate;
}

void	Client::addMsg(std::string message) {
	_msg += message;
}
void	Client::setHostname(std::string _new) {
	_hostname = _new;
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
		<< "   realname:" << _clients.at(j).getRealname() << RESET << std::endl;
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
	// eraseClientChannel(findClient(fd));
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

std::vector<std::string> Server::splitCmd(std::string msg)
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

std::string	Server::readMsg(int fd)
{
	std::string	msg;
	char	buff[256];
	bzero(buff, 256);
	std::vector<Client>::iterator cl = findClientIt(fd);
	msg = cl->getMsg();

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
		this->_cmd = splitCmd(readMsg(fd));
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

// Channel		&Server::findChannel(std::string name)
// {
// 	for (unsigned int i = 0; i < _channels.size(); i++)
// 	{
// 		if (_channels[i].getName() == name)
// 			return (_channels[i]);
// 	}
// 	throw (std::out_of_range("\033[1;94mdidn't find channel -_-\033[0m"));
// }

// void	Server::eraseClientChannel(Client &cl)
// {
// 	for (unsigned int i = 0; i < _channels.size(); i++)
// 	{
// 		_channels[i].eraseClient(cl);
// 	}
// 	std::vector<Channel>::iterator	it = _channels.begin();
// 	while (it != _channels.end())
// 	{
// 		if (it->getClients().empty())
// 			it = _channels.erase(it);
// 		else
// 			it++;
// 	}
// 	std::cout << "eraseClientChannel" << std::endl;
// }

// Channel::Channel(std::string Name) : _name(Name), _topic(), _chanPass(""), _fdOp(0), _limit(0)
// {}

// Channel::~Channel() {}

// getters & setters clients

// std::vector<Client>		&Channel::getClients() {
// 	return (_clients);
// }

// std::string					Channel::getName() const {
// 	return (_name);
// }
// std::string					Channel::getTopic() const {
// 	return (_topic);
// }

// std::string					Channel::getPassword() const {
// 	return (_chanPass);
// }

// int						Channel::getFdOp() const {
// 	return (_fdOp);
// }

// size_t					Channel::getLimit() const {
// 	return (_limit);
// }

// void					Channel::setTopic(std::string newTopic) {
// 	_topic = newTopic;
// }

// void					Channel::setFdOp(int fd) {
// 	_fdOp = fd;
// }

// void					Channel::setPassword(std::string pass) {
// 	_chanPass = pass;
// }

// void					Channel::setLimit(size_t limit) {
// 	_limit = limit;
// }

// void					Channel::addClient(Client &cl) {
// 	_clients.push_back(cl);
// }

// RPL PART
// std::string		util(std::string prefix, std::string name)
// {
// 	return (prefix + "PART :" + name);
// }

// void	Channel::eraseClient(Client &cl)
// {
// 	std::vector<Client>::iterator it;
// 	for (it = _clients.begin(); it != _clients.end(); it++)
// 	{
// 		std::cout << "here " << it->getNickname() << "==" << cl.getNickname() << std::endl;
// 		if (it->getFd() == cl.getFd())
// 		{
// 			std::cout << YELLOW << "erasing client..." << RESET << std::endl;
// 			broadcast(util(cl.getPrefix(), _name));
// 			_clients.erase(it);
// 			return ;
// 		}
// 	}
// 	std::cout << "not really erasing client" << _clients.size() << std::endl;
// }

// std::vector<Channel>::iterator	Server::findChannelIt(std::string name)
// {
// 	std::vector<Channel>::iterator ret = _channels.begin();
// 	std::vector<Channel>::iterator end = _channels.end();
// 	while (ret != end)
// 	{
// 		if (ret->getName() == name)
// 			return (ret);
// 		ret++;
// 	}
// 	throw (std::out_of_range("Error while searching for channel"));
// }

// void	Channel::broadcast(std::string msg, Client &cl)
// {
// 	msg += "\r\n";
// 	std::cout << "----> " << msg << std::endl;
// 	for (unsigned int i = 0; i < _clients.size(); i++)
// 	{
// 		if (cl.getFd() != _clients[i].getFd())
// 		{
// 			if (send(_clients[i].getFd(), msg.c_str(), msg.length(), 0) < 0)
// 				throw (std::runtime_error("error while broadcasting"));
// 		}
// 	}
// }

// void	Channel::broadcast(std::string msg)
// {
// 	msg += "\r\n";
// 	std::cout << "----> " << msg << std::endl;
// 	for (unsigned int i = 0; i < _clients.size(); i++)
// 	{
// 		if (send(_clients[i].getFd(), msg.c_str(), msg.length(), 0) < 0)
// 			throw (std::runtime_error("error while broadcasting"));
// 	}
// }

// void	Channel::debug()
// {
// 	for (unsigned int i = 0; i < _clients.size(); i++)
// 		std::cout << "#client " << i << " " << _clients[i].getNickname() << std::endl;
// }

Client::Client(int sockfd, std::string hostname) : _sockfd(sockfd), _hostname(hostname), _isoper(false)
{
	_state = HANDSHAKE;
	_msg = "";
}

Client::~Client() {}

std::string Client::getPrefix()
{
	std::string prefix = ":" + _nickname + (_username.empty() ? "" : "!" + _username) + (_hostname.empty() ? "" : "@" + _hostname);
	return (prefix);
}

void	Client::reply(std::string msg)
{
	std::string prefix = _nickname + (_username.empty() ? "" : "!" + _username) + (_hostname.empty() ? "" : "@" + _hostname);
	std::string paquet = prefix + " " + msg + "\r\n";
	std::cout << "----> " << paquet << std::endl;
	if (send(_sockfd, paquet.c_str(), paquet.length(), 0) < 0)
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

void    Client::welcome()
{
	if (_state != LOGIN || _nickname.empty() || _username.empty())
	{
		std::cout << YELLOW << "Waiting registration... " << _nickname << std::endl;
		return ;
	}
	_state = REGISTERED;
	reply("001 " + _nickname + " :Welcome " +_nickname +  " into our irc network");
	std::cout << _nickname << " is registered" << std::endl;
}
