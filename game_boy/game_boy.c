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

pthread_t hilo1;
pthread_t hilo2;
pthread_t hilo3;

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

int crear_conexion(char *ip, char* puerto);
void enviar_mensaje(char* mensaje, int socket_cliente);

void server_broker(){
	char* puerto = "6003";

	printf("id del thread: '%lu'\n", hilo1);
	char nombre[16];	//minimo es 16
	pthread_setname_np(hilo1, "Broker");
	pthread_getname_np(hilo1, nombre, 16);
	printf("nombre del thread: %s\n", nombre);

	int socket_cliente;

	t_log* logger;
//	t_config* config;

	logger = log_create("game_boy_broker.txt", "tp0", true, LOG_LEVEL_INFO);
	log_info(logger, "soy un log");

//	config = config_create("game_card.config");

//	puerto = config_get_string_value(config, "PUERTO_BROKER");
	log_info(logger, "puerto del broker: %s", puerto);

	//crear conexion
	socket_cliente = crear_conexion(IP, puerto);
	log_info(logger, "conexion creada\n");

	//enviar mensaje
	int vez = 1;
	while(1){
	enviar_mensaje("Buen dia broker soy el game boy\n", socket_cliente);


	//recibir mensaje
	char* buffer;
	printf("intentando recibir cod_op por vez numero %d\n", vez);
	int cod_op;
		if(recv(socket_cliente, &cod_op, sizeof(int), MSG_WAITALL) == -1)
		printf("error\n");
			else
		printf("se recibio la cod op: %d\n", cod_op);

		int size;
	printf("esperando recibir tamanio del mensaje\n");
	recv(socket_cliente, &size, sizeof(int), MSG_WAITALL);
	printf("se solicito recibir un tamanio de mensaje de: %d\n", size);
	buffer = malloc(size);
	recv(socket_cliente, buffer, size, MSG_WAITALL);

	//loguear mensaje recibido
	printf("mensaje rerespuesta del %s recibido: %s\n", nombre, buffer);
	free(buffer);

	sleep(TIEMPO_CHECK);
	vez++;
	}

	for(;;);
	log_info(logger, "fin del la conexion con el broker\n");
	close(socket_cliente);
	log_destroy(logger);
//	config_destroy(config);

}

void server_game_card(){
	char* puerto = "6004";

	printf("id del thread: '%lu'\n", hilo2);
	char nombre[16];	//minimo es 16
	pthread_setname_np(hilo2, "Game-card");
	pthread_getname_np(hilo2, nombre, 16);
	printf("nombre del thread: %s\n", nombre);

	int socket_cliente;

	t_log* logger;
//	t_config* config;

	logger = log_create("game_boy_game_card.txt", "tp0", true, LOG_LEVEL_INFO);
	log_info(logger, "soy un log");

//	config = config_create("game_card.config");

//	puerto = config_get_string_value(config, "PUERTO_BROKER");
	log_info(logger, "puerto del broker: %s", puerto);

	//crear conexion
	socket_cliente = crear_conexion(IP, puerto);
	log_info(logger, "conexion creada\n");

	//enviar mensaje
	int vez = 1;
	while(1){
	enviar_mensaje("Buen dia game_card soy el game boy\n", socket_cliente);


	//recibir mensaje
	char* buffer;
	printf("intentando recibir cod_op por vez numero %d\n", vez);
	int cod_op;
		if(recv(socket_cliente, &cod_op, sizeof(int), MSG_WAITALL) == -1)
		printf("error\n");
			else
		printf("se recibio la cod op: %d\n", cod_op);

		int size;
	printf("esperando recibir tamanio del mensaje\n");
	recv(socket_cliente, &size, sizeof(int), MSG_WAITALL);
	printf("se solicito recibir un tamanio de mensaje de: %d\n", size);
	buffer = malloc(size);
	recv(socket_cliente, buffer, size, MSG_WAITALL);

	//loguear mensaje recibido
	printf("mensaje rerespuesta del %s recibido: %s\n", nombre, buffer);
	free(buffer);

	sleep(TIEMPO_CHECK);
	vez++;
	}

	for(;;);
	log_info(logger, "fin del la conexion con el broker\n");
	close(socket_cliente);
	log_destroy(logger);
//	config_destroy(config);

}

