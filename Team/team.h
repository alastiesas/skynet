/*
 * team.h
 *
 *  Created on: 19 abr. 2020
 *      Author: utnso
 */

#ifndef TEAM_H_
#define TEAM_H_

#include"includes.h"
#include"utilities.h"
#include"team_structs.h"
#define DEADLOCK_PRIORITY 5
//---GLOBALS---

//logs: pueden desactivarse para no mostrarse en consola
//
bool probando = true;

//configuracion
t_config* config;
t_log* log;
t_log* log_utils;
uint32_t team_id;
char* ip_broker;
char* port_broker;

uint32_t time_delay = 1;//READ_ONLY
uint32_t quantum = 2;//READ_ONLY
uint32_t initial_estimate = 0;//READ_ONLY
double alpha = 0.5;//READ_ONLY
bool new_trainer_in_ready = false;//READ_ONLY
t_algorithm algorithm = FIFO;//READ_ONLY
uint32_t retry_time = 15;//READ_ONLY
uint32_t retry_count = 5;//READ_ONLY


//de aca para abajo, revisar condiciones de carrera

//team
t_dictionary* poke_map;//MUTEX = sem_poke_map
sem_t sem_poke_map;
t_list* objectives_list;//MUTEX = sem_objectives_list
sem_t sem_objectives_list;
bool caught_default_behavior = false;

//planificación
t_list* new_list;//MUTEX = sem_state_lists
t_list* ready_list;//MUTEX = sem_state_lists
t_list* block_list;//MUTEX = sem_state_lists
t_list* exec_list;//MUTEX = sem_state_lists
t_list* exit_list;//MUTEX = sem_state_lists
sem_t sem_state_lists;//todas usan el mismo semáforo
sem_t sem_scheduler;//short y long no se ejecutan a la vez
sem_t sem_short;
sem_t sem_long;
t_state_change_reason exit_cpu_reason = NO_CHANGE;//MUTEX = sem_cpu_info
bool team_succes = false;
sem_t sem_cpu;//mutex para que short y traienrs no puedan ejecutarse a la vez

//informes
uint32_t cpu_cycles = 0;//MUTEX = sem_cpu_info
uint32_t context_changes = 0;//MUTEX = sem_state_lists (el mismo que las colas)
uint32_t deadlock_priority;//Exclusivo de long_term
uint32_t deadlocks = 0;//Exclusivo de long_term -> deadlock_handler
uint32_t solved_deadlocks = 0;//Exclusivo de long_term -> deadlock_handler
sem_t sem_cpu_info;

//comunicacion
t_list* messages_list;//MUTEX = sem_messages_list | CONTADOR = sem_messages
t_dictionary* message_response;//MUTEX = sem_messages_recieve_list
sem_t sem_messages_list;
sem_t sem_messages;
sem_t sem_messages_recieve_list;

//---FIN GLOBALS---


void initialize_global_config();
void initialize_semaphores();
//entrenadores y objertivos
t_trainer* initialize_trainer(uint32_t id, char* config_position, char* onfig_objectives, char* config_pokemons);//inicializa un entrenador (pthread) en new_list
void initialize_trainers();//inicializa los entrenadores del conig
void initialize_global_objectives();
//FIN funciones iniciales

//funciones de entrenadores
void add_trainer_to_objective(t_trainer* trainer);
void trainer_thread(t_callback* callback_thread);
void trainer_assign_catch(char* pokemon, t_list* positions);
t_trainer* find_trainer_for_catch(t_position* position);
void assign_trade_couple(t_trainer* trainer1, char* pokemon1, t_trainer* trainer2, char* pokemon2);
void trainer_assign_move(t_trainer* trainer, char* pokemon, t_position* position, bool catching, uint32_t target_id);
void trainer_assign_trade(t_trainer* trainer, char* pokemon, uint32_t target_id);
//FIN funciones de entrenadores

//objetivos
bool success_global_objective();
bool pokemon_is_needed(char* pokemon,char* channel);
bool pokemon_is_needed_on_pokemap(char* pokemon);
bool pokemon_is_needed_on_trainer(char* pokemon);
//Pasar a diccionario ?
void add_objective(char* pokemon);
void add_caught(char* pokemon);
void add_catching(char* pokemon);
void sub_catching(char* pokemon);
//FIN objetivos

//transiciones
char* state_list_string(t_list* list);
void state_change(uint32_t index, t_list* from,t_list* to);
void transition_by_id(uint32_t id, t_list* from,t_list* to);
void transition_new_to_ready(uint32_t index);
void transition_ready_to_exec(uint32_t index);
void transition_exec_to_ready();
void transition_exec_to_block();
void transition_exec_to_exit();
void transition_block_to_ready(uint32_t index);
void transition_block_to_exit(uint32_t index);
//FIN transiciones

//planificacion
void long_term_scheduler();
bool possible_deadlock();
void deadlock_handler();
uint32_t deadlock_detector(t_dictionary* waiting_table, t_dictionary* held_table);
void assign_trade_couples(t_dictionary* waiting_table,t_dictionary* held_table, bool perfect);
void* short_thread();
// algoritmos de seleccion
void fifo_algorithm();
void rr_algorithm();
void sjfs_algorithm();
void sjfc_algorithm();
void exit_cpu();
char* exit_cpu_reason_string();
char* enter_cpu_reason_string();
void trainer_update_burst_estimate(t_trainer* trainer);
t_trainer* shortest_job_trainer_from_ready();
//callbacks segun algoritmo
void callback_fifo(t_trainer* trainer);
void callback_rr(t_trainer* trainer);
void callback_sjfs(t_trainer* trainer);
void callback_sjfc(t_trainer* trainer);
//FIN planificacion

//movimientos
void move_up(t_trainer* trainer);
void move_down(t_trainer* trainer);
void move_right(t_trainer* trainer);
void move_left(t_trainer* trainer);
void move(t_trainer* trainer);
//FIN movimientos

//catch
void catch(t_trainer* trainer);
void trainer_catch(t_trainer* trainer, bool result);
//FIN catch

//trade
uint32_t trade(t_trainer* trainer, uint32_t trade_cpu);
void trade_trainer(t_trainer* trainer1);
//FIN trade

//comunicación
void message_list_add_catch(t_trainer* trainer);
void* sender_thread();
void set_default_behavior(queue_code);
void process_message(serve_thread_args* args);
pthread_t subscribe(queue_code queue_code);
//FIN comunicación

//debugs
void debug_colas();
void debug_message_list();
//FIN debugs



void initialize_global_config() {

	team_id = config_get_int_value(config, "ID");

	ip_broker = config_get_string_value(config, "IP_BROKER");
	port_broker = config_get_string_value(config, "PUERTO_BROKER");
	retry_time = config_get_int_value(config, "TIEMPO_RECONEXION");
	retry_count = config_get_int_value(config, "CANTIDAD_DE_REINTENTOS_CONEXION");

	char* config_algorithm = config_get_string_value(config, "ALGORITMO_PLANIFICACION");
	algorithm = read_algorithm(config_algorithm);
	time_delay = config_get_int_value(config, "RETARDO_CICLO_CPU");
	quantum = config_get_int_value(config, "QUANTUM");
	initial_estimate = config_get_int_value(config, "ESTIMACION_INICIAL");
	if(config_has_property(config, "ALPHA")) {
		alpha = config_get_double_value(config, "ALPHA");
	}
	printf("CONFIG-> algoritmo: %s\tretardo: %d\tquantum: %d\testimacion inicial: %d\n", config_algorithm, time_delay, quantum, initial_estimate);
}

void initialize_semaphores() {
	sem_init(&sem_state_lists, 0, 1);
	sem_init(&sem_poke_map, 0, 1);
	sem_init(&sem_objectives_list, 0, 1);
	sem_init(&sem_cpu, 0, 1);
	sem_init(&sem_cpu_info, 0, 1);

}

