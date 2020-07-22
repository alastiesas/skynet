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
	initialize_global_objectives();



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
	//subscribe(OPERATION_CAUGHT);
	caught_default_behavior = true;
//	sleep(5);


	pthread_t long_tid;

	pthread_create(&long_tid, NULL, long_thread, NULL);

	pthread_t short_tid;

	printf("Creacion Hilo short\n");
	pthread_create(&short_tid, NULL, short_thread, NULL);



	pthread_join(long_tid, NULL);
	printf("pthread_join(long_tid, NULL);'n");

	pthread_join(short_tid, NULL);
	printf("pthread_join(short_tid, NULL);\n");

	log_info(log, "Cantidad de ciclos de CPU totales: %d", cpu_cycles);
	log_info(log, "Cantidad de cambios de contexto realizados: %d", context_changes);
	log_info(log, "deadlocks: producidos: %d\tresueltos: %d", deadlocks, solved_deadlocks);


	exit(0);

}
