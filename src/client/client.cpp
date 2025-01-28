/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: samusanc <samusanc@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/14 21:17:39 by samusanc          #+#    #+#             */
/*   Updated: 2025/01/28 17:10:45 by samusanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "client.hpp"
client::client(struct pollfd socket, int port)
{
	int new_socket;
	this->_request = NULL;

	if ((new_socket = accept(socket.fd, NULL, NULL)) == -1)
		throw (std::runtime_error("Accept fail."));

	this->_pollfd_client = utils::pollfd_from_fd(new_socket, POLLIN | POLLOUT);
	this->_pollfd_server = socket;
	this->port = port;
	std::cout << "[Info]: New client in port: " << port << std::endl;
}

client::~client()
{
	if (this->_request)
		delete this->_request;
	exit(-1);
}

struct pollfd	client::get_pollfd()
{
	return (this->_pollfd_client);
}

request		*client::get_request()
{
	return (this->_request);
}

int			client::get_fd()
{
	return (this->_pollfd_client.fd);
}

void		client::free_request()
{
	delete this->_request;
	this->_request = NULL;
}

cgi_status	client::_cgi_status()
{
	if (!this->_request)
		throw (std::runtime_error("Client dont have a request"));
	return (this->_request->_cgi_status);
}

bool	client::check_cgi_timeout()
{
	return (this->_request->_cgi->cgi_timeout());
}

void	client::cgi_timeout()
{
	this->close_cgi();
	(this->get_request())->_error_code = 408;
}

void	client::close_cgi()
{
	(this->get_request())->close_cgi();
}
