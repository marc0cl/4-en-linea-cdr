// Incluimos el archivo de encabezado que contiene la definición de la clase MiTablero
#include "boardgame.h"  
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

using namespace std;

// Función de envoltura para iniciar el juego en un hilo
void *juego_wrapper(void *arg) {
    // Convertimos el argumento genérico a un par de datos (socket y dirección del cliente)
    pair<int, struct sockaddr_in> *datos = (pair<int, struct sockaddr_in> *)arg;
    int socket_cliente = datos->first;
    struct sockaddr_in direccionCliente = datos->second;
    // Creamos un objeto MiTablero para iniciar el juego con el cliente
    MiTablero juego;
    juego.iniciarJuego(socket_cliente, direccionCliente);
    // Cerramos el socket del cliente
    close(socket_cliente);
    // Liberamos la memoria del par de datos
    delete datos;
    // Salimos del hilo
    pthread_exit(NULL);
}

int main(int argc, char **argv) {
    // Verificamos la cantidad de argumentos proporcionados al programa
    if (argc != 2) {
        cerr << "Uso: " << argv[0] << " <puerto>" << endl;
        return 1;
    }

    // Obtenemos el número de puerto del argumento de línea de comandos
    int port = atoi(argv[1]);
    int socket_server = 0;
    struct sockaddr_in direccionServidor, direccionCliente;

    // Creamos el socket del servidor
    if ((socket_server = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        cerr << "No se pudo crear el socket." << endl;
        return 1;
    }

    // Configuramos la dirección del servidor
    memset(&direccionServidor, 0, sizeof(direccionServidor));
    direccionServidor.sin_family = AF_INET;
    direccionServidor.sin_addr.s_addr = htonl(INADDR_ANY);
    direccionServidor.sin_port = htons(port);

    // Asociamos el socket con la dirección del servidor
    if (bind(socket_server, (struct sockaddr *)&direccionServidor, sizeof(direccionServidor)) < 0) {
        cerr << "Error en bind()." << endl;
        return 1;
    }

    // Ponemos el socket en modo de escucha para aceptar conexiones entrantes
    if (listen(socket_server, 1024) < 0) {
        cerr << "Error en listen()." << endl;
        return 1;
    }

    socklen_t addr_size;
    addr_size = sizeof(struct sockaddr_in);

    cout << "Esperando conexiones..." << endl;
    // Bucle principal para aceptar conexiones y crear hilos para manejarlas
    while (true) {
        int socket_cliente;

        // Aceptamos una conexión entrante
        if ((socket_cliente = accept(socket_server, (struct sockaddr *)&direccionCliente, &addr_size)) < 0) {
            cerr << "Error en accept()." << endl;
            continue;
        }

        pthread_t hilo;
        // Creamos un par de datos (socket del cliente y dirección del cliente) para pasar al hilo
        pair<int, struct sockaddr_in> *datos = new pair<int, struct sockaddr_in>(socket_cliente, direccionCliente);
        // Creamos un hilo para manejar la conexión con el cliente
        if (pthread_create(&hilo, NULL, juego_wrapper, (void *)datos) != 0) {
            cerr << "Error creando hilo." << endl;
            delete datos;
            close(socket_cliente);
        } else {
            // Desvinculamos el hilo principal del hilo secundario para liberar sus recursos automáticamente
            pthread_detach(hilo);
        }
    }

    return 0;
}
