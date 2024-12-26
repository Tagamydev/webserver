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

int main_loop(webserver &server)
{
	std::vector<struct pollfd>	fdsList;
	std::map<int, request*>		_client_and_request;
	serverFd*	server_fd;

	server_fd = new serverFd(1234);
	fdsList.push_back(pollfd_from_fd(server_fd->_fd, POLLIN));

	int new_socket;

	while (true)
	{
		if (poll(fdsList.data(), static_cast<unsigned int>(fdsList.size()), 0) < 0)
			throw (std::runtime_error("Poll fail."));

		for (int i = 0; i < static_cast<unsigned int>(fdsList.size()); ++i)
		{
			if (fdsList[i].revents & POLLIN)
			{
				if (fdsList[i].fd == server_fd->_fd)
				{
					// new client

					if ((new_socket = accept(server_fd->_fd, NULL, NULL)) == -1)
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

					close(fdsList[i].fd);
					delete tmp_req;
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
		std::string	config_file = path_config_file(argc, argv);
		webserver	server(config_file);	

		main_loop(server);
	}
	catch (const std::exception &e)
	{
		std::cout << "[Fatal]: " << e.what() << std::endl;
	}
}