void callback_fifo(t_trainer* trainer){
	printf("callback FIFO[%d]\n", algorithm);

	switch(trainer->action) {
	case FREE:
		//liberar cpu y llamar al short
		trainer->burst = 0;
		sem_wait(&sem_cpu_info);
		exit_cpu_reason = BURST;
		sem_post(&sem_cpu_info);
		sem_post(&sem_short);
		break;
	case MOVE:
		//continuar ejecutando o volver a ready
		sem_wait(&sem_cpu_info);
		exit_cpu_reason = NO_CHANGE;
		sem_post(&sem_cpu_info);
		sem_post(&trainer->sem_thread);
		break;
	case CATCH:
		//continuar ejecutando o volver a ready
		sem_wait(&sem_cpu_info);
		exit_cpu_reason = NO_CHANGE;
		sem_post(&sem_cpu_info);
		sem_post(&trainer->sem_thread);
		break;
	case CATCHING:
		//liberar cpu y llamar al short
		trainer->burst = 0;
		sem_wait(&sem_cpu_info);
		exit_cpu_reason = CATCH_PETITION;
		sem_post(&sem_cpu_info);
		sem_post(&sem_short);
		break;
	case TRADE:
		//liberar cpu y llamar al short
		trainer->burst = 0;
		sem_wait(&sem_cpu_info);
		exit_cpu_reason = TRADE_WAITING;
		sem_post(&sem_cpu_info);
		sem_post(&sem_short);
		break;
	case FINISH:
		//liberar cpu y llamar al short
		trainer->burst = 0;
		sem_wait(&sem_cpu_info);
		exit_cpu_reason = EXIT;
		sem_post(&sem_cpu_info);
		sem_post(&sem_short);
		break;
	default:
		sem_wait(&sem_cpu_info);
		exit_cpu_reason = NO_CHANGE;
		sem_post(&sem_cpu_info);
		sem_post(&trainer->sem_thread);
		break;
	}

}
void callback_rr(t_trainer* trainer) {
	printf("callback RR[%d]\n", algorithm);

	switch(trainer->action) {
		case FREE:
			//liberar cpu y llamar al short
			trainer->burst = 0;
			sem_wait(&sem_cpu_info);
			exit_cpu_reason = BURST;
			sem_post(&sem_cpu_info);
			sem_post(&sem_short);
			break;
		case MOVE:
			//continuar ejecutando o volver a ready
			if(trainer_full_quantum(trainer, quantum)) {
				//si se cumpleto el quantum, se desaloja
				trainer->burst = 0;
				sem_wait(&sem_cpu_info);
				exit_cpu_reason = FULL_QUANTUM;
				sem_post(&sem_cpu_info);
				sem_post(&sem_short);
			}else {
				//si no completó el quantum, continua ejecutando
				sem_wait(&sem_cpu_info);
				exit_cpu_reason = NO_CHANGE;
				sem_post(&sem_cpu_info);
				sem_post(&trainer->sem_thread);
			}
			break;
		case CATCH:
			//continuar ejecutando o volver a ready
			if(trainer_full_quantum(trainer, quantum)) {
				//si se cumpleto el quantum, se desaloja
				trainer->burst = 0;
				sem_wait(&sem_cpu_info);
				exit_cpu_reason = FULL_QUANTUM;
				sem_post(&sem_cpu_info);
				sem_post(&sem_short);
			}else {
				//si no completó el quantum, continua ejecutando
				sem_wait(&sem_cpu_info);
				exit_cpu_reason = NO_CHANGE;
				sem_post(&sem_cpu_info);
				sem_post(&trainer->sem_thread);
			}
			break;
		case CATCHING:
			//liberar cpu y llamar al short
			trainer->burst = 0;
			sem_wait(&sem_cpu_info);
			exit_cpu_reason = CATCH_PETITION;
			sem_post(&sem_cpu_info);
			sem_post(&sem_short);
			break;
		case TRADE:
			//liberar cpu y llamar al short
			trainer->burst = 0;
			sem_wait(&sem_cpu_info);
			exit_cpu_reason = TRADE_WAITING;
			sem_post(&sem_cpu_info);
			sem_post(&sem_short);
			break;
		case FINISH:
			//liberar cpu y llamar al short
			trainer->burst = 0;
			sem_wait(&sem_cpu_info);
			exit_cpu_reason = EXIT;
			sem_post(&sem_cpu_info);
			sem_post(&sem_short);
			break;
		default:
			sem_wait(&sem_cpu_info);
			exit_cpu_reason = NO_CHANGE;
			sem_post(&sem_cpu_info);
			sem_post(&trainer->sem_thread);
			break;
		}
}
void callback_sjfs(t_trainer* trainer) {
	printf("callback SJF-SD[%d]\n", algorithm, trainer_burst_estimate(trainer));

	switch(trainer->action) {
		case FREE:
			//liberar cpu y llamar al short
			trainer_update_burst_estimate(trainer);
			trainer->burst = 0;
			sem_wait(&sem_cpu_info);
			exit_cpu_reason = BURST;
			sem_post(&sem_cpu_info);
			sem_post(&sem_short);
			break;
		case MOVE:
			//continuar ejecutando o volver a ready
			sem_wait(&sem_cpu_info);
			exit_cpu_reason = NO_CHANGE;
			sem_post(&sem_cpu_info);
			sem_post(&trainer->sem_thread);
			break;
		case CATCH:
			//continuar ejecutando o volver a ready
			sem_wait(&sem_cpu_info);
			exit_cpu_reason = NO_CHANGE;
			sem_post(&sem_cpu_info);
			sem_post(&trainer->sem_thread);
			break;
		case CATCHING:
			//liberar cpu y llamar al short
			trainer_update_burst_estimate(trainer);
			trainer->burst = 0;
			sem_wait(&sem_cpu_info);
			exit_cpu_reason = CATCH_PETITION;
			sem_post(&sem_cpu_info);
			sem_post(&sem_short);
			break;
		case TRADE:
			//liberar cpu y llamar al short
			trainer_update_burst_estimate(trainer);
			trainer->burst = 0;
			sem_wait(&sem_cpu_info);
			exit_cpu_reason = TRADE_WAITING;
			sem_post(&sem_cpu_info);
			sem_post(&sem_short);
			break;
		case FINISH:
			//liberar cpu y llamar al short
			trainer_update_burst_estimate(trainer);
			trainer->burst = 0;
			sem_wait(&sem_cpu_info);
			exit_cpu_reason = EXIT;
			sem_post(&sem_cpu_info);
			sem_post(&sem_short);
			break;
		default:
			sem_wait(&sem_cpu_info);
			exit_cpu_reason = NO_CHANGE;
			sem_post(&sem_cpu_info);
			sem_post(&trainer->sem_thread);
			break;
	}

}
void callback_sjfc(t_trainer* trainer) {
	printf("callback SJF-CD[%d], estimado: %d, posible desalojo: %d\n", algorithm, trainer_burst_estimate(trainer), new_trainer_in_ready);

	switch(trainer->action) {
		case FREE:
			//liberar cpu y llamar al short
			trainer_update_burst_estimate(trainer);
			trainer->burst = 0;
			sem_wait(&sem_cpu_info);
			exit_cpu_reason = BURST;
			sem_post(&sem_cpu_info);
			sem_post(&sem_short);
			break;
		case MOVE:
			//continuar ejecutando o volver a ready
			if(new_trainer_in_ready) {
				t_trainer* sj_trainer = shortest_job_trainer_from_ready();
				if(trainer_burst_estimate(sj_trainer) < trainer_burst_estimate(trainer)){
					//Uno de los nuevos en ready tiene menor estimado -> se desaloja (sin actualizar el estimado
					sem_wait(&sem_cpu_info);
					exit_cpu_reason = SJF_PRIORITY;
					sem_post(&sem_cpu_info);
					sem_post(&sem_short);
				} else {//Ninguno con menor estimado -> no se desaloja
					new_trainer_in_ready = false;
					sem_wait(&sem_cpu_info);
					exit_cpu_reason = NO_CHANGE;
					sem_post(&sem_cpu_info);
					sem_post(&trainer->sem_thread);
				}
			} else {//flag en false -> imposible ser desalojado
				sem_wait(&sem_cpu_info);
				exit_cpu_reason = NO_CHANGE;
				sem_post(&sem_cpu_info);
				sem_post(&trainer->sem_thread);
			}
			break;
		case CATCH:
			//continuar ejecutando o volver a ready
			if(new_trainer_in_ready) {
				t_trainer* sj_trainer = shortest_job_trainer_from_ready();
				if(trainer_burst_estimate(sj_trainer) < trainer_burst_estimate(trainer)){
					//Uno de los nuevos en ready tiene menor estimado -> se desaloja (sin actualizar el estimado
					sem_wait(&sem_cpu_info);
					exit_cpu_reason = SJF_PRIORITY;
					sem_post(&sem_cpu_info);
					sem_post(&sem_short);
				} else {//Ninguno con menor estimado -> no se desaloja
					new_trainer_in_ready = false;
					sem_wait(&sem_cpu_info);
					exit_cpu_reason = NO_CHANGE;
					sem_post(&sem_cpu_info);
					sem_post(&trainer->sem_thread);
				}
			} else {//flag en false -> imposible ser desalojado
				sem_wait(&sem_cpu_info);
				exit_cpu_reason = NO_CHANGE;
				sem_post(&sem_cpu_info);
				sem_post(&trainer->sem_thread);
			}
			break;
		case CATCHING:
			//liberar cpu y llamar al short
			trainer_update_burst_estimate(trainer);
			trainer->burst = 0;
			sem_wait(&sem_cpu_info);
			exit_cpu_reason = CATCH_PETITION;
			sem_post(&sem_cpu_info);
			sem_post(&sem_short);
			break;
		case TRADE:
			//liberar cpu y llamar al short
//			trainer_update_burst_estimate(trainer);
//			trainer->burst = 0;
//			exit_cpu_reason = TRADE_WAITING;
//			sem_post(&sem_short);
			sem_wait(&sem_cpu_info);
			exit_cpu_reason = NO_CHANGE;
			sem_post(&sem_cpu_info);
			sem_post(&trainer->sem_thread);
			break;
		case FINISH:
			//liberar cpu y llamar al short
			trainer_update_burst_estimate(trainer);
			trainer->burst = 0;
			sem_wait(&sem_cpu_info);
			exit_cpu_reason = EXIT;
			sem_post(&sem_cpu_info);
			sem_post(&sem_short);
			break;
		default:
			sem_wait(&sem_cpu_info);
			exit_cpu_reason = NO_CHANGE;
			sem_post(&sem_cpu_info);
			sem_post(&trainer->sem_thread);
			break;
	}

}

t_trainer* initialize_trainer(uint32_t id, char* config_position, char* onfig_objectives, char* config_pokemons)
{
	t_trainer* trainer = create_trainer_from_config(id, config_position, onfig_objectives, config_pokemons);
	trainer->burst_estimate = initial_estimate;
	t_callback* callback_thread = malloc(sizeof(t_callback));
	callback_thread->trainer = trainer;


	switch(algorithm) {
		case FIFO:
			callback_thread->callback = &callback_fifo;
			break;
		case RR:
			callback_thread->callback = &callback_rr;
			break;
		case SJFS:
			callback_thread->callback = &callback_sjfs;
			break;
		case SJFC:
			callback_thread->callback = &callback_sjfc;
			break;
		default:
			callback_thread->callback = &callback_fifo;
			break;
	}
	pthread_create(&(trainer->tid), NULL, trainer_thread, callback_thread);



	return trainer;
}


