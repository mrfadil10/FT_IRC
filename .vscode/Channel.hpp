/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ibenaait <ibenaait@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/16 08:11:31 by ibenaait          #+#    #+#             */
/*   Updated: 2024/09/01 18:58:09 by ibenaait         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// #ifndef CHANNEL
// # define CHANNEL

// #include "irc.hpp"

// class Client;
// class Server;
// class Channel
// {
//     private:
//         std::string name;
//         std::string password;
//         std::string topic;
//         std::string user;
//         size_t max_client;
//         size_t nbr_clinet;
//         std::map<Client*,int> client;
//         std::vector<Client> add_client;
//     public:
//         Channel(std::string _name,std::string _password);
//         ~Channel();
//         Channel(const Channel &c);
//         Channel &operator=(const Channel &c);
//         std::string get_name() const;
//         std::string get_password() const;
//         std::string get_topic() const;
//         size_t get_nbr_client() const;
//         size_t get_max_client() const;
    
//         void set_max_client(size_t &m);
//         void set_name(std::string &_name);
//         void set_password(std::string &_passord);
//         void set_topic(std::string &_topic);
//         void set_client(Client &c);
//         void set_client_role(Client &c, int role);
        
//         int find_Client(Client &c);
//         int ft_check_if_isClient(Client &c);
//         void    ft_add_client(Client &c);
//         void    ft_add_Client_channel(Client &c);
//         void    ft_rm_client(Client &c);
    
// };


// //// server channel function
// ////

// int     &Server::ft_check_if_channel_exist(std::string &target)
// {

//     std::vector<Channel>::iterator i ;
//     for ( i = _channels.begin(); i != this->_channels.end() ; i++)
//     {
//         if(i->get_name() == target)
//             return *i;
//     }
// }

// /////client 

// ////
// Channel::Channel(std::string _name,std::string _password):name(_name),password(_password)
// {}

// Channel::~Channel()
// {
// }
// Channel::Channel(const Channel &c)
// {
//     *this = c;
// }
// Channel& Channel::operator=(const Channel& other) {
//     if (this != &other) {
//         this->get_name() = other.get_name();
//         this->get_password() = other.get_password();
//         this->get_topic() = other.get_topic();
//     }
//     return *this;
// }
// std::string Channel::get_name() const
// {
//     return name;
// }
// std::string Channel::get_password() const
// {
//     return password;
// }
// std::string Channel::get_topic() const
// {
//     return topic;
// }
// size_t Channel::get_nbr_client() const
// {
//     return client.size();
// }

// size_t Channel::get_max_client() const
// {
//     return max_client;
// }
// void Channel::set_max_client(size_t &m)
// {
//     max_client = m;
// }
// void Channel::set_name(std::string &_name)
// {
//     name = _name;
// }
// void Channel::set_password(std::string &_password)
// {
//     password = _password;
// }
// void Channel::set_topic(std::string &_topic)
// {
//     topic = _topic;
// }
// void Channel::set_client(Client &c)
// {
//     add_client.push_back(c);
// }
// void    Channel::set_client_role(Client &c, int role)
// {
//     if(ft_check_if_isClient(c) == 0)
//     {
//         Client *ptr = &c;
//         std::map<Client *,int>::iterator it = client.find(ptr);
//         it->second = role;
//     }
// }
// int Channel::ft_check_if_isClient(Client &c)
// {
//     Client *ptr = &c;
//     std::map<Client*,int>::iterator map = client.find(ptr);
//     if(map != client.end())
//         return 1;
//     return 0;
// }
// int Channel::find_Client(Client &c)
// {
//     Client *ptr = &c;
//     std::vector<Client>::iterator v = std::find(add_client.begin(),add_client.end(),c);
//     if(client.find(ptr) == client.end())
//         return 0;
//     return 1;
// }
// void    Channel::ft_add_Client_channel(Client &c)
// {
//     if(ft_check_if_isClient(c) == 0 )
//     {
//         Client *ptr = &c;
//         client[ptr] = 0;
//         nbr_clinet++;
//     }
// }
// void     Channel::ft_add_client(Client &c)
// {
//     Client *ptr = &c;
//     if(client.find(ptr) != client.end())
//         return;
//     if(find_Client(c))
//         set_client(c);
// }

// void    Channel::ft_rm_client(Client &c)
// {
//     if(ft_check_if_isClient(c) == 0)
//     {
//         Client *ptr = &c;
//         client.erase(ptr);
//     }
// }
// #endif
