#pragma once
#ifndef	SERVER_HPP
# define SERVER_HPP
# include "main.hpp"
# include "location.hpp"

class location;

class	server{
	//this need a location class diferent from config to make a map with that!!
	public:
		server();
		server(std::string &content);
		~server();

		// mandatory
		std::vector<int>				_ports;
		// std::vector<std::string>		_hosts; not used
		std::vector<std::string>		_names;
		int								_max_body_size;
		bool							_is_relative;
		// error pages example: error_page 500 502 503 504 /50x.html;
		std::map<int, std::string>		_error_pages;
		std::map<std::string, location>	_locations;

		//utils
		void	print_config_file();

		//getter
		const int &get_first_port() const;
		const std::vector<int> &get_ports() const;
		const std::string &get_first_name() const;
		const std::vector<std::string> &get_names() const;

	private:
		// init vars
		/* init error page by default */
		void initErrorPages(void);

		// parser
		void		check_save_parameters(std::stringstream &contentStream);
		void		check_location(std::string line);
		void		process_parameters(std::stringstream &contentStream, std::string line);
		bool		is_valid_port(std::string port);
		void		check_location_list();
		void 		check_empty_set_default();

};

#endif
