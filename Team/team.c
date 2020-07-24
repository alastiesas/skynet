/*
 * team.c
 *
 *  Created on: 11 abr. 2020
 *      Author: utnso
 */
#include "team.h"



//#include <libs/conexion.h>
//atoi(test_split[0])

int main(void)
{
	log = log_create("team.log", "team program", true, LOG_LEVEL_INFO);
	log_utils = log_create("team_utils.log", "team program", true, LOG_LEVEL_INFO);

//	//PRUEBAS DURAS
	t_position* positions = calloc(2, sizeof(t_position));
	positions->x = 9;
	positions->y = 9;
	(positions+1)->x = 1;
	(positions+1)->y = 9;
	t_message_localized* mensaje_prueba = create_message_localized(1,"Gengar",2,positions);
//
	t_position* positions2 = calloc(2, sizeof(t_position));
	positions2->x = 9;
	positions2->y = 9;
	(positions2+1)->x = 1;
	(positions2+1)->y = 9;
	t_message_localized* mensaje_prueba2 = create_message_localized(1,"Gengar",2,positions2);
//
//	serve_thread_args* argsgsgs = malloc(sizeof(serve_thread_args));//TODO TESTING
//	argsgsgs->op_code = OPERATION_LOCALIZED;//TODO TESTING
//	argsgsgs-> message = mensaje_prueba;//TODO TESTING
//	process_message(argsgsgs);//TODO TESTING
//	free(argsgsgs);
//
//	exit(0);


	config = config_create("team.config");
	initialize_global_config();
	initialize_semaphores();

	log_info(log, "inicio programa team[%d]", team_id);

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

//	printf("SE INICIARON LOS ENTRENADORES\n");
//	debug_colas();
//	t_trainer* retest_trainer = list_get(new_list, 0);
//	add_pokemon(retest_trainer, "Paralax");
//	add_pokemon(retest_trainer, "Charmander");
//	sub_pokemon(retest_trainer, "Charmander");
//	add_pokemon(retest_trainer, "Raiquaza");
//	debug_trainer(retest_trainer);
//
//	printf("trainer[%d] necesita Onix? %s\n", retest_trainer->id, trainer_needs(retest_trainer, "Onix")?"SI":"NO");
//	printf("trainer[%d] necesita Squirtle? %s\n", retest_trainer->id,trainer_needs(retest_trainer, "Squirtle")?"SI":"NO");

	initialize_global_objectives();

	serve_thread_args* argumentos = malloc(sizeof(serve_thread_args));//TODO TESTING
	argumentos->op_code = OPERATION_LOCALIZED;//TODO TESTING
	argumentos-> message = mensaje_prueba;//TODO TESTING
	process_message(argumentos);//TODO TESTING

	serve_thread_args* argumentos2 = malloc(sizeof(serve_thread_args));//TODO TESTING
	argumentos2->op_code = OPERATION_LOCALIZED;//TODO TESTING
	argumentos2-> message = mensaje_prueba2;//TODO TESTING
	process_message(argumentos);//TODO TESTING

	//exit(0);
//	debug_objective_list();
//	t_list* held_table = trainer_held_pokemons(retest_trainer);
//	printf("trainer[%d] retiene:\n", retest_trainer->id);
//	void debug_table(char* pokemon) {
//		printf("%s\n", pokemon);
//	}
//	list_iterate(held_table, &debug_table);
//
//	t_list* wait_table = trainer_waiting_pokemons(retest_trainer);
//	printf("trainer[%d] espera:\n", retest_trainer->id);
//	list_iterate(wait_table, &debug_table);



	printf("SE INICIARON LOS OBJETIVOS GLOBALES\n");
//	exit(0);



	//REEMPLAZO DE SUBSCRIBE PARA DEBUG DE DEADLOCKS!

	//FIN DEBUG DEADLOCKS
//	t_position* test_position_0 = malloc(sizeof(t_position));
//	test_position_0->x = 1;
//	test_position_0->y = 0;
//	add_to_poke_map("Raichu",(void*) test_position_0);
//*
//	t_position* test_position_1 = malloc(sizeof(t_position));
//	test_position_1->x = 1;
//	test_position_1->y = 1;
//	add_to_poke_map("uno",(void*) test_position_1);
//
//	t_position* test_position_2 = malloc(sizeof(t_position));
//	test_position_2->x = 2;
//	test_position_2->y = 2;
//	add_to_poke_map("dos",(void*) test_position_2);
//
//	t_position* test_position_3 = malloc(sizeof(t_position));
//	test_position_3->x = 6;
//	test_position_3->y = 7;
//	add_to_poke_map("tres",(void*) test_position_3);
//
//	t_position* test_position_4 = malloc(sizeof(t_position));
//	test_position_4->x = 13;
//	test_position_4->y = 13;
//	add_to_poke_map("cuatro",(void*) test_position_4);
	//*/
	printf("EL TAMAÑO DE READY ES %d\n", list_size(ready_list));

	subscribe(OPERATION_APPEARED);
	subscribe(OPERATION_LOCALIZED);
	subscribe(OPERATION_CAUGHT);
//	caught_default_behavior = true;
//	sleep(5);


	pthread_t long_tid;

	pthread_create(&long_tid, NULL, long_thread, NULL);

	pthread_t short_tid;

	printf("Creacion Hilo short\n");
	pthread_create(&short_tid, NULL, short_thread, NULL);

	//PRUEBAS
//	serve_thread_args* argumentos = malloc(sizeof(serve_thread_args));//TODO TESTING
//	argumentos->op_code = OPERATION_LOCALIZED;//TODO TESTING
//	argumentos-> message = mensaje_prueba;//TODO TESTING
//	process_message(argumentos);//TODO TESTING
	//FIN PRUEBAS

	pthread_join(long_tid, NULL);
	printf("pthread_join(long_tid, NULL);'n");

	pthread_join(short_tid, NULL);
	printf("pthread_join(short_tid, NULL);\n");

	log_info(log, "Cantidad de ciclos de CPU totales: %d", cpu_cycles);
	log_info(log, "Cantidad de cambios de contexto realizados: %d", context_changes);
	log_info(log, "deadlocks: producidos: %d\tresueltos: %d", deadlocks, solved_deadlocks);


	exit(0);

}
