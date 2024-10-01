/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ibenaait <ibenaait@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/10 17:28:00 by mfadil            #+#    #+#             */
/*   Updated: 2024/10/01 22:01:29 by ibenaait         ###   ########.fr       */
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
void		Client::setIsOperator(bool ischeck)
{
	_isoper = ischeck;
}
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
	// _clients.push_back(Client(new_fd, buf));
	_clients[new_fd] = new Client(new_fd,buf);
	pollfd pollfd = {new_fd, POLLIN, 0};
	_pollfds.push_back(pollfd);
}

void	Server::displayClient()
{
	std::string state;
	size_t i = _clients.size();
	std::map<int,Client*>::iterator it = _clients.begin();
	std::cout << GREEN << "Clients connected: " << i << RESET << std::endl;
	int j = 0;
	while(it != _clients.end())
	{
		state = (it->second->getState() == REGISTERED) ? "yes" : "no";
		std::cout << BLUE << "client[" << j << "]:" << " registered:" << state
		<< "   nick:" << it->second->getNickname()
		<< "   user:" << it->second->getUsername()
		<< "   realname:" << it->second->getFullname() << RESET << std::endl;
		j++;
		it++;
	}
	std::cout << std::endl;
	return ;
}

void	Server::eraseClient(int fd)
{
	std::map<int,Client*>::iterator it = _clients.begin();

	for (; it != _clients.end(); ++it)
	{
		if (it->second->getFd() == fd)
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

	// std::vector<std::string>	cmd;
	// std::stringstream	str(msg);
	// std::string				tm;
	// if (msg == "\n")
	// 	return (cmd);
	// while (std::getline(str, tm, '\n'))
	// {
	// 	cmd.push_back(tm);

	// }
	// return (cmd);
    std::vector<std::string> cmd;
    std::string part;
    std::istringstream iss(msg);
	
    while (std::getline(iss, part, ' ')) 
	{
        size_t colonPos = part.find(':');
        if (colonPos != std::string::npos) {
            if (colonPos > 0) {
                cmd.push_back(part.substr(0, colonPos));
            }
            std::string restOfMessage = part.substr(colonPos + 1);
            std::string remaining;
            if (std::getline(iss, remaining)) {
                restOfMessage += " " + remaining;
            }
			
            cmd.push_back(restOfMessage);
            return cmd; 
        }
		if(!part.empty())
			cmd.push_back(part);
    }
    return cmd;
}

std::map<int,Client*>::iterator	Server::findClientIt(int fd)
{
	std::map<int,Client*>::iterator ret = _clients.find(fd);
	// std::vector<Client>::iterator end = _clients.end();
	if(ret != _clients.end())
	{
		return ret;
	}
	throw (std::out_of_range("\033[1;91mError while searching for user3\033[0m"));
}

std::string	Server::readMessage(int fd)
{
	std::string	msg;
	char	buff[256];
	bzero(buff, 256);
	std::map<int,Client*>::iterator cl = findClientIt(fd);
	msg = cl->second->getMsg();
	// std::cout << "msg : <" << msg << ">"<< std::endl;

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
		cl->second->setMsg(buff);
		msg += buff;
	}
	cl->second->setMsg("");
	return (msg);
}

void	Server::handleMessage(int fd)
{
	try
	{
		this->_cmd = readMessage(fd);
		// std::cout << this->_cmd.size() << std::endl;
	}
	catch (const std::exception& e)
	{
		clientDisconnect(fd);
		std::cerr << e.what() << '\n';
		return ;
	}
	try
	{
		std::vector<std::string> v = splitCommands(del_break(this->_cmd)); 
		// for (std::vector<std::string>::iterator it = v.begin(); it != v.end(); it++)
		parseCmd(v.at(0), *findClient(fd));
		displayClient();
	}
	catch(const std::exception& e)
	{
		_clients[fd]->reply("->");
	}
}

