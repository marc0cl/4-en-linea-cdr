#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <vector>

using namespace std;

const int filas = 6;
const int columnas = 7;

class MiTablero {
private:
    char tablero[filas][columnas];

public:
    MiTablero() {
        reiniciar();
    }

    void reiniciar() {
        for (int i = 0; i < filas; i++) {
            for (int j = 0; j < columnas; j++) {
                tablero[i][j] = ' ';
            }
        }
    }

    void mostrar(int socket_cliente) {
        string str_tablero = "TABLERO\n";
        for (int i = 0; i < filas; i++) {
            str_tablero += to_string(filas - i) + "|";
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

    bool revisarGanador(char ficha) {
        for (int i = 0; i < filas; i++) {
            for (int j = 0; j <= columnas - 4; j++) {
                if (tablero[i][j] == ficha && tablero[i][j + 1] == ficha && tablero[i][j + 2] == ficha && tablero[i][j + 3] == ficha) {
                    return true;
                }
            }
        }
        for (int i = 0; i <= filas - 4; i++) {
            for (int j = 0; j < columnas; j++) {
                if (tablero[i][j] == ficha && tablero[i + 1][j] == ficha && tablero[i + 2][j] == ficha && tablero[i + 3][j] == ficha) {
                    return true;
                }
            }
        }
        for (int i = 0; i <= filas - 4; i++) {
            for (int j = 0; j <= columnas - 4; j++) {
                if (tablero[i][j] == ficha && tablero[i + 1][j + 1] == ficha && tablero[i + 2][j + 2] == ficha && tablero[i + 3][j + 3] == ficha) {
                    return true;
                }
            }
        }
        for (int i = 0; i <= filas - 4; i++) {
            for (int j = 3; j < columnas; j++) {
                if (tablero[i][j] == ficha && tablero[i + 1][j - 1] == ficha && tablero[i + 2][j - 2] == ficha && tablero[i + 3][j - 3] == ficha) {
                    return true;
                }
            }
        }
        return false;
    }

    bool tableroCompleto() {
        for (int i = 0; i < filas; ++i) {
            for (int j = 0; j < columnas; ++j) {
                if (tablero[i][j] == ' ') {
                    return false;
                }
            }
        }
        return true;
    }

    bool ponerFicha(int col, char ficha) {
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

class MiJuego {
private:
    MiTablero tablero;

public:
    void iniciarJuego(int socket_cliente, struct sockaddr_in direccionCliente) {
        srand(time(NULL));
        char buffer[1024];
        memset(buffer, 0, sizeof(buffer));
        int n_bytes = 0;
        char ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(direccionCliente.sin_addr), ip, INET_ADDRSTRLEN);
        cout << "Nuevo juego [" << ip << ":" << ntohs(direccionCliente.sin_port) << "]" << endl;
        tablero.reiniciar();
        char turno = (rand() % 2 == 0) ? 'S' : 'C';
        string mensaje = "Ingrese un numero para iniciar el juego";
        send(socket_cliente, mensaje.c_str(), mensaje.size(), 0);
        n_bytes = recv(socket_cliente, buffer, 1024, 0);
        tablero.mostrar(socket_cliente);

        if (turno == 'C') {
            cout << "Juego [" << ip << ":" << ntohs(direccionCliente.sin_port) << "]: empieza el cliente" << endl;
        } else {
            cout << "Juego [" << ip << ":" << ntohs(direccionCliente.sin_port) << "]: empieza el servidor" << endl;
        }

        if (n_bytes <= 0) {
            cout << "Error en la conexión, saliendo del juego." << endl;
        } else {
            while (true) {
                int columna;
                buffer[n_bytes] = '\0';
                if (turno == 'C') {
                    mensaje = "Es tu turno, ingrese columna. (1-7) ";
                    send(socket_cliente, mensaje.c_str(), mensaje.size(), 0);
                    n_bytes = recv(socket_cliente, buffer, 1024, 0);
                    buffer[n_bytes] = '\0';
                    if (strncmp(buffer, "salir", 5) == 0) {
                        cout << "Juego [" << ip << ":" << ntohs(direccionCliente.sin_port) << "] ha salido del juego." << endl;
                        break;
                    }
                    columna = atoi(&buffer[0]);
                    while (tablero.columnaLlena(columna)) {
                        mensaje = "Columna llena, elija otra: ";
                        send(socket_cliente, mensaje.c_str(), mensaje.size(), 0);
                        n_bytes = recv(socket_cliente, buffer, 1024, 0);
                        if (strncmp(buffer, "salir", 5) == 0) {
                            cout << "Juego [" << ip << ":" << ntohs(direccionCliente.sin_port) << "] ha salido del juego." << endl;
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
                            cout << "Juego [" << ip << ":" << ntohs(direccionCliente.sin_port) << "] ha salido del juego." << endl;
                            break;
                        }
                        buffer[n_bytes] = '\0';
                        columna = atoi(&buffer[0]);
                    }
                    tablero.ponerFicha(columna, 'C');
                    cout << "Juego [" << ip << ":" << ntohs(direccionCliente.sin_port) << "]: Cliente juega columna " << columna << endl;
                    if (tablero.revisarGanador('C')) {
                        tablero.mostrar(socket_cliente);
                        mensaje = "Ganador Cliente, felicidades\nFin del Juego.\n";
                        send(socket_cliente, mensaje.c_str(), mensaje.size(), 0);
                        cout << "Juego [" << ip << ":" << ntohs(direccionCliente.sin_port) << "]: gana cliente." << endl;
                        break;
                    }
                    if (tablero.tableroCompleto()) {
                        tablero.mostrar(socket_cliente);
                        mensaje = "Tablero lleno, es un empate.";
                        send(socket_cliente, mensaje.c_str(), mensaje.size(), 0);
                        cout << "Juego [" << ip << ":" << ntohs(direccionCliente.sin_port) << "]: empate." << endl;
                        break;
                    }
                    mensaje = "\nTurno del Servidor...\n";
                    send(socket_cliente, mensaje.c_str(), mensaje.size(), 0);
                    ponerFichaServidor();
                    if (tablero.revisarGanador('S')) {
                        tablero.mostrar(socket_cliente);
                        mensaje = "Gana el Servidor...\nFin del juego.\n";
                        send(socket_cliente, mensaje.c_str(), mensaje.size(), 0);
                        cout << "Juego [" << ip << ":" << ntohs(direccionCliente.sin_port) << "]: gana servidor." << endl;
                        break;
                    }
                    tablero.mostrar(socket_cliente);
                } else if (turno == 'S') {
                    mensaje = "Juega el servidor\nEspera tu turno\n";
                    send(socket_cliente, mensaje.c_str(), mensaje.size(), 0);
                    ponerFichaServidor();
                    tablero.mostrar(socket_cliente);
                    mensaje = "Es tu turno, ingrese columna. (1-7): ";
                    send(socket_cliente, mensaje.c_str(), mensaje.size(), 0);
                    n_bytes = recv(socket_cliente, buffer, 1024, 0);
                    if (strncmp(buffer, "salir", 5) == 0) {
                        cout << "Juego [" << ip << ":" << ntohs(direccionCliente.sin_port) << "] ha salido del juego." << endl;
                        break;
                    }
                    buffer[n_bytes] = '\0';
                    columna = atoi(&buffer[0]);
                    while (tablero.columnaLlena(columna)) {
                        mensaje = "Columna llena, elija otra: ";
                        send(socket_cliente, mensaje.c_str(), mensaje.size(), 0);
                        n_bytes = recv(socket_cliente, buffer, 1024, 0);
                        if (strncmp(buffer, "salir", 5) == 0) {
                            cout << "Juego [" << ip << ":" << ntohs(direccionCliente.sin_port) << "] ha salido del juego." << endl;
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
                            cout << "Juego [" << ip << ":" << ntohs(direccionCliente.sin_port) << "] ha salido del juego." << endl;
                            break;
                        }
                        buffer[n_bytes] = '\0';
                        columna = atoi(&buffer[0]);
                    }
                    tablero.ponerFicha(columna, 'C');
                    cout << "Juego [" << ip << ":" << ntohs(direccionCliente.sin_port) << "]: Cliente juega columna " << columna << endl;
                    if (tablero.revisarGanador('C')) {
                        tablero.mostrar(socket_cliente);
                        mensaje = "Ganador Cliente, felicidades\nFin del Juego.\n";
                        send(socket_cliente, mensaje.c_str(), mensaje.size(), 0);
                        cout << "Juego [" << ip << ":" << ntohs(direccionCliente.sin_port) << "]: gana cliente." << endl;
                        break;
                    }
                    if (tablero.tableroCompleto()) {
                        tablero.mostrar(socket_cliente);
                        mensaje = "Tablero lleno, es un empate.";
                        send(socket_cliente, mensaje.c_str(), mensaje.size(), 0);
                        cout << "Juego [" << ip << ":" << ntohs(direccionCliente.sin_port) << "]: empate." << endl;
                        break;
                    }
                    if (tablero.revisarGanador('S')) {
                        tablero.mostrar(socket_cliente);
                        mensaje = "Gana el Servidor...\nFin del juego.\n";
                        send(socket_cliente, mensaje.c_str(), mensaje.size(), 0);
                        cout << "Juego [" << ip << ":" << ntohs(direccionCliente.sin_port) << "]: gana servidor." << endl;
                        break;
                    }
                    tablero.mostrar(socket_cliente);
                }
            }
        }
    }

private:
    void ponerFichaServidor() {
        srand(time(NULL));
        bool puesto = false;
        while (!puesto) {
            int col = rand() % columnas;
            if (tablero.ponerFicha(col + 1, 'S')) {
                puesto = true;
            }
        }
    }
};

void *juego_wrapper(void *arg) {
    pair<int, struct sockaddr_in> *datos = (pair<int, struct sockaddr_in> *)arg;
    int socket_cliente = datos->first;
    struct sockaddr_in direccionCliente = datos->second;
    MiJuego juego;
    juego.iniciarJuego(socket_cliente, direccionCliente);
    close(socket_cliente);
    delete datos;
    pthread_exit(NULL);
}

int main(int argc, char **argv) {
    if (argc != 2) {
        cerr << "Uso: " << argv[0] << " <puerto>" << endl;
        return 1;
    }

    int port = atoi(argv[1]);
    int socket_server = 0;
    struct sockaddr_in direccionServidor, direccionCliente;

    if ((socket_server = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        cerr << "No se pudo crear el socket." << endl;
        return 1;
    }

    memset(&direccionServidor, 0, sizeof(direccionServidor));
    direccionServidor.sin_family = AF_INET;
    direccionServidor.sin_addr.s_addr = htonl(INADDR_ANY);
    direccionServidor.sin_port = htons(port);

    if (bind(socket_server, (struct sockaddr *)&direccionServidor, sizeof(direccionServidor)) < 0) {
        cerr << "Error en bind()." << endl;
        return 1;
    }

    if (listen(socket_server, 1024) < 0) {
        cerr << "Error en listen()." << endl;
        return 1;
    }

    socklen_t addr_size;
    addr_size = sizeof(struct sockaddr_in);

    cout << "Esperando conexiones..." << endl;
    while (true) {
        int socket_cliente;

        if ((socket_cliente = accept(socket_server, (struct sockaddr *)&direccionCliente, &addr_size)) < 0) {
            cerr << "Error en accept()." << endl;
            continue;
        }

        pthread_t hilo;
        pair<int, struct sockaddr_in> *datos = new pair<int, struct sockaddr_in>(socket_cliente, direccionCliente);
        if (pthread_create(&hilo, NULL, juego_wrapper, (void *)datos) != 0) {
            cerr << "Error creando hilo." << endl;
            delete datos;
            close(socket_cliente);
        } else {
            pthread_detach(hilo);
        }
    }

    return 0;
}
