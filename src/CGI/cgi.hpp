#pragma once
#ifndef	CGI_HPP
# define CGI_HPP
# include "main.hpp"
# include "webserver.hpp"
# include "request.hpp"
# define CGI_TIMEOUT 7

class webserver;
class request;

class	cgi
{
	public:
		cgi(webserver &webserver);
		~cgi();

		bool		_is_ready;
		std::string	_cgi_response;
		void		send_request_to_cgi();
		void		read_from_cgi();
		bool		check_cgi_timeout();

	private:
		webserver	*_webserver;
		request		*_request;
		char		**_env;

		int			pos_pollIN;
		int			fd_pollIN;
		int			pos_pollOUT;
		int			fd_pollOUT;
};

#endif