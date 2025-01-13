/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: samusanc <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/11 08:36:52 by samusanc          #+#    #+#             */
/*   Updated: 2024/10/08 13:31:12 by samusanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.hpp"
#include "utils.hpp"

std::string get_actual_date()
{
	time_t now;
	time_t *lock;
	std::string result;

	lock = &now;
	now = time(lock);
	if (lock)
		result = std::string(ctime(lock));
	else
		result = std::string("error in getting time!!!");
	size_t pos = result.find('\n');
	if (pos != std::string::npos)
		result.erase(pos, 1); // Elimina el carácter '\n'
	return (result);
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

std::vector<struct pollfd>	make_fd_list()
{
	std::vector<struct pollfd>	result;

	add_server_fds_to_list(result, _loop._port_serverFd);
	add_client_fds_to_list(result, _loop._client_clientFd);
	add_cgi_fds_to_list(result, _loop._clientFd_cgiFd);
	return (result);
}

unsigned int length_list(std::vector<struct pollfd> &list)
{
	return (static_cast<unsigned int>(list.size()));
}

void	make_poll(std::vector<struct pollfd> &list)
{
	int	result;

	result = poll(list.data(), length_list(list), 0);
	if (result < 0)
		throw (std::runtime_error("Poll fail."));
}

void	loophandler::delete_client(client *_client)
{
	std::map<client *, struct pollfd>	&_list = this->client_clientFd;
	std::map<client *, struct pollfd>::iterator i;

	i = _list.find(_list.find(_client));
	if (i == _list.end())
	{
		delete _client;
		return ;
	}
	_list.erase(_list.find(_client));
	delete _client;
}

void	loophandler::send_response(int &i, client *_client, 
std::vector<struct pollfd> &list)
{
	if (_client->get_request())
	{
		response	_response = response(_client->get_request(), *this->_webserver);
		bool		_keep_alive;
	
		_keep_alive = _response._keep_alive;
		utils::send_response(_client->get_fd(), _response.str());
		_client->free_request();
	}
	if (!_keep_alive)
	{
		delete_client(_client);
		list = make_fd_list(_list);
		--i;
	}
}

void	cgi_timeout(client *_client)
{
	(_client->get_request())->close_cgi();
	(_client->get_request())->_error_code = 408;
	(_client->get_request())->_cgi_status = DONE;
}

client	*get_client_from_clientFd(int fd)
{

	if (!result)
		throw (std::runtime_error("client not found from fd"));
}

void	loophandler::handle_client(int &i, std::vector<struct pollfd> &list)
{
	client			*_client;
	struct pollfd	socket;

	socket = list[i];
	client = this->get_client_from_clientFd(socket.fd);

	if (_client->cgi_status() == WAITING)
	{
		if (check timeout)
			cgi_timeout(_client);
		else
			return ;
	}
	send_response(i, client, list);
}

bool	loophandler::is_server(int fd)
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

bool	loophandler::is_cgi(int fd)
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


void loophandler::delete_fd_from_cgi_list(int fd)
{
	std::<int, struct pollfd>::iterator	i;
	std::<int, struct pollfd>::iterator	ie;

	for (; i != ie ; i++)
	{
		if (i->second.fd == fd)
			this->_client_Fd_cgiFd.erase(i);
	}
}

void	loophandler::delete_cgi_from_list(cgi *_cgi)
{
	this->delete_fd_from_cgi_list(_cgi->read_fd.fd);
	this->delete_fd_from_cgi_list(_cgi->write_fd.fd);
}

void	loophandler::read_from_cgi(int &i, std::vector<struct pollfd> &list)
{
	struct pollfd socket = list[i];
	client	*_client;
	cgi		*_cgi;

	_client = this->get_client_from_clientFd(this->find_clientFd_from_cgiFd(fd));
	_cgi = _client->get_cgi();
	_cgi.read();
	this->delete_cgi_from_list(_cgi);
	_client->close_cgi();
	list = make_fd_list(list);
	--i;
}

void	loophandler::new_client(struct pollfd socket)
{
	client	*_client;

	_client = new client(socket);
	if (!_client)
		throw (std::runtime_error("error making client"));
	this->_client_clientFd[_client] = socket;
}

void	loophandler::new_request(int fd)
{
	client	*_client;

	_client = this->find_client_from_clientFd(fd);
	_client->_request = new request(_client);
}

void	loophandler::check_additions(int &i, std::vector<struct pollfd> &list)
{
	struct pollfd socket = list[i];

	if (is_server(socket.fd))
		this->new_client(socket);
	else if (is_cgi(socket.fd))
		this->read_from_cgi(i, list);
	else
		this->new_request(socket.fd);
}

void	loophandler::send_to_cgi(int &i, std::vector<struct pollfd> &list)
{
	std::cout << "this is a message for the cgi" << std::endl;
	this->delete_fd_from_cgi_list(_cgi->write_fd.fd);
	list = make_fd_list(list);
	--i;
}

void	server_loop(webserver &server)
{
	std::vector<struct pollfd>	list = make_fd_list();
	int	i = 0;

	make_poll(list);
	for (; i < length_list(list); i++)
	{
		if (list[i].revents & POLLIN)
		{
			server._loop->check_additions(i, list);
			if (list[i].revents & POLLOUT)
				server._loop->handle_client(i, list);
		}
		if (list[i].revents & POLLOUT)
			server.loop->send_to_cgi(i, list);
		else
			throw (std::runtime_error("idk what is this"));

	}
}

int main_loop(webserver &server)
{
	while (true)
		server_loop(server);
}

std::string	path_config_file(int argc, char **argv)
{
	std::string	config_file = "./conf.d/webserver.conf";
	if (argc != 2)
		std::cout << "[Info]: configuration file not found," <<
		"starting server with the default configuration file from: " 
		<< config_file << std::endl;
	else
		config_file = "./" + std::string(argv[1]);
	return (config_file);
}

int main(int argc, char **argv)
{
	try
	{
		//start config file arser here
		// server	server_parsed(path);
		std::string	config_file = path_config_file(argc, argv);
		webserver	server(config_file);	
		
		main_loop(server);
	}
	catch (const std::exception &e)
	{
		std::cout << "[Fatal]: " << e.what() << std::endl;
	}
}
