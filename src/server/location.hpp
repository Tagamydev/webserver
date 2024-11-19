#pragma once
#ifndef	LOCATION_HPP
# define LOCATION_HPP
# include "main.hpp"

class	location{
	std::string	location;
	// this need to be reworked idk if you can have both without problems
	std::string	root;
	std::string	alias;
	std::string	index_file;
	bool		get_enabled;
	bool		post_enabled;
	bool		delete_enabled;
	bool		autoindex_enabled;
	bool		file_uploading_enabled;
};

#endif