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

bool	find_server_fd(std::list<int> &_serversFD, int value)
{
	std::list<int>::iterator i = std::find(_serversFD.begin(), _serversFD.end(), value);
	if (i == _serversFD.end())
		return (false);
	return (true);
}

int main_loop(webserver &server)
{
	std::vector<struct pollfd>	fdsList;
	std::map<int, request*>		_client_and_request;
	std::map<int, int>			_socket_serverFD;
	std::list<int>				_serversFD;
	serverFd*	server_fd;
	serverFd*	server_fd2;

	server_fd = new serverFd(1234);
	fdsList.push_back(pollfd_from_fd(server_fd->_fd, POLLIN));
	_socket_serverFD[1234] = server_fd->_fd;
	_serversFD.push_back(server_fd->_fd);

	server_fd2 = new serverFd(4321);
	fdsList.push_back(pollfd_from_fd(server_fd2->_fd, POLLIN));
	_socket_serverFD[4321] = server_fd2->_fd;
	_serversFD.push_back(server_fd2->_fd);

	int new_socket;

	while (true)
	{
		if (poll(fdsList.data(), static_cast<unsigned int>(fdsList.size()), 0) < 0)
			throw (std::runtime_error("Poll fail."));

		for (int i = 0; i < static_cast<unsigned int>(fdsList.size()); ++i)
		{
			if (fdsList[i].revents & POLLIN)
			{
				if (find_server_fd(_serversFD, fdsList[i].fd))
				{
					// new client

					if ((new_socket = accept(fdsList[i].fd, NULL, NULL)) == -1)
						throw (std::runtime_error("Accept fail."));

					fdsList.push_back(pollfd_from_fd(new_socket, POLLIN | POLLOUT));

					std::cout << "Nueva conexión aceptada\n";
				}
				else
					_client_and_request[i] = new request(fdsList[i].fd);

				if (fdsList[i].events & POLLOUT)
				{
					request	*tmp_req = _client_and_request[i];
					response respuesta = response(*tmp_req, server);

					send_response(fdsList[i].fd, respuesta.str());

					delete tmp_req;
					// if (!connection_keep_alive)
						close(fdsList[i].fd);
						fdsList.erase(fdsList.begin() + i);
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
		//start config file arser here
		// server	server_parsed(path);
		std::string	config_file = path_config_file(argc, argv);
		webserver	server(config_file);	
		
		// main_loop(server);
	}
	catch (const std::exception &e)
	{
		std::cout << "[Fatal]: " << e.what() << std::endl;
	}
}
