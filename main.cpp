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
#include <sys/stat.h>

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
	size_t pos = result.find('\n');
    if (pos != std::string::npos)
        result.erase(pos, 1); // Elimina el carácter '\n'
	return (result);
}

class	location{
	std::string	location;
	// this need to be reworked idk if you can have both without problems
	std::string	root;
	std::string	alias;
	std::string	index_file;
	bool		get_enabled;
	bool		post_enabled;
	bool		delete_enabled;
	bool		autoindex_enabled;
	bool		file_uploading_enabled;
};

class	config{
	//this need a location class diferent from config to make a map with that!!
	public:
		int			port;
		int			body_size_limit;
		std::string	host;
		std::string	default_file;
		std::string	cgi_extention;
		std::map<int, std::string>		error_pages;
		std::map<std::string, location>	locations;
};

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

request::request(int fd)
{
	this->http_version = "1.1";
	this->uri = "/minecraft.jpg";
	this->method = "GET";

}
request::~request(){}

class   response{
	public:

		// con/destructor
		response(request &rq);
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
		std::map<std::string, std::string>	mime_types_list;
		void		set_status_codes_list();
		void		set_mime_types_list();
		void		do_get();
		void		do_post();
		void		do_delete();
		void		get_file(std::string &path);
		std::string	get_mimeType(std::string &path);
		std::string print_status_line();
		std::string print_headers();
		request		*request_form;
};

response::response(request &req)
{
	this->set_status_codes_list();
	this->set_mime_types_list();
	this->http_version = "1.1";
	this->request_form = &req;
	this->headers["Server"] = "42 Samusanc/Daviles simple webserver";
	this->headers["Date"] = get_actual_date();
	if (req.method == "GET")
		this->do_get();
	else if (req.method == "POST")
		this->do_post();
	else if (req.method == "DELETE")
		this->do_delete();
	else
	{
		this->status_code = 404;
		this->body = "a";
		this->headers["Content-Length"] = "0";
	}
}

response::~response(){}

std::string	response::get_mimeType(std::string &path)
{
	size_t	dotPos = path.find_last_of('.');

    if (dotPos != std::string::npos) {
        std::string extension = path.substr(dotPos);
        if (this->mime_types_list.count(extension)) {
            return this->mime_types_list[extension];
        }
    }
	return ("application/octet-stream");
}

void	response::get_file(std::string &path)
{
	std::ifstream	file(path.c_str());
	std::stringstream	buff;
	std::stringstream	length;

	if (file.is_open())
	{
		buff << file.rdbuf();
		file.close();
		this->body = buff.str();
		this->status_code = 200;
		this->headers["Content-type"] = this->get_mimeType(path);
		length << this->body.length();
		this->headers["Content-Length"] = length.str();
	}
	else
	{
		this->status_code = 404;

	}

}

void	response::do_get()
{
	std::string	path;
	struct stat pathStat;

	path = "." + this->request_form->uri;
	if (!request_form)
		return ;
	std::cout << path << std::endl;
	if (stat(path.c_str(), &pathStat) == 0)
	{
		if (S_ISREG(pathStat.st_mode))
		{
			// regular file
			this->get_file(path);
		}
		else if (S_ISDIR(pathStat.st_mode))
		{
			// directory

		}
		else
		{
			// maybe 400?

		}
	}
	else
	{
		// not found

	}
}

void	response::do_post()
{
	if (!request_form)
		return ;
}

void	response::do_delete()
{
	if (!request_form)
		return ;
}

void	response::set_mime_types_list()
{
    this->mime_types_list[".html"] = "text/html";
    this->mime_types_list[".htm"] = "text/html";
    this->mime_types_list[".css"] = "text/css";
    this->mime_types_list[".js"] = "application/javascript";
    this->mime_types_list[".json"] = "application/json";
    this->mime_types_list[".xml"] = "application/xml";
    this->mime_types_list[".txt"] = "text/plain";
    this->mime_types_list[".csv"] = "text/csv";
    this->mime_types_list[".jpg"] = "image/jpeg";
    this->mime_types_list[".jpeg"] = "image/jpeg";
    this->mime_types_list[".png"] = "image/png";
    this->mime_types_list[".gif"] = "image/gif";
    this->mime_types_list[".bmp"] = "image/bmp";
    this->mime_types_list[".webp"] = "image/webp";
    this->mime_types_list[".svg"] = "image/svg+xml";
    this->mime_types_list[".ico"] = "image/vnd.microsoft.icon";
    this->mime_types_list[".woff"] = "font/woff";
    this->mime_types_list[".woff2"] = "font/woff2";
    this->mime_types_list[".ttf"] = "font/ttf";
    this->mime_types_list[".otf"] = "font/otf";
    this->mime_types_list[".eot"] = "application/vnd.ms-fontobject";
    this->mime_types_list[".vtt"] = "text/vtt";
    this->mime_types_list[".mp4"] = "video/mp4";
    this->mime_types_list[".mpeg"] = "video/mpeg";
    this->mime_types_list[".webm"] = "video/webm";
    this->mime_types_list[".avi"] = "video/x-msvideo";
    this->mime_types_list[".mp3"] = "audio/mpeg";
    this->mime_types_list[".wav"] = "audio/wav";
    this->mime_types_list[".ogg"] = "audio/ogg";
    this->mime_types_list[".pdf"] = "application/pdf";
    this->mime_types_list[".zip"] = "application/zip";
    this->mime_types_list[".tar"] = "application/x-tar";
    this->mime_types_list[".gz"] = "application/gzip";
    this->mime_types_list[".7z"] = "application/x-7z-compressed";
    this->mime_types_list[".rar"] = "application/vnd.rar";
    this->mime_types_list[".doc"] = "application/msword";
    this->mime_types_list[".docx"] = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
    this->mime_types_list[".xls"] = "application/vnd.ms-excel";
    this->mime_types_list[".xlsx"] = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
    this->mime_types_list[".ppt"] = "application/vnd.ms-powerpoint";
    this->mime_types_list[".pptx"] = "application/vnd.openxmlformats-officedocument.presentationml.presentation";
}

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


int	main()
{
	try
	{
		int	fd;

		fd = open("request.txt", O_RDONLY);
		if (fd < 0)
			return (-1);
		request		req = request(fd);
		response	respuesta = response(req);
		std::cout << respuesta.str() << std::endl;
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
}