void initialize_trainers()
{
	printf("\nINICIALIZANDO ENTRENADORES\n");
	char** positions_config = config_get_array_value(config,"POSICIONES_ENTRENADORES");
	char** objectives_config = config_get_array_value(config,"OBJETIVOS_ENTRENADORES");
	char** pokemons_config = config_get_array_value(config,"POKEMON_ENTRENADORES");
	bool no_more_pokemons = false;
	int i = 0;
	while(positions_config[i] != NULL){
		t_trainer* trainer = NULL;
		if(no_more_pokemons) {
			if(pokemons_config[i] == NULL) {
				no_more_pokemons = true;
				trainer = initialize_trainer(i+1, positions_config[i], objectives_config[i], NULL);
			} else {
				trainer = initialize_trainer(i+1, positions_config[i], objectives_config[i], pokemons_config[i]);
			}
		} else {
			trainer = initialize_trainer(i+1, positions_config[i], objectives_config[i], NULL);
		}

		sem_wait(&sem_state_lists);
		if(trainer_success_objective(trainer))
			list_add(exit_list, trainer);
		else if(trainer_full(trainer)) {
			list_add(block_list, trainer);
		} else
			list_add(new_list, trainer);
		sem_post(&sem_state_lists);

		i++;
	}
	//liberar memoria (los char**)
	//free_string_list(positions_config);
	//free_string_list(objectives_config);
	//free_string_list(pokemons_config);

	printf("TODOS LOS ENTRENADORES HAN SIDO CONFIGURADOS\n");
	//TODO BORRAR LISTAS LEVANTADAS DEL CONFIG
}




void add_objective(char* pokemon)
{
	sem_wait(&sem_objectives_list);
	t_objective* objective = find_key(objectives_list, pokemon);
	if(objective != NULL)
		objective->count++;
	else{
		objective = malloc(sizeof(t_objective));
		objective->pokemon = create_copy_string(pokemon);
		objective->count = 1;
		objective->caught = 0;
		objective->catching = 0;
		list_add(objectives_list,(void*)objective);
	}
	sem_post(&sem_objectives_list);
}

void add_caught(char* pokemon)
{
	sem_wait(&sem_objectives_list);
	t_objective* objective = find_key(objectives_list, pokemon);
	if(objective != NULL){
		objective->caught++;
		if(objective->catching>0)
			objective->catching--;
	}
	else {
		objective = malloc(sizeof(t_objective));
		objective->pokemon = create_copy_string(pokemon);
		objective->count = 0;
		objective->caught = 1;
		objective->catching = 0;
		list_add(objectives_list,(void*)objective);

	}
	sem_post(&sem_objectives_list);
}

void add_catching(char* pokemon)
{
	t_objective* objective = find_key(objectives_list, pokemon);
	if(objective != NULL){
		objective->catching++;
	}
	else
		printf("Lo rompiste todo, maldito idiota add_catching\n");
}

void sub_catching(char* pokemon)
{
	sem_wait(&sem_objectives_list);
	t_objective* objective = find_key(objectives_list, pokemon);
	printf("ENTRO EN EL SUB CATCHING\n");
	if(objective != NULL){
		if(objective->catching>0)
			objective->catching--;
	}
	else
		printf("Lo rompiste todo, maldito idiota sub_catching\n");

	sem_post(&sem_objectives_list);
	printf("SALE DE sub_catching\n");
}


bool pokemon_is_needed(char* pokemon,char* channel)
{
	printf("Consultando si %s esta en la lista de objetivos\n", pokemon);
	//t_objective* test = (t_objective*) list_get(objectives_list,0);
	//printf("LN 301 the pokemon OBJECTIVE is %s\n", test->pokemon);
	bool needed = 0;
	sem_wait(&sem_objectives_list);
	t_objective* objective = find_key(objectives_list, pokemon);
	sem_post(&sem_objectives_list);

	if(objective == NULL)
		printf("no necesitamos un %s\n", pokemon);
	else{

		printf("si necesitamos un %s\n", pokemon);
		if(strcmp(channel,"trainer") == 0)
			needed = objective->count > (objective->caught + objective->catching);
		else
			needed = 1;
	}

	//DEBUG DE PRUEBA
	/*
	printf("objective count ->> %d\n",objective->count);
	printf("objective caught ->> %d\n",objective->caught);
	printf("objective catching ->> %d\n",objective->catching);
	printf("result is %d\n",needed);
	*/
	//return (objective->count > (objective->caught + objective->catching));
	return needed;
}

bool pokemon_is_needed_on_trainer(char* pokemon)
{
	return pokemon_is_needed(pokemon,"trainer");
}

bool pokemon_is_needed_on_pokemap(char* pokemon)
{

	return pokemon_is_needed(pokemon,"pokemap");
}

void add_trainer_to_objective(t_trainer* trainer)
{
	int i = 0;
	while(trainer->objectives[i]!= NULL){
		add_objective(trainer->objectives[i]);
		i++;
	}
	i = 0;
	while(trainer->pokemons[i]!= NULL){
		add_caught(trainer->pokemons[i]);
		i++;
	}
}

void initialize_global_objectives()
{
	sem_wait(&sem_objectives_list);
	objectives_list = list_create();
	sem_post(&sem_objectives_list);

	sem_wait(&sem_state_lists);
	list_iterate(new_list, &add_trainer_to_objective);
	list_iterate(block_list, &add_trainer_to_objective);
	sem_post(&sem_state_lists);

}

bool success_global_objective()
{
	bool success = false;

	sem_wait(&sem_objectives_list);
	bool all_pokemon_catch = list_all_satisfy(objectives_list,&success_objective);
	sem_post(&sem_objectives_list);

	 if(all_pokemon_catch) {

		sem_wait(&sem_state_lists);
		 if(list_size(block_list) == 0 && list_size(new_list) == 0 && list_size(ready_list) == 0 && list_size(exec_list) == 0){
			 success = true;
		 }
		sem_post(&sem_state_lists);

	 }
	return success;
}

void trainer_thread(t_callback* callback_thread)
{
	//if(/*si es 0 menor*/)
		//funcion cambiar valor global de variable interrumption
		//COMO SE DESALOJA A UN HILO DE ENTENADOR, COMO SE ENTERA EL PLANIFICADOR O EL HILO DE EJECUCION!!
		//while(1) agregar
	t_trainer* trainer = callback_thread->trainer;

	printf("hola soy el entrenador %d\n", (int)trainer->tid);
	debug_trainer(trainer);
	uint32_t trade_cpu = 0;
	while(trainer->action != FINISH){
		sem_wait(&trainer->sem_thread);
		sem_wait(&sem_cpu);

		switch(trainer->action){
			case MOVE:
				printf("trainer[%d]->(comando MOVE), objetivo: [%s (%d, %d)]\n", trainer->id, trainer->target->pokemon, trainer->target->position->x, trainer->target->position->y);
				//t_position* previous_position = create_position(trainer->position->x, trainer->position->y);
				char* previous_string = malloc(sizeof(char)*10);
				sprintf(previous_string, "%d, %d", trainer->position->x, trainer->position->y);
				//aca va un if, no un while
				if(trainer->position->x != trainer->target->position->x || trainer->position->y != trainer->target->position->y){
					move(trainer);
					log_info(log, "trainer[%d] operación: movimiento (%s) -> (%d, %d)", trainer->id, previous_string, trainer->position->x,trainer->position->y);
				}
				else if(trainer->target->catching)
					trainer->action = CATCH;
				else
					trainer->action = TRADE;

				free(previous_string);

				break;
			case CATCH:
				log_info(log, "trainer[%d] operación: atrapar %s en (%d, %d)", trainer->id, trainer->target->pokemon, trainer->position->x, trainer->position->y);
				catch(trainer);

				break;
			case CATCHING:
				printf("Estoy atrapando pokemon, comando CATCHING\n");
				break;
			case TRADE:
				trade_cpu = trade(trainer,trade_cpu);
				printf("Estoy tradeando pokemon, comando TRADE\n");
				break;
			case FINISH:

				//en teoria nunca llega acá. . .
				//pthread_exit(NULL);
				break;
			default:
				printf("No hago nada\n");
//				trainer->action = FREE;
				break;
		}
		sem_post(&sem_cpu);
		callback_thread->callback(trainer);
	}


	printf("HILO debug del entrenador %s\n", trainer->objectives[0]);
	//pthread_mutex_unlock(trainer->semThread);
	//sem_post(&trainer->sem_thread);
	printf("trainer[%d] destruido\n", trainer->id);
	log_info(log, "trainer[%d] finalizado con exito cantidad de ciclos utilizados: %d", trainer->id, trainer->cpu_cycles);
	destroy_trainer(trainer);
}

//encuentra al entrenador mas cerca de la posicion en ambas listas

//compara la posicion de los 2 entrenadores y la lista.

//setea al entrenador, el pokemon y la distancia objetivo

//dependiendo de la lista en la que este hace el transition.

//VAMOS POR ACA FEDE!!!!!!!!! 13/05/2020
//large_term_scheduler
//void 		  dictionary_iterator(t_dictionary *, void(*closure)(char*,void*));



