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

void	make_poll()
{

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
std::map<request *, struct pollfd> &_list)
{
	std::map<request *, struct pollfd>::iterator	i;
	std::map<request *, struct pollfd>::iterator	ie;

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

std::vector<int>	make_fd_list()
{
	std::vector<struct pollfd>	result;

	add_server_fds_to_list(result, _loop._port_serverFd);
	add_client_fds_to_list(result, _loop._request_clientFd);
	add_cgi_fds_to_list(result, _loop._clientFd_cgiFd);
	return (result);
}

unsigned int length_list(std::vector<int> &list)
{
	return (static_cast<unsigned int>(list.size()));
}

void	make_poll(std::vector<int> &list)
{
	int	result;

	result = poll(list.data(), length_list(list), 0);
	if (result < 0)
		throw (std::runtime_error("Poll fail."));
}

int	server_loop()
{
	std::vector<int>	list = make_fd_list();
	int	i = 0;

	make_poll(list);
	for (; i < length_list(list); i++)
	{
		if (check poll IN)
		{
			if (check server)
			{

			}
			else if (check cgi)
			{

			}
			else
			{

			}

			if (check poll OUT)
			{
				if (response && !cgi)
				{

					send response
					close client
					--i;
					list = make_fd_list();
				}
				else if (response && cgi)
				{
					if (cgi is ready)
					{

						close cgi
						send response
						close client
						--i;
						list = make_fd_list();
					}
					else
					{
						if (check timeout)
						{

							close cgi
							send response
							close client
							--i;
							list = make_fd_list();
						}
					}

				}
				else
					throw (std::runtime_error("idk what is this"));
			}
		}
		else if (check poll OUT)
		{

		}
		else
			throw (std::runtime_error("idk what is this"));

	}
}

int main_loop(webserver &server)
{
	loopHandler		_loop(server);
	bool			_cgi_hand;

	std::cout << "------------- Starting loop -------------" << std::endl;
	while (true)
	{
		_loop.do_poll();

		for (int i = 0; i < _loop.total_fds() ; ++i)
		{
			_cgi_hand = false;
			//std::cout << "[Log]: " << "---- making client number: " << i << " ----" << std::endl;
			//std::cout << "[Log]: " << "---- client fd: " << _loop._fdsList[i].fd << " ----" << std::endl;
			if (_loop.check_poll_in(i))
			{
				std::cout << "start...";
				if (_loop.check_poll_in_server(i))
				{
					std::cout << "[Log]: " << "new client..." << std::endl;
					_loop.new_client(i);
				}
				else if (_loop.check_poll_in_cgi(i))
				{
					std::cout << "[Log]: before: "<<  _loop.total_fds() << std::endl;
					std::cout << "[Log]: " << "reading from cgi output..." << std::endl;
					cgi	*tmp;
					request	*tmp_r;

					tmp = _loop.get_cgi_from_client(i);
					tmp_r = tmp->_request;
					if (!tmp)
						throw (std::runtime_error("get cgi fail. cgi is NULL"));

					/*
					tmp->read_from_cgi(_loop._fdsList[i].fd);
					std::cout << "[Log]: " << "after read..." << std::endl;
					delete tmp;
					tmp_r->_cgi = NULL;
					std::cout << "[Log]: after: "<<  _loop.total_fds() << std::endl;
					_cgi_hand = true;
					//exit(-1);
					*/
				}
				else
				{
					std::cout << "[Log]: " << "new request from client: "<< i << std::endl;
					_loop.new_request(i);
				}

				if (_loop.check_poll_out(i) && !_cgi_hand)
				{
					std::cout << "[Log]: " << "---- poll out event in: " << _loop._fdsList[i].fd << " ----" << std::endl;

					request	*tmp_req = _loop.get_request_from_client(i);

					if (!tmp_req)
						std::cout << "error " << std::endl;
					else if (!tmp_req->_cgi)
					{
						std::cout << "[Log]: " << "sending response..." << std::endl;
						_loop.send_response_client(i, tmp_req);
					}
					else
					{
						if (tmp_req->_cgi->_is_ready)
						{
							std::cout << "[Log]: " << "sending response with client with cgi..." << std::endl;
							_loop.send_response_client(i, tmp_req);
						}
						else
						{
							std::cout << "[Log]: " << "making cgi response..." << std::endl;
							if (tmp_req->_cgi->check_cgi_timeout())
							{
								delete tmp_req->_cgi;
								tmp_req->_cgi = NULL;
								tmp_req->_error_code = 408;

								_loop.send_response_client(i, tmp_req);
								tmp_req = NULL;
							}
						}
					}
					std::cout << "[Log]: " << "---- end of poll out event in: " << i << " ----" << std::endl;
				}
				std::cout << "...end" << std::endl ;
			}
			else if (_loop.check_poll_in_cgi(i))
			{
				//std::cout << "[Log]: " << "sending request to cgi..." << std::endl;
				cgi	*tmp;

				tmp = _loop.get_cgi_from_client(i);
				if (!tmp)
					throw (std::runtime_error("get cgi fail. cgi is NULL"));
				tmp->send_request_to_cgi();
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
		
		main_loop(server);
	}
	catch (const std::exception &e)
	{
		std::cout << "[Fatal]: " << e.what() << std::endl;
	}
}
