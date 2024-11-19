/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: samusanc <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/11 08:36:52 by samusanc          #+#    #+#             */
/*   Updated: 2024/10/08 13:31:12 by samusanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>
#include <iostream>
#include <fstream>
#include <sstream> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstdio>    
#include <map>


class   request{
	public:

		// con/destructor
		request(int fd);
		~request(void);

	// FIRST LINE

		// QUERY
		std::map<std::string, std::string>	query_str;	
		// key, value

		// METHOD: GET, POST, DELETE
		std::string	method;

		std::string	uri;
		std::string	http_version;

	// HEADERS
		std::map<std::string, std::string>	headers;	
		// header, content

	// BODY
		std::string	body;
};

class   response{
	public:

		// con/destructor
		response();
		~response(void);

	// STATUS LINE

		std::string	http_version;
		int			status_code;

	// HEADERS
		std::map<std::string, std::string>	headers;	
		// header, content

	// BODY
		std::string	body;
	
	// PRINT RESPONSE
		std::string	str();


	// FUNTIONS

	private:
		std::map<int, std::string>	status_codes_list;
		void		set_status_codes_list();
		std::string print_status_line();
		std::string print_headers();
};

response::response()
{
	this->set_status_codes_list();
	this->status_code = 200;
	this->http_version = "1.1";
	this->body = "a";
	this->headers["Content-Length"] = "0";
}

response::~response(){}

void	response::set_status_codes_list()
{
	this->status_codes_list[100] = "Continue";
	this->status_codes_list[101] = "Switching Protocols";
	this->status_codes_list[102] = "Processing";
	this->status_codes_list[200] = "OK";
	this->status_codes_list[201] = "Created";
	this->status_codes_list[202] = "Accepted";
	this->status_codes_list[203] = "Non-authoritative Information";
	this->status_codes_list[204] = "No Content";
	this->status_codes_list[205] = "Reset Content";
	this->status_codes_list[206] = "Partial Content";
	this->status_codes_list[207] = "Multi-Status";
	this->status_codes_list[208] = "Already Reported";
	this->status_codes_list[226] = "IM Used";
	this->status_codes_list[300] = "Multiple Choices";
	this->status_codes_list[301] = "Moved Permanently";
	this->status_codes_list[302] = "Found";
	this->status_codes_list[303] = "See Other";
	this->status_codes_list[304] = "Not Modified";
	this->status_codes_list[305] = "Use Proxy";
	this->status_codes_list[307] = "Temporary Redirect";
	this->status_codes_list[308] = "Permanent Redirect";
	this->status_codes_list[400] = "Bad Request";
	this->status_codes_list[401] = "Unauthorized";
	this->status_codes_list[402] = "Payment Required";
	this->status_codes_list[403] = "Forbidden";
	this->status_codes_list[404] = "Not Found";
	this->status_codes_list[405] = "Method Not Allowed";
	this->status_codes_list[406] = "Not Acceptable";
	this->status_codes_list[407] = "Proxy Authentication Required";
	this->status_codes_list[408] = "Request Timeout";
	this->status_codes_list[409] = "Conflict";
	this->status_codes_list[410] = "Gone";
	this->status_codes_list[411] = "Length Required";
	this->status_codes_list[412] = "Precondition Failed";
	this->status_codes_list[413] = "Payload Too Large";
	this->status_codes_list[414] = "Request-URI Too Long";
	this->status_codes_list[415] = "Unsupported Media Type";
	this->status_codes_list[416] = "Requested Range Not Satisfiable";
	this->status_codes_list[417] = "Expectation Failed";
	this->status_codes_list[418] = "I’m a teapot";
	this->status_codes_list[421] = "Misdirected Request";
	this->status_codes_list[422] = "Unprocessable Entity";
	this->status_codes_list[423] = "Locked";
	this->status_codes_list[424] = "Failed Dependency";
	this->status_codes_list[426] = "Upgrade Required";
	this->status_codes_list[428] = "Precondition Required";
	this->status_codes_list[429] = "Too Many Requests";
	this->status_codes_list[431] = "Request Header Fields Too Large";
	this->status_codes_list[444] = "Connection Closed Without Response";
	this->status_codes_list[451] = "Unavailable For Legal Reasons";
	this->status_codes_list[499] = "Client Closed Request";
	this->status_codes_list[500] = "Internal Server Error";
	this->status_codes_list[501] = "Not Implemented";
	this->status_codes_list[502] = "Bad Gateway";
	this->status_codes_list[503] = "Service Unavailable";
	this->status_codes_list[504] = "Gateway Timeout";
	this->status_codes_list[505] = "HTTP Version Not Supported";
	this->status_codes_list[506] = "Variant Also Negotiates";
	this->status_codes_list[507] = "Insufficient Storage";
	this->status_codes_list[508] = "Loop Detected";
	this->status_codes_list[510] = "Not Extended";
	this->status_codes_list[511] = "Network Authentication Required";
	this->status_codes_list[599] = "Network Connect Timeout Error";
}

std::string	response::print_status_line()
{
	std::stringstream	result;

	result 
	<< "HTTP/" << this->http_version << " " 
	<< this->status_code << " " 
	<< this->status_codes_list[this->status_code] << std::endl;

	return (result.str());
}

std::string	response::print_headers()
{
	std::stringstream	result;
	std::map<std::string, std::string>::iterator	i = this->headers.begin();
	std::map<std::string, std::string>::iterator	ie = this->headers.end();

	for (; i != ie; i++) {
		result << i->first << ": " << i->second;
		result << std::endl;
	}

	return (result.str());
}

std::string	response::str()
{
	std::stringstream	result;

	result << this->print_status_line();
	result << this->print_headers();
	result << std::endl;
	result << this->body;

	return (result.str());
}


std::string get_actual_date()
{
	time_t		now;
	time_t		*lock;
	std::string	result;

	lock = &now;
	now = time(lock);
	if (lock)
		result = std::string(ctime(lock));
	else
		result = std::string("error in getting time!!!");
	return (result);
}

int	main()
{
	try
	{
		int	fd;

		fd = open("request.txt", O_RDONLY);
		if (fd < 0)
			return (-1);

		response	respuesta = response();
		std::cout << respuesta.str() << std::endl;
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
}