void long_thread() {
	sem_init(&sem_long, 0, 0);
	sem_init(&sem_scheduler, 0, 1);
	//sem post para pruebas
	sem_post(&sem_long);
	while(!team_succes){
		if(success_global_objective()) {
			printf("OBJETIVOS GLOBALES CUMPLIDOS, TERMINA EL PROGRAMA!!!!!!\n");
			printf("OBJETIVOS GLOBALES CUMPLIDOS, TERMINA EL PROGRAMA!!!!!!\n");
			team_succes = true;
		}else {
			printf("NO SE CUMPLIERON LOS OBJETIVOS GLOBALES\n");
			printf("NO SE CUMPLIERON LOS OBJETIVOS GLOBALES\n");
			sleep(2);
		}

		sem_wait(&sem_long);
		sem_wait(&sem_scheduler);
		//printf("esta aca??\n");
		long_term_scheduler();
		sem_post(&sem_scheduler);

		sem_post(&sem_short);

	}
	log_info(log, "TERMINO EL LONG------------------------------------");

}

void* short_thread()
{
	void (*short_term_scheduler)(void);
	switch(algorithm) {
	case FIFO:
		short_term_scheduler = &fifo_algorithm;
		break;
	case RR:
		short_term_scheduler = &rr_algorithm;
		break;
	case SJFS:
		short_term_scheduler = &sjfs_algorithm;
		break;
	case SJFC:
		short_term_scheduler = &sjfc_algorithm;
		break;
	default:
		printf("no se leyo el algoritmo en el config, default: FIFO\n");
		short_term_scheduler = &fifo_algorithm;
		break;
	}
	sem_init(&sem_short, 0, 0);

	while(!team_succes){

		if(success_global_objective()) {
			printf("OBJETIVOS GLOBALES CUMPLIDOS, TERMINA EL PROGRAMA!!!!!!\n");
			printf("OBJETIVOS GLOBALES CUMPLIDOS, TERMINA EL PROGRAMA!!!!!!\n");
			team_succes = true;
		}else {
			printf("NO SE CUMPLIERON LOS OBJETIVOS GLOBALES\n");
			printf("NO SE CUMPLIERON LOS OBJETIVOS GLOBALES\n");
			sleep(2);
		}

		sem_wait(&sem_short);
		sem_wait(&sem_cpu);
		sem_wait(&sem_scheduler);
		//printf("esta wacho aca??\n");
		//debug_message_list();
		short_term_scheduler();
		sem_post(&sem_scheduler);
		sem_post(&sem_cpu);
	}
}

void long_term_scheduler(){

	//PRUEBAAAAA



	//revisar size ready
	sem_wait(&sem_state_lists);
	uint32_t size_ready = list_size(ready_list);
	sem_post(&sem_state_lists);
	sem_wait(&sem_poke_map);
	dictionary_iterator(poke_map, &trainer_assign_catch);
	sem_post(&sem_poke_map);

	sem_wait(&sem_state_lists);
	if(list_size(ready_list) == size_ready) {
		//aumento el size del ready? SINO CORRO DEAD LOCK
		deadlock_priority++;
	}

	bool idle_cpu = list_size(ready_list) == 0 && list_size(exec_list) == 0;
	sem_post(&sem_state_lists);

	//debug_colas();


	if(idle_cpu || deadlock_priority == DEADLOCK_PRIORITY) {
		deadlock_priority = 0;//resetear prioridad
		if(idle_cpu) {
		log_info(log, "inicio de algoritmo de deteccion de deadlocks razon: CPU ociosa");

		} else {
			log_info(log, "inicio de algoritmo de deteccion de deadlocks razon: se corrío %d veces el planificador de largo plazo sin poder agregar entrenadores a ready", DEADLOCK_PRIORITY);

		}

		if(possible_deadlock()) {

			deadlock_handler();
		}else {
			printf("NO HAY DEADLOCK ---------------------------\n");
		}
	} else {
		printf("NO HAY RAZON PARA EVALUAR DEADLOCKS\n");
	}
	//TODO completarlo: que pasa cuando no tenemos posiciones en el pokemap
}

bool possible_deadlock(){

	sem_wait(&sem_state_lists);
	int32_t locked = list_count_satisfying(block_list, &trainer_locked);
	sem_post(&sem_state_lists);
	printf("\t\tlocked trainers: %d\n", locked);
	bool deadlock = locked > 1;
	return deadlock;
}


void deadlock_handler(){
	printf("PRIMERO PAREJAS PERFECTAS\n");

	t_dictionary* waiting_table = dictionary_create();
	t_dictionary* held_table = dictionary_create();




	uint32_t locked_trainers = deadlock_detector(waiting_table,held_table);

	log_info(log, "entrenadores interbloqueados: %d, pokemon (especies) retenidos: %d, pokemon (especies) en espera: %d", locked_trainers, dictionary_size(held_table), dictionary_size(waiting_table));
	uint32_t deadlocks_before = deadlocks;

	//fixeamos parejas perfectas

	void debug_table(char* key, t_list* table) {
		void debug_list(t_trainer* trainer) {
			printf(" [%d] ", trainer->id);
		}

		printf("%s: ", key);
		list_iterate(table, &debug_list);
		printf("\n");
	}

//	printf("TABLA DE WAITING:\n");
//	dictionary_iterator(waiting_table, &debug_table);
//	printf("TABLA DE HELD:\n");
//	dictionary_iterator(held_table, &debug_table);

	assign_trade_couples(waiting_table,held_table, true);

	printf("AHORA PAREJAS SIMPLES\n");
	dictionary_destroy_and_destroy_elements(waiting_table, &list_destroy);
	dictionary_destroy_and_destroy_elements(held_table, &list_destroy);

	waiting_table = dictionary_create();
	held_table = dictionary_create();
//	debug_colas();

	deadlock_detector(waiting_table,held_table);

//	printf("TABLA DE WAITING:\n");
//	dictionary_iterator(waiting_table, &debug_table);
//	printf("TABLA DE HELD:\n");
//	dictionary_iterator(held_table, &debug_table);
	assign_trade_couples(waiting_table,held_table, false);

	dictionary_destroy_and_destroy_elements(waiting_table, &list_destroy);
	dictionary_destroy_and_destroy_elements(held_table, &list_destroy);

	uint32_t deadlocks_found = deadlocks - deadlocks_before;
	if(deadlocks_found > 0) {
		if(deadlocks_found > 1) {
		log_info(log, "resultado de algoritmo de detección de deadlocks: se encontraron %d deadlocks para resolver", deadlocks - deadlocks_before);
		} else {
			log_info(log, "resultado de algoritmo de detección de deadlocks: se encontró %d deadlock para resolver", deadlocks - deadlocks_before);
		}
	} else {
		log_info(log, "resultado de algoritmo de detección de deadlocks: no se encontraron deadlocks (los entrenadores que se encuentren bloqueados no se bloquean mutuamente)");
	}

}

uint32_t deadlock_detector(t_dictionary* waiting_table, t_dictionary* held_table) {
	printf("DEADLOCKDETECTOR\n");
	//tomo los entrenadores que esten interbloqueados
	sem_wait(&sem_state_lists);
	t_list* locked_trainers = list_filter(block_list, &trainer_locked);
	sem_post(&sem_state_lists);

	printf("ENTRENADORES BLOQUEADOS: %d\n", list_size(locked_trainers));


//	list_iterate(locked_trainers, &debug_trainer);

	void add_to_tables(t_trainer* trainer) {
		//uint32_t* id = malloc(sizeof(uint32_t));
		//*id = trainer->id;

		t_list* waiting_pokemons = trainer_waiting_pokemons(trainer);
		t_list* held_pokemons = trainer_held_pokemons(trainer);

		void add_to_waiting(char* pokemon) {
			if(dictionary_has_key(waiting_table, pokemon)){
				t_list* trainers  = dictionary_get(waiting_table, pokemon);
				list_add(trainers ,trainer);
			} else {
				t_list* trainers = list_create();
				list_add(trainers ,trainer);
				dictionary_put(waiting_table,pokemon, trainers);
			}

		}
		void add_to_held(char* pokemon) {
			t_list* trainers;
			if(dictionary_has_key(held_table, pokemon)){
				trainers  = dictionary_get(held_table, pokemon);
				list_add(trainers ,trainer);
			} else {
				trainers = list_create();
				list_add(trainers ,trainer);
				dictionary_put(held_table, pokemon, trainers);
			}

		}

		list_iterate(held_pokemons, &add_to_held);

		list_iterate(waiting_pokemons, &add_to_waiting);
	}

	list_iterate(locked_trainers, &add_to_tables);

	return list_size(locked_trainers);
	//printf("dictionary has %d\n",dictionary_has_key(waiting_table,"Pidgey"));

	//printf("trainer has charmander %d\n",list_get((dictionary_get(waiting_table, "Pidgey")),0));

	//buscar posibles intercambios parejas > indivuales

}

t_trainer* closest_couple(t_trainer* trainer, t_list* holding_trainers_list) {

	t_trainer* trainer_couple = NULL;
	int32_t trainer_couple_index = closest_free_trainer_deadlock(holding_trainers_list, trainer->position);

	if(trainer_couple_index>=0)//si encontro un closest lo asigna, sino queda en null
		trainer_couple = list_get(holding_trainers_list, trainer_couple_index);

	return trainer_couple;
}

t_trainer* closest_perfect_couple(t_trainer* trainer, t_list* holding_trainers_list) {
	t_list* trainer_holds = trainer_held_pokemons(trainer);
	printf("\n");

	bool matches(t_trainer* possible_couple) {

		bool trainer_needs_any(char* pokemon) {
			return trainer_needs(possible_couple, pokemon);
		}

		return list_any_satisfy(trainer_holds, &trainer_needs_any);//trade perfecto solo si necesita alguno de los que holdea el trainer
	}

	t_list* possible_copuples_list = list_filter(holding_trainers_list, &matches);//filtro por quienes pueden hacer un trade perfecto

	return closest_couple(trainer, possible_copuples_list);

}

