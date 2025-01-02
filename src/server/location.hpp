#pragma once
#ifndef	LOCATION_HPP
# define LOCATION_HPP
# include "main.hpp"

class	location{

	public:
		location();
		~location();
		location(std::string line);





		// std::map<int, std::string>	_return;

		// bool		_index;
		bool		_auto_index;
		bool		_cgi_enabled;
		// bool		_get_enabled;
		// bool		_post_enabled;
		// bool		_delete_enabled;

		void	set_priv_attribute(std::string line);	
		void	print_location_content();

		const std::string &get_path() const;
		// const std::string &get_root() const;

	private:
		std::string	_path;
		std::string _root;
		std::string _alias;
		std::string	_index_file;
		std::string	_save_file;
		std::vector<std::string> _allowed_methods;
		std::string _cgi_path;
		std::string	_return_code;
		std::string	_return_path;
};

#endif