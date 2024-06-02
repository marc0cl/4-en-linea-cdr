// Importamos las librerías necesarias para el funcionamiento del programa.
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;

// Definición de la clase MiCliente
class MiCliente {
private:
    int mi_socket;  // Descriptor del socket
    struct sockaddr_in servidor;  // Estructura para la dirección del servidor
    const int tam_buffer = 1024;  // Tamaño del búfer para recibir datos

public:
    // Constructor de la clase MiCliente
    MiCliente(const string &ip, int puerto) {
        // Creación del socket
        mi_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (mi_socket == -1) {
            throw runtime_error("Error al crear el socket.");
        }

        // Configuración de la dirección del servidor
        servidor.sin_addr.s_addr = inet_addr(ip.c_str());
        servidor.sin_family = AF_INET;
        servidor.sin_port = htons(puerto);

        // Conexión al servidor
        if (connect(mi_socket, (struct sockaddr *)&servidor, sizeof(servidor)) < 0) {
            throw runtime_error("No se pudo conectar.");
        }

        // Confirmación de conexión exitosa
        cout << "Conectado al servidor en " << ip << ":" << puerto << "." << endl;
    }

    // Destructor de la clase MiCliente
    ~MiCliente() {
        // Cierre del socket al destruir el objeto
        close(mi_socket);
        cout << "Conexión cerrada." << endl;
    }

    // Método para la comunicación con el servidor
    void comunicarConServidor() {
        char buffer[tam_buffer];  // Búfer para almacenar los datos recibidos del servidor
        fd_set conjunto_lectura;  
        struct timeval temporizador;  // Temporizador para el select()

        while (true) {
            // Configuración del conjunto de descriptores de archivo para el select()
            FD_ZERO(&conjunto_lectura);
            FD_SET(mi_socket, &conjunto_lectura);  // Agregar el socket del cliente al conjunto
            FD_SET(STDIN_FILENO, &conjunto_lectura);  // Agregar la entrada estándar al conjunto

            // Configuración del temporizador para el select()
            temporizador.tv_sec = 5;
            temporizador.tv_usec = 0;

            // Ejecución del select() para monitorear la actividad en los descriptores de archivo
            int actividad = select(mi_socket + 1, &conjunto_lectura, NULL, NULL, &temporizador);

            if (actividad < 0) {
                cout << "Error en select." << endl;
                break;
            }

            // Verificar si hay actividad en el socket del servidor
            if (FD_ISSET(mi_socket, &conjunto_lectura)) {
                // Limpiar el búfer y recibir datos del servidor
                memset(buffer, 0, tam_buffer);
                int len = recv(mi_socket, buffer, tam_buffer - 1, 0);
                if (len > 0) {
                    cout << buffer << endl;
                } else {
                    cout << "El servidor cerró la conexión." << endl;
                    break;
                }
            }

            // Verificar si hay actividad en la entrada estándar (stdin)
            if (FD_ISSET(STDIN_FILENO, &conjunto_lectura)) {
                // Solicitar al usuario que ingrese datos
                cout << " ";
                string entrada;
                getline(cin, entrada);

                // Enviar los datos al servidor si no están vacíos
                if (!entrada.empty()) {
                    send(mi_socket, entrada.c_str(), entrada.length(), 0);
                    // Verificar si el usuario desea salir
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
    // Verificar la cantidad de argumentos proporcionados
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