void server_team(){
	char* puerto = "6005";

	printf("id del thread: '%lu'\n", hilo3);
	char nombre[16];	//minimo es 16
	pthread_setname_np(hilo3, "Team");
	pthread_getname_np(hilo3, nombre, 16);
	printf("nombre del thread: %s\n", nombre);

	int socket_cliente;

	t_log* logger;
//	t_config* config;

	logger = log_create("game_boy_team.txt", "tp0", true, LOG_LEVEL_INFO);
	log_info(logger, "soy un log");

//	config = config_create("game_card.config");

//	puerto = config_get_string_value(config, "PUERTO_BROKER");
	log_info(logger, "puerto del broker: %s", puerto);

	//crear conexion
	socket_cliente = crear_conexion(IP, puerto);
	log_info(logger, "conexion creada\n");

	//enviar mensaje
	int vez = 1;
	while(1){
	enviar_mensaje("Buen dia team soy el game boy\n", socket_cliente);


	//recibir mensaje
	char* buffer;
	printf("intentando recibir cod_op por vez numero %d\n", vez);
	int cod_op;
		if(recv(socket_cliente, &cod_op, sizeof(int), MSG_WAITALL) == -1)
		printf("error\n");
			else
		printf("se recibio la cod op: %d\n", cod_op);

		int size;
	printf("esperando recibir tamanio del mensaje\n");
	recv(socket_cliente, &size, sizeof(int), MSG_WAITALL);
	printf("se solicito recibir un tamanio de mensaje de: %d\n", size);
	buffer = malloc(size);
	recv(socket_cliente, buffer, size, MSG_WAITALL);

	//loguear mensaje recibido
	printf("mensaje rerespuesta del %s recibido: %s\n", nombre, buffer);
	free(buffer);

	sleep(TIEMPO_CHECK);
	vez++;
	}

	for(;;);
	log_info(logger, "fin del la conexion con el broker\n");
	close(socket_cliente);
	log_destroy(logger);
//	config_destroy(config);

}

int main(void) {
	puts("!!!Hola bienvenido al Game Boy!!!\n"); /* prints !!!Hello World!!! */

	pthread_create(&hilo1, NULL, (void*) server_broker, NULL);

	pthread_create(&hilo2, NULL, (void*) server_game_card, NULL);

	pthread_create(&hilo3, NULL, (void*) server_team, NULL);


	for(;;);
	puts("Fin\n");

	return EXIT_SUCCESS;
}


int crear_conexion(char *ip, char* puerto)
{
	int socket_cliente;
	char modulo[16];
	int tid = pthread_self();
	pthread_getname_np(tid, modulo, 16);
	int conexion = -2;
	while (conexion < 0){
		if (conexion == -1){
			printf("Reintentando en %d segundos\n", TIEMPO_REINTENTO);
				sleep(TIEMPO_REINTENTO);
		}

	struct addrinfo hints;
	struct addrinfo *server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, puerto, &hints, &server_info);

	socket_cliente = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);

	conexion = connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen);
	if(conexion == -1)
		printf("error de conexion con el %s\n", modulo);

	freeaddrinfo(server_info);
	}

	return socket_cliente;
}

void enviar_mensaje(char* mensaje, int socket_cliente)
{
	//Quiero mandar el stream de datos	|cod_op|size|mensaje|

	t_buffer *ptr_buffer = malloc(sizeof(t_buffer));
	t_paquete *paquete = malloc(sizeof(t_paquete));

	//meto la cod_op en el paquete
	paquete->codigo_operacion = MENSAJE;
	//asigno el buffer que previamente reserve memoria
	paquete->buffer = ptr_buffer;
	//asigno el size del buffer
	paquete->buffer->size = strlen(mensaje) + 1;
	//Con el size calculado, reservo memoria para el payload
	paquete->buffer->stream = malloc(paquete->buffer->size);
	//con memcpy() lleno el stream
	memcpy(paquete->buffer->stream, mensaje, paquete->buffer->size);


	int bytes = sizeof(int)*2 + ptr_buffer->size;

	//meto el cod_op + size + mensaje todo en un stream de datos
	void* a_enviar = malloc(bytes);
	int offset = 0;

	memcpy(a_enviar, &(paquete->codigo_operacion), sizeof(int));
	offset += sizeof(int);
	memcpy(a_enviar + offset, &(paquete->buffer->size), sizeof(int));
	offset += sizeof(int);
	memcpy(a_enviar + offset, paquete->buffer->stream, paquete->buffer->size);

	printf("Intentando enviar\n");
	if(send(socket_cliente, a_enviar, bytes, 0) == -1)
		printf("Error al enviar\n");
	else
		printf("Enviado\n");

	free(a_enviar);
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);


}
