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


void *myThreadFun(void *vargp)
{
    sleep(1);
    printf("Printing GeeksQuiz from Thread \n");
    return NULL;
}


int main(void)
{
	uint32_t a = 123;
	int conexion;
	char* ip;
	char* puerto;

	//config = config_create("team.config");
	//log = log_create("team.log", "team", LOG_CONSOLE, LOG_LEVEL_INFO);

	log = log_create("team.log", "team program", true, LOG_LEVEL_INFO);
	log_info(log, "hola soy lasti123");

	config = config_create("team.config");
	//bool test = config_has_property(config, "IP");


	// hilo de servidor

	//pthread_t tid;
	//pthread_create(&tid, NULL, fake_broker_thread, NULL);


	subscribe(5);
	sleep(5);



	pthread_t tid;
	pthread_create(&tid, NULL, sender_thread, NULL);

	//pthread_join(tid, NULL);

	//pthread_join(tid, NULL);

	//printf(test ? "true" : "false");

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


	puerto = config_get_string_value(config, "PUERTO");

	messages_list = list_create();

	new_list = list_create();
	ready_list = list_create();
	block_list = list_create();
	exec_list = list_create();
	exit_list = list_create();
	poke_map = dictionary_create();

	t_position* test_position_1 = malloc(sizeof(t_position*));
	test_position_1->x = 4;
	test_position_1->y = 8;

	t_position* test_position_2 = malloc(sizeof(t_position*));
	test_position_2->x = 1;
	test_position_2->y = 4;

	t_position* test_position_3 = malloc(sizeof(t_position*));
	test_position_3->x = 3;
	test_position_3->y = 3;

	add_to_poke_map("Pikachu",(void*) test_position_1);
	add_to_poke_map("Charmander",(void*) test_position_2);

	initialize_trainers(test_postions, test_objetivos, test_pokemons);

	objetives_list = list_create();

	objetives_list = initialize_global_objectives(new_list);


	//pthread_t tid;
	//pthread_t tid2;
	pthread_attr_t attr;

	printf("aca llego!!!\n");
	long_term_scheduler();
	sleep(4);
	printf("EL TAMAÑO DE READY ES %d\n", list_size(ready_list));
	printf("aca NO llego!!!\n");
	printf("aca llego!!!\n");
	sleep(4);
	printf("aca NO llego!!!\n");

	pthread_t exec_tid;

	pthread_create(&exec_tid, NULL, exec_thread, NULL);


	pthread_join(exec_tid, NULL);

	//HASTA ACA DEJAMOS COMENTADO

	/*
	void* map(t_trainer* trainer){
		void iterate(pokemon){
			calcular_distancia(trainer,pokemon);
		}

		list_iterate(poke_map,iterate);


	}
	*/

	//t_list* new_list = list_map(lista,map);












	//printf("el debug de los entrenador %d\n", ((t_trainer*) list_get(exec_list,0))->position->y);
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
	log_info(log, message);
	terminar_programa(conexion, log, config);
	*/
  /*
	char* string = "Team";

	int peso = pesoString(string);

	printf("el peso es %d. \n", peso);
  */
	exit(0);

}
