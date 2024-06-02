#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

using namespace std;

void print_board(const char* board_str) {
    // Imprimir el tablero con guiones bajos para espacios vacíos y representar las jugadas con 'S' para el servidor y 'C' para el cliente
    for(int i = 0; board_str[i] != '\0'; i++) {
        if(board_str[i] == '.')
            cout << '_'; // Guion bajo para espacios vacíos
        else
            cout << board_str[i];
    }
    cout << endl; // Agregar un salto de línea al final del tablero
}

void jugar(int socket_server) {
    char buffer[1024];
    int n_bytes = 0;

    while (true) {
        memset(buffer, '\0', sizeof(char) * 1024);
        n_bytes = recv(socket_server, buffer, 1024, 0);
        
        if (n_bytes <= 0) {
            cerr << "Error al recibir datos del servidor" << endl;
            break;
        }
        
        buffer[n_bytes] = '\0';
        if (strncmp(buffer, "WIN", 3) == 0) {
            cout << "Has ganado!" << endl;
            break;
        } else if (strncmp(buffer, "LOSE", 4) == 0) {
            cout << "Has perdido!" << endl;
            break;
        } else if (strncmp(buffer, "DRAW", 4) == 0) {
            cout << "Es un empate!" << endl;
            break;
        } else if (strncmp(buffer, "INVALID", 7) == 0) {
            cout << "Movimiento no válido. Intente nuevamente." << endl;
        } else {
            print_board(buffer);
            cout << "Ingrese la columna (1-7) o 'Q' para salir: ";
            string input;
            cin >> input;

            if (input == "Q") {
                send(socket_server, input.c_str(), input.length(), 0);
                cout << "Saliendo del juego..." << endl;
                break;
            } else if (input.length() == 1 && isdigit(input[0]) && input[0] >= '1' && input[0] <= '7') {
                string command = "C " + input;
                send(socket_server, command.c_str(), command.length(), 0);
                // Después de enviar el movimiento del cliente, recibir y mostrar el tablero actualizado
                memset(buffer, '\0', sizeof(char) * 1024);
                n_bytes = recv(socket_server, buffer, 1024, 0);
                if (n_bytes > 0) {
                    buffer[n_bytes] = '\0';
                    print_board(buffer);
                }
            } else {
                cout << "Entrada no válida. Intente nuevamente." << endl;
            }
        }
    }
    close(socket_server);
}

int main(int argc, char **argv) {
    if (argc != 3) {
        cout << "Uso: " << argv[0] << " <direccion IP> <puerto>" << endl;
        return 1;
    }

    char *ip = argv[1];
    int port = atoi(argv[2]);
    int socket_server;
    struct sockaddr_in direccionServidor;

    cout << "Creating socket ...\n";
    if ((socket_server = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        cout << "Error creating socket\n";
        exit(EXIT_FAILURE);
    }

    cout << "Configuring socket address structure ...\n";
    memset(&direccionServidor, 0, sizeof(direccionServidor));
    direccionServidor.sin_family = AF_INET;
    direccionServidor.sin_port = htons(port);

    if (inet_pton(AF_INET, ip, &direccionServidor.sin_addr) <= 0) {
        cout << "Invalid address/ Address not supported\n";
        return -1;
    }

    cout << "Connecting to the server ...\n";
    if (connect(socket_server, (struct sockaddr *)&direccionServidor, sizeof(direccionServidor)) < 0) {
        cout << "Connection failed\n";
        return -1;
    }

    jugar(socket_server);

    close(socket_server);
    return 0;
}
