#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <thread>
#include <vector>
#include <mutex>
#include "Board.h"

using namespace std;

void jugar(int socket_cliente, const string& client_ip, int client_port, mutex& board_mutex) {
    Board board;
    char player1 = Board::PLAYER1;
    char player2 = Board::PLAYER2;
    char current_player = player1;

    cout << "Juego [" << client_ip << ":" << client_port << "]: inicia juego el cliente." << endl;

    while (true) {
        // Enviar el tablero actual al cliente
        string board_str;
        {
            lock_guard<mutex> lock(board_mutex);
            board_str = board.to_string();
        }
        send(socket_cliente, board_str.c_str(), board_str.length(), 0);

        if (current_player == player1) {
            // Turno del cliente
            char buffer[1024];
            memset(buffer, '\0', sizeof(char) * 1024);
            int n_bytes = recv(socket_cliente, buffer, 1024, 0);

            if (n_bytes <= 0) {
                cerr << "Error al recibir datos del cliente." << endl;
                break;
            }

            buffer[n_bytes] = '\0';
            if (buffer[0] == 'Q') {
                cout << "El cliente se desconectó." << endl;
                break;
            } else if (buffer[0] == 'C' && buffer[1] == ' ' && isdigit(buffer[2])) {
                int col = buffer[2] - '0' - 1; // Convertir de char a índice de columna
                if (col >= 0 && col < Board::COLS) {
                    bool valid_move;
                    {
                        lock_guard<mutex> lock(board_mutex);
                        valid_move = board.make_move(col, current_player);
                    }
                    if (valid_move) {
                        cout << "Juego [" << client_ip << ":" << client_port << "]: cliente juega columna " << col + 1 << "." << endl;
                        bool win, draw;
                        {
                            lock_guard<mutex> lock(board_mutex);
                            win = board.check_winner(current_player);
                            draw = board.is_draw();
                        }
                        if (win) {
                            send(socket_cliente, "WIN\n", 4, 0);
                            cout << "Juego [" << client_ip << ":" << client_port << "]: el cliente ganó." << endl;
                            break;
                        } else if (draw) {
                            send(socket_cliente, "DRAW\n", 5, 0);
                            cout << "Juego [" << client_ip << ":" << client_port << "]: el juego terminó en empate." << endl;
                            break;
                        } else {
                            current_player = player2; // Cambiar de turno
                        }
                    } else {
                        send(socket_cliente, "INVALID\n", 8, 0);
                    }
                }
            }
        } else {
            // Turno del servidor
            int col;
            do {
                {
                    lock_guard<mutex> lock(board_mutex);
                    col = rand() % Board::COLS;
                }
                bool valid_move;
                {
                    lock_guard<mutex> lock(board_mutex);
                    valid_move = board.is_valid_move(col);
                }
                if (valid_move) {
                    break;
                }
            } while (true);

            {
                lock_guard<mutex> lock(board_mutex);
                board.make_move(col, current_player);
            }

            cout << "Juego [" << client_ip << ":" << client_port << "]: servidor juega columna " << col + 1 << "." << endl;

            bool win, draw;
            {
                lock_guard<mutex> lock(board_mutex);
                win = board.check_winner(current_player);
                draw = board.is_draw();
            }

            if (win) {
                send(socket_cliente, "LOSE\n", 5, 0);
                cout << "Juego [" << client_ip << ":" << client_port << "]: el servidor ganó." << endl;
                break;
            } else if (draw) {
                send(socket_cliente, "DRAW\n", 5, 0);
                cout << "Juego [" << client_ip << ":" << client_port << "]: el juego terminó en empate." << endl;
                break;
            } else {
                current_player = player1; // Cambiar de turno
            }
        }
    }
    close(socket_cliente);
}

int main(int argc, char **argv) {
    if (argc != 2) {
        cout << "Uso: " << argv[0] << " <puerto>" << endl;
        return 1;
    }

    srand(time(NULL));

    int port = atoi(argv[1]);
    int socket_server = 0;
    struct sockaddr_in direccionServidor, direccionCliente;

    cout << "Esperando conexiones ..." << endl;

    if ((socket_server = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        cout << "Error creando el socket de escucha" << endl;
        exit(EXIT_FAILURE);
    }

    memset(&direccionServidor, 0, sizeof(direccionServidor));
    direccionServidor.sin_family = AF_INET;
    direccionServidor.sin_addr.s_addr = htonl(INADDR_ANY);
    direccionServidor.sin_port = htons(port);

    if (::bind(socket_server, (struct sockaddr *)&direccionServidor, sizeof(direccionServidor)) < 0) {
        cout << "Error en el enlace del socket" << endl;
        exit(EXIT_FAILURE);
    }

    if (listen(socket_server, 1024) < 0) {
        cout << "Error al escuchar conexiones" << endl;
        exit(EXIT_FAILURE);
    }

    socklen_t addr_size = sizeof(struct sockaddr_in);
    vector<thread> threads;
    mutex board_mutex;

    while (true) {
        int socket_cliente;
        if ((socket_cliente = accept(socket_server, (struct sockaddr *)&direccionCliente, &addr_size)) < 0) {
            cerr << "Error al aceptar la conexión" << endl;
            continue; // Continuar esperando conexiones en caso de error
        }

        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(direccionCliente.sin_addr), client_ip, INET_ADDRSTRLEN);
        int client_port = ntohs(direccionCliente.sin_port);

        cout << "Juego nuevo[" << client_ip << ":" << client_port << "]" << endl;

        threads.emplace_back(jugar, socket_cliente, client_ip, client_port, ref(board_mutex));
    }

    for (auto &thread : threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }

    close(socket_server);
    return 0;
}
