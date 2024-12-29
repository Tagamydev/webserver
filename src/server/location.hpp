#pragma once
#ifndef	LOCATION_HPP
# define LOCATION_HPP
# include "main.hpp"

class	location{

	public:
		std::string	location;
		// this need to be reworked idk if you can have both without problems


		std::map<int, std::string>	_return;

		std::string	_path;
		// meaning if it is root or alias
		bool		_is_relative;
		std::string _root_alias;
		std::string _cgi;
		std::string	_index_file;
		size_t		_max_client_body_size;
		std::vector<std::string> _allowed_methods;


		bool		_index;
		bool		_get_enabled;
		bool		_post_enabled;
		bool		_delete_enabled;

		bool		_autoindex_enabled;
};

#endif