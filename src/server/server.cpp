#include "server.hpp"

server::server()
{

}

server::~server()
{

}

/*
returns of stat()
S_ISREG(m)  is it a regular file?
S_ISDIR(m)  directory?
S_ISCHR(m)  character device?
S_ISBLK(m)  block device?
S_ISFIFO(m) FIFO (named pipe)?
S_ISLNK(m)  symbolic link?  (Not in POSIX.1-1996.)
S_ISSOCK(m) socket?  (Not in POSIX.1-1996.)
*/
void	server::get_file_info(std::string path)
{
	struct stat st;
	
    if (stat(path.c_str(), &st) == -1)
		throw std::runtime_error("Error reading config file.");
    else if (S_ISDIR(st.st_mode))
		throw std::runtime_error("Error reading config file. Is a directory.");
    else if (S_ISREG(st.st_mode))
    	return ;
    else
		throw std::runtime_error("Error reading config file. Not a regular file.");
}