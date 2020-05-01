
#ifndef CONEXION_H_
#define CONEXION_H_

#include <sys/socket.h>
#include <arpa/inet.h> //INADDR_ANY e INET_ADDR
#include <commons/log.h>

#define ERROR -1 // Las system-calls de sockets retornan -1 en caso de error

typedef enum  {
    ENTERO,
	TEXTO
} TipoDato;

typedef enum {
	VACIO,
	INIT,
    CREATE_HILO,
	SCHEDULE_NEXT,
	WAIT,
	SIGNAL,
	JOIN,
	CLOSE
} TipoOperacion;

int crearSocketEscucha (int puerto, t_log* logger);
int crearSocketServidor(int puerto, t_log* logger);
int crearSocket(t_log* logger);
int crearSocketCliente(char *ipServidor, int puerto, t_log* logger);
int aceptarCliente(int fd_servidor, t_log* logger);
int enviarEntero(int fdDestinatario, int enteroEnviar,  t_log* logger);
int enviarTexto(int fdDestinatario, char* textoEnviar,  t_log* logger);
int recibirEntero(int fdOrigen, t_log* logger);
char* recibirTexto(int fdOrigen, t_log* logger);

void escucharSocketsEn(int fd_socket, t_log* logger);
void freeCharArray(char** charArray);
int pesoString(char *string);


#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include<pthread.h>
#include<commons/log.h>
#include<commons/string.h>
#include "serialize.h"
#include "structs.h"

#define TIEMPO_REINTENTO 10

pthread_t thread;

struct thread_args {
    int32_t socket;
    t_log* logger;
};

void iniciar_servidor(char* puerto, t_log* logger);
void esperar_cliente(int32_t socket_servidor, t_log* logger);
void recibir_muchos_mensajes(void* input);
void process_request(op_code cod_op, int32_t cliente_fd, t_log* logger);



uint32_t connect_to_server(char * ip, char * puerto, t_log* logger);



#endif /* CONEXION_H_ */
