#include "utils.hpp"

struct pollfd utils::pollfd_from_fd(int fd, short events)
{
	struct pollfd tmp;

	memset(&tmp, 0, sizeof(tmp));
	tmp.fd = fd;
	tmp.events = events;
	return (tmp);
}

void utils::send_response(int socket_fd, const std::string &response_str)
{
	size_t response_length = response_str.length();

	send(socket_fd, response_str.c_str(), response_length, 0);
}

std::string	utils::read_file(int fd)
{
    const std::size_t bufferSize = MAX_BUFFER_LENGTH;
    char buffer[bufferSize];
    std::string result;

    ssize_t bytesRead;

	bytesRead = recv(fd, buffer, bufferSize, 0);
	std::cout << "[Log]: " << "fd value: " << fd << std::endl;
    if (bytesRead == -1)
		throw std::runtime_error("Error reading file descriptor.");
	result.append(buffer, bytesRead);
	return (result);
}
