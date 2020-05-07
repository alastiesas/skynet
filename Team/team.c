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
	char** test_postions = config_get_array_value(config,"POSICIONES_ENTRENADORES");
	char** test_split_position = string_split(test_postions[0], "|");

	printf("debug del test POSICION %d\n", atoi(test_split_position[0]));
	printf("debug del test POSICION 1 %d\n", atoi(test_split_position[1]));


	char** test_pokemons = config_get_array_value(config,"POKEMON_ENTRENADORES");
	char** test_split_pokemon = string_split(test_pokemons[0], "|");

	printf("debug del test POKEMON %s\n", test_split_pokemon[0]);
	printf("debug del test POKEMON 1 %s\n", test_split_pokemon[1]);

	//OBJETIVOS_ENTRENADORES

	char** test_objetivos = config_get_array_value(config,"OBJETIVOS_ENTRENADORES");
	char** test_split_objetivo = string_split(test_objetivos[0], "|");

	printf("debug del test OBJETIVO %s\n", test_split_objetivo[0]);
	printf("debug del test OBJETIVO 1 %s\n", test_split_objetivo[1]);

	int j = size_array_config(test_split_objetivo);



	printf("el valor de la J es %d\n", j);

	int count = size_array(test_objetivos[0]);
	printf("el count termina con %d\n", count);





	//printf("tamaño de la lista de entrenadores es %d\n", size_test);
	//printf("debug del test list 2 %s\n", test_split[2]);
	puerto = config_get_string_value(config, "PUERTO");



	//ciclo para cargar una lista de entrenadores.
	//t_trainer* test_entrenador = construct_trainer(test_postions[0], test_objetivos[0], test_pokemons[0]);
	t_list* new_list = initialize_trainers(test_postions, test_objetivos, test_pokemons);
	t_list* ready_list = list_create();
	t_list* block_list = list_create();
	t_list* exec_list = list_create();
	t_list* exit_list = list_create();
	state_change(1,new_list, ready_list);
	t_trainer* trainer = (t_trainer*) list_get(ready_list,0);

	printf("el debug de los entrenador %s\n", trainer->objectives[2]);

	//FIFO funcion generica que recibe tipo de planificador, "fifo" agarra el primero en la cola
	//agarra lista ready y se fija cual es el que pasa a lista exec


	//5 colas para los estados

	/*
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
	*/
  /*
	char* string = "Team";

	int peso = pesoString(string);

	printf("el peso es %d. \n", peso);
  */
}
