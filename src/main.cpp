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

void send_response(int socket_fd, const std::string &response_str)
{
	size_t response_length = response_str.length();

	send(socket_fd, response_str.c_str(), response_length, 0);
}


struct pollfd pollfd_from_fd(int fd, short events)
{
	struct pollfd tmp;

	memset(&tmp, 0, sizeof(tmp));
	tmp.fd = fd;
	tmp.events = events;
	return (tmp);
}

bool	check_if_fd_match(std::list<int> &_serversFD, int value)
{
	std::list<int>::iterator i = std::find(_serversFD.begin(), _serversFD.end(), value);
	if (i == _serversFD.end())
		return (false);
	return (true);
}

struct loopHandler{
	public:
		loopHandler(webserver &server);
		~loopHandler();

		std::vector<struct pollfd>	_fdsList;

		void			do_poll();
		unsigned int	total_fds();

		int				port_from_client_fd(int fd);

		void			new_client(int number);
		void			new_request(int number);
		void			new_server(int port);

		bool			check_poll_in(int number);
		bool			check_poll_in_server(int number);
		bool			check_poll_in_cgi(int number);

		bool			check_poll_out(int number);
		bool			check_poll_out_cgi(int number);

		request			*get_request_from_client(int n_client);

	private:
		std::map<int, request*>		_client_and_request;

		// this is saved to find the port by the server FD along with the _clientFD_serverFD
		// this allow us to find the port of a client from the client fd
		std::map<int, int>			_serverFD_port;
		std::map<int, int>			_clientFD_serverFD;

		// list of all servers fd's for loop check
		std::list<int>				_serversFD;
		std::list<int>				_cgiFD;

		webserver					*_webserver;

		//Utils
};

loopHandler::loopHandler(webserver &server)
{
	_webserver = &server;

	this->new_server(1234);
	this->new_server(4321);
	this->new_server(7777);
}

loopHandler::~loopHandler()
{
	// close all fds from fdList

}

void loopHandler::new_server(int port)
{
	serverFd*	server_fd;

	server_fd = new serverFd(port);
	this->_fdsList.push_back(pollfd_from_fd(server_fd->_fd, POLLIN));
	this->_serverFD_port[server_fd->_fd] = port;
	this->_serversFD.push_back(server_fd->_fd);
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

	this->_fdsList.push_back(pollfd_from_fd(new_socket, POLLIN | POLLOUT));
	this->_clientFD_serverFD[this->_fdsList.back().fd] = this->_fdsList[number].fd;

	std::cout << "[LOG]: New client in port: " 
	<< this->port_from_client_fd(this->_fdsList.back().fd) 
	<< std::endl;
}

void	loopHandler::new_request(int number)
{
	this->_client_and_request[number] = new request(this->_fdsList[number].fd, *this->_webserver, number);
}

request	*loopHandler::get_request_from_client(int n_client)
{
	return (this->_client_and_request[n_client]);
}

int main_loop(webserver &server)
{
	loopHandler		_loop(server);

	while (true)
	{
		_loop.do_poll();

		for (int i = 0; i < _loop.total_fds() ; ++i)
		{
			if (_loop.check_poll_in(i))
			{
				if (_loop.check_poll_in_server(i))
				{
					// new client
					_loop.new_client(i);
				}
				else if (_loop.check_poll_in_cgi(i))
				{
					;
				}
				else
					_loop.new_request(i);

				if (_loop.check_poll_out(i))
				{
					request	*tmp_req = _loop.get_request_from_client(i);

					if (!tmp_req->_cgi)
					{
						// not cgi response
						response _response = response(*tmp_req, server);

						send_response(_loop._fdsList[i].fd, _response.str());

						delete tmp_req;
						// if (!connection_keep_alive)
							close(_loop._fdsList[i].fd);
							_loop._fdsList.erase(_loop._fdsList.begin() + i);
					}
					else
					{
						// cgi response
						if (tmp_req->_cgi->_is_ready)
						{
							;

						}
						else
						{
							;
							// check time out
						}

					}
				}
			}
		}
	}

	return 0;
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
		std::string	config_file = path_config_file(argc, argv);
		webserver	server(config_file);	

		main_loop(server);
	}
	catch (const std::exception &e)
	{
		std::cout << "[Fatal]: " << e.what() << std::endl;
	}
}
