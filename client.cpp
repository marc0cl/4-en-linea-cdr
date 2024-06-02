// Importamos las librerías necesarias para el funcionamiento del programa.
#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "boardgame.h"

#define PORT 8080 // Definimos el puerto

int main(int argc, char const *argv[]) {
    // Declaramos variables para el socket del cliente y la estructura del servidor
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};

    // Intentamos crear el socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "Socket creation error" << std::endl;
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT); // Asignamos el puerto definido anteriormente

    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address/ Address not supported" << std::endl;
        return -1;
    }

    // Intentamos conectarnos al servidor
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Connection Failed" << std::endl;
        return -1;
    }

    // Enviamos un mensaje al servidor
    send(sock, "Hello from client", strlen("Hello from client"), 0);
    std::cout << "Hello message sent" << std::endl;

    // Leemos el mensaje del servidor
    valread = read(sock, buffer, 1024);
    std::cout << buffer << std::endl; // Imprimimos el mensaje recibido

    return 0;
}
