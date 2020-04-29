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
#include <conexion.h>
#include <mensajes.h>

#define IP "127.0.0.1"



pthread_t hilo1;
pthread_t hilo2;
pthread_t hilo3;

t_config* config;

void cliente_game_card(){
	char* yo = "Broker";
	char* el = "GameCard";

	t_log* logger;
	logger = initialize_thread(yo, el, hilo1);

	char* puerto;
puerto="6001";//	puerto = config_get_string_value(config, "PUERTO_BROKER");

	log_info(logger, "Iniciando servidor en el puerto: %s", puerto);

	iniciar_servidor(puerto, logger);

}

void cliente_team(){
	char* yo = "Broker";
	char* el = "Team";

	t_log* logger;
	logger = initialize_thread(yo, el, hilo2);

	char* puerto;
	puerto="6002";//	puerto = config_get_string_value(config, "PUERTO_BROKER");

	log_info(logger, "Iniciando servidor en el puerto: %s", puerto);

	iniciar_servidor(puerto, logger);

}

void cliente_game_boy(){
	char* yo = "Broker";
	char* el = "GameBoy";

	t_log* logger;
	logger = initialize_thread(yo, el, hilo3);

	char* puerto;
	puerto="6003";//	puerto = config_get_string_value(config, "PUERTO_BROKER");

	log_info(logger, "Iniciando servidor en el puerto: %s", puerto);

	iniciar_servidor(puerto, logger);

}


int main(void) {
	puts("!!!Hola bienvenido al broker!!!\n"); /* prints !!!Hello World!!! */

	//Se crea el logger	obligatorio
		t_log* obligatorio;		//ver que pide loguear el tp
		if((obligatorio = log_create("Broker.txt", "Broker", LOG_CONSOLE, LOG_LEVEL_INFO)) == NULL){
			puts("No se pudo crear el log");
		}
		else
			log_info(obligatorio, "Log del Broker creado");


	//Crear config
		if((config = config_create("broker.config")) == NULL){
			log_error(obligatorio, "No se pudo crear la config");
		}
		else
			log_info(obligatorio, "config creada");


	pthread_create(&hilo1, NULL, (void*) cliente_game_card, NULL);

	pthread_create(&hilo2, NULL, (void*) cliente_team, NULL);

	pthread_create(&hilo3, NULL, (void*) cliente_game_boy, NULL);


	for(;;);
	puts("Fin\n");

	return EXIT_SUCCESS;
}





