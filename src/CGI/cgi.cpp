#include "cgi.hpp"

cgi::cgi(webserver &webserver)
{
	this->_webserver = &webserver;
	this->_is_ready = false;

}