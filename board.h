#ifndef BOARD_H
#define BOARD_H

#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>

class Tablero {
public:
    static const int filas = 6; // Número de filas del tablero
    static const int columnas = 7; // Número de columnas del tablero
    char tablero[filas][columnas]; // Matriz que representa el tablero

    // Constructor de la clase Tablero
    Tablero() {
        reiniciar(); // Inicializamos el tablero
    }

    void reiniciar() {
        for (int i = 0; i < filas; i++) {
            for (int j = 0; j < columnas; j++) {
                tablero[i][j] = ' ';
            }
        }
    }

    void imprimirTablero(int socket_cliente) {
        std::string str_tablero = "TABLERO\n";
        for (int i = 0; i < filas; i++) {
            str_tablero += std::to_string(filas - i) + "|";
            for (int j = 0; j < columnas; j++) {
                str_tablero += tablero[i][j];
                str_tablero += " ";
            }
            str_tablero += "\n";
        }
        str_tablero += "  -------------\n";
        str_tablero += "  1 2 3 4 5 6 7\n";
        send(socket_cliente, str_tablero.c_str(), str_tablero.size(), 0);
    }

    // Función para comprobar si hay un ganador
    bool comprobarGanador(char ficha) {
        // Comprobamos filas
        for (int i = 0; i < filas; i++) {
            for (int j = 0; j <= columnas - 4; j++) {
                if (tablero[i][j] == ficha && tablero[i][j + 1] == ficha && tablero[i][j + 2] == ficha && tablero[i][j + 3] == ficha) {
                    return true;
                }
            }
        }
        // Comprobamos columnas
        for (int i = 0; i <= filas - 4; i++) {
            for (int j = 0; j < columnas; j++) {
                if (tablero[i][j] == ficha && tablero[i + 1][j] == ficha && tablero[i + 2][j] == ficha && tablero[i + 3][j] == ficha) {
                    return true;
                }
            }
        }
        // Comprobamos diagonales descendentes
        for (int i = 0; i <= filas - 4; i++) {
            for (int j = 0; j <= columnas - 4; j++) {
                if (tablero[i][j] == ficha && tablero[i + 1][j + 1] == ficha && tablero[i + 2][j + 2] == ficha && tablero[i + 3][j + 3] == ficha) {
                    return true;
                }
            }
        }
        // Comprobamos diagonales ascendentes
        for (int i = 0; i <= filas - 4; i++) {
            for (int j = 3; j < columnas; j++) {
                if (tablero[i][j] == ficha && tablero[i + 1][j - 1] == ficha && tablero[i + 2][j - 2] == ficha && tablero[i + 3][j - 3] == ficha) {
                    return true;
                }
            }
        }
        return false;
    }

    bool estaLleno() {
        for (int i = 0; i < filas; ++i) {
            for (int j = 0; j < columnas; ++j) {
                if (tablero[i][j] == ' ') {
                    return false;
                }
            }
        }
        return true;
    }

    bool colocarFicha(int col, char ficha) {
        for (int i = filas - 1; i >= 0; i--) {
            if (tablero[i][col - 1] == ' ') {
                tablero[i][col - 1] = ficha;
                return true;
            }
        }
        return false;
    }

    bool columnaLlena(int col) {
        for (int i = 0; i < filas; ++i) {
            if (tablero[i][col - 1] == ' ') {
                return false;
            }
        }
        return true;
    }
};

#endif