/*
 ============================================================================
 Name        : broker.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include<commons/config.h>
#include <serverUtils.h>
#include <clientUtils.h>

#define IP "127.0.0.1"


pthread_t thread;
pthread_t hilo1;
pthread_t hilo2;
pthread_t hilo3;

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





