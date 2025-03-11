/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: samusanc <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/22 09:08:35 by samusanc          #+#    #+#             */
/*   Updated: 2025/02/11 06:55:13 by samusanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef	REQUEST_HPP
# define REQUEST_HPP
# include "main.hpp"
# include "webserver.hpp"
# include "cgi.hpp"

# define MAX_URI_LENGTH 4096
# define MAX_CONTENT_LENGTH 30000000
# define MAX_BUFFER_LENGTH 2048

class webserver;
class loopHandler;
class cgi;
class client;
class server;
class location;

class   request
{
	public:
		request(client *_client, webserver *_webserver, std::vector<struct pollfd> &list);
		~request(void);

		// CGI
		cgi			*_cgi;
		cgi_status	_cgi_status;
		std::string	_cgi_response;
        bool			_is_cgi;
		void		close_cgi();
		bool		check_if_cgi();

		// QUERY
		std::string		_query;
		std::string		_method;
		std::string		_uri;
		std::string		_uri_file;
		std::string		_uri_params; // vars after ? on filename
		std::string		_http_version;

		// BODY
		std::string		_body;
		std::string		_body_parsed;

		// STATES FOR RESPONSE
		int				_error_code;
		std::string		_debug_msg;
		std::map<std::string, std::string>	_cgi_extensions;	

		server			*_server;
		location		*_location;

		// Getters
		std::map<std::string, std::string>	get_headers();


	private:
		int				_fd;

		int				_has_body;
		int				_content_length;
        bool			_chunked_flag;
		bool			_multiform_flag;
		std::string		_boundary;

		std::map<std::string, std::string>	_headers;	

		void	parsing();
		void	debug();
		void	clear();

		void	get_server(client *_client, webserver *_webserver);
		void	get_location();


		void	set_error_code(int code, std::string msg);
		void	set_cgi_extension();

		void	print_request();
		void	print_header();
		void	print_body();
		void	print_others();
		void	check_config_file(client *_client, webserver *_webserver);
        void	check_save_request_line(std::string line);
        void	is_valid_method(std::string line);
        void	is_valid_uri(std::string &line);
        void	is_valid_httpv(std::string line);
		void	parse_headers();
		void	parse_body(server *this_server);
        void	process_uri(std::string line);
        void	process_headers(std::stringstream &reqFile, std::string line);
		void	process_body(std::stringstream &reqFile, std::string line);
		void	process_chunked();
		void	save_headers(std::string &line);
		bool	space_in_header_name(std::string line);
};

#endif