void	Server::parseCmd(std::string str, Client &cl)
{
    std::map<std::string, int (Server::*)(std::string, Client &)> cmdMap;
	cmdMap["PASS"]    = &Server::cmdPass;
    cmdMap["NICK"]    = &Server::cmdNick;
    cmdMap["USER"]    = &Server::cmdUser;
    cmdMap["JOIN"]    = &Server::JOIN;
    cmdMap["MODE"]    = &Server::MODE;
    cmdMap["TOPIC"]   = &Server::TOPIC;
    cmdMap["KICK"]    = &Server::KICK;
    cmdMap["PART"]    = &Server::PART;
    cmdMap["INVITE"]  = &Server::INVITE;
    cmdMap["PRIVMSG"] = &Server::PRIVMSG;
	
    std::map<std::string, int (Server::*)(std::string, Client &)>::iterator it = cmdMap.find(str);
    if (it != cmdMap.end()) {
        (this->*(it->second))(del_break(_cmd), cl);
    } else {
        cl.reply(ERR_UNKNOWNCOMMAND(cl.getHost(), cl.getNickname(), del_break(str)));
    }
}

// Client		&Server::findClient(std::string nickname)
// {
// 	for (unsigned int i = 0; i < _clients.size(); i++)
// 	{
// 		if (_clients[i].getNickname() == nickname)
// 			return (_clients[i]);
// 	}
// 	throw (std::out_of_range("\033[1;91mError while searching for user1\033[0m"));
// }
int		Server::findClientInS(std::string nickname)
{
	std::map<int,Client*>::iterator cl = _clients.begin();
	while(cl != _clients.end())
	{
		if (cl->second->getNickname().compare(nickname) == 0)
			return (1);
	}
	return 0;
}

Client		*Server::findClient(int fd)
{
	std::map<int,Client*>::iterator cl = _clients.find(fd);
	if(cl != _clients.end())
		return cl->second;
	// for (unsigned int i = 0; i < _clients.size(); i++)
	// {
	// 	if (_clients[i].getFd() == fd)
	// 		return (_clients[i]);
	// }
	return NULL;
}

Client::Client(int sockfd, std::string hostname) : sockfd(sockfd), host(hostname), _isoper(false)
{
	state = HANDSHAKE;
	msg = "";
	creatServerTime = time(NULL);
}

Client::~Client() {}

Client::Client(const Client &c)
{
	*this = c;
}
Client &Client::operator=(const Client &c)
{
	if(this != &c)
	{
		this->nick = c.nick;
		this->fullname = c.fullname;
		this->username = c.username;
		this->host = c.host;
		this->state = c.state;
		this->sockfd = c.sockfd;
		this->_isoper = c._isoper;
		this->is_invisible = c.is_invisible;
		this->msg = c.msg;
	}
	return *this;
}
// bool Client::operator!=(const Client& c)const
// {
// 	return (this->nick != c.nick ||
//             this->fullname != c.fullname ||
//             this->username != c.username ||
//             this->host != c.host ||
//             this->state != c.state ||
//             this->sockfd != c.sockfd ||
//             this->_isoper != c._isoper ||
//             this->is_invisible != c.is_invisible ||
//             this->msg != c.msg);
// }
bool Client::operator==(const Client& c) const {
    return (this->getNickname() == c.getNickname() && this->getUsername() ==c.getUsername());
}
bool Client::operator<=(const Client& c) const {
        return ((this->getNickname() < c.getNickname() && this->getUsername() < c.getUsername()) \
		 || (this->getNickname() == c.getNickname() && this->getUsername() == c.getUsername()));
}
bool Client::operator>=(const Client& c) const {
        return ((this->getNickname() > c.getNickname() && this->getUsername() > c.getUsername()) \
		 || (this->getNickname() == c.getNickname() && this->getUsername() == c.getUsername()));
}
bool Client::operator<(const Client& c) const {
        return !((this->getNickname() < c.getNickname() && this->getUsername() < c.getUsername()));
}
std::string Client::getPrefix()
{
	std::string prefix = ":" + nick + (username.empty() ? "" : "!" + username) + (host.empty() ? "" : "@" + host);
	return (prefix);
}
void	Channel::eraseClient(std::string nickname)
{
	client.erase(nickname);
}
void	Client::reply(std::string msg)
{
	std::cout << I_GREEN << "----> " << msg << RESET << std::endl;
	if (send(sockfd, msg.c_str(), msg.length(), 0) < 0)
		throw (std::runtime_error("\033[1;91mError while sending message\033[0m"));
}
void			Server::joinZero(Client &c)
{
	std::map<std::string,Channel*>::iterator it  = _channels.begin();
	while (it != _channels.end())
	{
		if(it->second->checkIfIsClientNickName(c.getNickname()))
		{
			PART("PART "+it->first+" :Sorry",c);
		}
		it++;
	}
	
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
	reply(REPLY_WELCOME(nick,username,host));
	reply(REPLY_YOURHOST(nick,host));
	// reply(REPLY_CREATED(nick,host,getTimeSc()));
	// reply(REPLY_CREATED(nick,host,getStartTimestp()));
	
	std::cout << B_CYAN << nick << " is registered" << RESET << std::endl;
}

