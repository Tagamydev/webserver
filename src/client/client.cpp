/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: samusanc <samusanc@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/14 21:17:39 by samusanc          #+#    #+#             */
/*   Updated: 2025/01/28 17:29:25 by samusanc         ###   ########.fr       */
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
	close(this->_pollfd_client.fd);
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
	if (this->_request->_cgi->cgi_timeout()) {
        this->cgi_timeout(); // Handle timeout
        return true;
    }
    return false;
	// return (this->_request->_cgi->cgi_timeout());
}

void	client::cgi_timeout()
{
	std::cerr << "[Timeout]: CGI execution exceeded time limit!" << std::endl;

    // Close CGI resources and mark as timed out
    this->_request->_cgi->terminate_cgi();
    this->close_cgi();

    // Send a 408 Request Timeout response
    this->get_request()->_error_code = 408;
}

void	client::close_cgi()
{
	(this->get_request())->close_cgi();
}
