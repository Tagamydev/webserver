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
		std::string	status_menssage;
		int			status_code;

	// HEADERS
		std::map<std::string, std::string>	headers;	
		// header, content

	// BODY
		std::string	body;
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

int	main()
{
	try
	{
		int	fd;

		fd = open("request.txt", O_RDONLY);
		if (fd < 0)
			return (-1);

		response	respuesta = response();
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
}
