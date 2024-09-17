/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: samusanc <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/11 08:36:52 by samusanc          #+#    #+#             */
/*   Updated: 2024/06/04 21:23:30 by samusanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>
#include <iostream>

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
    std::string server;
    std::string date;
    std::string content_type;
    size_t      content_length;
    std::string last_modified;
    response();
    std::string set_actual_date();
}

std::string response::set_actual_date()
{
  time_t        now;
  time_t        *lock;
  std::string   result;

  lock = &now;
  now = time(lock);
  if (lock)
    result = std::string(ctime(lock));
  else
    result = std::string("error in getting time!!!");
}

response::respose()
{
  this->server = "default server";
  this->set_actual_date();
  this->content_type = "default content type";

}

int	main()
{
  response  respuesta = response();

  respuesta.server = string("SimpleHTTP/0.6 Python/3.11.2");
  std::cout << respuesta.server << std::endl;
  std::cout << respuesta.date << std::endl;
  std::cout << respuesta.content_type << std::endl;
  std::cout << respuesta.content_length << std::endl;
  std::cout << respuesta.get_actual_date() << std::endl;
}
