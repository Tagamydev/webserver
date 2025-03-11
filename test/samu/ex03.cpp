/*

¡Me parece una idea genial! Introducir un Ejercicio 3.5 para familiarizarnos con poll es clave antes de llegar al ejercicio final. Este nuevo ejercicio será más sencillo y servirá como puente para entender el rol de poll en la gestión de múltiples conexiones.

Ejercicio 3.5: Introducción a poll
Meta:
Crear un programa que utilice poll para monitorear:

La actividad de entrada estándar (stdin), como cuando escribes algo en la consola.
Un socket en modo "escuchar" que acepta conexiones entrantes.
Cuando detecte actividad:

Si hay datos en stdin, los muestra en pantalla.
Si llega una conexión al socket, la acepta y muestra el mensaje "Cliente conectado".
Enunciado detallado:
Este ejercicio es una introducción práctica a poll. Necesitas:

Crear un socket que escuche en el puerto 8080 (como en el Ejercicio 1).
Usar poll para monitorear dos descriptores de archivo:
STDIN_FILENO (entrada estándar).
El descriptor del socket (para nuevas conexiones).
Si detectas actividad en STDIN_FILENO, lee lo que el usuario escribe en la consola y muéstralo en pantalla.
Si detectas actividad en el socket, acepta la conexión y muestra "Cliente conectado".
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

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    socklen_t addrlen = sizeof(address);

    // Crear socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Error al crear el socket");
        exit(EXIT_FAILURE);
    }
    std::cout << "Socket creado con éxito.\n";

    // Configurar opciones del socket
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("Error en setsockopt");
        exit(EXIT_FAILURE);
    }

    // Configurar dirección del servidor
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Vincular el socket al puerto
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("Error al vincular el socket");
        exit(EXIT_FAILURE);
    }
    std::cout << "Socket vinculado al puerto " << PORT << " con éxito.\n";

    // Poner el socket en modo "escuchar"
    if (listen(server_fd, 3) < 0) {
        perror("Error al poner el socket en modo escuchar");
        exit(EXIT_FAILURE);
    }
    std::cout << "Socket en modo escuchar. Esperando conexiones...\n";

    // Configurar `poll`
    struct pollfd fds[2];
    fds[0].fd = STDIN_FILENO; // Monitorear entrada estándar
    fds[0].events = POLLIN;   // Esperar datos listos para leer
    fds[1].fd = server_fd;    // Monitorear el socket en modo escuchar
    fds[1].events = POLLIN;   // Esperar conexiones entrantes

    while (true) {
        int poll_count = poll(fds, 2, -1); // Esperar indefinidamente hasta que ocurra algo
        if (poll_count < 0) {
            perror("Error en poll");
            exit(EXIT_FAILURE);
        }
    }
}
