/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: samusanc <samusanc@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/14 07:46:33 by samusanc          #+#    #+#             */
/*   Updated: 2025/01/18 15:02:18 by samusanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cgi.hpp"
#include "utils.hpp"
#include <signal.h>

cgi::cgi(request &_request, client *_client,
std::vector<struct pollfd> &list, webserver *_webserver)
{
	std::cout << BLUE;
	utils::print_debug("Creating new cgi...");
	std::cout << RESET;

	this->_request = &_request;
	this->_request->_cgi_status = WAITING;

	int status;
	int pid;
	int	pipeIN[2];
	int	pipeOUT[2];
	
	if (pipe(pipeIN) == -1 || pipe(pipeOUT) == -1)
		throw std::runtime_error("Pipe fail!.");

	this->init_env(_request.get_headers());
	
	// Apply FD_CLOEXEC to prevent leaks during execve
	// fcntl(pipeIN[0], F_SETFD, fcntl(pipeIN[0], F_GETFD) | FD_CLOEXEC);
	// fcntl(pipeIN[1], F_SETFD, fcntl(pipeIN[1], F_GETFD) | FD_CLOEXEC);
	// fcntl(pipeOUT[0], F_SETFD, fcntl(pipeOUT[0], F_GETFD) | FD_CLOEXEC);
	// fcntl(pipeOUT[1], F_SETFD, fcntl(pipeOUT[1], F_GETFD) | FD_CLOEXEC);

	

	pid = fork();
	if (pid < 0)
	{
		free_env();
		throw std::runtime_error("Fork failed on CGI execution."); // free env? or it calls the destructor?
	}

	if (pid == 0)
	{
		try
		{
			
			
			close(pipeIN[0]);
			close(pipeOUT[1]);
			if (dup2(pipeIN[1], STDOUT_FILENO) == -1 || dup2(pipeOUT[0], STDIN_FILENO) == -1)
			{
				throw (std::runtime_error("[FATAL]: dup2 failed on CGI execution."));
			}
			close(pipeIN[1]);
			close(pipeOUT[0]);


			// Close all non-standard FDs
			
			for (int fd = 3; fd < MAX_FD; ++fd) 
			{
				close(fd);
			}
			
			char * argv[] = {
				const_cast<char*>(this->_env_tmp["INTERPRETER"].c_str()),
				const_cast<char*>(this->_env_tmp["SCRIPT_NAME"].c_str()),	
				NULL
			};

			int	error;
			struct stat info;
			if (stat(argv[0], &info) != 0) 
			{
				throw (std::runtime_error("Cannot access path (doesn't exist or no permission)."));
			}
			
			error = execve(argv[0], argv, this->_env);
			std::cerr << "HERE" << std::endl;
			std::cerr << "[FATAL]: execve fail inside fork, log[" << error << "]" << argv[0] << std::endl;

			throw (std::runtime_error("[FATAL]: CGI execution failed."));
			// std::cerr << "[FATAL]: execve fail inside fork, log[" << error << "]" << argv[0] << std::endl;
			exit(127);
		}
		catch(const std::exception& e)
		{
			std::cerr << e.what() << std::endl;
       		exit(EXIT_FAILURE);
		}
		
		
	}
	this->_id = pid;
	pid_t result = waitpid(pid, &status, 0);
	if (result == -1) 
	{
		std::cerr << "[Error]: waitpid failed"<< std::endl;
		// Handle CGI error (e.g., send 500 Internal Server Error)
	} 
	else if (WIFEXITED(status))
	{
		this->_exitstatus = WIFEXITED(status);
		if (this->_exitstatus != 0) 
		{
			std::cerr << "[Error]: CGI process failed with exit code: " << this->_exitstatus << std::endl;
			this->_request->_cgi_status = DONE;
			// Handle CGI error (e.g., send 500 Internal Server Error)
		}
	}
	if (WIFSIGNALED(status)) //code 2
	{
		std::cerr << "[Error]: CGI process killed by signal: " << WTERMSIG(status) << std::endl;
		this->_request->_cgi_status = DONE;
		//Logger::log(Logger::WARN,"CGI.cpp", "WTERMSIG "+ to_string(ret));
	}
	
	close(pipeIN[1]);
	close(pipeOUT[0]);
	fcntl(pipeIN[0], F_SETFL, O_NONBLOCK);
	fcntl(pipeOUT[1], F_SETFL, O_NONBLOCK);
	// Optional: Make pipes non-blocking
	// fcntl(pipeIN[0], F_SETFL, fcntl(pipeIN[0], F_GETFL) | O_NONBLOCK);
	// fcntl(pipeOUT[1], F_SETFL, fcntl(pipeOUT[1], F_GETFL) | O_NONBLOCK);

	std::cout << "[Log]: " << "starting fds addition to vector list..." << std::endl;

	// the write fd go first, because this way you
	// can send first the info to the cgi and then read the cgi response
	this->_write_fd = utils::pollfd_from_fd(pipeOUT[1], POLLOUT);
	_webserver->_loop->add_cgi(_client->get_fd(), this->_write_fd);
	if (this->_request->_cgi_status == WAITING)
	{
		std::cout << "[Debug]: " << "fd to write to cgi: "<< this->_write_fd.fd << std::endl;
		write_to_cgi(_request._body);

	}


	this->_read_fd = utils::pollfd_from_fd(pipeIN[0], POLLIN);
	_webserver->_loop->add_cgi(_client->get_fd(), this->_read_fd);
	std::cout << "[Debug]: " << "fd to read from cgi: "<< this->_read_fd.fd << std::endl;
	_webserver->_loop->make_fd_list(list);

}

cgi::~cgi()
{
	// working on this, check on andres webserver
	if (_id != 0)
	{
		kill(_id, SIGKILL);
		waitpid(_id,&_exitstatus, 0);
		// close(_pip[0]);
		// close(_pipOut[1]);
		close(this->_read_fd.fd);
		close(this->_write_fd.fd);
		free_env();
		_id = 0;
	}



	// close(this->_read_fd.fd);
	// close(this->_write_fd.fd);
	// free_env();
	// // i need to close the cgi???
	// kill(this->_id, SIGKILL);
}

void cgi::read_to_cgi()
{
	std::cout << BLUE;
	utils::print_debug("Reading from cgi");
	std::cout << RESET;
	std::string	result;

	result = utils::read_file(this->_read_fd.fd);
	this->_request->_cgi_status = DONE;
	this->_request->_cgi_response = result;
}

void cgi::write_to_cgi(std::string &content)
{
	std::cout << BLUE;
    utils::print_debug("Write to CGI");
	std::cout << RESET;

	// this is not working because use send function that works only with sockets.
	// utils::send_response(this->_write_fd.fd, this->_request->_body);

    utils::print_debug("Writing request body to CGI script...");

    size_t total_sent = 0;
    size_t data_length = this->_request->_body.size();


    while (total_sent < data_length) 
	{
        ssize_t sent_now = write(this->_write_fd.fd, this->_request->_body.c_str() + total_sent, data_length - total_sent);

        if (sent_now == -1) 
		{
			std::cerr << "[FATAL]: failed to write to CGI." << std::endl;
            throw std::runtime_error("Error writing to CGI pipe.");
        }

        total_sent += sent_now;
    }

    // Important: Close the write pipe so the CGI knows the input is complete
    close(this->_write_fd.fd);

    std::ostringstream debug_message;
    debug_message << "Successfully wrote " << total_sent << " bytes to CGI.";
    utils::print_debug(debug_message.str());

}

bool cgi::cgi_timeout()
{
	return (false);
}

std::string cgi::set_cgi_interpreter(std::string filename, std::map<std::string, std::string> _cgi_extensions)
{
	size_t	dotPos = filename.find_last_of('.');
	std::string extension = "";

    if (dotPos != std::string::npos)
        extension = filename.substr(dotPos);
	if (extension.empty())
		return ("/usr/bin/bash");
	
	std::map<std::string, std::string>::iterator it = _cgi_extensions.find(extension);
	if (it != _cgi_extensions.end() && !it->second.empty())
	{
		return (it->second);
	}
	return ("/usr/bin/bash");
}


void	cgi::init_env(std::map<std::string, std::string> _headers)
{
	std::map<std::string, std::string>::iterator it;
	std::string	name;
	std::string	key;

	//save headers
	for (it = _headers.begin(); it != _headers.end(); *it++)
	{
		if (it->first == "content-length" || it->first == "content-type")
			name = it->first;
		else if (it->first == "host")
			continue;
		else
		{
			if (it->first.empty())
				continue;
			name = "HTTP_" + it->first;
		}
		utils::ft_to_upper(name);
		utils::ft_to_underscore(name);
		this->_env_tmp[name] = it->second;
	}
	//init manual headers
	this->_env_tmp["INTERPRETER"] = set_cgi_interpreter(this->_request->_uri_file, this->_request->_cgi_extensions);
	this->_env_tmp["REQUEST_METHOD"] = this->_request->_method;
	this->_env_tmp["SCRIPT_NAME"] = this->_request->_uri_file; // check where to init (The path to the CGI script being executed.)
	this->_env_tmp["SCRIPT_FILENAME"] = this->_request->_uri_file; // check where to init (The path to the CGI script being executed.)
	this->_env_tmp["REQUEST_URI"] = this->_request->_uri;
	if(!this->_request->_uri_params.empty())
		this->_env_tmp["QUERY_STRING"] = this->_request->_uri_params;
	this->_env_tmp["SERVER_NAME"] = _headers["host"];
	this->_env_tmp["SERVER_PROTOCOL"] = this->_request->_http_version;
	this->_env_tmp["REDIRECT_STATUS"] = "200";

	//create char** env
	this->_env = new char*[2 + _env_tmp.size()]; // +1 for INTERPRETER
	// Add interpreter on pos 0
	(this->_env)[0] = new char[this->_env_tmp["INTERPRETER"].size() + 1];
	std::strcpy((this->_env)[0],((this->_env_tmp["INTERPRETER"]).c_str()));
	
	int i =1;
	for (std::map<std::string, std::string>::iterator it = _env_tmp.begin(); 
				it != _env_tmp.end(); it++)
	{
		(this->_env)[i] = new char[(*it).first.size() +(*it).second.size() + 2];
		std::strcpy((this->_env)[i],((*it).first + "=" + (*it).second).c_str());
		i++;
	}
	(this->_env)[_env_tmp.size()] = NULL;
	_env_size = i;

	// utils::print_map_content(this->_env_tmp, "CGI ENV");
}

void cgi::free_env()
{
	utils::print_debug("freeing env");
	for(unsigned int i = 0; i < this->_env_size ; i++){
		delete[] this->_env[i];
	}
	delete[] this->_env;
	utils::print_debug("freeing env done");
}