///////////////////////////////////////////////////////////
void		Client::setIsInvisible(bool ischeck)
{
	is_invisible = ischeck;
}
// void		Client::setReceivesServerNotices(bool ischeck)
// {
// 	receives_server_notices = ischeck;
// }
// void		Client::setReceivesWallops(bool ischeck)
// {
// 	receives_wallops = ischeck;
// }

// //////////////////////////
// bool		Client::getIsInvisible() const
// {
// 	return (is_invisible);
// }
// bool		Client::getReceivesServerNotices() const
// {
// 	return (receives_server_notices);
// }
// bool		Client::getReceivesWallops() const
// {
// 	return(receives_wallops);
// }


////


//// server channel function
////

Channel	*Server::getChannel(std::string &target)
{

    std::map<std::string,Channel*>::iterator i = _channels.find(target);
	if(i != _channels.end())
		return i->second;
	return NULL;
}
// int comparison(std::string a,std::string b)
// {
// 	int s = 0;
// 	if(a.size()>= b.size())
// 	{
// 		for (size_t i = 0; i < a.size(); i++)
// 		{
// 			if(a[i] >= b[i])
// 				s = a[i] - b[i];
// 		}
		
// 	}
// }
int     Server::checkIfChannelExist(std::string &target)
{
    std::map<std::string,Channel*>::iterator i = _channels.find(target);
	if(i != _channels.end())
		return 1;
	// return NULL;
	return 0;
}
void    Channel::removeClientNickName(std::string const &nickname)
{
	this->client.erase(nickname);
}
// void	Server::set_Channel(Channel const &ch)
// {
// 	_channels.push_back(ch);
// }
/////client 

////
std::string getTimeSc()
{
        // Check if creatchannelTime is valid
		time_t		creatchannelTime;
		std::stringstream		ss;
		time(&creatchannelTime);
        // if (creatchannelTime == -1) {
        //     return "Error: Invalid time value";
        // }

        // // Convert to UTC using gmtime()
		// // std::cout <<"ok ss  "<< creatchannelTime<<std::endl;
        // struct tm* gmTime = gmtime(&creatchannelTime);
        // if (gmTime == NULL) {
        //     return "Error: Failed to convert to UTC";
        // }
        // // Format the time to YYYY-MM-DD HH:MM:SS
        // char buffer[30];
        // strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", gmTime);

        // // Append milliseconds as ".000"
        // std::string result(buffer);
    	// result += ".000";
		// // std::stringstream ss;
		ss << creatchannelTime;

    return ss.str();

}

Channel::Channel(std::string _name):name(_name)
{
	// mode = 0;
	isKey = false;
	isInviteOnly = false;
	isTopic = false;
    timeScCh = getTimeSc();
	mode.insert('t');
	islimit = false;
	chTopicOp = false;
	topic = "";
	max_client = -1;
	
}

