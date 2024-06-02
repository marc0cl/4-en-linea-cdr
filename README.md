# Uso de sockets con juego Conecta 4

Este repositorio contiene el uso de sockets concurrentes en C++ con el juego Conecta 4.

## Requisitos

- Se necesita tener instalado un compilador de C++ compatible, como g++.
- Se recomienda un sistema operativo basado en Unix, como Linux o macOS, para ejecutar correctamente los comandos Makefile.

## Compilación

Para compilar el juego, simplemente ejecuta el comando `make` en tu terminal. Esto compilará los archivos fuente y generará los ejecutables `cliente` y `servidor`.

```bash
make
```

## Ejecución

Esto permitirá ejecutar los comandos de servidor y cliente

### Servidor
```bash
./servidor <Puerto>
```

### Cliente
```bash
./cliente <IP local> <Puerto>
```