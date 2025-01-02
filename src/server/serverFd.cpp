#include "serverFd.hpp"

serverFd::serverFd(int port)
{
	struct sockaddr_in address;
	int addrlen;
	int server_fd;
	int opt;

    this->_port = port;
    addrlen = sizeof(address);
    opt = 1;

	// make socket
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
		throw (std::runtime_error("Error: Fatal socket allocation fail!"));

	// setup socket option to be reusable after the end of the program
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
		throw (std::runtime_error("Error: Fatal set sock opt fail!"));

	// config address and port
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(port);

	// bind socket
	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
		throw (std::runtime_error("Error: Fatal bind fail!"));

	// start socket listeing, 3 is the number of places avalibles in the wait list
	if (listen(server_fd, 3) < 0)
		throw (std::runtime_error("Error: Fatal listen fail!"));

    this->_fd = server_fd;
}

serverFd::~serverFd()
{
    this->_port = 0;
}