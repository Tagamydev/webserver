#pragma once
#ifndef	CGI_HPP
# define CGI_HPP
# include "main.hpp"
# include "webserver.hpp"
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
# include "request.hpp"
class request;

class	cgi
{
	public:

		cgi(request &_request, client *_client);
		~cgi();

		void		write(std::string &content);
		void		read();
		bool		check_cgi_timeout();
	private:
		request			*_request;
		char			**_env;
		struct pollfd	_read_fd;
		struct pollfd	_write_fd;
		int				_id;
};

#endif