void	Channel::addIviteClient(int fd, const std::string& nickname)
{
	this->invite[nickname] = fd;
}
std::string Channel::getTime()
{
	return this->timeScCh;
}
std::string Channel::getTimeTop()
{
	return this->timeScTo;
}
int Channel::getModeSize()
{
	return this->mode.size();
}
void Channel::setIsTopic(bool istopic)
{
	this->isTopic = istopic;
}
bool Channel::getIsTopic()
{
	return this->isTopic;
}
void Channel::setChTopOp(bool chtopic)
{
	this->chTopicOp = chtopic;
}
bool Channel::getChTopOp()
{
	return this->chTopicOp;
}
void Channel::setTimeTop(std::string const &time)
{
			this->timeScTo = time;
}
void Channel::setTime(std::string const &time)
{
	this->timeScCh = time;
}
Channel::~Channel()
{
	// std::map<Client*,bool>::iterator v = client.begin();
	// while (v != client.end())
	// {
	// 	if(v->first)
	// 		delete v->first;
	// 	v++;
	// }
	// client.clear();
}
// std::string getTime()
// {
//         // Check if creatchannelTime is valid
// 		time_t		creatchannelTime;
// 		std::stringstream		ss;
// 		// time(&creatchannelTime);
//         // if (creatchannelTime == -1) {
//         //     return "Error: Invalid time value";
//         // }

//         // // Convert to UTC using gmtime()
// 		// // std::cout <<"ok ss  "<< creatchannelTime<<std::endl;
//         // struct tm* gmTime = gmtime(&creatchannelTime);
//         // if (gmTime == NULL) {
//         //     return "Error: Failed to convert to UTC";
//         // }

//         // // Format the time to YYYY-MM-DD HH:MM:SS
//         // char buffer[30];
//         // strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", gmTime);

//         // // Append milliseconds as ".000"
//         // std::string result(buffer);
//     	// result += ".000";
// 		// std::stringstream ss;
// 		ss << creatchannelTime;

//     return ss.str();

// }

Channel::Channel(const Channel &c)
{
    *this  = c;
}
// std::string Client::getStartTimestp() const {
//     std::stringstream ss;
//     ss << this->creatServerTime;
// 	struct tm timeinfo;
//     memset(&timeinfo, 0, sizeof(struct tm));

//     // Parse the input string
//     if (sscanf(ss.str().c_str(), "%d-%d-%d %d:%d:%d",
//                &timeinfo.tm_year, &timeinfo.tm_mon, &timeinfo.tm_mday,
//                &timeinfo.tm_hour, &timeinfo.tm_min, &timeinfo.tm_sec) != 6) {
//         return "Error: Invalid time format";
//     }

//     // Adjust the parsed values
//     timeinfo.tm_year -= 1900;  // Year is years since 1900
//     timeinfo.tm_mon -= 1;      // Month is 0-11

//     // Convert to time_t
//     time_t rawtime = mktime(&timeinfo);
//     if (rawtime == -1) {
//         return "Error: Failed to convert time";
//     }

//     // Convert to UTC
//     struct tm* gmTime = gmtime(&rawtime);
//     if (gmTime == NULL) {
//         return "Error: Failed to convert to UTC";
//     }

//     // Format for IRC server time
//     char buffer[30];
//     strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", gmTime);

//     // Append ".000" for milliseconds (IRC format)
//     std::string result(buffer);
//     result += ".000";

