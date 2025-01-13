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
		cgi_status	_cgi_status();
		int			get_fd();
		void		free_request();

	private:
		request			*_request;
		struct	pollfd	_pollfd;
		int				port;
};

#endif
