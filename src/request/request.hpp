/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: samusanc <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/22 09:08:35 by samusanc          #+#    #+#             */
/*   Updated: 2025/01/25 15:41:41 by samusanc         ###   ########.fr       */
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
		void		close_cgi();
		bool		check_if_cgi();

		// QUERY
		std::string		_method;
		std::string		_uri;
		std::string		_uri_file;
		std::string		_uri_params;

		// BODY
		std::string		_body;
		std::string		_body_parsed;

		// STATES FOR RESPONSE
		int				_error_code;
		std::string		_debug_msg;

		server			*_server;
		location		*_location;

	private:
		int				_fd;

		int				_body_length;
		int				_has_body;
        bool			_chunked_flag;
		std::string		_http_version;

		std::map<std::string, std::string>	_query_str;	
		std::map<std::string, std::string>	_headers;	

		void	parsing();
		void	debug();
		void	clear();

		void	get_server(client *_client, webserver *_webserver);
		void	get_location();

		void	set_error_code(int code, std::string msg);
		void	print_request();
		void	print_header();
		void	print_body();
		void	print_others();
		void	check_config_file(client *_client, webserver *_webserver);
        void	check_save_request_line(std::string line);
        void	is_valid_method(std::string line);
        void	is_valid_uri(std::string &line);
        void	process_uri(std::string line);
        void	is_valid_httpv(std::string line);
        void	process_headers(std::stringstream &reqFile, std::string line);
		void	save_headers(std::string &line);
		void	parse_headers();
		bool	space_in_header_name(std::string line);
		void	process_body(std::stringstream &reqFile, std::string line);
};

#endif
