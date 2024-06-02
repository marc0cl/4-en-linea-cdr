#ifndef BOARDGAME_H
#define BOARDGAME_H

#include "board.h"
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <vector>

class MiTablero {
private:
    Tablero tablero;

public:
    void iniciarJuego(int socket_cliente, struct sockaddr_in direccionCliente) {
        srand(time(NULL));
        char buffer[1024];
        memset(buffer, 0, sizeof(buffer));
        int n_bytes = 0;
        char ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(direccionCliente.sin_addr), ip, INET_ADDRSTRLEN);
        std::cout << "Nuevo juego [" << ip << ":" << ntohs(direccionCliente.sin_port) << "]" << std::endl;
        tablero.reiniciar(); // Reiniciamos el tablero
        char turno = (rand() % 2 == 0) ? 'S' : 'C'; // Vemos aleatoriamente quién empieza
        std::string mensaje = "Ingrese un numero para iniciar el juego";
        send(socket_cliente, mensaje.c_str(), mensaje.size(), 0);
        n_bytes = recv(socket_cliente, buffer, 1024, 0);
        tablero.imprimirTablero(socket_cliente); // Mostramos el tablero

        // Decimos quién empieza el juego
        if (turno == 'C') {
            std::cout << "Juego [" << ip << ":" << ntohs(direccionCliente.sin_port) << "]: empieza el cliente" << std::endl;
        } else {
            std::cout << "Juego [" << ip << ":" << ntohs(direccionCliente.sin_port) << "]: empieza el servidor" << std::endl;
        }

