/*
Ejercicio 2: Aceptar conexiones y leer datos
Meta: Aceptar una conexión y leer datos enviados por un cliente.

Enunciado:

Usa el socket creado en el Ejercicio 1.
Acepta una conexión entrante.
Lee un mensaje enviado por el cliente y muéstralo en consola.
Cierra la conexión.
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

    // Cerrar la conexión
    close(new_socket);
    close(server_fd);

    return 0;
}


