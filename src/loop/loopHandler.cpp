/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   loopHandler.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: samusanc <samusanc@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/14 10:40:56 by samusanc          #+#    #+#             */
/*   Updated: 2025/01/21 17:33:29 by samusanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "loopHandler.hpp"
#include "utils.hpp"

loopHandler::loopHandler(webserver &webserver)
{
	this->_webserver = &webserver;

	this->new_server(1234);
//	this->new_server(4321);
//	this->new_server(7777);
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
	i--;
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
	if (!result)
		throw (std::runtime_error("client not found from fd"));
	return (result);
}

void	loopHandler::send_response(int &i, std::vector<struct pollfd> &list)
{
	utils::print_debug("sending response");
	client *_client;
	struct pollfd	socket;

	socket = list[i];
	_client = this->get_client_from_clientFd(socket.fd);
	if (_client->get_request())
	{
		response	_response = response(_client->get_request(), this->_webserver);
		bool		_keep_alive;
	
		_keep_alive = _response._keep_alive;
		utils::send_response(_client->get_fd(), _response.str());
		_client->free_request();
		if (!_keep_alive)
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
			_client->cgi_timeout();
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

bool	loopHandler::is_cgi(int fd)
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
	utils::print_debug("deleting cgi from list");
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
	std::cout << "[Debug]: Reading from cgi" << std::endl;
	struct pollfd socket = list[i];
	client	*_client;
	cgi		*_cgi;

	_client = this->get_client_from_clientFd(
	this->get_clientFd_from_cgiFd(socket.fd));
	_cgi = _client->get_request()->_cgi;
	_cgi->read();
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
	utils::print_debug("new client");
	client	*_client;

	_client = new client(socket, this->get_port_from_fd(socket.fd));
	if (!_client)
		throw (std::runtime_error("error making client"));
	this->_client_clientFd[_client] = _client->get_pollfd();
}

void	loopHandler::new_request(int fd, std::vector<struct pollfd> &list)
{
	client	*_client;

	_client = this->get_client_from_clientFd(fd);
	utils::print_debug("assigning new request to client");
	_client->_request = new request(_client, this->_webserver, list);
}

void	loopHandler::add_cgi(int clientFd, struct pollfd cgiFd)
{
	this->_clientFd_cgiFd[clientFd] = cgiFd;
}

void	loopHandler::check_additions(int &i, std::vector<struct pollfd> &list)
{
	struct pollfd socket = list[i];

	if (is_server(socket.fd))
	{
		this->new_client(socket);
		this->make_fd_list(list);
	}
	else if (is_cgi(socket.fd))
		this->read_from_cgi(i, list);
	else
		this->new_request(socket.fd, list);
}

void	loopHandler::send_to_cgi(int &i, std::vector<struct pollfd> &list)
{
	client *_client;
	struct pollfd	socket;

	socket = list[i];
	_client = this->get_client_from_clientFd(socket.fd);
	std::cout << "this is a message for the cgi" << std::endl;
	this->delete_fd_from_cgi_list(_client->get_request()->_cgi->_write_fd.fd, i);
	this->make_fd_list(list);
}


/*
bool	check_if_fd_match(std::list<int> &_serversFD, int value)
{
	std::list<int>::iterator i = std::find(_serversFD.begin(), _serversFD.end(), value);
	if (i == _serversFD.end())
		return (false);
	return (true);
}


void	loopHandler::delete_FD_from_FD_list(int fd, std::list<int> &list)
{
	std::list<int>::iterator	i = list.begin();
	std::list<int>::iterator	ie = list.end();

	for (; i != ie; i++)
	{
		if (*i == fd)
		{
			list.erase(i);
			break ;
		}
	}
}

void	loopHandler::delete_FD_from_pollFD_list(int fd, std::vector<struct pollfd> &list)
{
	std::cout << "deleting fd number: "<< fd << std::endl;
	std::vector<struct pollfd>::iterator	i = list.begin();
	std::vector<struct pollfd>::iterator	ie = list.end();

	for (; i != ie; i++)
	{
		if ((*i).fd == fd)
		{
			list.erase(i);
			break ;
		}
	}
}


void	loopHandler::send_response_client(int n_client, request *tmp_req)
{
	response	_response = response(*tmp_req, *this->_webserver);
	bool		_keep_alive;

	_keep_alive = _response._keep_alive;
	utils::send_response(this->_fdsList[n_client].fd, _response.str());
	delete tmp_req;
	if (!_keep_alive)
	{
		close(this->_fdsList[n_client].fd);
		this->_fdsList.erase(this->_fdsList.begin() + n_client);
	}
}

cgi	*loopHandler::get_cgi_from_client(int n_client)
{
	request	*tmp_req = this->get_request_from_client(this->_cgi_request[this->_fdsList[n_client].fd]);

	if (!tmp_req)
		throw (std::runtime_error("get cgi fail. Request is NULL"));
	return (tmp_req->_cgi);
}



void	loopHandler::do_poll()
{
	int	result;

	result = poll(_fdsList.data(), this->total_fds(), 0);
	if (result < 0)
		throw (std::runtime_error("Poll fail."));
}

int	loopHandler::port_from_client_fd(int fd)
{
	int	serverFD;
	int	port;

	serverFD = this->_clientFD_serverFD[fd];
	port = this->_serverFD_port[serverFD];
	return (port);
}

unsigned int loopHandler::total_fds()
{
	return (static_cast<unsigned int>(_fdsList.size()));
}

bool	loopHandler::check_poll_in(int number)
{
	if (_fdsList[number].revents & POLLIN)
		return (true);
	return (false);
}

bool	loopHandler::check_poll_in_server(int number)
{
	if (check_if_fd_match(this->_serversFD, this->_fdsList[number].fd))
		return (true);
	return (false);
}

bool	loopHandler::check_poll_in_cgi(int number)
{
	if (check_if_fd_match(this->_cgiFD, this->_fdsList[number].fd))
		return (true);
	return (false);
}


bool	loopHandler::check_poll_out(int number)
{
	if ((_fdsList[number].events & POLLOUT))
		return (true);
	return (false);
}

void	loopHandler::new_client(int number)
{
	int new_socket;

	if ((new_socket = accept(this->_fdsList[number].fd, NULL, NULL)) == -1)
		throw (std::runtime_error("Accept fail."));

	this->_fdsList.push_back(utils::pollfd_from_fd(new_socket, POLLIN | POLLOUT));
	this->_clientFD_serverFD[this->_fdsList.back().fd] = this->_fdsList[number].fd;

	std::cout << "[Info]: New client in port: " 
	<< this->port_from_client_fd(this->_fdsList.back().fd) 
	<< std::endl;
}

void	loopHandler::new_request(int number)
{
	this->_client_and_request[number] = new request(this->_fdsList[number].fd, *this->_webserver, number, *this);
}

request	*loopHandler::get_request_from_client(int n_client)
{
	return (this->_client_and_request[n_client]);
}

*/
