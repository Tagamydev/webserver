#pragma once
#ifndef	LOCATION_HPP
# define LOCATION_HPP
# include "main.hpp"

class	location{

	public:
		location();
		~location();
		location(std::string line);

		std::string	_name;
		std::string	_path; // uri?
		std::string _root;
		std::string _alias;
		std::string	_index_file;
		size_t		_max_client_body_size;
		std::vector<std::string> _allowed_methods;
		std::string _cgi;
		std::string	_return;



		// std::map<int, std::string>	_return;

		bool		_index;
		bool		_auto_index;
		// bool		_get_enabled;
		// bool		_post_enabled;
		// bool		_delete_enabled;

};

#endif