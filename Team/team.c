/*
 * team.c
 *
 *  Created on: 11 abr. 2020
 *      Author: utnso
 */
#include "team.h"



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
	char* ip;
	char* puerto;

	log = log_create("team.log", "team program", true, LOG_LEVEL_INFO);
	log_info(log, "STARTING TEAM PROGRAM, GOOD LUCK");

	config = config_create("team.config");
	ip = config_get_string_value(config, "IP");
	puerto = config_get_string_value(config, "PUERTO");


	subscribe(6);
	sleep(5);



	pthread_t tid;
	pthread_create(&tid, NULL, sender_thread, NULL);

	//pthread_join(tid, NULL);

	//pthread_join(tid, NULL);

	//printf(test ? "true" : "false");


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

	int j = string_list_size(test_split_objetivo);



	printf("el valor de la J es %d\n", j);

	int count = size_array(test_objetivos[0]);
	printf("el count termina con %d\n", count);




	messages_list = list_create();
	message_response = dictionary_create();
	new_list = list_create();
	ready_list = list_create();
	block_list = list_create();
	exec_list = list_create();
	exit_list = list_create();
	poke_map = dictionary_create();



	initialize_trainers();
	initialize_global_objectives();

	t_position* test_position_1 = malloc(sizeof(t_position*));
	test_position_1->x = 0;
	test_position_1->y = 0;
	add_to_poke_map("Pikachu",(void*) test_position_1);

	t_position* test_position_2 = malloc(sizeof(t_position*));
	test_position_2->x = 3;
	test_position_2->y = 8;
	add_to_poke_map("Charmander",(void*) test_position_2);

	t_position* test_position_3 = malloc(sizeof(t_position*));
	test_position_3->x = 6;
	test_position_3->y = 6;
	add_to_poke_map("Bulbasaur",(void*) test_position_3);

	//pthread_t tid;
	//pthread_t tid2;

	long_term_scheduler();
	sleep(4);
	printf("EL TAMAÑO DE READY ES %d\n", list_size(ready_list));

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
	sleep(9999999999);
	exit(0);

}