void assign_trade_couples(t_dictionary* waiting_table,t_dictionary* held_table, bool perfect)
{
	//RECORRO LISTA DE TRAINERS, por cada trainer encuentro su pareja en la tabla de waiting y guardo la que tenga menor distancia.

	void assign_trade_first(char* pokemon,t_list* trainer_list){
		//POR CADA ENTRENADOR ENTRO CON EL
		void assing_trade_second(t_trainer* trainer){
			if(dictionary_has_key(held_table,pokemon)){
				//aca me traigo los tainers de held y tengo que comparar en cada uno la distancia.
				t_trainer* trainer_couple = NULL;
				if(perfect){
					trainer_couple = closest_perfect_couple(trainer, dictionary_get(held_table,pokemon));
				}
				else{
					trainer_couple = closest_couple(trainer, dictionary_get(held_table,pokemon));
				}
				//aca buscar segun perfect o no
				if(trainer_couple != NULL){
//					printf("SE SELECCIONO COMO COUPLE A\n");
//					debug_trainer(trainer_couple);
//					printf("PARA\n");
//					debug_trainer(trainer);
					if(trainer->action == FREE && trainer_couple->action == FREE){
						char* pokemon_from_held = NULL;
						t_list* trainer_held_pokemons_list = trainer_held_pokemons(trainer);
						if(perfect) {
							bool couple_needs(char* one_pokemon) {
								return trainer_needs(trainer_couple, one_pokemon);
							}
							t_list* couple_match_pokemons = list_filter(trainer_held_pokemons_list, couple_needs);
							pokemon_from_held = list_get(couple_match_pokemons,0);

						}else {
							pokemon_from_held = list_get(trainer_held_pokemons_list,0);
						}


						assign_trade_couple(trainer, pokemon_from_held, trainer_couple, pokemon);
					}
				}
			}

		}
		list_iterate(trainer_list,&assing_trade_second);
	}



	//list_iterate(waiting_table, &assign_trade)
	//ITERO SOBRE LOS QUE QUIERO
	dictionary_iterator(waiting_table, &assign_trade_first);
	//ITERO WAITING por cada uno, entro a held y agarro al trainer mas cercano. Luego a uno le pongo action MOVE (al destino) y al otro TRADE
	//para la seleccion ambos trainner deben estar en FREE
	//list_iterate(, void(*closure)(void*));

}//assign_trade_couples

void assign_trade_couple(t_trainer* trainer1, char* pokemon1, t_trainer* trainer2, char* pokemon2) {
	printf("DEBE REALIZARSE EL SIGUIENTE INTERCAMBIO\n");

	deadlocks++;
	log_info(log, "deadlock detectado -> intecambio planificado:\ntrainer[%d] ->%s-> trainer[%d]\ntrainer[%d] ->%s-> trainer[%d]", trainer1->id, pokemon1, trainer2->id, trainer2->id, pokemon2, trainer1->id);

//	printf("trainer[%d] ->%s-> trainer[%d]\n", trainer1->id, pokemon1, trainer2->id);
//	printf("trainer[%d] ->%s-> trainer[%d]\n", trainer2->id, pokemon2, trainer1->id);

	trainer_assign_move(trainer1, pokemon1, trainer2->position, 0, trainer2->id);
	trainer_assign_trade(trainer2, pokemon2, trainer1->id);

//	printf("\tassign_trade_couple\n");
//	debug_trainer(trainer1);
//	debug_trainer(trainer2);
//	printf("\tassign_trade_couple\n");


}//*/
/*
void short_term_scheduler()
{

	switch(algorithm){
		case FIFO:
			//printf("estoy en fifo (short term)1\n");
			fifo_algorithm();
			//printf("estoy en fifo (short term)2\n");
			break;
		case RR:
			rr_algorithm();
			break;
		case SJFS:
			sjfs_algorithm();
			break;
		case SJFC:
			sjfc_algorithm();
			break;
		default:
			printf("Estoy en nada\n");
			break;
	}

	// lo unico que hace es mueve de ready to exec
}*/

void fifo_algorithm()
{
	printf("fifo_algorithm\n");

	exit_cpu();

	sem_wait(&sem_state_lists);
	uint32_t ready_size =  list_size(ready_list);
	sem_post(&sem_state_lists);

	if(ready_size > 0){
		transition_ready_to_exec(0);//toma el primero de la cola ready
	}
	else{
		//toma el mando el long
		sem_post(&sem_long);
	}

}

void rr_algorithm()
{
	printf("rr_algorithm\n");

	exit_cpu();

	sem_wait(&sem_state_lists);
	uint32_t ready_size =  list_size(ready_list);
	sem_post(&sem_state_lists);
	if(ready_size > 0){
		transition_ready_to_exec(0);//toma el primero de la cola ready
	}
	else{
		//toma el mando el long
		sem_post(&sem_long);
	}
}

void sjfs_algorithm()
{
	printf("sjfs_algorithm\n");

	exit_cpu();

	sem_wait(&sem_state_lists);
	uint32_t ready_size =  list_size(ready_list);
	sem_post(&sem_state_lists);
	if(ready_size > 0){
		t_trainer* trainer = shortest_job_trainer_from_ready();
		if(trainer != NULL) {
			transition_ready_to_exec_by_trainer(trainer);
//			transition_by_id(trainer->id, ready_list, exec_list);
//			sem_post(&trainer->sem_thread);
		}
	}
	else{
		//toma el mando el long
		sem_post(&sem_long);
	}
}

void sjfc_algorithm()
{
	printf("sjfc_algorithm\n");
	new_trainer_in_ready = false;

	exit_cpu();

	sem_wait(&sem_state_lists);
	uint32_t ready_size =  list_size(ready_list);
	sem_post(&sem_state_lists);
	if(ready_size > 0){
		t_trainer* trainer = shortest_job_trainer_from_ready();
		if(trainer != NULL) {
			transition_ready_to_exec_by_trainer(trainer);
//			transition_by_id(trainer->id, ready_list, exec_list);
//			sem_post(&trainer->sem_thread);
		}
	} else{
		//toma el mando el long
		sem_post(&sem_long);
	}
}

void exit_cpu() {

	sem_wait(&sem_state_lists);
	t_trainer* trainer = NULL;
	if(list_size(exec_list) > 0) {
		trainer = list_get(exec_list, 0);
	}
	sem_post(&sem_state_lists);

	if(trainer != NULL) {
		char* state_change_log = NULL;
		switch(trainer->action) {
			case FREE://pasa a block
				state_change_log = create_copy_string("exec -> block");
				transition_exec_to_block();
				break;
			case MOVE://no terminó, vuelve a ready
				state_change_log = create_copy_string("exec -> ready");
				transition_exec_to_ready();
				break;
			case CATCH://no terminó, vuelve a ready
				state_change_log = create_copy_string("exec -> ready");
				transition_exec_to_ready();
				break;
			case CATCHING://pasa a block
				state_change_log = create_copy_string("exec -> block");
				transition_exec_to_block();
				break;
			case TRADE://pasa a block
				state_change_log = create_copy_string("exec -> block");
				transition_exec_to_block();
				break;
			case FINISH://pasa a exit
				state_change_log = create_copy_string("exec -> exit");
				transition_exec_to_exit();
				break;
		}//fin switch


		sem_wait(&sem_cpu_info);
		int reason_int = exit_cpu_reason;
		sem_post(&sem_cpu_info);
		if(reason_int != NO_CHANGE) {
			char* reason = exit_cpu_reason_string();
			log_info(log, "trainer[%d] cambio de estado (%s), razon: %s", trainer->id, state_change_log, reason);
			free(reason);
		}

		free(state_change_log);
	}//fin if

}

char* exit_cpu_reason_string() {
	char* reason = NULL;
	sem_wait(&sem_cpu_info);
	int reason_int = exit_cpu_reason;
	sem_post(&sem_cpu_info);
	switch(reason_int) {
	case NO_CHANGE:
		reason = create_copy_string("sin cambios");
		break;
	case BURST:
		reason = create_copy_string("fin de ráfaga");
		break;
	case CATCH_PETITION:
		reason = create_copy_string("fin de ráfaga, catch");
		break;
	case TRADE_WAITING:
		reason = create_copy_string("fin de ráfaga, intercambio");
		break;
	case FULL_QUANTUM:
		reason = create_copy_string("desalojo por fin de quantum");
		break;
	case SJF_PRIORITY:
		reason = create_copy_string("desalojo por prioridad SJF");
		break;
	default:
		reason = create_copy_string("default");
		break;
	}
	return reason;
}

char* enter_cpu_reason_string() {
	char* reason = NULL;
	switch(algorithm) {
		case FIFO:
			reason = create_copy_string("[FIFO] primero en ready");
			break;
		case RR:
			reason = create_copy_string("[RR] primero en ready");
			break;
		case SJFS:
			reason = create_copy_string("[SJF-SD] menor estimado de siguiente ráfaga");
			break;
		case SJFC:
			reason = create_copy_string("[SJF-CD] menor estimado de siguiente ráfaga");
			break;
		}
	return reason;
}

void trainer_update_burst_estimate(t_trainer* trainer) {

	trainer->burst_estimate = alpha * trainer->burst + (1-alpha) * trainer->burst_estimate;
}

t_trainer* shortest_job_trainer_from_ready() {
	t_trainer* shortest_job_trainer = NULL;
	uint32_t shortest_estimate = -1;
	void shortest_estimate_trainer(t_trainer* trainer) {
		if(shortest_estimate < 0 || trainer_burst_estimate(trainer) < shortest_estimate) {
			shortest_job_trainer = trainer;
			shortest_estimate = trainer->burst_estimate;
		}
	}
	sem_wait(&sem_state_lists);
	list_iterate(ready_list, &shortest_estimate_trainer);
	sem_post(&sem_state_lists);
	return shortest_job_trainer;
}

