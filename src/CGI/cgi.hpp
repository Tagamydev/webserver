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

		bool	_is_ready;

	private:
		webserver	*_webserver;
		request		*_request;


};


#endif