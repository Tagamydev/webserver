/*

Ejercicio 4 (Final): Manejar múltiples clientes con poll
Meta: Crear un servidor que acepte múltiples conexiones simultáneas y responda con una página HTML.

Enunciado:
Usa el código que hemos desarrollado paso a paso y agrega:

Uso de poll para manejar múltiples conexiones.
Un ciclo continuo donde el servidor pueda aceptar nuevas conexiones y responder a clientes existentes.
Cerrar conexiones cuando los clientes terminen.
 */


#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <poll.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 10

const char *RESPONSE = 
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/html\r\n"
    "Content-Length: 14\r\n"
    "\r\n"
    "<html>OK</html>";

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE];

    // Crear socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket falló");
        exit(EXIT_FAILURE);
    }

    // Configurar socket para reutilización de dirección
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt falló");
        exit(EXIT_FAILURE);
    }

    // Configurar dirección y puerto
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Vincular socket
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind falló");
        exit(EXIT_FAILURE);
    }

    // Escuchar en el socket
    if (listen(server_fd, 3) < 0) {
        perror("Listen falló");
        exit(EXIT_FAILURE);
    }

    // Configuración para poll
    struct pollfd fds[MAX_CLIENTS];
    memset(fds, 0, sizeof(fds));
    fds[0].fd = server_fd;
    fds[0].events = POLLIN;

    int client_count = 1;

    std::cout << "Servidor escuchando en el puerto " << PORT << "...\n";

    while (true) {
        int poll_count = poll(fds, client_count, -1);

        if (poll_count < 0) {
            perror("Poll falló");
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < client_count; ++i) {
            if (fds[i].revents & POLLIN) {
                if (fds[i].fd == server_fd) {
                    // Nueva conexión
                    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
                        perror("Accept falló");
                        continue;
                    }

                    if (client_count < MAX_CLIENTS) {
                        fds[client_count].fd = new_socket;
                        fds[client_count].events = POLLIN;
                        ++client_count;
                        std::cout << "Nueva conexión aceptada\n";
                    } else {
                        std::cerr << "Máximo de clientes alcanzado\n";
                        close(new_socket);
                    }
                } else {
                    // Leer datos de un cliente existente
                    int valread = read(fds[i].fd, buffer, BUFFER_SIZE);
                    if (valread <= 0) {
                        // Cliente desconectado
                        std::cout << "Cliente desconectado\n";
                        close(fds[i].fd);
                        fds[i] = fds[--client_count];
                    } else {
                        // Responder con página por defecto
                        std::cout << "Solicitud recibida:\n" << buffer << "\n";
                        if (write(fds[i].fd, RESPONSE, strlen(RESPONSE)))
						{
							std::cerr << "[FATAL]: failed to write to CGI." << std::endl;
							throw std::runtime_error("Error writing to CGI pipe.");
						}
                        close(fds[i].fd);
                        fds[i] = fds[--client_count];
                    }
                }
            }
        }
    }

    return 0;
}

