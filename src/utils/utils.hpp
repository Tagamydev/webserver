#pragma once
#ifndef	UTILS_HPP
# define UTILS_HPP
# include "main.hpp"

class	utils
{
	public:
		~utils();
		static struct pollfd pollfd_from_fd(int fd, short events);
	private:
		utils();

};

#endif
