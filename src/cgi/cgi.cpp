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
		// error = execle("/bin/python3", "/bin/python3", "./www/cgi/simple-cgi.py", (char *)NULL, this->_env);
		// error = execle("./www/cgi-bin/upload.py", "./www/cgi-bin/upload.py",  (char *)NULL, this->_env);
		// error = execle("./www/cgi-bin/form-handler.cgi", "./www/cgi-bin/form-handler.cgi",  (char *)NULL, this->_env);
		error = execle(this->_env[0], this->_env[0],  (char *)NULL, this->_env);

		std::cerr << "[FATAL]: execle fail inside fork, log[" << error << "]" << std::endl;
		exit(-1);
	}
	this->_id = pid;

	close(pipeIN[1]);
	close(pipeOUT[0]);

	// fcntl(pipeIN[0], F_SETFL, O_NONBLOCK);
	// fcntl(pipeOUT[1], F_SETFL, O_NONBLOCK);

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
	// if (_id != 0)
	// {
	// 	kill(_id, SIGKILL);
	// 	waitpid(_id,&_exitstatus, 0);
	// 	// close(_pip[0]);
	// 	// close(_pipOut[1]);
	// 	close(this->_read_fd.fd);
	// 	close(this->_write_fd.fd);
	// 	free_env();
	// 	_id = 0;
	// }



	close(this->_read_fd.fd);
	close(this->_write_fd.fd);
	free_env();
	// i need to close the cgi???
	kill(this->_id, SIGKILL);
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

void cgi::write(std::string &content)
{
	// this need to be changed to the real body
	utils::send_response(this->_write_fd.fd, this->_request->_body);
}

bool cgi::cgi_timeout()
{
	return (false);
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
	this->_env_tmp["REQUEST_METHOD"] = this->_request->_method;
	this->_env_tmp["SCRIPT_NAME"] = this->_request->_uri_file; // check where to init (The path to the CGI script being executed.)
	this->_env_tmp["REQUEST_URI"] = this->_request->_uri;
	if(!this->_request->_uri_params.empty())
		this->_env_tmp["QUERY_STRING"] = this->_request->_uri_params;
	this->_env_tmp["SERVER_NAME"] = _headers["host"];
	// this->_env_tmp["SERVER_PORT"] = ; port on location?
	this->_env_tmp["SERVER_PROTOCOL"] = this->_request->_http_version;

	//create char** env
	this->_env = new char*[2 + _env_tmp.size()]; // +1 for exec route
	
	
	(this->_env)[0] = new char[6 + this->_env_tmp["SCRIPT_NAME"].size() + 1];
	std::strcpy((this->_env)[0],(("./www" + this->_env_tmp["SCRIPT_NAME"]).c_str()));
	
	
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
		delete[] _env[i];
	}
	delete[] _env;
	utils::print_debug("freeing env done");
}
