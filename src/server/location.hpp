#pragma once
#ifndef	LOCATION_HPP
# define LOCATION_HPP
# include "main.hpp"

class	location{

	public:
		std::string	location;
		// this need to be reworked idk if you can have both without problems

		std::string	_path;

		std::map<int, std::string>	_return;

		// meaning if it is root or alias
		bool		_is_relative;
		std::string	_index_file;

		bool		_get_enabled;
		bool		_post_enabled;
		bool		_delete_enabled;

		bool		_autoindex_enabled;
};

#endif