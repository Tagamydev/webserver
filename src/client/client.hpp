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
		client();
		~client();

		bool		compare_fd();
		request		*get_request();
		int			get_fd();
		void		free_request();

		bool		check_cgi_timeout();
		void		cgi_timeout();
		cgi_status	_cgi_status();

	private:
		request			*_request;
		struct	pollfd	_pollfd;
		int				port;
};

#endif