void trainer_assign_move(t_trainer* trainer, char* pokemon, t_position* position, bool catching, uint32_t target_id /*0 = no trainer*/)
{
	//trainer_assign_move asigna el target y envía el trainer a ready
//	printf("\nse asignara al entrenador[%d] a %s al pokemon %s, en la posicion (%d, %d)\n", trainer->id, catching?"atrapar":"intercambiar", pokemon, position->x, position->y);
	trainer->action = MOVE;
	trainer_set_target(trainer, create_target(pokemon, position, target_id, catching));
//	debug_trainer(trainer);
	transition_from_id_to_ready(trainer->id);
}

void trainer_assign_trade(t_trainer* trainer, char* pokemon, uint32_t target_id) {
	trainer->action = TRADE;
	trainer_set_target(trainer, create_target(pokemon, trainer->position, target_id, 0));
}

void trainer_assign_catch(char* pokemon, t_list* positions)
{
	//char* pokemon = malloc((strlen(key)+1)*sizeof(char));
	//memcpy(pokemon, key, strlen(key)+1);//sin esto rompe

	t_link_element* element = positions->head;
	t_position* position;

	void assign_closest_trainer(t_position* position) {
		//t_list* list_from = NULL;
		printf("\n---Buscar entrenador más cercano a %s (%d, %d) en la cola NEW---\n", pokemon, position->x, position->y);
		t_trainer* selected_trainer = find_trainer_for_catch(position);

		if(selected_trainer != NULL) {
			add_catching(pokemon);



			trainer_assign_move(selected_trainer, pokemon, position, 1, 0);
			bool condition(t_position* iterate_position) {
				return (distance(position, iterate_position) == 0);
			}
			printf("ENRENADOR ASIGNADO, SE ELIMINARÁ LA POSICION (%d, %d) del pokemap en [%s]\n", position->x, position->y, pokemon);
			list_remove_by_condition(positions, &condition);
		}

	}
	list_iterate(positions, &assign_closest_trainer);
}

t_trainer* find_trainer_for_catch(t_position* position) {
	t_trainer* selected_trainer = NULL;
	t_trainer* trainer_new = NULL;
	t_trainer* trainer_block = NULL;
	printf("\n---Buscar entrenador más cercano a (%d, %d) en la cola NEW---\n", position->x, position->y);
	sem_wait(&sem_state_lists);
	int32_t closest_from_new = closest_free_trainer_job(new_list, position);
	sem_post(&sem_state_lists);

	printf("\n---Buscar entrenador más cercano a (%d. %d) en la cola BLOCKED---\n", position->x, position->y);
	sem_wait(&sem_state_lists);
	int32_t closest_from_block = closest_free_trainer_job(block_list, position);
	sem_post(&sem_state_lists);

	if(closest_from_new >= 0){
		sem_wait(&sem_state_lists);
		trainer_new = list_get(new_list,closest_from_new);
		sem_post(&sem_state_lists);
	}
	if(closest_from_block >= 0){
		sem_wait(&sem_state_lists);
		trainer_block = list_get(block_list,closest_from_block);
		sem_post(&sem_state_lists);
	}

	if(trainer_new != NULL && (trainer_block == NULL || first_closer(trainer_new, trainer_block, position))){
		selected_trainer = trainer_new;
	}
	else if(trainer_block != NULL && (trainer_new == NULL || first_closer(trainer_block, trainer_new, position))){
		selected_trainer = trainer_block;
	}
	else{
		printf("no hay entrenadores en lasstas de new ni block \n");
	}
	return selected_trainer;
}

void add_to_poke_map(char* pokemon, t_position* position)
{
	printf("ENTRA EN add_to_poke_map\n");
	//CASO DE QUE NO ESTE EL POKEMON EN EL MAP
	sem_wait(&sem_poke_map);
	printf("PASA EL SEMAFORO\n");
	if(!dictionary_has_key(poke_map, pokemon)){
		t_list* positions = list_create();
		list_add(positions, position);
		dictionary_put(poke_map, pokemon, positions);
	}
	else
	{
		t_list* positions = (t_list*) dictionary_get(poke_map,pokemon);
		list_add(positions, position);
	}
	sem_post(&sem_poke_map);
	//EL CASO DE QUE ESTE EL POKEMON EN EL MAPA
	//buscar el pokemon en la lista y retornar el indice


	printf("SALDRIA DE add_to_poke_map\n");
}


//transiciones
char* state_list_string(t_list* list) {
	char* state_string = NULL;
	if(list == new_list) {
		state_string = create_copy_string("new");
	}else if(list == ready_list) {
		state_string = create_copy_string("ready");
	}else if(list == block_list) {
		state_string = create_copy_string("block");
	}else if(list == exec_list) {
		state_string = create_copy_string("exec");
	}else if(list == exit_list) {
		state_string = create_copy_string("exit");
	}else {
		state_string = create_copy_string("error");
	}
	return state_string;
}

void state_change(uint32_t index, t_list* from,t_list* to)
{
	sem_wait(&sem_state_lists);
	t_trainer* trainer = list_remove(from, index);
	list_add(to, trainer);
	sem_post(&sem_state_lists);
	context_changes++;
	if(to == exit_list) {
		trainer->action = FINISH;
		sem_post(&trainer->sem_thread);
	}

}//NO ES RESPONSABLE DEL log

void transition_by_id(uint32_t id, t_list* from,t_list* to) {
	bool condition(void* trainer) {
		return (((t_trainer*)trainer)->id == id);
	}
	if(to==ready_list) {
		new_trainer_in_ready = true;
	}

	t_trainer* trainer = NULL;

	sem_wait(&sem_state_lists);
	if(list_size(from)>1)
		trainer = list_remove_by_condition(from, &condition);
	else
		trainer = list_remove(from, 0);

	if(trainer != NULL){
		list_add(to, trainer);
		context_changes++;
	}
	else {
		printf("*ERROR* NO SE PUDO HACER LA TRANSICIÓN, EL ENTRENADOR NO SE ENCONTRABA EN LA LISTA INDICADA *ERROR*\n");
	}
	sem_post(&sem_state_lists);
	
	if(to == exit_list) {
		char* state = state_list_string(from);
		log_info(log, "trainer[%d] cambio de estado (%s -> exit), razon: objetivo cumplido", trainer->id, state);
		free(state);

		trainer->action = FINISH;
		sem_post(&trainer->sem_thread);//este post es exclusivamente para que el trainer libere memoria, lo cual no lo tomamos como tiempo de CPU
	}

}//YA TIENE log

void transition_from_id_to_ready(uint32_t id) {
	bool condition(void* trainer) {
		return (((t_trainer*)trainer)->id == id);
	}

	char* state_change_log = NULL;
	sem_wait(&sem_state_lists);
	t_trainer* trainer = list_remove_by_condition(new_list, &condition);
	if(trainer != NULL) {
		state_change_log = create_copy_string("new -> ready");
		list_add(ready_list, trainer);
		context_changes++;
		new_trainer_in_ready = true;
		printf("new_list->trainer[%d]->ready_list\n", trainer->id);
	} else {
		trainer = list_remove_by_condition(block_list, &condition);
		if(trainer != NULL) {
			state_change_log = create_copy_string("block -> ready");
			list_add(ready_list, trainer);
			context_changes++;
			new_trainer_in_ready = true;
			printf("block_list->trainer[%d]->ready_list\n", trainer->id);
		}else {
			printf("**ERROR**SE INTENTÓ HACER UNA TRANSICIÓN A READY DE UN ENTRENADOR QUE NO SE ENCUENTRA EN NEW NI BLOCK!**ERROR**\n");

		}
	}
	sem_post(&sem_state_lists);
	if(trainer != NULL) {
		char* reason = NULL;
		if(trainer->target->catching) {
			reason = create_copy_string("atrapar pokemon");
		} else {

			reason = create_copy_string("realizar intercambio");
		}
		log_info(log, "trainer[%d] cambio de estado (%s), razon: %s", trainer->id, state_change_log, reason);
		free(reason);
	}
}//YA TIENE log

void transition_new_to_ready(uint32_t index)
{
	state_change(index,new_list,ready_list);
	new_trainer_in_ready = true;
}//NO SE USA

void transition_ready_to_exec(uint32_t index)
{
	state_change(index,ready_list,exec_list);
	sem_wait(&sem_state_lists);
	t_trainer* trainer = list_get(exec_list,0);
	sem_post(&sem_state_lists);
	char* reason = enter_cpu_reason_string();
	log_info(log, "trainer[%d] cambio de estado (ready -> exec), razon: %s", trainer->id, reason);
	sem_post(&trainer->sem_thread);
}//YA TIENE log

void transition_ready_to_exec_by_trainer(t_trainer* trainer) {
	transition_by_id(trainer->id, ready_list, exec_list);
	char* reason = enter_cpu_reason_string();
	log_info(log, "trainer[%d] cambio de estado (ready -> exec), razon: %s", trainer->id, reason);
	sem_post(&trainer->sem_thread);
}//YA TIENE log

void transition_exec_to_ready()
{
	state_change(0,exec_list,ready_list);
	//new_trainer_in_ready = true;//Si pasa de EXEC a READY es porque fue desalojado
}//YA TIENE log EN EXIT_CPU

void transition_exec_to_block()
{
	state_change(0,exec_list,block_list);
}//YA TIENE log EN EXIT_CPU

void transition_exec_to_exit()
{
	state_change(0,exec_list,exit_list);
}//YA TIENE log EN EXIT_CPU

