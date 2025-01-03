#pragma once
#ifndef WEBSERVER_HPP
# define WEBSERVER_HPP
# include "main.hpp"
# include "server.hpp"
# include "cgi.hpp"

class server;
class cgi;

class loopHandler;
class webserver
{
	public:
		webserver(std::string &path);
		~webserver();

		//parser
		void	get_file_info(std::string path);
		std::string	save_config_file(std::string &path);
		void	set_server_list(std::string path);
		std::list<std::string>	check_save_server_vector(std::string &_config_file);
		void	remove_comments(std::string &_config_file);
		void	check_brackets(std::string _config_file);
		size_t	find_start_server(size_t start, std::string _config_file);
		size_t	find_end_server(size_t start, std::string _config_file);
		void	print_list_content(std::list<std::string> list);



		std::string	status_message(int code);
		std::string	mime_types(std::string &extention);
		std::map<std::string, std::string>	_mime_types;
		loopHandler							*_loop;



	private:
		std::list<server>	_servers;
		std::string			_config_file;
		// parser
		std::list<std::string>			_server_block_list;

		std::map<int, std::string>			_status_codes;
		void	set_status_codes();
		void	set_mime_types();

};

#endif
