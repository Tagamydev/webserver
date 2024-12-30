#include "loopHandler.hpp"
#include "utils.hpp"

bool	check_if_fd_match(std::list<int> &_serversFD, int value)
{
	std::list<int>::iterator i = std::find(_serversFD.begin(), _serversFD.end(), value);
	if (i == _serversFD.end())
		return (false);
	return (true);
}

loopHandler::loopHandler(webserver &server)
{
	this->_webserver = &server;
	this->_webserver->_loop = this;

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
	this->_fdsList.push_back(utils::pollfd_from_fd(server_fd->_fd, POLLIN));
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

	this->_fdsList.push_back(utils::pollfd_from_fd(new_socket, POLLIN | POLLOUT));
	this->_clientFD_serverFD[this->_fdsList.back().fd] = this->_fdsList[number].fd;

	std::cout << "[Info]: New client in port: " 
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