void transition_block_to_ready(uint32_t index)
{
	state_change(index,block_list,ready_list);
	new_trainer_in_ready = true;
}//NO SE USA

void transition_block_to_exit(uint32_t index)
{
	state_change(index,block_list,exit_list);
}//NO SE USA
//FIN transiciones



void move_up(t_trainer* trainer)
{
	sleep(time_delay);
	trainer->position->y++;
	trainer->cpu_cycles++;
	trainer->burst++;
	sem_wait(&sem_cpu_info);
	cpu_cycles++;
	sem_post(&sem_cpu_info);
}

void move_down(t_trainer* trainer)
{
	sleep(time_delay);
	trainer->position->y--;
	trainer->cpu_cycles++;
	trainer->burst++;
	sem_wait(&sem_cpu_info);
	cpu_cycles++;
	sem_post(&sem_cpu_info);
}

void move_right(t_trainer* trainer)
{
	sleep(time_delay);
	trainer->position->x++;
	trainer->cpu_cycles++;
	trainer->burst++;
	sem_wait(&sem_cpu_info);
	cpu_cycles++;
	sem_post(&sem_cpu_info);
}

void move_left(t_trainer* trainer)
{
	sleep(time_delay);
	trainer->position->x--;
	trainer->cpu_cycles++;
	trainer->burst++;
	sem_wait(&sem_cpu_info);
	cpu_cycles++;
	sem_post(&sem_cpu_info);
}

void move(t_trainer* trainer)
{
	if(trainer->target->position->x > trainer->position->x)
		move_right(trainer);
	else if(trainer->target->position->x < trainer->position->x)
		move_left(trainer);
	else if(trainer->target->position->y > trainer->position->y)
		move_up(trainer);
	else if(trainer->target->position->y < trainer->position->y)
		move_down(trainer);
}


void* sender_thread()
{

	sem_init(&sem_messages_list, 0, 1);
	sem_init(&sem_messages, 0, 0);
	sem_init(&sem_messages_recieve_list, 0, 1);
	while(1){

		if(team_succes) {
			pthread_exit(NULL);
		}


//		debug_colas();
//		debug_message_list();
		sem_wait(&sem_messages);
		printf("me clave en sem messages list\n");
		sem_wait(&sem_messages_list);
		t_message_team* message = list_remove(messages_list, 0);
		sem_post(&sem_messages_list);
		t_message_catch* catch = create_message_catch_long(message->pokemon, message->position->x, message->position->y);


		t_package* package = serialize_catch(catch);
		//TODO ESTO DEBE IR EN OTRO HILO, CREAR NUEVO HILO PARA CADA SEND_MESSAGE
		int32_t correlative_id = send_message(ip_broker, port_broker, package);//send_message ya libera el paquete

		log_info(log, "mensaje CATCH enviado: [ID: %d, pokemon: %s, posicion: (%d, %d)]", correlative_id, message->pokemon, message->position->x, message->position->y);

		printf("ENVIAMOS MENSAJE AL BROKER ID %d\n",correlative_id);
		printf("salio para el broker\n");

		destroy_message_catch(catch);

		char str_correlative_id[6];
		sprintf(str_correlative_id,"%d",correlative_id);
		sem_wait(&sem_messages_recieve_list);
		dictionary_put(message_response,str_correlative_id,message->trainer);
		sem_post(&sem_messages_recieve_list);

		//Liberar el mensaje removido de la lista de mensajes TODO NO ANDA
//		printf("limpiando message team \n");
//		destroy_message_team(message);
//		printf("ya se limpio message team \n");

	}

	//send al broker, (pokemon, posicion, el entrenador)

	//

	//pthread_t tid;
	//pthread_create(&tid, NULL, subscribe, NULL);
	//pthread_join(tid, NULL);



}

/*void* fake_broker_thread()
{
	//SERVIDOR QUE ESCUCHA EN PUERTO TAL! 6001
	//PRINTF ALGO DE MENSAJE
	//RESPONDE ACK
}//*/



/*
subscribe --------------->>>>>> servidor que esta escuchando (BROKER)

*/

void set_default_behavior(queue_code queue) {
	switch(queue) {
	case OPERATION_APPEARED:
		break;
	case OPERATION_LOCALIZED:
		break;
	case OPERATION_CAUGHT:
		//los catch se toman como exitosos automáticamente
		caught_default_behavior = true;
		break;
	}
}

void process_message(serve_thread_args* args) {
	operation_code op_code = args->op_code;
	void* message = args->message;
	switch(op_code) {
	case OPERATION_NEW:
		printf("SE RECIBIO UN  NEW, PERO NO SE QUE HACER <----------------------------\n");
	break;
	case OPERATION_APPEARED:

		log_info(log, "appeared recibido: [ID: %d, CORRELATIVE_ID: %d, SIZE: %d, POKEMON: %s, POSITION: (%d, %d)]", ((t_message_appeared*)(message))->id, ((t_message_appeared*)(message))->correlative_id, ((t_message_appeared*)(message))->size_pokemon_name, ((t_message_appeared*)(message))->pokemon_name, ((t_message_appeared*)(message))->position->x, ((t_message_appeared*)(message))->position->y);

		//SOLO SE AGREGA SI ES REQUERIDO EN OBJETIVOS GLOBALES
		if(pokemon_is_needed_on_pokemap(((t_message_appeared*)(message))->pokemon_name)){//se necesita el pokemon?
			printf("LLEGA ADENTRO DEL IF\n");
			add_to_poke_map(((t_message_appeared*)(message))->pokemon_name,((t_message_appeared*)(message))->position);
			printf("TERMINA add_to_poke_map\n");
			//long_term_scheduler();
			sem_post(&sem_long);//SI APARECE UN NUEVO POKEMON NECESITADO, SE HACE POST AL LONG PARA QUE PLANIFIQUE A UN ENTRENADOR A BUSCARLO
		}
		printf("SALE DEL IF\n");
//		debug_colas();
	break;
	case OPERATION_GET:
		printf("SE RECIBIO UN  GET, PERO NO SE QUE HACER <----------------------------\n");
	break;
	case OPERATION_LOCALIZED:
		printf("SE RECIBIO UN  LOCALIZED[");
		printf("ID: %d, ", ((t_message_localized*)(message))->id);
		printf("CORRELATIVE_ID: %d, ", ((t_message_localized*)(message))->correlative_id);
		printf("SIZE: %d, ", ((t_message_localized*)(message))->size_pokemon_name);
		printf("POKEMON: %s, ", ((t_message_localized*)(message))->pokemon_name);
		printf("POSITION_AMOUNT: %d, ", ((t_message_localized*)(message))->position_amount);
		printf("POSITIONS: [");
		t_position* test = ((t_message_localized*)(message))->positions;
		char* positions_string = string_new();
		for(int i = 0; i < ((t_message_localized*)(message))->position_amount; i++){
			char* aux = malloc(12);
			sprintf(aux, " (%d, %d) ", (test+i)->x, (test+i)->y);

		}
		printf("]\n");
		log_info(log, "localized recibido: [ID: %d, CORRELATIVE_ID: %d, SIZE: %d, POKEMON: %s, POSITION_AMOUNT: %d, POSITIONS:%s]", ((t_message_localized*)(message))->id, ((t_message_localized*)(message))->correlative_id, ((t_message_localized*)(message))->size_pokemon_name, ((t_message_localized*)(message))->pokemon_name, ((t_message_localized*)(message))->position_amount, positions_string);

		destroy_message_localized(message);

	break;
	case OPERATION_CATCH:
		printf("SE RECIBIO UN  CATCH, PERO NO SE QUE HACER <----------------------------\n");
	break;
	case OPERATION_CAUGHT:
//		printf("SE RECIBIO UN  CAUGHT [");
//		printf("ID: %d, ",((t_message_caught*)(message))->id);
//		printf("CORRELATIVE_ID %d, ",((t_message_caught*)(message))->correlative_id);
//		printf("RESULT: %d]<----------\n",((t_message_caught*)(message))->result);

		log_info(log, "mensaje CAUGHT recibido [ID: %d, CORRELATIVE_ID %d, RESULT: %s]", ((t_message_caught*)(message))->id, ((t_message_caught*)(message))->correlative_id, ((t_message_caught*)(message))->result?"OK":"FAIL");

		char str_correlative_id[6];
		sprintf(str_correlative_id,"%d",((t_message_caught*)(message))->correlative_id);
		if(dictionary_has_key(message_response,str_correlative_id) == 1){
			log_info(log, "mensaje CAUGHT [ID: %d, CORRELATIVE_ID %d, RESULT: %s] PROCESADO", ((t_message_caught*)(message))->id, ((t_message_caught*)(message))->correlative_id, ((t_message_caught*)(message))->result?"OK":"FAIL");

			t_trainer* trainer = (t_trainer*) dictionary_remove(message_response, str_correlative_id);

			trainer_catch(trainer, ((t_message_caught*)(message))->result);
			if(trainer_success_objective(trainer)) {
				transition_by_id(trainer->id, block_list, exit_list);//YA HACE log a EXIT FALTA FROM
			}
		}
		else{
			printf("SE IGNORA EL MENSAJE PERRO\n");
			log_info(log, "mensaje CAUGHT [ID: %d, CORRELATIVE_ID %d, RESULT: %s] IGNORADO", ((t_message_caught*)(message))->id, ((t_message_caught*)(message))->correlative_id, ((t_message_caught*)(message))->result?"OK":"FAIL");

		}
		printf("SE VA A BORRAR EL MENSAJE\n");
		destroy_message_caught(message);
		printf("SE BORRO EL MENSAJE\n");
//		debug_message_list();
	break;
	default:
		printf("CODIGO DE OPERACION ERRONEO\n");
	break;
	}
	//TODO limpiar punteros de mensajes y argumentos.
}


