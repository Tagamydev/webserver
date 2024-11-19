#pragma once
#ifndef	SERVER_HPP
# define SERVER_HPP
# include "main.hpp"

class	server{
	//this need a location class diferent from config to make a map with that!!
	public:
		int			port;
		int			body_size_limit;
		std::string	host;
		std::string	default_file;
		std::string	cgi_extention;
		std::map<int, std::string>		error_pages;
		std::map<std::string, location>	locations;
};

#endif
