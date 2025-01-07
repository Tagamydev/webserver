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
	
	this->read_fd = -1;
	this->write_fd = -1;

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

	char	test[100];

	int err = read(pipeIN[0], test, 99);
	test[99] = '\0';
	std::cout << "this is a test: " << err << test << std::endl;

	std::cout << "[Log]: " << "starting fds addition to vector list..." << std::endl;

	this->read_fd = pipeIN[0];
	_loop._cgiFD.push_back(read_fd);
	_loop._fdsList.push_back(utils::pollfd_from_fd(pipeIN[0], POLLIN));
	_loop._cgi_request[this->read_fd] = this->_request->_request_number;
	std::cout << "[Log]: " << "fd IN: "<< this->read_fd << std::endl;

	this->write_fd = pipeOUT[1];
	_loop._cgiFD.push_back(write_fd);
	//_loop._fdsList.push_back(utils::pollfd_from_fd(pipeOUT[1], POLLOUT));
	_loop._cgi_request[this->write_fd] = this->_request->_request_number;
	std::cout << "[Log]: " << "fd OUT: "<< this->write_fd << std::endl;

}

cgi::~cgi()
{

	loopHandler	*_loop;

	_loop = this->_webserver->_loop;

	if (this->read_fd != -1)
	{
		_loop->delete_FD_from_FD_list(this->read_fd, _loop->_cgiFD);
		_loop->delete_FD_from_pollFD_list(this->read_fd, _loop->_fdsList);
		close(this->read_fd);
	}
	if (this->write_fd != -1)
	{
		_loop->delete_FD_from_FD_list(this->write_fd, _loop->_cgiFD);
		_loop->delete_FD_from_pollFD_list(this->write_fd, _loop->_fdsList);
		close(this->write_fd);
	}

}

void cgi::read_from_cgi(int fd)
{
	this->_cgi_response = utils::read_file(this->read_fd);
	this->_is_ready = true;
}

void cgi::send_request_to_cgi()
{
	// this need to be changed to the real body
	utils::send_response(this->write_fd, this->_request->_body);
}

bool cgi::check_cgi_timeout()
{
	return (false);
}
