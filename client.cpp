#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;

class MiCliente {
private:
    int mi_socket;
    struct sockaddr_in servidor;
    const int tam_buffer = 1024;

public:
    MiCliente(const string &ip, int puerto) {
        mi_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (mi_socket == -1) {
            throw runtime_error("Error al crear el socket.");
        }

        servidor.sin_addr.s_addr = inet_addr(ip.c_str());
        servidor.sin_family = AF_INET;
        servidor.sin_port = htons(puerto);

        if (connect(mi_socket, (struct sockaddr *)&servidor, sizeof(servidor)) < 0) {
            throw runtime_error("No se pudo conectar.");
        }

        cout << "Conectado al servidor en " << ip << ":" << puerto << "." << endl;
    }

    ~MiCliente() {
        close(mi_socket);
        cout << "Conexión cerrada." << endl;
    }

    void comunicarConServidor() {
        char buffer[tam_buffer];
        fd_set conjunto_lectura;
        struct timeval temporizador;

        while (true) {
            FD_ZERO(&conjunto_lectura);
            FD_SET(mi_socket, &conjunto_lectura);
            FD_SET(STDIN_FILENO, &conjunto_lectura);

            temporizador.tv_sec = 5;
            temporizador.tv_usec = 0;

            int actividad = select(mi_socket + 1, &conjunto_lectura, NULL, NULL, &temporizador);

            if (actividad < 0) {
                cout << "Error en select." << endl;
                break;
            }

            if (FD_ISSET(mi_socket, &conjunto_lectura)) {
                memset(buffer, 0, tam_buffer);
                int len = recv(mi_socket, buffer, tam_buffer - 1, 0);
                if (len > 0) {
                    cout << buffer << endl;
                } else {
                    cout << "El servidor cerró la conexión." << endl;
                    break;
                }
            }

            if (FD_ISSET(STDIN_FILENO, &conjunto_lectura)) {
                cout << " ";
                string entrada;
                getline(cin, entrada);

                if (!entrada.empty()) {
                    send(mi_socket, entrada.c_str(), entrada.length(), 0);
                    if (entrada == "salir") {
                        cout << "Cerrando la conexión..." << endl;
                        break;
                    }
                }
            }
        }
    }
};

int main(int argc, char *argv[]) {
    if (argc != 3) {
        cerr << "Uso: " << argv[0] << " <IP> <puerto>" << endl;
        return 1;
    }

    try {
        MiCliente cliente(argv[1], atoi(argv[2]));
        cliente.comunicarConServidor();
    } catch (const exception &e) {
        cerr << e.what() << endl;
        return 1;
    }

    return 0;
}
