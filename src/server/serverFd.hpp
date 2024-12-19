#pragma once
#ifndef	SERVERFD_HPP
# define SERVERFD_HPP
# include "main.hpp"

class	serverFd{
	public:
		serverFd(int port);
		~serverFd();

		int	_port;
		int	_fd;
};

#endif
