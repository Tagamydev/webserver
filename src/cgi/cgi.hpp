/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: samusanc <samusanc@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/14 07:46:25 by samusanc          #+#    #+#             */
/*   Updated: 2025/01/14 21:36:50 by samusanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef	CGI_HPP
# define CGI_HPP
# define CGI_TIMEOUT 7

class webserver;
class loopHandler;
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
	public:

		cgi(request &_request, client *_client);
		~cgi();

		void			write(std::string &content);
		void			read();
		bool			cgi_timeout();
		struct pollfd	_read_fd;
		struct pollfd	_write_fd;

	private:
		request			*_request;
		char			**_env;
		int				_id;
};

#endif
