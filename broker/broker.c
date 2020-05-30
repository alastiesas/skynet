/*
 ============================================================================
 Name        : broker.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include<commons/log.h>
#include<commons/config.h>
#include<sys/socket.h>
#include<unistd.h>
#include<netdb.h>
#include<commons/collections/list.h>
#include<string.h>

#define IP "127.0.0.1"
#define TIEMPO_REINTENTO 10
#define TIEMPO_CHECK 15

typedef enum
{
	MENSAJE=1
}op_code;

typedef struct
{
	int size;
	void* stream;
} t_buffer;

typedef struct
{
	op_code codigo_operacion;
	t_buffer* buffer;
} t_paquete;

pthread_t thread;
pthread_t hilo1;
pthread_t hilo2;
pthread_t hilo3;

void iniciar_servidor(char * puerto);
void esperar_cliente(int socket_servidor);
void serve_client(int* socket);
void process_request(int cod_op, int cliente_fd);
void* recibir_mensaje(int socket_cliente, int* size);
void* serializar_paquete(t_paquete* paquete, int bytes);
void devolver_mensaje(void* payload, int size, int socket_cliente);

void cliente_game_card(){

	char* puerto = "6001";

	printf("id del thread: '%lu'\n", hilo1);
	char nombre[16];	//minimo es 16
	pthread_setname_np(hilo1, "Game-card");
	pthread_getname_np(hilo1, nombre, 16);
	printf("nombre del thread: %s\n", nombre);


	t_log* logger;
//	t_config* config;

	logger = log_create("broker_game_card.txt", "tp0", true, LOG_LEVEL_INFO);
	log_info(logger, "soy un log");

	//config = config_create("broker.config");
	//puerto = config_get_string_value(config, "PUERTO_TEAM");
	log_info(logger, "Mi puerto es: %s", puerto);

	puts("Por iniciar servidor");
	iniciar_servidor(puerto);

}

void cliente_team(){

	char* puerto = "6002";

	printf("id del thread: '%lu'\n", hilo2);
	char nombre[16];	//minimo es 16
	pthread_setname_np(hilo2, "Team");
	pthread_getname_np(hilo2, nombre, 16);
	printf("nombre del thread: %s\n", nombre);

	t_log* logger;
//	t_config* config;

	logger = log_create("broker_team.txt", "tp0", true, LOG_LEVEL_INFO);
	log_info(logger, "soy un log");

	//config = config_create("broker.config");
	//puerto = config_get_string_value(config, "PUERTO_TEAM");
	log_info(logger, "Mi puerto es: %s", puerto);

	puts("Por iniciar servidor");
	iniciar_servidor(puerto);

}

void cliente_game_boy(){

	char* puerto = "6003";

	printf("id del thread: '%lu'\n", hilo3);
	char nombre[16];	//minimo es 16
	pthread_setname_np(hilo3, "Game-boy");
	pthread_getname_np(hilo3, nombre, 16);
	printf("nombre del thread: %s\n", nombre);

	t_log* logger;
//	t_config* config;

	logger = log_create("broker_game_boy.txt", "tp0", true, LOG_LEVEL_INFO);
	log_info(logger, "soy un log");

	//config = config_create("broker.config");
	//puerto = config_get_string_value(config, "PUERTO_TEAM");
	log_info(logger, "Mi puerto es: %s", puerto);

	puts("Por iniciar servidor");
	iniciar_servidor(puerto);

}


int main(void) {
	puts("!!!Hola bienvenido al broker!!!\n"); /* prints !!!Hello World!!! */


	pthread_create(&hilo1, NULL, (void*) cliente_game_card, NULL);

	pthread_create(&hilo2, NULL, (void*) cliente_team, NULL);

	pthread_create(&hilo3, NULL, (void*) cliente_game_boy, NULL);


	for(;;);
	puts("Fin\n");

	return EXIT_SUCCESS;
}


void iniciar_servidor(char * puerto)
{
	int socket_servidor;

    struct addrinfo hints, *servinfo, *p;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(IP, puerto, &hints, &servinfo);

    for (p=servinfo; p != NULL; p = p->ai_next)
    {
        if ((socket_servidor = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){
        	puts("Error de socket()");
        	continue;
        }

        if (bind(socket_servidor, p->ai_addr, p->ai_addrlen) == -1) {
            close(socket_servidor);
            puts("error de bind");
            continue;
        }
        break;
    }

	listen(socket_servidor, SOMAXCONN);
	puts("En escucha");

    freeaddrinfo(servinfo);

    while(1)
    	esperar_cliente(socket_servidor);
}

void esperar_cliente(int socket_servidor)
{
	struct sockaddr_in dir_cliente;

	int tam_direccion = sizeof(struct sockaddr_in);

	int socket_cliente = accept(socket_servidor, (void*) &dir_cliente, &tam_direccion);
	puts("Conexion aceptada");

	pthread_create(&thread,NULL,(void*)serve_client,&socket_cliente);
	pthread_detach(thread);
	printf("Se creo un thread para atender la conexion del cliente %d\n", socket_cliente);

}

void serve_client(int* socket)
{
	printf("cliente conectado\n");
	char modulo[16];
	int tid = pthread_self();
	pthread_getname_np(tid, modulo, 16);
	int vez = 1;
	while(1){
	printf("esperando cod_op del %s, por vez numero %d\n", modulo, vez);
	int cod_op;
	if(recv(*socket, &cod_op, sizeof(int), MSG_WAITALL) == -1)
		cod_op = -1;
	printf("se recibio la cod op: %d\n", cod_op);
	process_request(cod_op, *socket);
	vez++;
	}

}

void process_request(int cod_op, int cliente_fd) {
	int size;
	void* msg;
		switch (cod_op) {
		case MENSAJE:
			msg = recibir_mensaje(cliente_fd, &size);
			devolver_mensaje(msg, size, cliente_fd);
			free(msg);
			break;
		case 0:
			puts("se recibio la cod_op 0 asi que finaliza el thread de conexion");
			pthread_exit(NULL);
		case -1:
			puts("se recibio la cod_op -1 asi que finaliza el thread de conexion");
			pthread_exit(NULL);
		}
}

void* recibir_mensaje(int socket_cliente, int* size)
{
	void * buffer;

	printf("esperando recibir tamanio del mensaje\n");
	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	printf("se solicito recibir un tamanio de mensaje de: %d\n", *size);
	buffer = malloc(*size);
	recv(socket_cliente, buffer, *size, MSG_WAITALL);
	printf("mensaje recibido: %s\n", (char*) buffer);

	return buffer;
}

void* serializar_paquete(t_paquete* paquete, int bytes)
{
	void * magic = malloc(bytes);
	int desplazamiento = 0;

	memcpy(magic + desplazamiento, &(paquete->codigo_operacion), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(magic + desplazamiento, &(paquete->buffer->size), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(magic + desplazamiento, paquete->buffer->stream, paquete->buffer->size);
	desplazamiento+= paquete->buffer->size;

	return magic;
}

void devolver_mensaje(void* payload, int size, int socket_cliente)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));

	paquete->codigo_operacion = MENSAJE;
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = size;
	paquete->buffer->stream = malloc(paquete->buffer->size);
	memcpy(paquete->buffer->stream, payload, paquete->buffer->size);

	int bytes = paquete->buffer->size + 2*sizeof(int);

	void* a_enviar = serializar_paquete(paquete, bytes);

	printf("Intentando devolver mensaje\n");
	if(send(socket_cliente, a_enviar, bytes, 0) == -1)
		printf("Error al enviar\n");
	else
		printf("Enviado\n");

	free(a_enviar);
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}


