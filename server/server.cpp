/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ibenaait <ibenaait@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/10 17:28:00 by mfadil            #+#    #+#             */
/*   Updated: 2024/09/14 18:48:56 by ibenaait         ###   ########.fr       */
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
	// int					i = 0;
	std::vector<std::string>	cmd;
	std::stringstream	str(msg);
	std::string				tm;
	if (msg == "\n")
		return (cmd);
	while (std::getline(str, tm, '\n'))
	{
		cmd.push_back(tm);
		// std::cout << cmd.at(i++) << std::endl;
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
	throw (std::out_of_range("\033[1;91mError while searching for user3\033[0m"));
}

std::string	Server::readMessage(int fd)
{
	std::string	msg;
	char	buff[256];
	bzero(buff, 256);
	std::vector<Client>::iterator cl = findClientIt(fd);
	msg = cl->getMsg();
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
		// std::cout << this->_cmd.size() << std::endl;
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
	std::cout <<ss.str()+"      "<< tmp<<std::endl;
	args.push_back(tmp);
	std::cout << "Parse command : [" << tmp << "]" << std::endl;

	std::string cmds[15] = {"PASS", "NICK", "USER", "JOIN","MODE", "OPERATOR", "PRIVATE MESSAGE", "KILL", "PING", "PART", "LIST", "NAMES", "TOPIC", "KICK", "NOTICE"};

	int		(Server::*ptr[15])(std::vector<std::string> args, Client &cl) = {
			&Server::cmdPass,
			&Server::cmdNick,
			&Server::cmdUser,
			&Server::JOIN,
			&Server::MODE, 
			// &Server::OperCmd,
			// &Server::PrivMsgCmd,
			// &Server::KillCmd,
			// &Server::PingCmd,
			// &Server::PartCmd,
			// &Server::ListCmd,
			// &Server::NamesCmd,
			// &Server::TopicCmd,
			// &Server::KickCmd,
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
	throw (std::out_of_range("\033[1;91mError while searching for user1\033[0m"));
}
int		Server::findClientIn(std::string nickname)
{
	for (unsigned int i = 0; i < _clients.size(); i++)
	{
		if (_clients[i].getNickname() == nickname)
			return (1);
	}
	return 0;
}

Client		&Server::findClient(int fd)
{
	for (unsigned int i = 0; i < _clients.size(); i++)
	{
		if (_clients[i].getFd() == fd)
			return (_clients[i]);
	}
	throw (std::out_of_range("\033[1;91mError while searching for user2\033[0m"));
}

Client::Client(int sockfd, std::string hostname) : sockfd(sockfd), host(hostname), _isoper(false)
{
	state = HANDSHAKE;
	msg = "";
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

void	Client::reply(std::string msg)
{
	// std::string prefix = nick + (username.empty() ? "" : "!" + username) + (host.empty() ? "" : "@" + host);
	// std::string pack = msg + "\r\n";
	std::cout << I_GREEN << "----> " << msg << RESET << std::endl;
	if (send(sockfd, msg.c_str(), msg.length(), 0) < 0)
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
	reply("001 " + nick + " :Welcome " + nick +  " into our irc network\r\n");
	std::cout << B_CYAN << nick << " is registered" << RESET << std::endl;
}

///////////////////////////////////////////////////////////
void		Client::setIsInvisible(bool ischeck)
{
	is_invisible = ischeck;
}
void		Client::setReceivesServerNotices(bool ischeck)
{
	receives_server_notices = ischeck;
}
void		Client::setReceivesWallops(bool ischeck)
{
	receives_wallops = ischeck;
}

//////////////////////////
bool		Client::getIsInvisible() const
{
	return (is_invisible);
}
bool		Client::getReceivesServerNotices() const
{
	return (receives_server_notices);
}
bool		Client::getReceivesWallops() const
{
	return(receives_wallops);
}


////


//// server channel function
////

Channel     &Server::getChannel(std::string &target)
{

    std::vector<Channel>::iterator i = _channels.begin();
	while (i != _channels.end())
	{
		if(i->get_name().compare(target) == 0)
				return *i;
		i++;
	}
	throw std::runtime_error("Channel not found: " + target);
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
    std::vector<Channel>::iterator i = _channels.begin();
    while(i != _channels.end())
    {
		//std::cout << "name ::::"<< i->get_name()<< ":::::"+target<<":::::"<<i->get_name().compare(target)<<std::endl; 
        if(i->get_name() == target)
            return 1;
		i++;
    }
	return 0;
}

void	Server::set_Channel(Channel &ch)
{
	_channels.push_back(ch);
}
/////client 

////
Channel::Channel(std::string _name,std::string _password):name(_name),password(_password)
{
	// mode = 0;
	isKey = password.size() == 0 ? false : true;
	
	std::cout << isKey <<" "<<password.size()<< std::endl;
	isPrivatChannel = false;
	isInviteOnly = false;
	islimit = false;
	topic = "";
	nbr_client = 0;
	max_client = -1;
	
}

Channel::~Channel()
{
}
Channel::Channel(const Channel &c)
{
    *this  = c;
}
Channel& Channel::operator=(const Channel& other) {
    if (this != &other) {
		// mode = other.mode;
		// std::cout << other.name << " "+ other.password<<std::endl;
		name = other.name;
		password = other.password;
		topic = other.topic;
		client.clear();
		// for (size_t i = 0; i < other.client.size(); i++)
		// {
		// 	client[other.client];
		// }
		client.insert(other.client.begin(),other.client.end());
		invite.clear();
		for (size_t i = 0; i < other.invite.size(); i++)
		{
			invite.push_back(other.invite.at(i));
		}
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
void    Channel::setClientRole(Client &c, bool role)
{
    client[&c] = role;
}
int Channel::checkIfIsInvite(const Client &c)
{
    std::vector<Client>::iterator v = std::find(invite.begin(),invite.end(),c);
    if(v != invite.end())
        return 1;
    return 0;
}
int Channel::checkIfIsClient(Client &c)
{
    // std::map<Client*,bool>::iterator v = std::find(client.begin(),client.end(),c);
	if(client.find(&c) != client.end())
        return 1;
    return 0;
}
void Channel::allClientInChannel()
{
	std::map<Client *,bool>::iterator it = client.begin();
	while (it != client.end())
	{
		std::cout << I_GREEN << "----> " << it->first->getNickname() +" "<< it->second<<std::endl;
		it++;
	}
	
}
int Channel::checkIfIsClientNickName(std::string name)
{
    std::map<Client*,bool>::iterator v = client.begin();
	while (v != client.end())
	{
		//std::cout << "'"+v->first->getNickname() +"' '"+ name +"'"<< name.compare(v->first->getNickname())<<std::endl;
		if(v->first->getNickname() == name)
			return 1;
		v++;
	}
    return 0;
}
int Channel::findClientRole(Client &c)
{
    if(client.find(&c)->second == true)
        return 1;
    return 0;
}
void    Channel::inviteChannel(Client &c,std::string pass)
{
	if(checkIfIsClient(c))
		throw(1);
	if(getInviteOnly() == true && !checkIfInviteToChannel(c))
		throw(3);
	if(get_max_client() != -1 && get_max_client() > get_nbr_client())
		throw(2);
    else
    {
		// if(getKey() == false && pass.empty() == false)
		// 	throw(4);
		if(getKey() == false && password.compare(pass))
			throw(4);
		client[&c] = false;
        nbr_client++;
    }
}
void		Channel::ft_add_Member(Client &c)
{
	c.setIsoper(true);
	client[&c]=true;
	nbr_client++;
}
void		Channel::setClient(Client &c)
{
	c.setIsoper(false);
	client[&c] = false;
	nbr_client++;
}
int	Channel::checkIfInviteToChannel(Client &c)
{
	std::vector<Client>::iterator v = std::find(invite.begin(),invite.end(),c);
	if(v != invite.end())
		return 1;
	return 0;
}
void    Channel::ft_rm_client(Client &c)
{
    if(checkIfIsClient(c) == 0)
    {
		std::map<Client*,bool>::iterator it = client.find(&c);
        client.erase(it);
    }
}

void Channel::getMemberOp()
{
	std::vector<Client*>::iterator i ;
	// for ( i = client.begin(); i != client.end(); i++)
	// {
	// 	// f  =  i->getIsOper() ? 1: 0;
	// 	// std::cout << i->getFullname() +" "<< i->getNickname()+" "<< i->getIsOper()<<std::endl;
	// }
	
}
Client		&Channel::getClientByNickName(std::string Nickname)
{
	std::map<Client*,bool>::iterator it = client.begin();
	while (it != client.end())
	{
		//std::cout << Nickname+"' '"+(*it).first->getNickname()+"'"<<std::endl;
		if(Nickname.compare((*it).first->getNickname()) == 0)
			return *(*it).first;
		it++;
	}
	throw("");
}
std::string	Channel::get_list_of_names()
{
	std::string	names;
	std::map<Client*,bool>::iterator itr = client.begin();
	while (itr != client.end())
	{
		if ((*itr).second)
			names += "@" + (*itr).first->getNickname() + " ";
		else
			names += (*itr).first->getNickname() + " ";
		itr++;
	}
	return names;
}

void	Channel::sendReplyAll(const std::string &msg, std::string nick_name)
{
	// std::map<Client*,bool>::iterator itr = client.begin();
	// while (itr != client.end())
	// {
	// 	if (nick_name != (*itr).first->getNickname())
	// 	{
	// 		// send((*itr).first->getFd(), msg.c_str(), msg.le, 0);
	// 		// std::cout <<"bbbbbbbbbb"<< msg.c_str() << std::endl;
 	// 		if(send((*itr).first->getFd(), msg.c_str(), msg.length(), 0) < 0)
	// 			throw std::runtime_error("\033[1;91mError send.\033[0m");
	// 	}
	// 	itr++;
	// }

	 std::vector<Client*> clients_copy;
    for (std::map<Client*, bool>::const_iterator it = client.begin(); it != client.end(); ++it)
    {
        clients_copy.push_back(it->first);
    }

    std::vector<Client*> clients_to_remove;

    for (std::vector<Client*>::const_iterator it = clients_copy.begin(); it != clients_copy.end(); ++it)
    {
        Client* current_client = *it;
        if (nick_name != current_client->getNickname())
        {
            try 
            {
                if(send(current_client->getFd(), msg.c_str(), msg.length(), 0) < 0)
                {
                    throw std::runtime_error("\033[1;91mError send.\033[0m");
                }
            }
            catch (const std::exception &e)
            {
                std::cerr << "Error sending to client: " << e.what() << std::endl;
                clients_to_remove.push_back(current_client);
            }
        }
    }

    for (std::vector<Client*>::const_iterator remove_it = clients_to_remove.begin();
         remove_it != clients_to_remove.end(); ++remove_it)
    {
        client.erase(*remove_it);
    }
}
// void	Channel::sendReplyAllCl(const std::string &msg)
// {
// 	std::map<Client*,bool>::iterator itr = client.begin();
// 	while (itr != client.end())
// 	{
// 		// send((*itr).first->getFd(), msg.c_str(), msg.le, 0);
// 			// std::cout <<"bbbbbbbbbb"<< msg.c_str() << std::endl;
//  		if(send((*itr).first->getFd(), msg.c_str(), msg.length(), 0) < 0)
// 			throw std::runtime_error("\033[1;91mError send.\033[0m");
// 		itr++;
// 	}
// }
void	Server::replys(const Client &c, const std::string &msg)
{
	send(c.getFd(), msg.c_str(), strlen(msg.c_str()), 0);
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
	