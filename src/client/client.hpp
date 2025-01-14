#pragma once
#ifndef	CLIENT_HPP
# define CLIENT_HPP
# include "main.hpp"
# include "request.hpp"
# include "cgi.hpp"

class request;

class	client
{
	public:
		client(struct pollfd socket, int port);
		~client();

		request			*_request;
		int				port;
		struct	pollfd	_pollfd_server;

		request			*get_request();
		int				get_fd();
		struct pollfd	get_pollfd();
		void			free_request();
		bool			check_cgi_timeout();
		void			close_cgi();
		void			cgi_timeout();
		cgi_status		_cgi_status();

	private:
		struct	pollfd	_pollfd_client;
};

#endif
