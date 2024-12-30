#include "location.hpp"

location::location() : _root(DEFAULT_ROOT), _index(DEFAULT_INDEX),
_cgi(DEFAULT_CGI) {}

location::~location()
{}

location::location(std::string line) : _name(""), _path(""), _index_file(DEFAULT_INDEX), _cgi(DEFAULT_CGI),
      _max_client_body_size(DEFAULT_MAX_CLIENT_BODY_SIZE), _alias(""), _auto_index(false), _return("") 
{
	
}
