#pragma once
#ifndef	SERVER_HPP
# define SERVER_HPP
# include "main.hpp"

class	server{
	//this need a location class diferent from config to make a map with that!!
	public:
		server();
		~server();

		// mandatory
		std::list<int>			_ports;
		std::list<std::string>	_hosts;

		std::string				_path;
		bool					_is_relative;

		std::list<std::string>	_names;

		int			_body_size_limit;

		std::string	_default_file;
		std::string	_cgi_extention;

		// error pages example: error_page 500 502 503 504 /50x.html;
		std::map<std::list<int>, std::string>		error_pages;

		std::map<std::string, location>	locations;
};

#endif
