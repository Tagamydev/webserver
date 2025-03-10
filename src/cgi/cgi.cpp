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
	utils::print_debug("new cgi...");
	std::cout << "\n\n HERE CGIIIII!! \n\n" ;

	// this->_env = NULL;
	this->_request = &_request;
	this->_request->_cgi_status = WAITING;

	int pid;
	int	pipeIN[2];
	int	pipeOUT[2];
	
	if (pipe(pipeIN) == -1 || pipe(pipeOUT) == -1)
		throw std::runtime_error("Pipe fail!.");

	// create env
	this->init_env(_request.get_headers());

	pid = fork();
		if (pid < 0)
	{
		free_env();
		throw std::runtime_error("Fork fail!."); // free env? or it calls the destructor?
	}

	if (pid == 0)
	{
		int	error;
		close(pipeIN[0]);
		close(pipeOUT[1]);
		if (dup2(pipeIN[1], STDOUT_FILENO) == -1 || dup2(pipeOUT[0], STDIN_FILENO) == -1)
		{
			std::cerr << "[FATAL]: dup2 fail inside fork!." << std::endl; // free env? or it calls the destructor?
			exit(-1);
		}
		close(pipeIN[1]);
		close(pipeOUT[0]);

		//test first with access before execve
		
		//Create _argv for execve
		char * argv[] = {const_cast<char*>(this->_env_tmp["INTERPRETER"].c_str()),
						const_cast<char*>(this->_env_tmp["SCRIPT_NAME"].c_str()),	
						NULL};
		error = execve(argv[0], argv, this->_env);
		// std::cout << "\n\n PIPE IN " << pipeIN[1] <<  " PIPE OUT " << pipeOUT[0] << std::endl;

		std::cerr << "[FATAL]: execve fail inside fork, log[" << error << "]" << argv[0] << std::endl;
		free_env();
		exit(-1);
	}
	this->_id = pid;

	close(pipeIN[1]);
	close(pipeOUT[0]);


	std::cout << "[Log]: " << "starting fds addition to vector list..." << std::endl;

	// the write fd go first, because this way you
	// can send first the info to the cgi and then read the cgi response
	this->_write_fd = utils::pollfd_from_fd(pipeOUT[1], POLLOUT);
	_webserver->_loop->add_cgi(_client->get_fd(), this->_write_fd);
	std::cout << "[Debug]: " << "fd to write to cgi: "<< this->_write_fd.fd << std::endl;

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

void cgi::read()
{
	utils::print_debug("reading from cgi");
	std::string	result;

	result = utils::read_file(this->_read_fd.fd);
	this->_request->_cgi_status = DONE;
	this->_request->_cgi_response = result;

// to delete
	utils::print_debug("\nmakeDAVID\n");
	utils::print_debug(this->_request->_cgi_response);
}

void cgi::writee(std::string &content)
{
    utils::print_debug("\n\nwrite to CGI ");
	// this is not working because use send function that works only with sockets.
	// utils::send_response(this->_write_fd.fd, this->_request->_body);

    utils::print_debug("Writing request body to CGI script...");

    size_t total_sent = 0;
    size_t data_length = this->_request->_body.size();


    while (total_sent < data_length) 
	{
        ssize_t sent_now = write(this->_write_fd.fd, this->_request->_body.c_str() + total_sent, data_length - total_sent);

        if (sent_now == -1) {
            perror("write to CGI failed");
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
	// this->_env_tmp["INTERPRETE"] = "/usr/bin/php-cgi"; //set this with array
	
	// std::cout << "\n\nINTERPRETER " << 	this->_env_tmp["INTERPRETER"] << std::endl;

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
	
	
	// (this->_env)[0] = new char[6 + this->_env_tmp["SCRIPT_NAME"].size() + 1];
	// std::strcpy((this->_env)[0],(("./www" + this->_env_tmp["SCRIPT_NAME"]).c_str()));
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

	utils::print_map_content(this->_env_tmp, "CGI ENV");
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
