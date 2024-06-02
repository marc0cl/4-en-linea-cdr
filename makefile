# Makefile para compilar cliente y servidor

CXX = g++
CXXFLAGS = -std=c++11 -Wall
LDFLAGS = -lpthread

# Archivos fuente y objetos
CLIENT_SRC = cliente.cpp
SERVER_SRC = servidor.cpp

CLIENT_OBJ = $(CLIENT_SRC:.cpp=.o)
SERVER_OBJ = $(SERVER_SRC:.cpp=.o)

# Ejecutables
CLIENT_EXEC = cliente
SERVER_EXEC = servidor

# Regla por defecto
all: $(CLIENT_EXEC) $(SERVER_EXEC)

# Regla para compilar el cliente
$(CLIENT_EXEC): $(CLIENT_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Regla para compilar el servidor
$(SERVER_EXEC): $(SERVER_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Regla para compilar los archivos fuente a objetos
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Limpiar los archivos compilados
clean:
	rm -f $(CLIENT_OBJ) $(SERVER_OBJ) $(CLIENT_EXEC) $(SERVER_EXEC)

# Regla para ejecutar el cliente
run-client: $(CLIENT_EXEC)
	./$(CLIENT_EXEC) 127.0.0.1 8080

# Regla para ejecutar el servidor
run-server: $(SERVER_EXEC)
	./$(SERVER_EXEC) 8080

.PHONY: all clean run-client run-server
