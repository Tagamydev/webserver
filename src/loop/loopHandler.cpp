/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   loopHandler.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: samusanc <samusanc@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/14 10:40:56 by samusanc          #+#    #+#             */
/*   Updated: 2025/01/28 17:04:11 by samusanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "loopHandler.hpp"
#include "utils.hpp"
#include <exception>

loopHandler::loopHandler(webserver &webserver)
{
	this->_webserver = &webserver;
	this->open_ports();
}

void	loopHandler::open_ports()
{
	std::map<int, std::list<server *> >::iterator	i;
	std::map<int, std::list<server *> >::iterator	ie;

	i = this->_webserver->_port_servers_list.begin();
	ie = this->_webserver->_port_servers_list.end();
	for (; i != ie ; i++)
	{
		std::cout << "[Info]: opening port: " << i->first << std::endl;
		this->new_server(i->first);
	}
}

void	clear_server_cgi_Fd(std::map<int, struct pollfd> &_list)
{
	std::map<int, struct pollfd>::iterator	i;
	std::map<int, struct pollfd>::iterator	ie;

	i = _list.begin();
	ie = _list.end();
	for (; i != ie ; i++)
	{
		close(i->second.fd);
	}
	_list.clear();
}

void	clear_clientFd(std::map<client *, struct pollfd> &_list)
{
	std::map<client *, struct pollfd>::iterator	i;
	std::map<client *, struct pollfd>::iterator	ie;

	i = _list.begin();
	ie = _list.end();
	for (; i != ie ; i++)
	{
		close(i->second.fd);
		delete i->first;
	}
	_list.clear();
}

loopHandler::~loopHandler()
{
	clear_server_cgi_Fd(this->_port_serverFd);
	clear_clientFd(this->_client_clientFd);
	clear_server_cgi_Fd(this->_clientFd_cgiFd);
}

void loopHandler::new_server(int port)
{
	serverFd server_fd(port);

	// fcntl(server_fd._fd, F_SETFL, O_NONBLOCK);
	this->_port_serverFd[port] = utils::pollfd_from_fd(server_fd._fd, POLLIN);
}

void	add_server_fds_to_list(std::vector<struct pollfd> &result, 
std::map<int, struct pollfd> &_list)
{
	std::map<int, struct pollfd>::iterator	i;
	std::map<int, struct pollfd>::iterator	ie;

	i = _list.begin();
	ie = _list.end();
	for (; i != ie ; i++)
	{
		result.push_back(i->second);
	}
}

void	add_client_fds_to_list(std::vector<struct pollfd> &result, 
std::map<client *, struct pollfd> &_list)
{
	std::map<client *, struct pollfd>::iterator	i;
	std::map<client *, struct pollfd>::iterator	ie;

	i = _list.begin();
	ie = _list.end();
	for (; i != ie ; i++)
	{
		result.push_back(i->second);
	}
}

void	add_cgi_fds_to_list(std::vector<struct pollfd> &result, 
std::map<int, struct pollfd> &_list)
{
	std::map<int, struct pollfd>::iterator	i;
	std::map<int, struct pollfd>::iterator	ie;

	i = _list.begin();
	ie = _list.end();
	for (; i != ie ; i++)
	{
		result.push_back(i->second);
	}
}

void loopHandler::make_fd_list(std::vector<struct pollfd> &result)
{
	result.clear();
	add_server_fds_to_list(result, this->_port_serverFd);
	add_client_fds_to_list(result, this->_client_clientFd);
	add_cgi_fds_to_list(result, this->_clientFd_cgiFd);
}

void	loopHandler::delete_client(client *_client, int &_it)
{
	std::map<client *, struct pollfd>	&_list = this->_client_clientFd;
	std::map<client *, struct pollfd>::iterator i;

	i = _list.find(_client);
	if (i == _list.end())
	{
		delete _client;
		return ;
	}
	_list.erase(i);
	delete _client;
	_it--;
}

bool	loopHandler::fd_is_client(int fd)
{
	std::map<client *, struct pollfd>::iterator i;
	std::map<client *, struct pollfd>::iterator ie;
	client	*result = NULL;

	i = this->_client_clientFd.begin();
	ie = this->_client_clientFd.end();
	for (; i != ie ; i++)
	{
		if (i->second.fd == fd)
		{
			result = i->first;
			return (true);
		}
	}
	return (false);
}



client	*loopHandler::get_client_from_clientFd(int fd)
{
	std::map<client *, struct pollfd>::iterator i;
	std::map<client *, struct pollfd>::iterator ie;
	client	*result = NULL;
	
	
	// check on all lists?
	i = this->_client_clientFd.begin();
	ie = this->_client_clientFd.end();
	for (; i != ie ; i++)
	{
		if (i->second.fd == fd)
		{
			result = i->first;
			break ;
		}
	}
	
	//checking on cgi list
	if (!result)
	{
		std::map<int, struct pollfd>::iterator i;
		std::map<int, struct pollfd>::iterator ie;
		int result = 0;
		i = this->_clientFd_cgiFd.begin();
		ie = this->_clientFd_cgiFd.end();
		for (; i != ie ; i++)
		{
			if (i->second.fd == fd)
			{
				result = i->first;
				break ;
			}
		}
	}
	if (!result)
		throw (std::runtime_error("client not found from fd"));
	return (result);
}

void	loopHandler::send_response(int &i, std::vector<struct pollfd> &list)
{
	// utils::print_debug("sending response");
	client *_client;
	struct pollfd	socket;

	socket = list[i];
	_client = this->get_client_from_clientFd(socket.fd);
	if (_client->get_request())
	{
		response	_response = response(_client->get_request(), this->_webserver);
		bool		_keep_alive;
	
		_keep_alive = _response._keep_alive;
		try
		{
			utils::send_response(_client->get_fd(), _response.str());
			std::cout << "Response: \n" << YELLOW << std::endl;
			std::cout << _response.str() << std::endl;
			std::cout << RESET << std::endl;
		}
		catch (std::exception &e)
		{
			utils::print_info("fail to read/write into client reason: " + std::string(e.what()));
			_keep_alive = false;
		}
		_client->free_request();
		if (true)
		{
			delete_client(_client, i);
			this->make_fd_list(list);
		}
	}
}

void	loopHandler::handle_client(int &i, std::vector<struct pollfd> &list)
{
	client			*_client;
	struct pollfd	socket;

	socket = list[i];
	_client = this->get_client_from_clientFd(socket.fd);
	if (!_client->_request)
		return ;
	if (_client->_cgi_status() == WAITING)
	{
		if (_client->check_cgi_timeout())
			std::cerr << "[Log]: CGI timed out." << std::endl;
		else
			return ;
	}
	send_response(i, list);
}

bool	loopHandler::is_server(int fd)
{
	std::map<int, struct pollfd>::iterator	i;
	std::map<int, struct pollfd>::iterator	ie;

	i = this->_port_serverFd.begin();
	ie = this->_port_serverFd.end();
	for (; i != ie ; i++)
	{
		if (i->second.fd == fd)
			return (true);
	}
	return (false);
}

bool	loopHandler::fd_is_cgi(int fd)
{
	std::map<int, struct pollfd>::iterator	i;
	std::map<int, struct pollfd>::iterator	ie;

	i = this->_clientFd_cgiFd.begin();
	ie = this->_clientFd_cgiFd.end();
	for (; i != ie ; i++)
	{
		if (i->second.fd == fd)
			return (true);
	}
	return (false);
}


void	loopHandler::delete_fd_from_cgi_list(int fd, int &_it)
{
	std::map<int, struct pollfd>::iterator	i;
	std::map<int, struct pollfd>::iterator	ie;

	i = this->_clientFd_cgiFd.begin();
	ie = this->_clientFd_cgiFd.end();
	for (; i != ie ; i++)
	{
		if (i->second.fd == fd)
		{
			this->_clientFd_cgiFd.erase(i);
			_it--;
			return ;
		}
	}
}

void	loopHandler::delete_cgi_from_list(cgi *_cgi, int &i)
{
	// utils::print_debug("deleting cgi from list");
	this->delete_fd_from_cgi_list(_cgi->_read_fd.fd, i);
	this->delete_fd_from_cgi_list(_cgi->_write_fd.fd, i);
	//exit(-1);
}

int	loopHandler::get_clientFd_from_cgiFd(int fd)
{
	std::map<int, struct pollfd>::iterator	i;
	std::map<int, struct pollfd>::iterator	ie;

	i = this->_clientFd_cgiFd.begin();
	ie = this->_clientFd_cgiFd.end();
	for (; i != ie ; i++)
	{
		if (i->second.fd == fd)
			return (i->first);
	}
	throw (std::runtime_error("this file descriptor is not asociated with a client"));
}

void	loopHandler::read_from_cgi(int &i, std::vector<struct pollfd> &list)
{
	struct pollfd socket = list[i];
	client	*_client;
	cgi		*_cgi;
	
	_client = this->get_client_from_clientFd(this->get_clientFd_from_cgiFd(socket.fd));
	_cgi = _client->get_request()->_cgi;
	_cgi->read_to_cgi();

	this->delete_cgi_from_list(_cgi, i);
	_client->close_cgi();
	this->make_fd_list(list);
}

int	loopHandler::get_port_from_fd(int fd)
{
	std::map<int, struct pollfd>::iterator	i;
	std::map<int, struct pollfd>::iterator	ie;

	i = this->_port_serverFd.begin();
	ie = this->_port_serverFd.end();
	for (; i != ie ; i++)
	{
		if (i->second.fd == fd)
			return (i->first);
	}
	throw (std::runtime_error("invalid server, the fd does not match with any port"));
}

void	loopHandler::new_client(struct pollfd socket)
{
	// utils::print_debug("new client");
	client	*_client;

	_client = new client(socket, this->get_port_from_fd(socket.fd));
	if (!_client)
		throw (std::runtime_error("error making client"));
	this->_client_clientFd[_client] = _client->get_pollfd();
}

void	loopHandler::new_request(int fd, std::vector<struct pollfd> &list, int &i)
{
	client	*_client;

	_client = this->get_client_from_clientFd(fd);
	try
	{
		utils::print_debug("Assigning new request to client");
		_client->_request = new request(_client, this->_webserver, list);
	}
	catch (std::exception &e)
	{
		utils::print_info("fail to read/write into client reason: " + std::string(e.what()));
		this->delete_client(_client, i);
	}
}

void	loopHandler::add_cgi(int clientFd, struct pollfd cgiFd)
{
	this->_clientFd_cgiFd[clientFd] = cgiFd;
}

void	loopHandler::check_additions(int &i, std::vector<struct pollfd> &list)
{
	struct pollfd socket = list[i];

	if (loopHandler::is_server(socket.fd))
	{
		this->new_client(socket);
		this->make_fd_list(list);
	}
	else if (loopHandler::fd_is_cgi(socket.fd))
	{
		this->read_from_cgi(i, list);
	}
	else
		this->new_request(socket.fd, list, i);
}
void	loopHandler::send_to_cgi(int &i, std::vector<struct pollfd> &list) // Check if is needed
{
	client			*_client;
	struct pollfd	socket;


	socket = list[i];
	// _client = this->get_client_from_clientFd(socket.fd);
	_client = this->get_client_from_clientFd(this->get_clientFd_from_cgiFd(socket.fd));

	int result = send(_client->_request->_cgi->_write_fd.fd, _client->_request->_body.c_str(), _client->_request->_body.length(), 0);

	if (result == -1)
	{
		std::cerr << "[FATAL]: failed to send to CGI." << std::endl;
		throw std::runtime_error("Error sending to CGI pipe.");
	}
	if (result == 0)
	{
			std::cout << YELLOW;
			std::cout << "\nWarning, 0kb sended to cgi." << std::endl;
			std::cout << RESET;
	}
	
	this->delete_fd_from_cgi_list(_client->get_request()->_cgi->_write_fd.fd, i);
	this->make_fd_list(list);
}

