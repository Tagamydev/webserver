#include "request.hpp"

request::request(int fd)
{
	this->http_version = "1.1";
	this->uri = "/minecraft.jpg";
	this->method = "GET";
}

request::~request(){}