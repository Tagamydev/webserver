/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: samusanc <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/11 08:36:52 by samusanc          #+#    #+#             */
/*   Updated: 2025/01/16 12:14:45 by samusanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.hpp"
#include "loopHandler.hpp"
#include "utils.hpp"
#include <algorithm>

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


void	server_loop(webserver &server)
{
	std::vector<struct pollfd>	list;
	int	i = 0;

	server._loop->make_fd_list(list);
	make_poll(list);
	for (; i < length_list(list); i++)
	{
		std::cout << length_list(list);
		if (list[i].revents & POLLIN)
		{
			server._loop->check_additions(i, list);
			if (list[i].revents & POLLOUT)
				server._loop->handle_client(i, list);
		}
		/*
		else if (list[i].revents & POLLOUT)
			server._loop->send_to_cgi(i, list);
			*/
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
		std::string	config_file = path_config_file(argc, argv);
		webserver	server(config_file);	
		
		main_loop(server);
	}
	catch (const std::exception &e)
	{
		std::cout << "[Fatal]: " << e.what() << std::endl;
	}
}
