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

/*
HTTP/1.0 200 OK
Server: SimpleHTTP/0.6 Python/3.11.2
Date: Tue, 17 Sep 2024 14:31:36 GMT
Content-type: text/html
Content-Length: 171
Last-Modified: Tue, 17 Sep 2024 13:33:58 GMT

<!DOCTYPE html>

<html>

  <head>

	<title>Page Title</title>

  </head>

  <body>

	<h1>This is a Heading</h1>

	<p>This is a paragraph.</p>

  </body>

</html>

*/

class   response{
	public:
		// constructors
		response();

		std::string	content;
		std::string	http_version;
		std::string	http_status_code;
		std::string	server;
		std::string	date;
		std::string	content_type;
		std::string	content_length;
		std::string	last_modified;

		// funtions
		std::string	print_str();
		void		get_content(const std::string& path);
};

response::response()
{
	this->content= "default text";
	this->http_version= "HTTP/1.0";
	this->http_status_code= "200";
	this->server= "default text";
	this->date= "default text";
	this->content_type= "default text";
	this->content_length= "1";
	this->last_modified= "default text";
}

std::string	response::print_str()
{
	std::string		result;
	std::stringstream	buff;

	buff <<this->http_version << " " << this->http_status_code << " OK" << std::endl;
	buff << "Server: " << this->server << std::endl;
	buff << "Date: " << this->date << std::endl;
	buff << "Content-type: " << this->content_type << std::endl;
	buff << "Content-Length: " << this->content_length << std::endl;
	buff << "last_modified: " << this->last_modified << std::endl;
	buff << std::endl;
	buff << this->content << std::endl;
	result = buff.str();
	return (result);
}

/*
GET / HTTP/1.1

Host: localhost:8000

sec-ch-ua: "Not;A=Brand";v="24", "Chromium";v="128"

sec-ch-ua-mobile: ?0

sec-ch-ua-platform: "Linux"

Accept-Language: en-US,en;q=0.9

Upgrade-Insecure-Requests: 1

User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/128.0.6613.120 Safari/537.36

Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,;q=0.8,application/signed-exchange;v=b3;q=0.7

Sec-Fetch-Site: none

Sec-Fetch-Mode: navigate

Sec-Fetch-User: ?1

Sec-Fetch-Dest: document

Accept-Encoding: gzip, deflate, br

Connection: keep-alive
*/

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

std::string parse_helper(const std::string& input, std::string string_to_find)
{
	string_to_find = string_to_find + ": ";
	size_t	case_idk = input.find(string_to_find);

	if (case_idk != std::string::npos)
	{
		size_t end_idk = input.find("\n", case_idk);
		return (input.substr(case_idk + string_to_find.length() , end_idk - (case_idk + string_to_find.length())));
	}
	else
		return (std::string("default text"));
}

void request::parse(const std::string& input)
{
	// special cases
	/*
	this->method = parse_helper(input, "");
	this->path = parse_helper(input, "");
	this->htpp_version = parse_helper(input, "");
	*/

	this->host = parse_helper(input, "Host");
	this->sec_ch_ua = parse_helper(input, "sec-ch-ua");
	this->sec_ch_ua_mobile = parse_helper(input, "sec-ch-ua-mobile");
	this->sec_ch_ua_platform = parse_helper(input, "sec-ch-ua-platform");
	this->accept_language = parse_helper(input, "Accept-Language");
	this->upgrade_insecure_requests = parse_helper(input, "Upgrade-Insecure-Requests");
	this->user_agent = parse_helper(input, "User-Agent");
	this->accept = parse_helper(input, "Accept");
	this->sec_fetch_site = parse_helper(input, "Sec-Fetch-Site");
	this->sec_fetch_mode = parse_helper(input, "Sec-Fetch-Mode");
	this->sec_fetch_user = parse_helper(input, "Sec-Fetch-User");
	this->sec_fetch_dest = parse_helper(input, "Sec-Fetch-Dest");
	this->accept_encoding = parse_helper(input, "Accept-Encoding");
	this->connection = parse_helper(input, "Connection");
}

request::request(int fd)
{
	this->method = "default text";
	this->path = "default text";
	this->htpp_version = "default text";

	char	buffer[500];
	std::string	file;
	bzero(buffer, sizeof(char) * 500);
	while (read(fd, buffer, 500) > 0)
	{
		file = file + std::string(buffer);
		bzero(buffer, sizeof(char) * 500);
	}

	std::cout << "request ==========" << std::endl;
	std::cout << std::endl;
	std::cout << std::endl;
	std::cout << std::endl;
	
	this->parse(file);
	std::cout << this->host << std::endl;
	
	std::cout << this->method << std::endl;
	std::cout << this->path << std::endl;
	std::cout << this->htpp_version << std::endl;
	std::cout << this->host << std::endl;
	std::cout << this->sec_ch_ua << std::endl;
	std::cout << this->sec_ch_ua_mobile << std::endl;
	std::cout << this->sec_ch_ua_platform << std::endl;
	std::cout << this->accept_language << std::endl;
	std::cout << this->upgrade_insecure_requests << std::endl;
	std::cout << this->user_agent << std::endl;
	std::cout << this->accept << std::endl;
	std::cout << this->sec_fetch_site << std::endl;
	std::cout << this->sec_fetch_mode << std::endl;
	std::cout << this->sec_fetch_user << std::endl;
	std::cout << this->sec_fetch_dest << std::endl;
	std::cout << this->accept_encoding << std::endl;
	std::cout << this->connection << std::endl;

	close(fd);
	std::cout << std::endl;
	std::cout << std::endl;
	std::cout << std::endl;
	std::cout << "response ==========" << std::endl;
	std::cout << std::endl;
	std::cout << std::endl;

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

std::string	open_file(const std::string& path)
{
	std::string		result;
	std::stringstream	buff;
	std::ifstream		file(path.c_str());

	if (!file)
	{
		buff << "<!DOCTYPE html>\n\n<html lang=\"en\"><head>\n "
		<< "       <meta charset=\"utf-8\">\n        <title>Error response</title>\n "
		<< "   </head>\n    <body>\n        <h1>Error response</h1>\n    "
		<< "    <p>Error code: 404</p>\n        <p>Message: File not found.</p>\n "
		<< "       <p>Error code explanation: 404 - Nothing matches the given URI.</p>\n    \n\n</body></html>";
	}
	else
	{
		buff << file.rdbuf();
		file.close();
	}
	result = buff.str();
	return (result);
}

void	response::get_content(const std::string& path)
{
	std::stringstream	buff;
	
	this->content = open_file(path);
	buff << content.length();
	this->content_length = buff.str();
}

int	main()
{
	int	fd;

	fd = open("request.txt", O_RDONLY);
	if (fd < 0)
		return (-1);
	try
	{
		request		pedido = request(fd);
		response	respuesta = response();
	
		respuesta.get_content("./t.html");
		std::cout << respuesta.print_str();
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
}
