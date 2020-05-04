/*
 * team.c
 *
 *  Created on: 11 abr. 2020
 *      Author: utnso
 */
#include <stdio.h>
#include "team.h"
#include <stdint.h>

//#include <libs/conexion.h>
//atoi(test_split[0])

int main(void)
{
	uint32_t a = 123;
	int conexion;
	char* ip;
	char* puerto;

	t_log* logger;
	t_config* config;

	logger = iniciar_logger();
	log_info(logger, "hola soy lasti123");

	config = leer_config();
	bool test = config_has_property(config, "IP");
	printf(test ? "true" : "false");
	ip = config_get_string_value(config, "IP");

	//char** test_list = config_get_array_value(config,"POSICIONES_ENTRENADORESE");
	char** test_list = config_get_array_value(config,"POSICIONES_ENTRENADORES");
	char** test_split = string_split(test_list[0], "|");

	printf("debug del test list %d\n", atoi(test_split[0]));
	printf("debug del test list 1 %d\n", atoi(test_split[1]));
	//printf("debug del test list 2 %s\n", test_split[2]);
	puerto = config_get_string_value(config, "PUERTO");

	//crear conexion
	conexion = crear_conexion(ip, puerto);

	printf("Number = %d \n",conexion);
	//enviar mensaje
	enviar_mensaje("test servidor up \n",conexion);
	//recibir mensaje
	char* message;
	message = recibir_mensaje(conexion);
	//loguear mensaje recibido
	log_info(logger, message);
	terminar_programa(conexion, logger, config);
  /*
	char* string = "Team";

	int peso = pesoString(string);

	printf("el peso es %d. \n", peso);
  */
}
