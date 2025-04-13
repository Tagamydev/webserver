/*
Ejercicio 3: Responder al cliente
Meta: Responder con un mensaje predeterminado al cliente.

Enunciado:
Modifica el programa del Ejercicio 2 para que, después de leer los datos del cliente, le envíe una respuesta fija:
"HTTP/1.1 200 OK\r\nContent-Length: 13\r\n\r\nHello, World!".
*/

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define PORT 8080
#define BUFFER_SIZE 1024

const char *RESPONSE = 
    "HTTP/1.1 200 OK\r\n"
    "Content-Length: 13\r\n"
    "\r\n"
    "Hello, World!";

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};

    // Crear y configurar el socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    bind(server_fd, (struct sockaddr*)&address, sizeof(address));
    listen(server_fd, 3);

    std::cout << "Esperando conexión...\n";

    // Aceptar conexión
    if ((new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {
        perror("Error en accept");
        exit(EXIT_FAILURE);
    }

    std::cout << "Conexión aceptada.\n";

    // Leer datos del cliente
    int valread = read(new_socket, buffer, BUFFER_SIZE);
    if (valread > 0) {
        std::cout << "Mensaje recibido: " << buffer << "\n";
    }

    // Enviar respuesta al cliente
	if (write(new_socket, RESPONSE, strlen(RESPONSE))) 
	{
		std::cerr << "[FATAL]: failed to write to CGI." << std::endl;
		throw std::runtime_error("Error writing to CGI pipe.");
	}
    // Cerrar la conexión
    close(new_socket);
    close(server_fd);

    return 0;
}

