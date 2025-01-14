/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: samusanc <samusanc@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/14 21:17:39 by samusanc          #+#    #+#             */
/*   Updated: 2025/01/14 22:04:10 by samusanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "client.hpp"
client::client(struct pollfd socket, int port)
{
	this->_pollfd_server = socket;
	this->_pollfd_client = utils::pollfd_from_fd(socket.fd, POLLIN | POLLOUT);
}

client::~client(){}

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
