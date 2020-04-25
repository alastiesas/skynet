/*
 ============================================================================
 Name        : broker.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <pthread.h>
#include<commons/log.h>
#include<commons/config.h>
#include<sys/socket.h>
#include<commons/collections/list.h>
#include <serverUtils.h>
#include <clientUtils.h>

#define IP "127.0.0.1"

#define TIEMPO_CHECK 15

pthread_t thread;
pthread_t hilo1;
pthread_t hilo2;


void server_broker(){
	char* puerto = "6002";

	printf("id del thread: '%lu'\n", hilo1);
	char nombre[16];	//minimo es 16
	pthread_setname_np(hilo1, "Broker");
	pthread_getname_np(hilo1, nombre, 16);
	printf("nombre del thread: %s\n", nombre);

	int socket_cliente;

	t_log* logger;
//	t_config* config;

	logger = log_create("team_broker.txt", "tp0", true, LOG_LEVEL_INFO);
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
	enviar_mensaje("Buen dia broker soy el team\n", socket_cliente);


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

void cliente_game_boy(){
	char* puerto = "6005";

	printf("id del thread: '%lu'\n", hilo2);
	char nombre[16];	//minimo es 16
	pthread_setname_np(hilo2, "Game-boy");
	pthread_getname_np(hilo2, nombre, 16);
	printf("nombre del thread: %s\n", nombre);

	t_log* logger;
//	t_config* config;

	logger = log_create("team_game_boy.txt", "tp0", true, LOG_LEVEL_INFO);
	log_info(logger, "soy un log");

	//config = config_create("broker.config");
	//puerto = config_get_string_value(config, "PUERTO_TEAM");
	log_info(logger, "Mi puerto es: %s", puerto);

	puts("Por iniciar servidor");
	iniciar_servidor(puerto);


}


int main(void) {
	puts("!!!Hola bienvenido al broker!!!\n"); /* prints !!!Hello World!!! */

	pthread_create(&hilo1, NULL, (void*) server_broker, NULL);

	pthread_create(&hilo2, NULL, (void*) cliente_game_boy, NULL);


	for(;;);
	puts("Fin\n");

	return EXIT_SUCCESS;
}

