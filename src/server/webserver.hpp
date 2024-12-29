#pragma once
#ifndef WEBSERVER_HPP
# define WEBSERVER_HPP
# include "main.hpp"
# include "server.hpp"
# include "cgi.hpp"

class server;
class cgi;

class webserver
{
	public:
		webserver(std::string &path);
		~webserver();

		std::string	status_message(int code);
		std::string	mime_types(std::string &extention);
		std::map<std::string, std::string>	_mime_types;
		std::map<int, cgi*>					_cgi_list;

	private:
		std::list<server>	_servers;
		std::string			_config_file;

		std::map<int, std::string>			_status_codes;
		void	set_status_codes();
		void	set_mime_types();

};

#endif
