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
#include <conexion.h>
#include <mensajes.h>

#define IP "127.0.0.1"

#define TIEMPO_CHECK 15

pthread_t thread;
pthread_t hilo1;
pthread_t hilo2;

t_config* config;

void server_broker();
void cliente_game_boy();


int main(void) {
	puts("!!!Hola bienvenido al Team!!!\n"); /* prints !!!Hello World!!! */

	//Se crea el logger	obligatorio
		t_log* obligatorio;		//ver que pide loguear el tp
		if((obligatorio = log_create("Team.log", "Team", LOG_CONSOLE, LOG_LEVEL_INFO)) == NULL){
			puts("No se pudo crear el log");
		}
		else
			log_info(obligatorio, "Log del Team creado");

	//Crear config
		if((config = config_create("team.config")) == NULL){
			log_error(obligatorio, "No se pudo crear la config");
		}
		else
			log_info(obligatorio, "config creada");





	pthread_create(&hilo1, NULL, (void*) server_broker, NULL);

	pthread_create(&hilo2, NULL, (void*) cliente_game_boy, NULL);

	for(;;);
	puts("Fin\n");

	return EXIT_SUCCESS;
}

//--------Funciones de prueba

void server_broker(){

	char* yo = "Team";
	char* el = "Broker";

	t_log* logger;
	logger = initialize_thread(yo, el, hilo1);


	char* puerto; //config_get_string reserva la memoria necesaria
	char* ip;
puerto="6002";//		puerto = config_get_string_value(config, "PUERTO_BROKER");
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

void cliente_game_boy(){
	char* yo = "Team";
	char* el = "GameBoy";

	t_log* logger;
	logger = initialize_thread(yo, el, hilo2);

	char* puerto;
puerto="6005";//	puerto = config_get_string_value(config, "PUERTO_TEAM");

	log_info(logger, "Iniciando servidor en el puerto: %s", puerto);

	iniciar_servidor(puerto, logger);

}
