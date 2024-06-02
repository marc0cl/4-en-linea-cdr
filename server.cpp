#include "boardgame.h"
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

using namespace std;

void *juego_wrapper(void *arg) {
    pair<int, struct sockaddr_in> *datos = (pair<int, struct sockaddr_in> *)arg;
    int socket_cliente = datos->first;
    struct sockaddr_in direccionCliente = datos->second;
    MiTablero juego;
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
