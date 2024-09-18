/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: samusanc <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/11 08:36:52 by samusanc          #+#    #+#             */
/*   Updated: 2024/09/18 22:07:56 by samusanc         ###   ########.fr       */
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

/*
int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
	fd_set	readfds1;
	fd_set	readfds2;

    // Step 1: Create a socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

	bzero(&server_addr, sizeof(sockaddr));
	bzero(&client_addr, sizeof(sockaddr));
    server_addr.sin_family = AF_INET; // IPv4
    server_addr.sin_addr.s_addr = INADDR_ANY; // Bind to any available interface
	//server_addr.sin_addr.s_addr = htonl(213076433);
	int port_number;

	port_number = 8090;
    server_addr.sin_port = htons(port_number); // Port 8080

    // Step 3: Bind the socket to the IP address and port
    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Bind successful on port :%d\n", port_number);

    // Step 4: Now you can listen for connections, accept them, etc.
    listen(sockfd, 5);

	struct timeval timeout;
	timeout.tv_sec = 1;  // 5 seconds
	timeout.tv_usec = 0; // 0 microseconds
	
	int	max_fd;

	FD_ZERO(&readfds1);
	FD_ZERO(&readfds2);

	printf("sockfd:%d\n", sockfd);
	FD_SET(sockfd, &readfds1);
	max_fd = sockfd;
	char	str[100];
	int		select_num;
	int		client_fd;
	socklen_t	len;


	while (1)
	{
		readfds2 = readfds1;
		select_num = select(max_fd + 1, &readfds2, NULL, NULL, &timeout);
		if (select_num > 0)
		{
			for (int fd_i = 0; fd_i <= max_fd; fd_i++)
			{
				printf("test:%d, fd_i:%d\n",FD_ISSET(fd_i, &readfds2), fd_i);
				if (fd_i == sockfd && FD_ISSET(fd_i, &readfds2))
				{
					printf("new client!!\n");
					bzero(&client_addr, sizeof(sockaddr));
					client_fd = accept(sockfd, (struct sockaddr *)&server_addr, &len);
					FD_SET(client_fd, &readfds1);
					if (max_fd < client_fd)
						max_fd = client_fd;

				}
				else if (FD_ISSET(fd_i, &readfds2))
				{
					//recv(fd_i, str, 99, 0);
					char	*hello = "hello\n";

					recv(fd_i, hello, 5, 0);
					printf("in fd: %d: %s\n", fd_i, str);
				}

			}
		}
	}

    // Close the socket when done
    close(sockfd);
    return 0;
}
*/

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

class   request{
	public:
		// constructors
		request();

		std::string	method;
		std::string	path;
		std::string	htpp_version;
		std::string	host;
		std::string	sec_ch_ua;
		std::string	sec_ch_ua_mobile;
		std::string	sec_ch_ua_platform;
		std::string	accept_language;
		std::string	upgrade_insecure_requests;
		std::string	user_agent;
		std::string	accept;
		std::string	sec_fetch_site;
		std::string	sec_fetch_mode;
		std::string	sec_fetch_user;
		std::string	sec_fetch_dest;
		std::string	accept_encoding;
		std::string	connection;

		// funtions
		std::string	print_str();
};

request::request()
{
	this->method = "default text";
	this->path = "default text";
	this->htpp_version = "default text";
	this->host = "default text";
	this->sec_ch_ua = "default text";
	this->sec_ch_ua_mobile = "default text";
	this->sec_ch_ua_platform = "default text";
	this->accept_language = "default text";
	this->upgrade_insecure_requests = "default text";
	this->user_agent = "default text";
	this->accept = "default text";
	this->sec_fetch_site = "default text";
	this->sec_fetch_mode = "default text";
	this->sec_fetch_user = "default text";
	this->sec_fetch_dest = "default text";
	this->accept_encoding = "default text";
	this->connection = "default text";
};

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

std::string	open_file(const std::string& path, )
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
	response	respuesta = response();

	respuesta.get_content("./t.html");
	std::cout << respuesta.print_str();
}
