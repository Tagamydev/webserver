/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserver.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: samusanc <samusanc@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/15 08:03:15 by samusanc          #+#    #+#             */
/*   Updated: 2025/01/15 08:08:19 by samusanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef WEBSERVER_HPP
# define WEBSERVER_HPP
# include "main.hpp"
# include "server.hpp"
# include "cgi.hpp"

class server;
class cgi;
class loopHandler;

class webserver
{
	public:
		webserver(std::string &path);
		~webserver();

		loopHandler							*_loop;

		// init
		std::string				status_message(int code);
		std::string				mime_types(std::string &extention);
		
		std::map<std::string, std::string>	_mime_types;
		std::map<int, std::string>			_status_codes;

	private:
		std::vector<server>			_servers;
		std::string					_config_file;
		std::list<std::string>		_server_block_list;

		void						get_file_info(std::string path);
		std::string					save_config_file(std::string &path);
		void						set_server_list(std::string path);
		std::list<std::string>		check_save_server_vector(std::string &_config_file);
		void						remove_comments(std::string &_config_file);
		void						check_brackets(std::string _config_file);
		size_t						find_start_server(size_t start, std::string _config_file);
		size_t						find_end_server(size_t start, std::string _config_file);
		void						print_list_content(std::list<std::string> list);
		void 						check_servers();

		void						set_status_codes();
		void						set_mime_types();

};

#endif
