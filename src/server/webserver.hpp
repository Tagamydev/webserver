#pragma once
#ifndef WEBSERVER_HPP
# define WEBSERVER_HPP
# include "main.hpp"
# include "server.hpp"

class server;

class webserver
{
	public:
		webserver(std::string &path);
		~webserver();
		std::list<server>	_servers;

	private:
		std::string			_config_file;
};

#endif
