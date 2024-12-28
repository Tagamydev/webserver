#pragma once
#ifndef	CGI_HPP
# define CGI_HPP
# include "main.hpp"
# define	CGI_TIMEOUT 7

class cgi
{
	public:
		cgi();
		~cgi();
	
	private:
		webserver	*_webserver;
};

#endif