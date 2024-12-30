#pragma once
#ifndef	UTILS_HPP
# define UTILS_HPP
# include "main.hpp"

class	utils
{
	public:
		~utils();

		// funtions
		static struct pollfd pollfd_from_fd(int fd, short events);
		static void send_response(int socket_fd, const std::string &response_str);
		static std::string	read_file(int fd);

	private:
		utils();

};

#endif
