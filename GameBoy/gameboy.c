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
#include<commons/collections/list.h>
#include <conexion.h>
#include <mensajes.h>

#define LOG_CONSOLE true

pthread_t hilo1;
pthread_t hilo2;
pthread_t hilo3;

t_config* config;


void server_broker(){
	char* yo = "GameBoy";
	char* el = "Broker";

	t_log* logger;
	logger = initialize_thread(yo, el, hilo1);


	char* puerto; //config_get_string reserva la memoria necesaria
	char* ip;
puerto="6003";//		puerto = config_get_string_value(config, "PUERTO_BROKER");
		ip = config_get_string_value(config, "IP_BROKER");
			log_info(logger, "Puerto del Broker: %s", puerto);
			log_info(logger, "IP del Broker: %s", ip);

	uint32_t socket;

	//crear conexion
	socket = connect_to_server(ip, puerto, logger);


	//enviar muchos mensajes
	enviar_muchos_mensajes(yo, el, socket, logger);



	log_info(logger, "Fin del la conexion con el Broker\n");
	close(socket);
	log_destroy(logger);

}

void server_game_card(){
	char* yo = "GameBoy";
	char* el = "GameCard";

	t_log* logger;
	logger = initialize_thread(yo, el, hilo2);


	char* puerto; //config_get_string reserva la memoria necesaria
	char* ip;
puerto="6004";//		puerto = config_get_string_value(config, "PUERTO_GAMECARD");
		ip = config_get_string_value(config, "IP_GAMECARD");
			log_info(logger, "Puerto del GameCard: %s", puerto);
			log_info(logger, "IP del GameCard: %s", ip);

	uint32_t socket;

	//crear conexion
	socket = connect_to_server(ip, puerto, logger);


	//enviar muchos mensajes
	enviar_muchos_mensajes(yo, el, socket, logger);



	log_info(logger, "Fin del la conexion con el GameCard\n");
	close(socket);
	log_destroy(logger);



}

void server_team(){
	char* yo = "GameBoy";
	char* el = "Team";

	t_log* logger;
	logger = initialize_thread(yo, el, hilo3);


	char* puerto; //config_get_string reserva la memoria necesaria
	char* ip;
puerto="6005";//	puerto = config_get_string_value(config, "PUERTO_TEAM");
		ip = config_get_string_value(config, "IP_TEAM");
			log_info(logger, "Puerto del Team: %s", puerto);
			log_info(logger, "IP del Team: %s", ip);

	uint32_t socket;

	//crear conexion
	socket = connect_to_server(ip, puerto, logger);


	//enviar muchos mensajes
	enviar_muchos_mensajes(yo, el, socket, logger);



	log_info(logger, "Fin del la conexion con el Team\n");
	close(socket);
	log_destroy(logger);

}

int main(void) {
	puts("!!!Hola bienvenido al Game Boy!!!\n"); /* prints !!!Hello World!!! */

//Se crea el logger	obligatorio
	t_log* obligatorio;		//ver que pide loguear el tp
	if((obligatorio = log_create("GameBoy.txt", "GameBoy", LOG_CONSOLE, LOG_LEVEL_INFO)) == NULL){
		puts("No se pudo crear el log");
	}
	else
		log_info(obligatorio, "Log del GameBoy creado");


//Crear config
	if((config = config_create("gameBoy.config")) == NULL){
		log_error(obligatorio, "No se pudo crear la config");
	}
	else
		log_info(obligatorio, "config creada");


	pthread_create(&hilo1, NULL, (void*) server_broker, NULL);

	pthread_create(&hilo2, NULL, (void*) server_game_card, NULL);

	pthread_create(&hilo3, NULL, (void*) server_team, NULL);


	for(;;);
	config_destroy(config);
	puts("Fin\n");

	return EXIT_SUCCESS;
}