//     return result;
// }
Channel& Channel::operator=(const Channel& other) {
    if (this != &other) {
		name = other.name;
		password = other.password;
		topic = other.topic;
		client.clear();
		client.insert(other.client.begin(),other.client.end());
		invite.clear();
		invite.insert(other.invite.begin(),other.invite.end());
    }
    return *this;
}
bool	Channel::getLimit() const
{
	return islimit;
}
std::string Channel::get_name() const
{
    return name;
}
bool Channel::getInviteOnly() const
{
	return isInviteOnly;
}
std::string Channel::get_password() const
{
    return password;
}
std::string Channel::getTopic() const
{
    return topic;
}
long long Channel::get_nbr_client() const
{
    return client.size();
}
bool Channel::getKey() const
{
	return isKey;
}
long long Channel::get_max_client() const
{
    return max_client;
}
void Channel::setMaxClient(long long m)
{
    max_client = m;
}
void Channel::setInviteOnly(bool in)
{
	isInviteOnly = in;
}
void Channel::setName(std::string &_name)
{
    name = _name;
}
void Channel::setPassword(std::string &_password)
{
    password = _password;
}
void Channel::setTopic(std::string &_topic)
{
    topic = _topic;
}
void Channel::setKey(bool b)
{
	this->isKey = b;
}
void Channel::setLimit(bool l)
{
	islimit  = l;
}
// void Channel::setMode(char c)
// {
// 	mode.insert(c);
// }
void    Channel::setClientRole(std::string const &nickname, bool role)
{
    std::map<std::string,std::pair<bool,int> >::iterator v = client.find(nickname);
	v->second.first = role;
}
// int Channel::checkIfIsInvite(const Client &c)
// {
//     std::map<std::string,int>::iterator v = invite.find(c.getNickname());
//     if(v != invite.end())
//         return 1;
//     return 0;
// }
int Channel::checkIfIsClient(std::string const &nickname)
{
    // std::map<Client*,bool>::iterator v = std::find(client.begin(),client.end(),c);
	if(client.find(nickname) != client.end())
        return 1;
    return 0;
}
// void Channel::allClientInChannel()
// {
// 	std::map<Client *,bool>::iterator it = client.begin();
// 	while (it != client.end())
// 	{
// 		std::cout << I_GREEN << "----> " << it->first->getNickname() +" "<< it->second<<std::endl;
// 		it++;
// 	}
	
// }
int Channel::checkIfIsClientNickName(std::string name)
{
    std::map<std::string,std::pair<bool,int> >::iterator v = client.find(name);
	if(v != client.end())
		return 1;
    return 0;
}
int Channel::findClientRole(std::string nikname)
{
    std::map<std::string,std::pair<bool,int> >::iterator v = client.find(nikname);
	if(v != client.end())
		return v->second.first;
    return 0;
}

void Channel::addClient(int fd, const std::string& nickname, bool isOperator) {
        client[nickname] = std::pair<bool,int>(isOperator,fd);
    }
// void    Channel::inviteChannel(Client &c,std::string pass)
// {
// 	if(checkIfIsClient(c))
// 		throw(1);
// 	if(getInviteOnly() == true && !checkIfInviteToChannel(c))
// 		throw(3);
// 	if(get_max_client() != -1 && get_max_client() > get_nbr_client())
// 		throw(2);
//     else
//     {
// 		if(getKey() == false && password.compare(pass))
// 			throw(4);
// 		client[&c] = false;
//         nbr_client++;
//     }
// }
// void		Channel::addMember(std::string const &name)
// {
// 	client[name] = true;
// 	nbr_client++;
// }
// void		Channel::setClient(std::string const &name,bool Role)
// {
// 	client[name] = Role;
// 	nbr_client++;
// }
int	Channel::checkIfInviteToChannel(Client &c)
{
	std::map<std::string,int>::iterator v = invite.find(c.getNickname());
	if(v != invite.end())
		return 1;
	return 0;
}
// void    Channel::ft_rm_client(Client &c)
// {
//     if(checkIfIsClient(c) == 0)
//     {
// 		std::map<std::string,bool>::iterator it = client.find(c.getNickname());
//         client.erase(it);
//     }
// }

