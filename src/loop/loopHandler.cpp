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

void	loopHandler::delete_FD_from_FD_list(int fd, std::list<int> &list)
{
	std::list<int>::iterator	i = list.begin();
	std::list<int>::iterator	ie = list.end();

	for (; i != ie; i++)
	{
		if (*i == fd)
		{
			list.erase(i);
			break ;
		}
	}
}

void	loopHandler::delete_FD_from_pollFD_list(int fd, std::vector<struct pollfd> &list)
{
	std::vector<struct pollfd>::iterator	i = list.begin();
	std::vector<struct pollfd>::iterator	ie = list.end();

	for (; i != ie; i++)
	{
		if (i->fd == fd)
		{
			list.erase(i);
			break ;
		}
	}
}


void	loopHandler::send_response_client(int n_client, request *tmp_req)
{
	response	_response = response(*tmp_req, *this->_webserver);
	bool		_keep_alive;

	_keep_alive = _response._keep_alive;
	utils::send_response(this->_fdsList[n_client].fd, _response.str());
	delete tmp_req;
	if (!_keep_alive)
		close(this->_fdsList[n_client].fd);
		this->_fdsList.erase(this->_fdsList.begin() + n_client);
}

cgi	*loopHandler::get_cgi_from_client(int n_client)
{
	request	*tmp_req = this->get_request_from_client(this->_cgi_request[this->_fdsList[n_client].fd]);

	if (!tmp_req)
		throw (std::runtime_error("get cgi fail. Request is NULL"));
	return (tmp_req->_cgi);
}

void loopHandler::new_server(int port)
{
	serverFd server_fd(port);

	this->_fdsList.push_back(utils::pollfd_from_fd(server_fd._fd, POLLIN));
	this->_serverFD_port[server_fd._fd] = port;
	this->_serversFD.push_back(server_fd._fd);
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
	this->_client_and_request[number] = new request(this->_fdsList[number].fd, *this->_webserver, number, *this);
}

request	*loopHandler::get_request_from_client(int n_client)
{
	return (this->_client_and_request[n_client]);
}

