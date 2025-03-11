#include <poll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <vector>
#include <iostream>

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in server_addr = {AF_INET, htons(8080), INADDR_ANY};
    bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    listen(server_fd, 10);

    std::vector<pollfd> fds;
    fds.push_back({server_fd, POLLIN, 0});

    while (true) {
        int ret = poll(fds.data(), fds.size(), -1); // Wait indefinitely

        if (ret > 0) {
            for (size_t i = 0; i < fds.size(); ++i) 
            {
                if (fds[i].revents & POLLIN) 
                {
                    if (fds[i].fd == server_fd) 
                    {
                        // New connection
                        int client_fd = accept(server_fd, nullptr, nullptr);
                        fds.push_back({client_fd, POLLIN, 0});
                        std::cout << "New client connected\n";
                    } 
                    else 
                    {
                        // Handle client request
                        char buffer[1024];
                        int bytes = read(fds[i].fd, buffer, sizeof(buffer));
                        if (bytes > 0) 
                        {
                            std::cout << "Received data: " << buffer << std::endl;
                        } 
                        else 
                        {
                            close(fds[i].fd);
                            fds.erase(fds.begin() + i);
                            std::cout << "Client disconnected\n";
                        }
                    }
                }
            }
        }
    }

    close(server_fd);
    return 0;
}