void Channel::getMemberOp()
{
	std::vector<Client*>::iterator i ;
	// for ( i = client.begin(); i != client.end(); i++)
	// {
	// 	// f  =  i->getIsOper() ? 1: 0;
	// 	// std::cout << i->getFullname() +" "<< i->getNickname()+" "<< i->getIsOper()<<std::endl;
	// }
	
}
Client		*Server::getClientByNickNameS(std::string Nickname)
{
	std::map<int,Client*>::iterator it = _clients.begin();
	while (it != _clients.end())
	{
		if(it->second->getNickname().compare(Nickname) == 0)
			return it->second;
		it++;
	}
	return(NULL);
}
std::string	Channel::get_list_of_names()
{
	std::string	names;
	std::map<std::string,std::pair<bool,int> >::iterator itr = client.begin();
	while (itr != client.end())
	{
		if (itr->second.first)
			names += "@" + itr->first + " ";
		else
			names += itr->first + " ";
		itr++;
	}
	return names;
}
// void	Channel::setFdClien(int fd)
// {
// 	this->fdClient.push_back(fd);
// 	for (size_t i = 0; i < this->fdClient.size(); i++)
// 	{
// 		std::cout <<"fd =1===="<< this->fdClient.at(i)<<std::endl;
// 	}
// }
void	Channel::sendReplyAll(const std::string &msg,std::string nickname)
{
		std::map<std::string,std::pair<bool,int> >::iterator it = client.begin();
		while (it != client.end())
		{
			if (it->first.compare(nickname) != 0)
			{
				if(send(it->second.second, msg.c_str(), msg.length(), 0) < 0)
					throw std::runtime_error("\033[1;91mError send.\033[0m");
			}
			it++;
		}
}

void	Server::replys(const Client &c, const std::string &msg)
{
	send(c.getFd(), msg.c_str(), strlen(msg.c_str()), 0);
}
void Channel::setMode(char s)
{
	this->mode.insert(s);
}
void Channel::eraseMode(char s)
{
	this->mode.erase(s);
}
std::string Channel::getMode()
{
	std::string modeStr = "+";
	std::string arg = " ";
    for (std::set<char>::const_iterator it = this->mode.begin(); it != this->mode.end(); ++it) {
		if(*it == 'k')
		{
			arg+= this->password;
			arg+=" ";
		}
		else if(*it == 'l')
		{
			std::ostringstream oss;
			oss << this->max_client;
			arg+=oss.str();
			arg+=" ";
		}
        modeStr += *it;
    }
    return std::string(modeStr + arg);
}
bool Channel::hasMode(char mode) const {
    return this->mode.find(mode) != this->mode.end();
}
//  std::string Channel::get_mode_string() const {
//         std::string modeStr = "+";
//         if (mode & MODE_OPERATOR_PRIV) modeStr += "o";
//         if (mode & MODE_INVITE_ONLY) modeStr += "i";
//         if (mode & MODE_PROTECTED_TOPIC) modeStr += "t";
//         if (mode & MODE_CLIENT_LIMIT) modeStr += "l";
//         if (mode & MODE_KEY) modeStr += "k";
//         return modeStr;
//     }
// std::string NICK_SUCCESSS(std::string newNick)
// {
// 	return ("\033[1;92mNICK " + newNick + "\033[0m");
// }
// void			Server::getAllChannelAddByClient(Client &c,std::string oldnick,std::string newnick)
// {
// 	std::vector<Channel>::iterator it = _channels.begin();
// 	// std::cout << "OK!!!   "+_channels.size()+"!!\n";
// 	while (it != _channels.end())
// 	{
// 		if(it->checkIfIsClient(c))
// 		{
// 			// std::cout <<RPL_NICKCHANGE(oldnick,newnick,(*it).get_list_of_names(),c.getHost())<< " "<< newnick<<std::endl;
// 			it->sendReplyAll(RPL_NICKCHANGE(oldnick,newnick,(*it).get_list_of_names(),c.getHost()),oldnick);
// 		}
// 		it++;
// 	}
	
// }
	