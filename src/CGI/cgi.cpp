#include "cgi.hpp"
#include "utils.hpp"

cgi::cgi(webserver &webserver, loopHandler &_loop, request &_request)
{
	this->_webserver = &webserver;
	this->_is_ready = false;
	this->_env = NULL;
	this->_request = &_request;

	int pid;
	int	pipeIN[2];
	int	pipeOUT[2];
	
	this->fd_pollIN = -1;
	this->fd_pollOUT = -1;

	if (pipe(pipeIN) == -1 || pipe(pipeOUT) == -1)
		throw std::runtime_error("Pipe fail!.");

	pid = fork();
	if (pid < 0)
		throw std::runtime_error("Fork fail!.");

	if (!pid)
	{
		int	error;
		close(pipeIN[0]);
		close(pipeOUT[1]);
		if (dup2(pipeIN[1], STDOUT_FILENO) == -1 || dup2(pipeOUT[0], STDIN_FILENO) == -1)
		{
			std::cerr << "[FATAL]: dup2 fail inside fork!." << std::endl;
			exit(-1);
		}
	//	error = execle("/bin/python3 ./www/cgi/simple-cgi.py", "./www/cgi/simple-cgi.py", (char *)NULL, this->_env);
		error = execle("/bin/python3", "/bin/python3", "./www/cgi/simple-cgi.py", (char *)NULL, this->_env);

		std::cerr << "[FATAL]: execle fail inside fork, log[" << error << "]" << std::endl;
		exit(-1);
	}

	close(pipeIN[1]);
	close(pipeOUT[0]);

	fcntl(pipeIN[0], F_SETFL, O_NONBLOCK);
	fcntl(pipeOUT[1], F_SETFL, O_NONBLOCK);

	std::cout << "[Log]: " << "starting fds addition to vector list..." << std::endl;

	this->fd_pollIN = pipeIN[0];
	_loop._cgiFD.push_back(fd_pollIN);
	this->pos_pollIN = _loop.total_fds();
	_loop._fdsList.push_back(utils::pollfd_from_fd(pipeIN[0], POLLIN));
	_loop._cgi_request[this->fd_pollIN] = this->_request->_request_number;
	std::cout << "[Log]: " << "fd IN: "<< this->fd_pollIN << std::endl;

	this->fd_pollOUT = pipeOUT[1];
	_loop._cgiFD.push_back(fd_pollOUT);
	this->pos_pollOUT = _loop.total_fds();
	_loop._fdsList.push_back(utils::pollfd_from_fd(pipeOUT[1], POLLOUT));
	_loop._cgi_request[this->fd_pollOUT] = this->_request->_request_number;
	std::cout << "[Log]: " << "fd OUT: "<< this->fd_pollOUT << std::endl;

}

cgi::~cgi()
{

	loopHandler	*_loop;

	_loop = this->_webserver->_loop;

	if (this->fd_pollIN != -1)
	{
		_loop->delete_FD_from_FD_list(this->fd_pollIN, _loop->_cgiFD);
		_loop->delete_FD_from_pollFD_list(this->fd_pollIN, _loop->_fdsList);
		close(this->fd_pollIN);
	}
	if (this->fd_pollOUT != -1)
	{
		_loop->delete_FD_from_FD_list(this->fd_pollOUT, _loop->_cgiFD);
		_loop->delete_FD_from_pollFD_list(this->fd_pollOUT, _loop->_fdsList);
		close(this->fd_pollOUT);
	}

}

void cgi::read_from_cgi()
{
	this->_cgi_response = utils::read_file(this->fd_pollIN);
	this->_is_ready = true;
}

void cgi::send_request_to_cgi()
{
	// this need to be changed to the real body
	utils::send_response(this->fd_pollIN, this->_request->_body);
}

bool cgi::check_cgi_timeout()
{
	return (false);
}
