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
	t_trainer* trainer2 = (t_trainer*) list_get(new_list,0);
	printf("el debug de los entrenador %s\n", trainer->objectives[2]);

	t_objective* objective1 = malloc(sizeof(t_objective));

	objective1->pokemon = "pikachu";
	objective1->count = 1;

	t_objective* objective2 = malloc(sizeof(t_objective));

	objective2->pokemon = "charmander";
	objective2->count = 1;

	t_objective* objective3 = malloc(sizeof(t_objective));

	objective3->pokemon = "snorlax";
	objective3->count = 1;
	//printf("debug del bool %d\n",testing_bool);

	//

	t_list* objetives_list = list_create();

	/*
	list_add(objetives_list, objective1);
	list_add(objetives_list, objective2);
	list_add(objetives_list, objective3);

	t_objective* objective4 = find_key(objetives_list, "charmander");


	printf("el debug de objective4 %s\n", objective2->pokemon);
	printf("el debug de objective4 %s\n", objective4->pokemon);

	add_objective(objetives_list,"charmander");
	add_objective(objetives_list,"charmander");
	add_objective(objetives_list,"charmander");
	add_objective(objetives_list,"charmander");
	add_objective(objetives_list,"hunter");
	printf("el debug de objective4 %s\n", objective2->pokemon);
	printf("el debug de objective4 %d\n", objective2->count);

	t_objective* objective10 = find_key(objetives_list, "hunter");

	printf("el debug de objective4 %s\n", objective10->pokemon);
	printf("el debug de objective4 %d\n", objective10->count);
	printf("el debug de objective4 %d\n", objective10->caught);
	*/
	/*
	objetives_list = add_trainer_to_objective(objetives_list,trainer);
	t_objective* test_obj = (t_objective*) list_get(objetives_list,1);
	printf("the objective list has %s\n", test_obj->pokemon);
	*/
	objetives_list = initialize_global_objectives(new_list);
	t_objective* test_obj = (t_objective*) list_get(objetives_list,0);
	printf("the objective list has %s\n", test_obj->pokemon);
	printf("the objective list has %d\n", test_obj->count);
	printf("the objective list has %d\n", test_obj->caught);
	printf("the objective bool is %d\n", success_objective(test_obj));
	printf("the objective bool is %d\n", success_global_objective(new_list));
	//FIFO funcion generica que recibe tipo de planificador, "fifo" agarra el primero en la cola
	//agarra lista ready y se fija cual es el que pasa a lista exec


	//pthread_mutex_init(trainer->semThread,NULL);

	sem_init(&trainer->sem_thread, 0, 0);
	sem_init(&trainer2->sem_thread, 0, 0);
	//trainer->semThread = 0;
	//trainer2->semThread = 0;
	pthread_t tid;
	pthread_t tid2;
	pthread_attr_t attr;

	printf("Before Thread\n");
	pthread_create(&tid, NULL, trainer_thread, trainer);
	pthread_create(&tid2, NULL, trainer_thread, trainer2);
	printf("algoo\n");

		//trainer->semThread = 1;
	//pthread_mutex_init(trainer->semThread,NULL);
	sem_post(&trainer2->sem_thread);
	sleep(1);
	sem_post(&trainer->sem_thread);

	pthread_join(tid, NULL);
	pthread_join(tid2, NULL);
	printf("After Thread\n");

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
	exit(0);

}
