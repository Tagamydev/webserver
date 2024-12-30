#include "cgi.hpp"
#include "utils.hpp"

cgi::cgi(webserver &webserver)
{
	this->_webserver = &webserver;
	this->_is_ready = false;
	this->_env = NULL;

	int pid;
	int	pipeIN[2];
	int	pipeOUT[2];
	loopHandler	*_loop;
	
	this->fd_pollIN = -1;
	this->fd_pollOUT = -1;

	_loop = this->_webserver->_loop;
	if(!_loop)
		throw std::runtime_error("Loop not found.");

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
		error = execle("/bin/python3 ./www/cgi/simple-cgi.py", "/bin/python3 ./www/cgi/simple-cgi.py", (char *)NULL, this->_env);
		std::cerr << "[FATAL]: execle fail inside fork, log[" << error << "]" << std::endl;
		exit(-1);
	}

	close(pipeIN[1]);
	close(pipeOUT[0]);

	fcntl(pipeIN[0], F_SETFL, O_NONBLOCK);
	fcntl(pipeOUT[1], F_SETFL, O_NONBLOCK);

	this->pos_pollIN = _loop->total_fds();
	_loop->_fdsList[_loop->total_fds()] = utils::pollfd_from_fd(pipeIN[0], POLLIN);
	this->pos_pollOUT = _loop->total_fds();
	_loop->_fdsList[_loop->total_fds()] = utils::pollfd_from_fd(pipeOUT[1], POLLOUT);

	this->fd_pollIN = pipeIN[0];
	this->fd_pollOUT = pipeOUT[1];
	_loop->_cgi_request[this->fd_pollIN] = this->_request->_request_number;
	_loop->_cgi_request[this->fd_pollOUT] = this->_request->_request_number;

	_loop->_cgiFD.push_back(fd_pollIN);
	_loop->_cgiFD.push_back(fd_pollOUT);
}

cgi::~cgi()
{

	loopHandler	*_loop;

	_loop = this->_webserver->_loop;

	if (this->fd_pollIN != -1)
	{
		_loop->_fdsList.erase(_loop->_fdsList.begin() + this->pos_pollIN);
		_loop->delete_cgiFD_from_cgiFD_list(this->fd_pollIN);
		close(this->fd_pollIN);
	}
	if (this->fd_pollOUT != -1)
	{
		_loop->_fdsList.erase(_loop->_fdsList.begin() + this->pos_pollOUT);
		_loop->delete_cgiFD_from_cgiFD_list(this->fd_pollOUT);
		close(this->fd_pollOUT);
	}

}

void cgi::read_from_cgi()
{
	this->_is_ready = true;
}

void cgi::send_request_to_cgi()
{
	// this need to be changed to the real body
	utils::send_response(this->fd_pollIN, this->_request->_body);
}

bool cgi::check_cgi_timeout()
{
	return (true);
}
