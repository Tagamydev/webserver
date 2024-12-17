#pragma once
#ifndef	SERVER_HPP
# define SERVER_HPP
# include "main.hpp"

class	server{
	//this need a location class diferent from config to make a map with that!!
	public:

		// mandatory
		std::list<int>			_ports;
		std::list<std::string>	_names;

		std::string				_root;
		std::list<std::string>	_hosts;

		int				_body_size_limit;

		std::string		_default_file;

		// implement the root and alias directive
		
		std::string	default_file;
		std::string	cgi_extention;

		// error pages example: error_page 500 502 503 504 /50x.html;
		std::map<std::list<int>, std::string>		error_pages;

		std::map<std::string, location>	locations;
};

#endif