        // Si hay un error en la conexión
        if (n_bytes <= 0) {
            std::cout << "Error en la conexión, saliendo del juego." << std::endl;
        } else {
            // Bucle principal del juego
            while (true) {
                int columna;
                buffer[n_bytes] = '\0';
                if (turno == 'C') {
                    // Turno del cliente
                    mensaje = "Es tu turno, ingrese columna. (1-7) ";
                    send(socket_cliente, mensaje.c_str(), mensaje.size(), 0);
                    n_bytes = recv(socket_cliente, buffer, 1024, 0);
                    buffer[n_bytes] = '\0';
                    if (strncmp(buffer, "salir", 5) == 0) {
                        std::cout << "Juego [" << ip << ":" << ntohs(direccionCliente.sin_port) << "] ha salido del juego." << std::endl;
                        break;
                    }
                    columna = atoi(&buffer[0]);
                    while (tablero.columnaLlena(columna)) {
                        mensaje = "Columna llena, elija otra: ";
                        send(socket_cliente, mensaje.c_str(), mensaje.size(), 0);
                        n_bytes = recv(socket_cliente, buffer, 1024, 0);
                        if (strncmp(buffer, "salir", 5) == 0) {
                            std::cout << "Juego [" << ip << ":" << ntohs(direccionCliente.sin_port) << "] ha salido del juego." << std::endl;
                            break;
                        }
                        buffer[n_bytes] = '\0';
                        columna = atoi(&buffer[0]);
                    }
                    while (columna < 1 || columna > 7) {
                        mensaje = "Columna inválida, vuelva a ingresar: ";
                        send(socket_cliente, mensaje.c_str(), mensaje.size(), 0);
                        n_bytes = recv(socket_cliente, buffer, 1024, 0);
                        if (strncmp(buffer, "salir", 5) == 0) {
                            std::cout << "Juego [" << ip << ":" << ntohs(direccionCliente.sin_port) << "] ha salido del juego." << std::endl;
                            break;
                        }
                        buffer[n_bytes] = '\0';
                        columna = atoi(&buffer[0]);
                    }
                    tablero.colocarFicha(columna, 'C');
                    std::cout << "Juego [" << ip << ":" << ntohs(direccionCliente.sin_port) << "]: Cliente juega columna " << columna << std::endl;
                    if (tablero.comprobarGanador('C')) {
                        tablero.imprimirTablero(socket_cliente);
                        mensaje = "Ganador Cliente, felicidades\nFin del Juego.\n";
                        send(socket_cliente, mensaje.c_str(), mensaje.size(), 0);
                        std::cout << "Juego [" << ip << ":" << ntohs(direccionCliente.sin_port) << "]: gana cliente." << std::endl;
                        break;
                    }
                    if (tablero.estaLleno()) {
                        tablero.imprimirTablero(socket_cliente);
                        mensaje = "Tablero lleno, es un empate.";
                        send(socket_cliente, mensaje.c_str(), mensaje.size(), 0);
                        std::cout << "Juego [" << ip << ":" << ntohs(direccionCliente.sin_port) << "]: empate." << std::endl;
                        break;
                    }
                    mensaje = "\nTurno del Servidor...\n";
                    send(socket_cliente, mensaje.c_str(), mensaje.size(), 0);
                    colocarFichaServidor();
                    if (tablero.comprobarGanador('S')) {
                        tablero.imprimirTablero(socket_cliente);
                        mensaje = "Gana el Servidor...\nFin del juego.\n";
                        send(socket_cliente, mensaje.c_str(), mensaje.size(), 0);
                        std::cout << "Juego [" << ip << ":" << ntohs(direccionCliente.sin_port) << "]: gana servidor." << std::endl;
                        break;
                    }
                    tablero.imprimirTablero(socket_cliente);
                } else if (turno == 'S') {
                    // Turno del servidor
                    mensaje = "Juega el servidor\nEspera tu turno\n";
                    send(socket_cliente, mensaje.c_str(), mensaje.size(), 0);
                    colocarFichaServidor();
                    tablero.imprimirTablero(socket_cliente);
                    mensaje = "Es tu turno, ingrese columna. (1-7): ";
                    send(socket_cliente, mensaje.c_str(), mensaje.size(), 0);
                    n_bytes = recv(socket_cliente, buffer, 1024, 0);
                    if (strncmp(buffer, "salir", 5) == 0) {
                        std::cout << "Juego [" << ip << ":" << ntohs(direccionCliente.sin_port) << "] ha salido del juego." << std::endl;
                        break;
                    }
                    buffer[n_bytes] = '\0';
                    columna = atoi(&buffer[0]);
                    while (tablero.columnaLlena(columna)) {
                        mensaje = "Columna llena, elija otra: ";
                        send(socket_cliente, mensaje.c_str(), mensaje.size(), 0);
                        n_bytes = recv(socket_cliente, buffer, 1024, 0);
                        if (strncmp(buffer, "salir", 5) == 0) {
                            std::cout << "Juego [" << ip << ":" << ntohs(direccionCliente.sin_port) << "] ha salido del juego." << std::endl;
                            break;
                        }
                        buffer[n_bytes] = '\0';
                        columna = atoi(&buffer[0]);
                    }
                    while (columna < 1 || columna > 7) {
                        mensaje = "Columna inválida, vuelva a ingresar: ";
                        send(socket_cliente, mensaje.c_str(), mensaje.size(), 0);
                        n_bytes = recv(socket_cliente, buffer, 1024, 0);
                        if (strncmp(buffer, "salir", 5) == 0) {
                            std::cout << "Juego [" << ip << ":" << ntohs(direccionCliente.sin_port) << "] ha salido del juego." << std::endl;
                            break;
                        }
                        buffer[n_bytes] = '\0';
                        columna = atoi(&buffer[0]);
                    }
                    tablero.colocarFicha(columna, 'C');
                    std::cout << "Juego [" << ip << ":" << ntohs(direccionCliente.sin_port) << "]: Cliente juega columna " << columna << std::endl;
                    if (tablero.comprobarGanador('C')) {
                        tablero.imprimirTablero(socket_cliente);
                        mensaje = "Ganador Cliente, felicidades\nFin del Juego.\n";
                        send(socket_cliente, mensaje.c_str(), mensaje.size(), 0);
                        std::cout << "Juego [" << ip << ":" << ntohs(direccionCliente.sin_port) << "]: gana cliente." << std::endl;
                        break;
                    }
                    if (tablero.estaLleno()) {
                        tablero.imprimirTablero(socket_cliente);
                        mensaje = "Tablero lleno, es un empate.";
                        send(socket_cliente, mensaje.c_str(), mensaje.size(), 0);
                        std::cout << "Juego [" << ip << ":" << ntohs(direccionCliente.sin_port) << "]: empate." << std::endl;
                        break;
                    }
                    if (tablero.comprobarGanador('S')) {
                        tablero.imprimirTablero(socket_cliente);
                        mensaje = "Gana el Servidor...\nFin del juego.\n";
                        send(socket_cliente, mensaje.c_str(), mensaje.size(), 0);
                        std::cout << "Juego [" << ip << ":" << ntohs(direccionCliente.sin_port) << "]: gana servidor." << std::endl;
                        break;
                    }
                    tablero.imprimirTablero(socket_cliente);
                }
            }
        }
    }

private:
    // Función para colocar la ficha del servidor
    void colocarFichaServidor() {
        srand(time(NULL));
        bool puesto = false;
        while (!puesto) {
            int col = rand() % Tablero::columnas;
            if (tablero.colocarFicha(col + 1, 'S')) {
                puesto = true;
            }
        }
    }
};

#endif
