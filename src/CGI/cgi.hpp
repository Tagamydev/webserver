#pragma once
#ifndef	CGI_HPP
# define CGI_HPP
# include "main.hpp"
# include "webserver.hpp"
# include "request.hpp"
# define CGI_TIMEOUT 7

class webserver;
class loopHandler;
class request;

enum	cgi_status
{
	NONE,
	WAITING,
	DONE
};

class	cgi
{
	public:
		cgi(webserver &webserver, loopHandler &_loop, request &_request);
		~cgi();

		bool		_is_ready;
		std::string	_cgi_response;
		void		send_request_to_cgi();
		void		read_from_cgi(int fd);
		bool		check_cgi_timeout();

		request		*_request;
	private:
		webserver	*_webserver;
		char		**_env;

		struct pollfd	_read_fd;
		struct pollfd	_write_fd;
		int			_id;
};

#endif
