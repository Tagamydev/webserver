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

		//parser
		void	get_file_info(std::string path);
		std::string	save_config_file(std::string &path);
		void	set_server_list(std::string path);

		std::string	status_message(int code);
		std::string	mime_types(std::string &extention);
		std::map<std::string, std::string>	_mime_types;



	private:
		std::list<server>	_servers;
		std::string			_config_file;

		std::map<int, std::string>			_status_codes;
		void	set_status_codes();
		void	set_mime_types();

};

#endif
