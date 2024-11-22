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

#include "main.hpp"



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

void send_response(int socket_fd, const std::string &response_str)
{
    size_t total_sent = 0;
    size_t response_length = response_str.length();

    while (total_sent < response_length)
    {
        ssize_t bytes_sent = write(socket_fd, response_str.c_str() + total_sent, response_length - total_sent);

        if (bytes_sent < 0)
        {
            perror("Error al enviar la respuesta");
            break;
        }

        total_sent += bytes_sent;
    }

    if (total_sent < response_length)
    {
        std::cerr << "Advertencia: No se enviaron todos los bytes de la respuesta. Enviados: "
                  << total_sent << " de " << response_length << " bytes.\n";
    }
}

int	test()
{
	int	port = 1234;
	int max_clients = 10;
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1000];

	// make socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket falló");
        exit(EXIT_FAILURE);
    }

	// setup socket option to be reusable after the end of the program
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt falló");
        exit(EXIT_FAILURE);
    }

	// config address and port
	
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
		// AF_INET every possible net in ipv4
    address.sin_port = htons(port);
		// from number to machine readable port

    // bind socket
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
	{
        perror("Bind fail");
        exit(EXIT_FAILURE);
    }

	// start socket listeing, 3 is the number of places avalibles in the wait list
    if (listen(server_fd, 3) < 0) {
        perror("Listen fail");
        exit(EXIT_FAILURE);
    }

    // Configuración para poll
    struct pollfd fds[max_clients];
    memset(fds, 0, sizeof(fds));

    fds[0].fd = server_fd;
    fds[0].events = POLLIN;// triggered in IN operation

    int client_count = 1;

    std::cout << "Server listening in the port... " << port << "...\n";

    while (true) {
        int poll_count = poll(fds, client_count, -1);

        if (poll_count < 0) {
            perror("Poll failed");
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < client_count; ++i) {
            if (fds[i].revents & POLLIN)
			{
                if (fds[i].fd == server_fd)
				{
					// new client
                    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
                        perror("Accept fail");
                        continue;
                    }

                    if (client_count < max_clients)
					{
                        fds[client_count].fd = new_socket;
                        fds[client_count].events = POLLIN;
                        ++client_count;
                        std::cout << "Nueva conexión aceptada\n";
                    }
					else
					{
                        std::cerr << "max number of clients reached\n";
                        close(new_socket);
                    }
                }
				else
				{
                    try
                    {
					    request		req = request(fds[i].fd);
    					response	respuesta = response(req);

                        //write(2, respuesta.str().c_str() + 147, respuesta.str().length() - 147);
                        send_response(fds[i].fd, respuesta.str());

                        //close(fds[i].fd);
                        fds[i] = fds[--client_count];
                    }
                    catch (std::exception &e)
                    {
                        std::cout << e.what() << std::endl;
                    }
                }
            }
        }
    }

    return 0;


}

int	main()
{
	try
	{
		test();
		/*
		int	fd;

		fd = open("request.txt", O_RDONLY);
		if (fd < 0)
			return (-1);
		request		req = request(fd);
		response	respuesta = response(req);
		std::cout << respuesta.str() << std::endl;
		*/
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
}
