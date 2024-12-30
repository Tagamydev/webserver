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

void send_response(int socket_fd, const std::string &response_str)
{
	size_t response_length = response_str.length();

	send(socket_fd, response_str.c_str(), response_length, 0);
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
