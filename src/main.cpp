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

int test()
{
	std::vector<struct pollfd> _pollFds;
	serverFd*	server_fd;

	server_fd = new serverFd(1234);
	_pollFds.push_back(pollfd_from_fd(server_fd->_fd, POLLIN));

	int new_socket;

	while (true)
	{
		if (poll(_pollFds.data(), static_cast<unsigned int>(_pollFds.size()), 0) < 0)
			throw (std::runtime_error("Error: Fatal poll fail!"));

		for (int i = 0; i < static_cast<unsigned int>(_pollFds.size()); ++i)
		{
			if (fds[i].revents & POLLIN)
			{
				if (fds[i].fd == server_fd)
				{
					// new client
					if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
						throw (std::runtime_error("Error: Fatal acept fail!"));

					_pollFds.push_back(pollfd_from_fd(new_socket, POLLIN | POLLOUT));

					std::cout << "Nueva conexión aceptada\n";
				}
				else
					request req = request(fds[i].fd);

				if (fds[i].events & POLLOUT)
				{
					response respuesta = response(req);

					send_response(fds[i].fd, respuesta.str());
					close(fds[i].fd);
					fds[i] = fds[--client_count];
				}
			}
		}
	}

	return 0;
}

int main()
{
	try
	{
		test();

		/*
		int	fd;

		fd = open("request.txt", O_RDONLY);
		if (fd < 0)
			return (-1);
		request		req = request(fd);
		*/
		// response	respuesta = response(req);
		// std::cout << "\n<<<<    Response    >>>>" << std::endl;
		// std::cout << respuesta.str() << std::endl;
	}
	catch (const std::exception &e)
	{
		std::cout << e.what() << std::endl;
	}
}
