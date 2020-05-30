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
#include <routines.c>

#define LOG_CONSOLE true /*pending2*/

#define IP "127.0.0.1" /*pending2*/

/*pending3*/
int main(int argc, char* argv[]) {
	puts("starting gameboy");

	log = log_create("gameboy.log", "gameboy", LOG_CONSOLE, LOG_LEVEL_INFO);
	log_info(log, "log created");
	config = config_create("gameboy.config");
	log_info(log, "config created");

	if (strcmp(argv[1], PARAMETER_SUBSCRIBER) == 0 && argc == 4) {

		queue_code queue_code;
		if (strcmp(argv[2], PARAMETER_APPEARED_POKEMON) == 0) {
			queue_code = COLA_APPEARED;
		} else if (strcmp(argv[2], PARAMETER_CATCH_POKEMON) == 0) {
			queue_code = COLA_CATCH;
		} else if (strcmp(argv[2], PARAMETER_CAUGHT_POKEMON) == 0) {
			queue_code = COLA_CAUGHT;
		} else if (strcmp(argv[2], PARAMETER_GET_POKEMON) == 0) {
			queue_code = COLA_GET;
		} else if (strcmp(argv[2], PARAMETER_LOCALIZED_POKEMON) == 0) {
			queue_code = COLA_LOCALIZED;
		} else if (strcmp(argv[2], PARAMETER_NEW_POKEMON) == 0) {
			queue_code = COLA_NEW;
		} else {
			exit(EXIT_FAILURE);
		}
		subscribe(queue_code, argv[3]);

	} else {
		char* ip;
		char* port;
		t_paquete* package; /*pending1*/
		if (strcmp(argv[1], PARAMETER_BROKER) == 0) {

			ip = config_get_string_value(config, "BROKER_IP");
			port = config_get_string_value(config, "BROKER_PORT");
			if (argc == 3 && strcmp(argv[2], PARAMETER_GET_POKEMON) == 0) {

			} else if (argc == 5
					&& strcmp(argv[2], PARAMETER_CAUGHT_POKEMON) == 0) {

			} else if (argc == 6
					&& strcmp(argv[2], PARAMETER_CATCH_POKEMON) == 0) {

			} else if (argc == 7) {

				if (strcmp(argv[2], PARAMETER_APPEARED_POKEMON) == 0) {

				} else if (strcmp(argv[2], PARAMETER_NEW_POKEMON) == 0) {

				} else {
					exit(EXIT_FAILURE);
				}
			} else {
				exit(EXIT_FAILURE);
			}

		} else if (strcmp(argv[1], PARAMETER_GAMECARD) == 0) {

			ip = config_get_string_value(config, "GAMECARD_IP");
			port = config_get_string_value(config, "GAMECARD_PORT");
			if (argc == 5 && strcmp(argv[2], PARAMETER_GET_POKEMON) == 0) {

			} else if (argc == 7
					&& strcmp(argv[2], PARAMETER_CATCH_POKEMON) == 0) {

			} else if (argc == 8
					&& strcmp(argv[2], PARAMETER_NEW_POKEMON) == 0) {

			} else {
				exit(EXIT_FAILURE);
			}

		} else if (strcmp(argv[1], PARAMETER_TEAM) == 0 && argc == 6
				&& strcmp(argv[2], PARAMETER_APPEARED_POKEMON) == 0) {

			ip = config_get_string_value(config, "TEAM_IP");
			port = config_get_string_value(config, "TEAM_PORT");

		} else {
			exit(EXIT_FAILURE);
		}
		send(ip, port, package);

	}

	puts("ending gameboy");
	log_destroy(log);
	config_destroy(config);
	return EXIT_SUCCESS;
}

/*int32_t posX = atoi(argv[4]);	//convierte a int el argumento de main

	int32_t posY = atoi(argv[5]);
	int32_t cantidad = atoi(argv[6]);

	t_new* new = malloc(sizeof(t_new));
	new->id = 0;
	strcpy(new->nombre, argv[3]);				//valgrind dice "use of uninitialized value of size 4 en strcpy." Pero no aclara si es esta linea
	uint32_t size_name = strlen(new->nombre)+1;
	new->size_nombre = size_name;
	new->posX = posX;				//convertir string a int?
	new->posY = posY;
	new->cantidad = cantidad;
	printf("Se guardaron todos los datos en la estructura a serializar\n");

	//serializar
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete = serialize_new(new); /*pending1

	free(new->nombre);
	free(new);*/
