#pragma once
#ifndef	SERVER_HPP
# define SERVER_HPP
# include "main.hpp"
# include "location.hpp"

class location;

class location;

class	server{
	//this need a location class diferent from config to make a map with that!!
	public:
		server();
		server(std::string &content);
		~server();

		// mandatory
		std::list<int>			_ports;
		std::list<std::string>	_hosts;
		std::list<std::string>	_names;

		int			_max_body_size;
		bool					_is_relative;

		// error pages example: error_page 500 502 503 504 /50x.html;
		std::map<int, std::string>		_error_pages;

		std::map<std::string, location>	locations;
		//std::string				_path;
		//std::string	_default_file;

		//std::string	_cgi_extention;

		// init vars
/* init error page by default */
		void initErrorPages(void);


		// parser
		void		check_save_parameters(std::stringstream &contentStream);
		void		check_location(std::string line);
		void		process_parameters(std::stringstream &contentStream, std::string line);
		bool		is_valid_port(int port);
		location	process_location(std::string line);
		//Seters
		void		set_error_pages(std::string value);
};

#endif
