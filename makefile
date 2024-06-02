# Definimos el compilador a usar
CXX = g++

# Especificamos las banderas del compilador
CXXFLAGS = -std=c++11 -Wall

# Bandera para enlazar la librería pthread
LDFLAGS = -lpthread

# Definimos los archivos fuente para el cliente y el servidor
CLIENT_SRC = client.cpp
SERVER_SRC = server.cpp

# Definimos los archivos objeto generados a partir de los archivos fuente
CLIENT_OBJ = $(CLIENT_SRC:.cpp=.o)
SERVER_OBJ = $(SERVER_SRC:.cpp=.o)

# Nombres de los ejecutables para el cliente y el servidor
CLIENT_EXEC = cliente
SERVER_EXEC = servidor

# Regla para compilar todos los ejecutables
all: $(CLIENT_EXEC) $(SERVER_EXEC)

# Regla para compilar el ejecutable del cliente
$(CLIENT_EXEC): $(CLIENT_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Regla para compilar el ejecutable del servidor
$(SERVER_EXEC): $(SERVER_OBJ) $(TABLERO_OBJ) $(MITABLERO_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Regla para compilar archivos objeto a partir de archivos fuente
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Regla para limpiar los archivos generados
clean:
	rm -f $(CLIENT_OBJ) $(SERVER_OBJ) $(TABLERO_OBJ) $(MITABLERO_OBJ) $(CLIENT_EXEC) $(SERVER_EXEC)

# Regla para ejecutar el cliente
run-client: $(CLIENT_EXEC)
	./$(CLIENT_EXEC) 127.0.0.1 8080

# Regla para ejecutar el servidor
run-server: $(SERVER_EXEC)
	./$(SERVER_EXEC) 8080

# Declaramos las reglas 'all', 'clean', 'run-client', y 'run-server' como phony (no están asociadas a archivos)
.PHONY: all clean run-client run-server
