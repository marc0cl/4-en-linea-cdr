CXX = g++
CXXFLAGS = -std=c++11 -Wall
LDFLAGS = -lpthread

CLIENT_SRC = cliente.cpp
SERVER_SRC = servidor.cpp

CLIENT_OBJ = $(CLIENT_SRC:.cpp=.o)
SERVER_OBJ = $(SERVER_SRC:.cpp=.o)

CLIENT_EXEC = cliente
SERVER_EXEC = servidor

all: $(CLIENT_EXEC) $(SERVER_EXEC)

$(CLIENT_EXEC): $(CLIENT_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(SERVER_EXEC): $(SERVER_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(CLIENT_OBJ) $(SERVER_OBJ) $(CLIENT_EXEC) $(SERVER_EXEC)

run-client: $(CLIENT_EXEC)
	./$(CLIENT_EXEC) 127.0.0.1 8080

run-server: $(SERVER_EXEC)
	./$(SERVER_EXEC) 8080

.PHONY: all clean run-client run-server
