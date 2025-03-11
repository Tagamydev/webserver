#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define PORT 8080

int main() {
    int server_fd;
    struct sockaddr_in address;
    int opt = 1;

    // 1. Crear socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Error al crear el socket");
        exit(EXIT_FAILURE);
    }
    std::cout << "Socket creado con éxito.\n";

    // 2. Configurar opciones del socket
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("Error en setsockopt");
        exit(EXIT_FAILURE);
    }

    // 3. Configurar la dirección del servidor
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // 4. Vincular el socket al puerto
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("Error al vincular el socket");
        exit(EXIT_FAILURE);
    }
    std::cout << "Socket vinculado al puerto " << PORT << " con éxito.\n";

    // 5. Poner el socket en modo "escuchar"
    if (listen(server_fd, 3) < 0) {
        perror("Error al poner el socket en modo escuchar");
        exit(EXIT_FAILURE);
    }
    std::cout << "Socket en modo escuchar. Esperando conexiones...\n";

    // Mantén el programa corriendo para simular que el servidor está activo
    while (true) {
        sleep(1);
    }

    close(server_fd);
    return 0;
}
