/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: samusanc <samusanc@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/14 07:46:25 by samusanc          #+#    #+#             */
/*   Updated: 2025/01/16 16:45:37 by samusanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef	CGI_HPP
# define CGI_HPP
# define CGI_TIMEOUT 7
# define MAX_FD 100

class webserver;
struct loopHandler;
class client;

enum	cgi_status
{
	NONE,
	WAITING,
	DONE
};

// this is included here because the enum of cgi_status has Circular Dependencies conflicts
# include "main.hpp"
# include "webserver.hpp"
# include "request.hpp"
class request;

class	cgi
{
	private:
		time_t _start_time;   // When CGI started
		int _timeout_seconds;
		pid_t _pid;           // CGI process ID

	public:

		cgi(request &_request, client *_client, std::vector<struct pollfd> &list, webserver *webserver);
		~cgi();

		void			write_to_cgi(std::string &content);
		void			read_to_cgi();
		bool			cgi_timeout(); //useless
		void			terminate_cgi();
		
		struct pollfd	_read_fd;
		struct pollfd	_write_fd;
		int				_env_size;
		int				_exitstatus;
		
		private:
		request			*_request;
		std::map<std::string, std::string>	_env_tmp;
		char			**_env;
		int				_id;
		
		void			init_env(std::map<std::string, std::string>	_headers);
		void 			free_env();
		std::string		set_cgi_interpreter(std::string filename, std::map<std::string, std::string> _cgi_extensions);
		bool			get_cgi_extension(std::string ext);
		void			set_cgi_extension();

};

#endif
