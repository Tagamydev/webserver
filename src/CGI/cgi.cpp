#include "cgi.hpp"

cgi::cgi(webserver &webserver)
{
	this->_webserver = &webserver;
	this->_is_ready = false;
	this->_env = NULL;

	int pid;
	int	pipeIN[2];
	int	pipeOUT[2];

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

}