#pragma once
#ifndef	LOCATION_HPP
# define LOCATION_HPP
# include "main.hpp"

class request;

class	location{

	public:
		location();
		~location();
		location(std::string line);

		// std::map<int, std::string>	_return;

		// bool		_index;
		std::string	_index_file;
		bool		_auto_index;
		bool		_cgi_enabled;
		bool		_return;
		std::string	_return_code;
		std::string	_return_path;
		std::string	_path;
		std::string _root;
		std::string _alias;

		// bool		_get_enabled;
		// bool		_post_enabled;
		// bool		_delete_enabled;

		void	set_priv_attribute(std::string line);	
		void	print_location_content();

		const std::string &get_path() const;
		bool		is_allowed_method(std::string method);
		// const std::string &get_root() const;

	private:
		std::string	_save_file;
		std::vector<std::string> _allowed_methods;
		std::string _cgi_path;
};

#endif
