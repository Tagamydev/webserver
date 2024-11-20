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



int	main()
{
	try
	{
		int	fd;

		fd = open("request.txt", O_RDONLY);
		if (fd < 0)
			return (-1);
		request		req = request(fd);
		
        //David tests
        std::fstream    reqfile;
        std::string line;

        reqfile.open("request.txt");
        if (!reqfile.is_open())
            return(-1);
        getline(reqfile, line);
        req.check_startline(line);
        
		// response	respuesta = response(req);
		// std::cout << respuesta.str() << std::endl;
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
}
