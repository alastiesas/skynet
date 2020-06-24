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
	log = log_create("team.log", "team program", true, LOG_LEVEL_INFO);
	log_info(log, "STARTING TEAM PROGRAM, GOOD LUCK");

	config = config_create("team.config");


	subscribe(OPERATION_APPEARED);
	subscribe(OPERATION_LOCALIZED);
	subscribe(OPERATION_CAUGHT);
	sleep(5);

	pthread_t sender_tid;
	pthread_create(&sender_tid, NULL, sender_thread, NULL);

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

/*
	t_position* test_position_1 = malloc(sizeof(t_position));
	test_position_1->x = 0;
	test_position_1->y = 0;
	add_to_poke_map("Pikachu",(void*) test_position_1);

	t_position* test_position_2 = malloc(sizeof(t_position));
	test_position_2->x = 3;
	test_position_2->y = 8;
	add_to_poke_map("Charmander",(void*) test_position_2);

	t_position* test_position_3 = malloc(sizeof(t_position));
	test_position_3->x = 6;
	test_position_3->y = 6;
	add_to_poke_map("Bulbasaur",(void*) test_position_3);
	long_term_scheduler();*/
	printf("EL TAMAÑO DE READY ES %d\n", list_size(ready_list));

	pthread_t long_tid;

	pthread_create(&long_tid, NULL, long_thread, NULL);

	pthread_t short_tid;

	pthread_create(&short_tid, NULL, short_thread, NULL);


	pthread_join(short_tid, NULL);

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
	printf("aca no llega nunca no?\n");
	sleep(9999999999);
	exit(0);

}