pthread_t subscribe(queue_code queue_code) {
	printf("COD OPERATION %d\n", queue_code);



	int32_t socket = connect_to_server(ip_broker, port_broker,retry_time, retry_count, log);

	t_package* package = serialize_suscripcion(team_id, queue_code);
	if( socket != -1) { //conexion realizada

	send_paquete(socket, package);
	if (receive_ACK(socket, log_utils) == -1) {
		exit(EXIT_FAILURE);
	}


	//Quedarse recibiendo mensajes permanentemente, no hace falta otro hilo

	struct thread_args* args = malloc(sizeof(struct thread_args));
	args->socket = socket;
	args->logger = log_utils;
	args->function = process_message;
	pthread_t thread;
	pthread_create(&thread, NULL, (void*) listen_messages, args);

	} else {//conexion fallida -> comportamiento default
		log_info(log, "error de suscripción al broker en cola %s -> se realizará la operación default", queue_code_string(queue_code));
		set_default_behavior(queue_code);
	}

	return thread;

	//Al completar el objetivo global, enviar tres mensajes al broker con ID de proceso e ID de cola asi puede liberar memoria
}


int32_t send_message(char* ip, char* port, t_package* package) {

	int32_t socket = connect_to_server(ip, port,retry_time, retry_count, log_utils);
	send_paquete(socket, package);//ya libera el paquete

	int32_t correlative_id = receive_ID(socket, log_utils);
	send_ACK(socket, log_utils);
	return correlative_id;
}

void destroy_message_team(t_message_team* message){
	free(message->pokemon);
	free(message->position);
	free(message);
}

void catch(t_trainer* trainer){
		sleep(time_delay);
		trainer->action = CATCHING;
		trainer->cpu_cycles++;
		trainer->burst++;
		cpu_cycles++;


	if(caught_default_behavior) {
		trainer_catch(trainer, true);
	} else {
		pthread_t tid;
		pthread_create(&tid, NULL, &message_list_add_catch, trainer);
		printf(" -> Catch: %s\n", trainer->target->pokemon);
	}

}

void trainer_catch(t_trainer* trainer, bool result) {
	log_info(log, "trainer[%d] resultado de operación: atrapar %s en (%d, %d): %s", trainer->id, trainer->target->pokemon, trainer->position->x, trainer->position->y, result?"OK":"FAIL");
	if(result){
		add_pokemon(trainer, trainer->target->pokemon);
		add_caught(trainer->target->pokemon);
		sub_catching(trainer->target->pokemon);
//		debug_trainer(trainer);
		if(trainer_success_objective(trainer)){
			printf("ESTE ENTRENADOR TERMINO RE PIOLA\n");
			trainer->action = FINISH;

		} else{
			printf("ESTE ENTRENADOR NO CUMPLIO TODOS SUS OBJETIVOS\n");
			trainer->action = FREE;
		}

	} else {

		trainer->action = FREE;
		sub_catching(trainer->target->pokemon);
	}
	trainer_set_target(trainer, create_target("", create_position(0,0), 0, false));
	//trainer->target->catching = 0;
	//free(trainer->target->position);
	//trainer->target->position = NULL;
	//free(trainer->target->pokemon);
	//trainer->target->pokemon = NULL;
	printf("SALE DE trainer_catch\n");
}

uint32_t trade(t_trainer* trainer, uint32_t trade_cpu){

	sleep(time_delay);
	trainer->cpu_cycles++;
	trainer->burst++;
	sem_wait(&sem_cpu_info);
	cpu_cycles++;
	sem_post(&sem_cpu_info);
	trade_cpu++;
	printf("trade_cpu = %d\n", trade_cpu);

	if(trade_cpu == 5){
		//efectuamos el trade
		//entrenador1 target->pokemon pasa al inventario del entrenador2 y el entrenador2 target->pokemon pasa al inventario del 1
		log_info(log, "trainer[%d] operación: intercambio con trainer[%d]", trainer->id, trainer->target->trainer_id);
		trade_trainer(trainer);
		trade_cpu = 0;
	}

	return trade_cpu;



}

void trade_trainer(t_trainer* trainer1){
	bool condition(void* trainer) {
		return (((t_trainer*)trainer)->id == trainer1->target->trainer_id);
	}
	sem_wait(&sem_state_lists);
	t_trainer* trainer2 = list_get(list_filter(block_list, &condition),0);
	sem_post(&sem_state_lists);

	printf("\tBEFORE TRADE:\n");
//	debug_trainer(trainer1);
	if(trainer2 ==NULL) {
		printf("EL TRAINER ES NULL LA CONCHA DE LA LORA\n");
	}
//	debug_trainer(trainer2);
	if(trainer2 != NULL) {
		char* pokemon1 = create_copy_string(trainer1->target->pokemon);
		char* pokemon2 = create_copy_string(trainer2->target->pokemon);
		uint32_t index1 = 0;
		uint32_t index2 = 0;
		uint32_t i = 0;

		while(trainer1->pokemons[i] != NULL){
			if(strcmp(trainer1->pokemons[i],trainer1->target->pokemon) == 0){
				//pokemon1 = trainer1->pokemons[i];
				index1 = i;
			}
			i++;
		}
		i = 0;

		while(trainer2->pokemons[i] != NULL){
			if(strcmp(trainer2->pokemons[i],trainer2->target->pokemon) == 0){
				//pokemon2 = trainer2->pokemons[i];
				index2 = i;
			}
			i++;
		}

		if(pokemon1 != NULL && pokemon2 != NULL){
			free(trainer1->pokemons[index1]);
			free(trainer2->pokemons[index2]);
			trainer1->pokemons[index1] = pokemon2;
			trainer2->pokemons[index2] = pokemon1;
		}
		//ya se realizo el intercambio
		solved_deadlocks++;
		log_info(log, "deadlock resuelto -> intercambio realizado:\ntrainer[%d] ->%s-> trainer[%d]\ntrainer[%d] ->%s-> trainer[%d]", trainer1->id, pokemon1, trainer2->id, trainer2->id, pokemon2, trainer1->id);

	} else {
		printf("TRAINER2 = null, se rompio todo\n");
	}

	trainer_set_target(trainer1, create_target("", create_position(0,0), 0, false));
	trainer_set_target(trainer2, create_target("", create_position(0,0), 0, false));

//	destroy_target(trainer1->target);//NO ANDA
//	destroy_target(trainer2->target);//NO ANDA -> use set target vacio

	if(trainer_success_objective(trainer1) == 1){
		printf("ESTE ENTRENADOR TERMINO RE PIOLA\n");
		trainer1->action = FINISH;
		transition_by_id(trainer1->id, exec_list, exit_list);//YA HACE log a EXIT FALTA FROM
		if(success_global_objective()) {
			printf("BRAVO! EL TEAM A CUMPLIDO TODOS SUS OBJETIVOS\n");
		} else {
			printf("SEGUI PARTICIPANDO\n");
		}
	} else{
		printf("ESTE ENTRENADOR NO CUMPLIO TODOS SUS OBJETIVOS\n");
		trainer1->action = FREE;
	}

	if(trainer_success_objective(trainer2) == 1){
		printf("ESTE ENTRENADOR TERMINO RE PIOLA\n");
		trainer2->action = FINISH;
		transition_by_id(trainer2->id, block_list, exit_list);//YA HACE log a EXIT FALTA FROM
		if(success_global_objective()) {
			printf("BRAVO! EL TEAM A CUMPLIDO TODOS SUS OBJETIVOS\n");
		} else {
			printf("SEGUI PARTICIPANDO\n");
		}
	} else{
		printf("ESTE ENTRENADOR NO CUMPLIO TODOS SUS OBJETIVOS\n");
		trainer2->action = FREE;
	}


//	printf("\tAFTER TRADE:\n");
//	debug_trainer(trainer1);
//	debug_trainer(trainer2);

}

void message_list_add_catch(t_trainer* trainer) {
	printf(" message_list_add_catch acallego y dspues rompio\n");
	t_message_team* message = malloc(sizeof(t_message_team));
	printf(" message_list_add_catch NOO acallego y dspues rompio\n");
	message->trainer = trainer;
	//message->pokemon = malloc(strlen(trainer->target->pokemon)+1);
	printf("the size of the fucks %d %d \n",sizeof(message->pokemon),sizeof(trainer->target->pokemon));

	//memcpy(message->pokemon, trainer->target->pokemon, strlen(trainer->target->pokemon)+1);
	message->pokemon = create_copy_string(trainer->target->pokemon);
	message->pokemon = trainer->target->pokemon;
	message->position = malloc(sizeof(t_position));
	message->position->x = trainer->target->position->x;
	message->position->y = trainer->target->position->y;
	sem_wait(&sem_messages_list);
	list_add(messages_list,message);
	sem_post(&sem_messages_list);

	sem_post(&sem_messages);
	//TODO LIBERAR MEMORIA
}

void debug_colas() {
	printf("the size of all list are [new: %d] [ready: %d] [block: %d] [exec: %d] [exit: %d]\n",list_size(new_list),list_size(ready_list),list_size(block_list),list_size(exec_list), list_size(exit_list));
}
void debug_message_list() {
	void printf_function(char*key,void*n){
		printf("ID : %s \n",key);
		//agregar a que entrenador pertenece
	};
	printf("The IDs on message list response are these: \n");
	dictionary_iterator(message_response, &printf_function);//TODO BORRAR
}
#endif /* TEAM_H_ */
