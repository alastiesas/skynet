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
#define DEADLOCK_PRIORITY 10000
//---GLOBALS---

//logs: pueden desactivarse para no mostrarse en consola
//
bool debugging = false;

//configuracion
t_config* config;
t_log* log;
t_log* log_utils;
uint32_t team_id;
char* ip_broker;
char* port_broker;
char* log_file;
char* ip;
char* port;

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
t_list* poke_map_order;//MUTEX = sem_poke_map
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
t_state_change_reason exit_cpu_reason = START;//MUTEX = sem_cpu_info
bool team_succes = false;
sem_t sem_cpu;//mutex para que short y traienrs no puedan ejecutarse a la vez
bool idle_cpu = false;
bool starting = true;

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
void trainer_assign_catch(char* pokemon);
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
void long_thread();
void* short_thread();
// algoritmos de seleccion
void fifo_algorithm();
void rr_algorithm();
void sjfs_algorithm();
void sjfc_algorithm();
bool exit_cpu();
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
void team_send_catch(t_message_team* message);
void team_send_get(char* pokemon);
void set_default_behavior(queue_code);
bool add_to_pokemap_if_needed(char* pokemon, t_position* position);
void process_message(serve_thread_args* args);
pthread_t subscribe(queue_code queue_code);
void iniciar_servidor_team();
void wait_clients(int32_t socket_servidor, t_log* logger);
void team_serves_client(void* input);
//FIN comunicación

//debugs
void debug_colas();
void debug_message_list();
//FIN debugs

void destroy_poke_map();
void destroy_global_objectives();
void destroy_message_team(t_message_team* message);



void initialize_global_config() {

	log_file = config_get_string_value(config, "LOG_FILE");
	if(config_has_property(config, "DEBUGGING")) {
		uint32_t debug = config_get_int_value(config, "DEBUGGING");
		if(debug > 0) {
			debugging = true;
		}
	}

	team_id = config_get_int_value(config, "ID");
	ip = config_get_string_value(config, "IP");
	port = config_get_string_value(config, "PUERTO");


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


}

void initialize_semaphores() {
	sem_init(&sem_state_lists, 0, 1);
	sem_init(&sem_poke_map, 0, 1);
	sem_init(&sem_objectives_list, 0, 1);
	sem_init(&sem_cpu, 0, 1);
	sem_init(&sem_cpu_info, 0, 1);

}

void callback_fifo(t_trainer* trainer){

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
		sem_wait(&sem_cpu_info);
		exit_cpu_reason = NO_CHANGE;
		sem_post(&sem_cpu_info);
		sem_post(&trainer->sem_thread);
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
			sem_wait(&sem_cpu_info);
			exit_cpu_reason = NO_CHANGE;
			sem_post(&sem_cpu_info);
			sem_post(&trainer->sem_thread);
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
void callback_sjfc(t_trainer* trainer) {

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
	char** positions_config = config_get_array_value(config,"POSICIONES_ENTRENADORES");
	char** objectives_config = config_get_array_value(config,"OBJETIVOS_ENTRENADORES");
	char** pokemons_config = config_get_array_value(config,"POKEMON_ENTRENADORES");
	bool no_more_pokemons = pokemons_config == NULL;
	int i = 0;
	while(positions_config[i] != NULL){
		t_trainer* trainer = NULL;
		if(!no_more_pokemons) {
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
		if(trainer_success_objective(trainer)) {
			list_add(exit_list, trainer);
			trainer->action = FINISH;
			sem_post(&trainer->sem_thread);//Se hace post para que el trainer se borre y termine su hilo
		} else if(trainer_full(trainer)) {

			list_add(block_list, trainer);
		} else {

			list_add(new_list, trainer);
		}
		sem_post(&sem_state_lists);

		i++;
	}

	free_string_list(positions_config);
	free_string_list(objectives_config);
	free_string_list(pokemons_config);
	free(positions_config);
	free(objectives_config);
	free(pokemons_config);
}

void add_trainer_to_objective(t_trainer* trainer)
{

	void(*add)(char*);


	void add_x_times(char* pokemon, uint32_t* count) {
		for(int i = 0; i<*count; i++) {
			add(pokemon);
		}
	}

	add = &add_objective;
	dictionary_iterator(trainer->objectives, &add_x_times);

	add = &add_caught;
	dictionary_iterator(trainer->pokemons, &add_x_times);

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
		log_info(log_utils, "error de objetivos en add_catching");
}

void sub_catching(char* pokemon)
{
	sem_wait(&sem_objectives_list);
	t_objective* objective = find_key(objectives_list, pokemon);
	if(objective != NULL){
		if(objective->catching>0)
			objective->catching--;
	}
	else
		log_info(log_utils, "error de objetivos en sub_catching");

	sem_post(&sem_objectives_list);
}


bool pokemon_is_needed(char* pokemon,char* channel)
{
	bool needed = false;
	sem_wait(&sem_objectives_list);
	t_objective* objective = find_key(objectives_list, pokemon);
	sem_post(&sem_objectives_list);

	if(objective != NULL){
		if(strcmp(channel,"trainer") == 0){
			needed = objective->count > (objective->caught + objective->catching);
		}
		else{
			needed = true;
		}
	}

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

void initialize_global_objectives()
{
	sem_wait(&sem_objectives_list);
	objectives_list = list_create();
	sem_post(&sem_objectives_list);

	sem_wait(&sem_state_lists);
	list_iterate(new_list, &add_trainer_to_objective);
	list_iterate(block_list, &add_trainer_to_objective);
	sem_post(&sem_state_lists);



	bool team_needs_objective(t_objective* objective) {
		return pokemon_is_needed_on_trainer(objective->pokemon);
	}

	t_list* need_catch_objectives = list_filter(objectives_list, &team_needs_objective);


	void send_get_if_needed(t_objective* objective) {
		pthread_t* tid;
		pthread_create(&tid, NULL, &team_send_get, objective->pokemon);
	}

	list_iterate(need_catch_objectives, &send_get_if_needed);//para cada pokemon que necesite envia el get inicial
	list_destroy(need_catch_objectives);

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
	//if(/*si es 0 menor*/)f
		//funcion cambiar valor global de variable interrumption
		//COMO SE DESALOJA A UN HILO DE ENTENADOR, COMO SE ENTERA EL PLANIFICADOR O EL HILO DE EJECUCION!!
		//while(1) agregar
	t_trainer* trainer = callback_thread->trainer;

	debug_trainer(trainer);
	uint32_t trade_cpu = 0;
	while(trainer->action != FINISH){
		sem_wait(&trainer->sem_thread);

		sem_wait(&sem_cpu);

		switch(trainer->action){
			case MOVE:;
				//t_position* previous_position = create_position(trainer->position->x, trainer->position->y);
				char* previous_string = calloc(sizeof(char)*10, 1);
				sprintf(previous_string, "%d, %d", trainer->position->x, trainer->position->y);
				//aca va un if, no un while
				if(trainer->position->x != trainer->target->position->x || trainer->position->y != trainer->target->position->y){
					move(trainer);
					log_info(log, "trainer[%d (%d)] operación: movimiento (%s) -> (%d, %d), objetivo: %s %s(%d, %d)", trainer->id, trainer_burst_estimate(trainer), previous_string, trainer->position->x,trainer->position->y, trainer->target->catching?"atrapar":"intercambiar", trainer->target->pokemon, trainer->target->position->x, trainer->target->position->y);
				}
				else if(trainer->target->catching){
					trainer->action = CATCH;
				} else {
					trainer->action = TRADE;
				}

				free(previous_string);

				break;
			case CATCH:
				log_info(log, "trainer[%d (%d)] operación: atrapar %s en (%d, %d)", trainer->id, trainer_burst_estimate(trainer), trainer->target->pokemon, trainer->position->x, trainer->position->y);
				catch(trainer);

				break;
			case CATCHING:
				break;
			case TRADE:
				trade_cpu = trade(trainer,trade_cpu);
				break;
			case FINISH:

				break;
			default:
				break;
		}
		callback_thread->callback(trainer);
		sem_post(&sem_cpu);

	}



	log_info(log, "trainer[%d] finalizado con exito cantidad de ciclos utilizados: %d", trainer->id, trainer->cpu_cycles);
	destroy_trainer(trainer);

}

void long_thread() {
	sem_init(&sem_long, 0, 1);
	sem_init(&sem_scheduler, 0, 1);
	while(!team_succes){
		if(success_global_objective()) {
			team_succes = true;
		}

//		printf("esto es lo ultimo? \n");
		sem_wait(&sem_long);
		sem_wait(&sem_scheduler);
		long_term_scheduler();
		sem_post(&sem_scheduler);


		uint32_t ready_size = 0;
		sem_wait(&sem_state_lists);
		ready_size = list_size(ready_list);
		sem_post(&sem_state_lists);
		if(starting) {
			starting = false;
			sem_post(&sem_short);
		}
		if(idle_cpu &&  ready_size > 0) {
			idle_cpu = false;
			sem_post(&sem_short);
		}

	}

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
		log_info(log_utils, "no se leyo el algoritmo en el config, default: FIFO");
		short_term_scheduler = &fifo_algorithm;
		break;
	}
	sem_init(&sem_short, 0, 0);

	while(!team_succes){

		if(success_global_objective()) {
			team_succes = true;
		}

		sem_wait(&sem_short);
		sem_wait(&sem_cpu);
		sem_wait(&sem_scheduler);
		short_term_scheduler();
		sem_post(&sem_scheduler);
		sem_post(&sem_cpu);
	}
}

void long_term_scheduler(){

	//revisar size ready
	sem_wait(&sem_state_lists);
	uint32_t size_ready = list_size(ready_list);
	sem_post(&sem_state_lists);
	sem_wait(&sem_poke_map);
	list_iterate(poke_map_order, &trainer_assign_catch);
	//dictionary_iterator(poke_map, &trainer_assign_catch);
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
		log_info(log, "inicio de algoritmo de deteccion de deadlocks");

		if(possible_deadlock()) {

			deadlock_handler();
		}else {
			log_info(log,"resultado de algoritmo de detección de deadlocks: no hay entrenadores interbloqueados");
		}
	}
}

bool possible_deadlock(){

	sem_wait(&sem_state_lists);
	int32_t locked = list_count_satisfying(block_list, &trainer_locked);
	sem_post(&sem_state_lists);
	bool deadlock = locked > 1;
	return deadlock;
}


void deadlock_handler(){

	t_dictionary* waiting_table = dictionary_create();
	t_dictionary* held_table = dictionary_create();




	uint32_t locked_trainers = deadlock_detector(waiting_table,held_table);

	log_info(log, "resultado de algoritmo de detección de deadlocks: %d entrenadores interbloqueados, pokemon (especies) retenidos: %d, pokemon (especies) en espera: %d", locked_trainers, dictionary_size(held_table), dictionary_size(waiting_table));
	uint32_t deadlocks_before = deadlocks;
	//fixeamos parejas perfectas


//	void debug_table(char* key, t_list* table) {
//		void debug_list(t_trainer* trainer) {
//			printf(" [%d] ", trainer->id);
//		}
//
//		printf("%s: ", key);
//		list_iterate(table, &debug_list);
//		printf("\n");
//	}

//	printf("TABLA DE WAITING:\n");
//	dictionary_iterator(waiting_table, &debug_table);
//	printf("TABLA DE HELD:\n");
//	dictionary_iterator(held_table, &debug_table);

	assign_trade_couples(waiting_table,held_table, true);

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
	//tomo los entrenadores que esten interbloqueados
	sem_wait(&sem_state_lists);
	t_list* locked_trainers = list_filter(block_list, &trainer_locked);
	sem_post(&sem_state_lists);



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

		list_destroy_and_destroy_elements(waiting_pokemons, &free);
		list_destroy_and_destroy_elements(held_pokemons, &free);

	}

	list_iterate(locked_trainers, &add_to_tables);

	uint32_t lockec_trainers_aomunt = list_size(locked_trainers);
	list_destroy(locked_trainers);

	return lockec_trainers_aomunt;

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

	bool matches(t_trainer* possible_couple) {

		bool trainer_needs_any(char* pokemon) {
			return trainer_needs(possible_couple, pokemon);
		}

		return list_any_satisfy(trainer_holds, &trainer_needs_any);//trade perfecto solo si necesita alguno de los que holdea el trainer
	}

	t_list* possible_couples_list = list_filter(holding_trainers_list, &matches);//filtro por quienes pueden hacer un trade perfecto
	list_destroy_and_destroy_elements(trainer_holds, &free);
	t_trainer* closest_perfect_couple_trainer = closest_couple(trainer, possible_couples_list);
	list_destroy(possible_couples_list);
	return closest_perfect_couple_trainer;

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
				if(trainer_couple != NULL){
					if(trainer->action == FREE && trainer_couple->action == FREE){
						char* pokemon_from_held = NULL;
						t_list* trainer_held_pokemons_list = trainer_held_pokemons(trainer);
						if(perfect) {
							bool couple_needs(char* one_pokemon) {
								return trainer_needs(trainer_couple, one_pokemon);
							}
							t_list* couple_match_pokemons = list_filter(trainer_held_pokemons_list, couple_needs);
							pokemon_from_held = create_copy_string(list_get(couple_match_pokemons,0));
							list_destroy(couple_match_pokemons);

						}else {
							pokemon_from_held = create_copy_string(list_get(trainer_held_pokemons_list,0));
						}

						list_destroy_and_destroy_elements(trainer_held_pokemons_list, &free);
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

	deadlocks++;
	log_info(log, "deadlock detectado -> intecambio planificado:\ntrainer[%d] ->%s-> trainer[%d]\ntrainer[%d] ->%s-> trainer[%d]", trainer1->id, pokemon1, trainer2->id, trainer2->id, pokemon2, trainer1->id);


	trainer_assign_move(trainer1, pokemon1, trainer2->position, 0, trainer2->id);
	trainer_assign_trade(trainer2, pokemon2, trainer1->id);



}

void fifo_algorithm()
{
//	sem_wait(&sem_cpu_info);
//	int reason_int = exit_cpu_reason;
//	sem_post(&sem_cpu_info);

	sem_wait(&sem_state_lists);
	uint32_t ready_size =  list_size(ready_list);
	sem_post(&sem_state_lists);
	if(exit_cpu()) {
		sem_wait(&sem_state_lists);
		ready_size =  list_size(ready_list);
		sem_post(&sem_state_lists);

		if(ready_size > 0){
			transition_ready_to_exec(0);//toma el primero de la cola ready
		}
		else{
			idle_cpu = true;
			sem_post(&sem_long);//toma el mando el long
		}

	} else if(ready_size == 0) {
		idle_cpu = true;
		sem_post(&sem_long);//toma el mando el long
	}

}

void rr_algorithm()
{


	sem_wait(&sem_state_lists);
	uint32_t ready_size =  list_size(ready_list);
	sem_post(&sem_state_lists);
	if(exit_cpu()) {
		sem_wait(&sem_state_lists);
		ready_size =  list_size(ready_list);
		sem_post(&sem_state_lists);

		if(ready_size > 0){
			transition_ready_to_exec(0);//toma el primero de la cola ready
		}
		else{
			//toma el mando el long
			idle_cpu = true;
			sem_post(&sem_long);
		}

	}else if(ready_size == 0) {
		idle_cpu = true;
		sem_post(&sem_long);//toma el mando el long
	}

}

void sjfs_algorithm()
{


	sem_wait(&sem_state_lists);
	uint32_t ready_size =  list_size(ready_list);
	sem_post(&sem_state_lists);
	if(exit_cpu()) {
		sem_wait(&sem_state_lists);
		ready_size =  list_size(ready_list);
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
			idle_cpu = true;
			sem_post(&sem_long);
		}

	}else if(ready_size == 0) {
		idle_cpu = true;
		sem_post(&sem_long);//toma el mando el long
	}
}

void sjfc_algorithm()
{
	new_trainer_in_ready = false;


	sem_wait(&sem_state_lists);
	uint32_t ready_size =  list_size(ready_list);
	sem_post(&sem_state_lists);
	if(exit_cpu()) {
		sem_wait(&sem_state_lists);
		ready_size =  list_size(ready_list);
		sem_post(&sem_state_lists);

		if(ready_size > 0){
			t_trainer* trainer = shortest_job_trainer_from_ready();
			if(trainer != NULL) {
				transition_ready_to_exec_by_trainer(trainer);
	//			transition_by_id(trainer->id, ready_list, exec_list);
	//			sem_post(&trainer->sem_thread);
			}
		} else{
//			toma el mando el long
			idle_cpu = true;
			sem_post(&sem_long);
		}

	}else if(ready_size == 0) {
		idle_cpu = true;
		sem_post(&sem_long);//toma el mando el long
	}
}

bool exit_cpu() {
	sem_wait(&sem_cpu_info);
	int reason_int = exit_cpu_reason;
	sem_post(&sem_cpu_info);
	bool exit = false;
	if(reason_int != NO_CHANGE) {
		exit = true;

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
				log_info(log, "trainer[%d (%d)] cambio de estado (%s), razon: %s", trainer->id, trainer_burst_estimate(trainer), state_change_log, reason);
				free(reason);
			}

			free(state_change_log);
		}//fin if
	}
	return exit;
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
	trainer->action = MOVE;
	trainer_set_target(trainer, create_target(pokemon, position, target_id, catching));
	transition_from_id_to_ready(trainer->id);
}

void trainer_assign_trade(t_trainer* trainer, char* pokemon, uint32_t target_id) {
	trainer->action = TRADE;
	trainer_set_target(trainer, create_target(pokemon, trainer->position, target_id, 0));
}

void trainer_assign_catch(char* pokemon)
{
	t_list* positions = dictionary_get(poke_map, pokemon);
	//char* pokemon = malloc((strlen(key)+1)*sizeof(char));
	//memcpy(pokemon, key, strlen(key)+1);//sin esto rompe

	//t_link_element* element = positions->head;
	//t_position* position;

	bool assign_closest_trainer(t_position* position) {
		bool succes;
		t_trainer* selected_trainer = find_trainer_for_catch(position);

		if(selected_trainer != NULL) {
			add_catching(pokemon);

			trainer_assign_move(selected_trainer, pokemon, position, 1, 0);
			bool condition(t_position* iterate_position) {
				return (distance(position, iterate_position) == 0);
			}
			log_info(log_utils, "ENRENADOR ASIGNADO, SE ELIMINARÁ LA POSICION (%d, %d) del pokemap en [%s]\n", position->x, position->y, pokemon);
			list_remove_and_destroy_by_condition(positions, &condition, &free);
			succes = true;
		}else {
			succes = false;
		}

	}
	if(list_size(positions) > 0) {
		t_position* position = list_get(positions,0);
		if(assign_closest_trainer(position)) {
			bool condition(char* key) {
				return strcmp(key, pokemon) == 0;
			}
			char* pokemon_remove = list_remove_by_condition(poke_map_order, &condition);
			free(pokemon_remove);
		}//SINO NO BORRAMOS
	}
	//list_iterate(positions, &assign_closest_trainer);
}

t_trainer* find_trainer_for_catch(t_position* position) {
	t_trainer* selected_trainer = NULL;
	t_trainer* trainer_new = NULL;
	t_trainer* trainer_block = NULL;
	sem_wait(&sem_state_lists);
	int32_t closest_from_new = closest_free_trainer_job(new_list, position);
	sem_post(&sem_state_lists);


	sem_wait(&sem_state_lists);
	int32_t closest_from_block = closest_free_trainer_job(block_list, position);
	sem_post(&sem_state_lists);


	if(closest_from_new >= 0){
		sem_wait(&sem_state_lists);
//		debug_colas();
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

	return selected_trainer;
}

void add_to_poke_map(char* pokemon, t_position* position)
{
	t_position* position_copy = create_position(position->x, position->y);
	//CASO DE QUE NO ESTE EL POKEMON EN EL MAP
	sem_wait(&sem_poke_map);
	list_add(poke_map_order,create_copy_string(pokemon));
	if(!dictionary_has_key(poke_map, pokemon)){
		t_list* positions = list_create();
		list_add(positions, position_copy);
		dictionary_put(poke_map, pokemon, positions);
	}
	else
	{
		t_list* positions = (t_list*) dictionary_get(poke_map,pokemon);

		bool condition(t_position* pos) {
			//si alguna de las posiciones es la que se intenta agregar, no se agrega
			return (pos->x == position->x && pos->y == position->y);
		}
		if(!list_any_satisfy(positions, &condition)) {//si ninguna posicione del pokemap es la que se intenta agregar si se agrega
			list_add(positions, position_copy);
		}

	}
	sem_post(&sem_poke_map);
	//EL CASO DE QUE ESTE EL POKEMON EN EL MAPA
	//buscar el pokemon en la lista y retornar el indice
	sem_post(&sem_long);

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

	if(to == block_list && trainer->action == FREE) {
		sem_post(&sem_long);
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


	if(list_size(from)>0){
		trainer = list_remove_by_condition(from, &condition);

		if(trainer != NULL){
			list_add(to, trainer);
			context_changes++;

			if(to == exit_list) {
				char* state = state_list_string(from);
				log_info(log, "trainer[%d] cambio de estado (%s -> exit), razon: objetivo cumplido", trainer->id, state);
				free(state);

				trainer->action = FINISH;
				sem_post(&trainer->sem_thread);//este post es exclusivamente para que el trainer libere memoria, lo cual no lo tomamos como tiempo de CPU
			}else if(to == block_list && trainer->action == FREE) {

				sem_post(&sem_long);
			}

		}
		else {
			log_info(log_utils, "*ERROR* NO SE PUDO HACER LA TRANSICIÓN, EL ENTRENADOR NO SE ENCONTRABA EN LA LISTA INDICADA *ERROR*");
		}
	}
	sem_post(&sem_state_lists);
	

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
	} else {
		trainer = list_remove_by_condition(block_list, &condition);
		if(trainer != NULL) {
			state_change_log = create_copy_string("block -> ready");
			list_add(ready_list, trainer);
			context_changes++;
			new_trainer_in_ready = true;
		}else {
			log_info(log_utils, "**ERROR**SE INTENTÓ HACER UNA TRANSICIÓN A READY DE UN ENTRENADOR QUE NO SE ENCUENTRA EN NEW NI BLOCK!**ERROR**\n");

		}
	}
	sem_post(&sem_state_lists);
	if(trainer != NULL) {
		char* reason = NULL;
		if(trainer->target->catching) {
			reason = malloc(sizeof(char)*32);
			sprintf(reason, "atrapar %s(%d, %d)", trainer->target->pokemon, trainer->target->position->x, trainer->target->position->y);
		} else {

			reason = create_copy_string("realizar intercambio");
		}
		log_info(log, "trainer[%d] cambio de estado (%s), razon: %s", trainer->id, state_change_log, reason);
		free(state_change_log);
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
	log_info(log, "trainer[%d (%d)] cambio de estado (ready -> exec), razon: %s", trainer->id, trainer->burst_estimate, reason);
	sem_post(&trainer->sem_thread);
	free(reason);
}//YA TIENE log

void transition_ready_to_exec_by_trainer(t_trainer* trainer) {
	transition_by_id(trainer->id, ready_list, exec_list);
	char* reason = enter_cpu_reason_string();
	log_info(log, "trainer[%d (%d)] cambio de estado (ready -> exec), razon: %s", trainer->id, trainer->burst_estimate, reason);
	sem_post(&trainer->sem_thread);
	free(reason);
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
	while(!team_succes){


//		debug_colas();
//		debug_message_list();
		sem_wait(&sem_messages);
		sem_wait(&sem_messages_list);
		t_message_team* message = list_remove(messages_list, 0);
		sem_post(&sem_messages_list);

		pthread_t catch_sender_tid;
		pthread_create(&catch_sender_tid, NULL, team_send_catch, message);


	}




}

void team_send_catch(t_message_team* message) {
	//primero creo la estructura de mensaje
	t_message_catch* catch = create_message_catch_long(message->pokemon, message->position->x, message->position->y);

	//segundo serializo la estructura en el paquete
	t_package* package = serialize_catch(catch);

	//tercero envio el paquete
	int32_t correlative_id = team_send_package(ip_broker, port_broker, package);//send_message ya libera el paquete
	log_info(log, "mensaje CATCH enviado: [ID: %d, pokemon: %s, posicion: (%d, %d)]", correlative_id, message->pokemon, message->position->x, message->position->y);
	destroy_message_catch(catch);

	if(correlative_id != -1) {
		char str_correlative_id[6];
		sprintf(str_correlative_id,"%d",correlative_id);
		sem_wait(&sem_messages_recieve_list);
		dictionary_put(message_response,str_correlative_id,message->trainer);
		sem_post(&sem_messages_recieve_list);

	} else {
		log_info(log, "error de envío de mensaje CATCH -> se realizará la operación default");
		trainer_catch(message->trainer, true);
		if(message->trainer->action == FREE) {
			sem_post(&sem_long);

		}
	}
	//por ultimo agrego el correlative id a la lista de espera de caught

	destroy_message_team(message);
}

void team_send_get(char* pokemon) {

	t_message_get* get = create_message_get(pokemon);
	t_package* package = serialize_get(get);

	int32_t id = team_send_package(ip_broker, port_broker, package);//send_message ya libera el paquete
	if(id != -1) {

	log_info(log, "mensaje GET enviado: [ID: %d, pokemon: %s]", id, pokemon);

	} else {
		log_info(log, "error de envío de mensaje GET -> se realizará la operación default");
	}

	destroy_message_get(get);
}

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

bool add_to_pokemap_if_needed(char* pokemon, t_position* position) {
	bool pokemon_added = false;
	if(pokemon_is_needed_on_pokemap(pokemon)){//se necesita el pokemon?

		add_to_poke_map(pokemon, position);
		pokemon_added = true;

		//long_term_scheduler();
		//SI APARECE UN NUEVO POKEMON NECESITADO, SE HACE POST AL LONG PARA QUE PLANIFIQUE A UN ENTRENADOR A BUSCARLO
	}
	return pokemon_added;
}

void process_message(serve_thread_args* args) {
	operation_code op_code = args->op_code;
	void* message = args->message;
	switch(op_code) {
	case OPERATION_NEW:
		log_info(log_utils, "new recibido:, se ignora");
		destroy_message_new(message);
	break;
	case OPERATION_APPEARED:;
		t_message_appeared* appeared_message = ((t_message_appeared*)(message));

		//SOLO SE AGREGA SI ES REQUERIDO EN OBJETIVOS GLOBALES
		bool added = add_to_pokemap_if_needed(appeared_message->pokemon_name,appeared_message->position);
		log_info(log, "appeared recibido: [ID: %d, CORRELATIVE_ID: %d, SIZE: %d, POKEMON: %s, POSITION: (%d, %d)], %s", ((t_message_appeared*)(message))->id, ((t_message_appeared*)(message))->correlative_id, ((t_message_appeared*)(message))->size_pokemon_name, ((t_message_appeared*)(message))->pokemon_name, ((t_message_appeared*)(message))->position->x, ((t_message_appeared*)(message))->position->y, added?"PROCESADO":"IGNORADO");

		destroy_message_appeared(message);

//		debug_colas();
	break;
	case OPERATION_GET:
		log_info(log_utils, "get recibido:, se ignora");
	break;
	case OPERATION_LOCALIZED:;
		t_message_localized* localized_message = ((t_message_localized*)(message));
		char* positions_string =  calloc(100, sizeof(char));
		for(int i = 0; i < localized_message->position_amount; i++){
			char* aux = calloc(12, sizeof(char));
			sprintf(aux, " (%d, %d) ", (localized_message->positions+i)->x, (localized_message->positions+i)->y);
			strcat(positions_string, aux);
			free(aux);
		}

		bool already_on_map = false;
		sem_wait(&sem_poke_map);
		already_on_map = dictionary_has_key(poke_map, localized_message->pokemon_name);
		sem_post(&sem_poke_map);

		log_info(log, "localized recibido: [ID: %d, CORRELATIVE_ID: %d, SIZE: %d, POKEMON: %s, POSITION_AMOUNT: %d, POSITIONS:%s] %s", localized_message->id, localized_message->correlative_id, localized_message->size_pokemon_name, localized_message->pokemon_name, localized_message->position_amount, positions_string, already_on_map?"IGNORADO":"PROCESADO");

		if((!already_on_map) && pokemon_is_needed_on_pokemap(localized_message->pokemon_name)){//se necesita el pokemon?

			for(int i = 0; i < ((t_message_localized*)(message))->position_amount;i++) {
				add_to_poke_map(localized_message->pokemon_name,(localized_message->positions+i));
			}
		}

		free(positions_string);
		destroy_message_localized(message);

	break;
	case OPERATION_CATCH:;
		log_info(log_utils, "catch recibido:, se ignora");
		destroy_message_catch(message);
	break;
	case OPERATION_CAUGHT:;
	t_message_caught* caught_message = ((t_message_caught*)(message));
		log_info(log, "mensaje CAUGHT recibido [ID: %d, CORRELATIVE_ID %d, RESULT: %s]", caught_message->id, caught_message->correlative_id, caught_message->result?"OK":"FAIL");

		char str_correlative_id[6];
		sprintf(str_correlative_id,"%d",((t_message_caught*)(message))->correlative_id);
		if(dictionary_has_key(message_response,str_correlative_id) == 1){
			log_info(log, "mensaje CAUGHT [ID: %d, CORRELATIVE_ID %d, RESULT: %s] PROCESADO", caught_message->id, caught_message->correlative_id, caught_message->result?"OK":"FAIL");

			t_trainer* trainer = (t_trainer*) dictionary_remove(message_response, str_correlative_id);

			trainer_catch(trainer, caught_message->result);
			if(trainer_success_objective(trainer)) {
				transition_by_id(trainer->id, block_list, exit_list);//YA HACE log a EXIT FALTA FROM
			} else if(trainer->action == FREE){
				sem_post(&sem_long);
			}
		}
		else{
			log_info(log, "mensaje CAUGHT [ID: %d, CORRELATIVE_ID %d, RESULT: %s] IGNORADO", caught_message->id, caught_message->correlative_id, caught_message->result?"OK":"FAIL");
		}
		destroy_message_caught(message);
	break;
	default:
		log_info(log_utils, "mensaje (codigo desconocido) recibido:, se ignora");
	break;
	}

	free(args);

}


pthread_t subscribe(queue_code queue_code) {

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
		char* queue_code_string_value = queue_code_string(queue_code);
		log_info(log, "error de suscripción al broker en cola %s -> se realizará la operación default", queue_code_string_value);
		set_default_behavior(queue_code);
		free(queue_code_string_value);
	}

	return thread;

	//Al completar el objetivo global, enviar tres mensajes al broker con ID de proceso e ID de cola asi puede liberar memoria
}


int32_t team_send_package(char* ip, char* port, t_package* package) {
	int32_t correlative_id = -1;
	int32_t socket = connect_to_server(ip, port,retry_time, retry_count, log);
	if(socket != -1) {
		if(send_paquete(socket, package) != -1) {
			correlative_id = receive_ID(socket, log);
			if(correlative_id != -1) {
				send_ACK(socket, log_utils);
			} else {
				return -1;
			}
		} else {
			return -1;
		}

	} else {
		return -1;
	}


	return correlative_id;
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
	}

}

void trainer_catch(t_trainer* trainer, bool result) {
	log_info(log, "trainer[%d] resultado de operación: atrapar %s en (%d, %d): %s", trainer->id, trainer->target->pokemon, trainer->position->x, trainer->position->y, result?"OK":"FAIL");
	if(result){
		add_pokemon(trainer, trainer->target->pokemon);
		add_caught(trainer->target->pokemon);
//		debug_trainer(trainer);
		if(trainer_success_objective(trainer)){
			trainer->action = FINISH;

		} else{
			trainer->action = FREE;
		}

	} else {

		trainer->action = FREE;
		sub_catching(trainer->target->pokemon);
	}
	t_position* empty_target_position = create_position(0,0);
	trainer_set_target(trainer, create_target("", empty_target_position, 0, false));
	free(empty_target_position);
}

uint32_t trade(t_trainer* trainer, uint32_t trade_cpu){

	sleep(time_delay);
	trainer->cpu_cycles++;
	trainer->burst++;
	sem_wait(&sem_cpu_info);
	cpu_cycles++;
	sem_post(&sem_cpu_info);
	trade_cpu++;

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
	t_list* list_trainer_id = list_filter(block_list, &condition);
	t_trainer* trainer2 = list_get(list_trainer_id,0);
	sem_post(&sem_state_lists);
	list_destroy(list_trainer_id);

	bool new_trainer_in_block = false;
	if(trainer2 != NULL) {

		char* pokemon1 = create_copy_string(trainer1->target->pokemon);
		char* pokemon2 = create_copy_string(trainer2->target->pokemon);
		sub_pokemon(trainer1, pokemon1);
		sub_pokemon(trainer2, pokemon2);
		add_pokemon(trainer1, pokemon2);
		add_pokemon(trainer2, pokemon1);

		//ya se realizo el intercambio
		solved_deadlocks++;
		log_info(log, "deadlock resuelto -> intercambio realizado:\ntrainer[%d] ->%s-> trainer[%d]\ntrainer[%d] ->%s-> trainer[%d]", trainer1->id, pokemon1, trainer2->id, trainer2->id, pokemon2, trainer1->id);
		free(pokemon1);
		free(pokemon2);

		t_position* empty_target_position = create_position(0,0);
		trainer_set_target(trainer1, create_target("", empty_target_position, 0, false));
		trainer_set_target(trainer2, create_target("", empty_target_position, 0, false));
		free(empty_target_position);

		if(trainer_success_objective(trainer1) == 1){
			trainer1->action = FINISH;
			transition_by_id(trainer1->id, exec_list, exit_list);//YA HACE log a EXIT FALTA FROM
		} else{
			trainer1->action = FREE;
			new_trainer_in_block = true;
		}

		if(trainer_success_objective(trainer2) == 1){
			trainer2->action = FINISH;
			transition_by_id(trainer2->id, block_list, exit_list);//YA HACE log a EXIT FALTA FROM
		} else{
			trainer2->action = FREE;
			new_trainer_in_block = true;
		}
	} else {
		log_info(log_utils, "ERROR TRAINER NULL EN TRADE");
		exit(-1);
	}

	if(new_trainer_in_block) {
		sem_post(&sem_long);
	}
}


void message_list_add_catch(t_trainer* trainer) {
	t_message_team* message = malloc(sizeof(t_message_team));
	message->trainer = trainer;
	//message->pokemon = malloc(strlen(trainer->target->pokemon)+1);

	//memcpy(message->pokemon, trainer->target->pokemon, strlen(trainer->target->pokemon)+1);
	message->pokemon = create_copy_string(trainer->target->pokemon);
	message->position = malloc(sizeof(t_position));
	message->position->x = trainer->target->position->x;
	message->position->y = trainer->target->position->y;
	sem_wait(&sem_messages_list);
	list_add(messages_list,message);
	sem_post(&sem_messages_list);

	sem_post(&sem_messages);
}

void debug_colas() {
	if (debugging){
	printf("the size of all list are [new: %d] [ready: %d] [block: %d] [exec: %d] [exit: %d]\n",list_size(new_list),list_size(ready_list),list_size(block_list),list_size(exec_list), list_size(exit_list));

	}
}
void debug_message_list() {
	if (debugging){
	void printf_function(char*key,void*n){
		printf("ID : %s \n",key);
		//agregar a que entrenador pertenece
	};
	printf("The IDs on message list response are these: \n");
	dictionary_iterator(message_response, &printf_function);
	}
}


void destroy_poke_map() {

	void destroy_pokemon_in_pokemap(t_position* positions) {

		list_destroy_and_destroy_elements(positions, &free);
	}
	dictionary_destroy_and_destroy_elements(poke_map, &destroy_pokemon_in_pokemap);
	list_destroy_and_destroy_elements(poke_map_order, &free);
}
void debug_trainer(t_trainer* trainer) {
	if(debugging) {
		printf_trainer(trainer);
	}

}

void destroy_global_objectives() {
	void destroy_objective(t_objective* objective) {
		free(objective->pokemon);
		free(objective);
	}
	list_destroy_and_destroy_elements(objectives_list, &destroy_objective);
}
void destroy_message_team(t_message_team* message) {
	free(message->pokemon);
	free(message->position);
	free(message);
}






//SERVIDOR ESCUCHA

void iniciar_servidor_team()
{
	pthread_t thread = pthread_self();
	int32_t socket_servidor;


    struct addrinfo hints, *servinfo, *p;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(ip, port, &hints, &servinfo);

    for (p=servinfo; p != NULL; p = p->ai_next)
    {
        if ((socket_servidor = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){
        	log_error(log_utils, "Error de socket()");
        	continue;
        }
        int reuse = 1;
        if (setsockopt(socket_servidor, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(reuse)) < 0)
            perror("setsockopt(SO_REUSEADDR) failed");

		#ifdef SO_REUSEPORT
        if (setsockopt(socket_servidor, SOL_SOCKET, SO_REUSEPORT, (const char*)&reuse, sizeof(reuse)) < 0)
            perror("setsockopt(SO_REUSEPORT) failed");
    	#endif

        if (bind(socket_servidor, p->ai_addr, p->ai_addrlen) == -1) {
            close(socket_servidor);
            log_error(log_utils, "Error de bind (el puerto esta ocupado), reinicie el programa");
            for(;;);
            continue;
        }
        break;
    }

	listen(socket_servidor, SOMAXCONN);
	log_info(log_utils, "Escuchando gameboys en %s:%s", ip, port);

    freeaddrinfo(servinfo);

    while(1)
    	wait_clients(socket_servidor, log);
}

void wait_clients(int32_t socket_servidor, t_log* logger)
{
	pthread_t self = pthread_self();
	struct sockaddr_in dir_cliente;

	uint32_t tam_direccion = sizeof(struct sockaddr_in);

	log_info(log_utils, "Esperando conexion en el thread %d", self);

	int32_t socket_cliente;
	if((socket_cliente = accept(socket_servidor, (void*) &dir_cliente, &tam_direccion)) == -1)
		log_error(log_utils, "Error al aceptar cliente");
	else{
		log_info(log_utils, "Conexion aceptada");
	}


    struct thread_args* args = malloc(sizeof(struct thread_args));
    args->socket = socket_cliente;
    args->logger = logger;
    args->function = NULL;
    pthread_t team_serves_client_tid;
	pthread_create(&team_serves_client_tid,NULL,(void*)team_serves_client, (void *)args);
	pthread_detach(team_serves_client_tid);

}


void team_serves_client(void* input){
	int32_t socket = ((struct thread_args*)input)->socket;
	t_log*	logger = ((struct thread_args*)input)->logger;
	free(input);

	log_info(log_utils, "Se creo un thread para recibir mensajes del cliente %d\n", socket);

	operation_code cod_op;

	int recibido = recv_with_retry(socket, &cod_op, sizeof(int32_t), MSG_WAITALL);
	if(recibido == -1)
		log_error(log_utils, "Error del recv()");
	if(recibido == 0)
		log_error(log_utils, "Se recibieron 0 bytes, se cierra el recv()");

	log_info(log_utils, "se recibieron %d bytes", recibido);

	log_info(log_utils, "se recibio la cod op: %d\n", cod_op);

    log_info(log_utils, "se recibio el cod op: %d\n", cod_op);
	void* message = process_request(cod_op, socket, log_utils);

//se crea un nuevo hilo para atender el mensaje, y se vuelve a la escucha
	serve_thread_args* argus = malloc(sizeof(serve_thread_args));
	argus->op_code = cod_op;
	argus->message = message;

	pthread_t serve_thread;
	process_message(argus);
//	pthread_create(&serve_thread,NULL, process_message, (void *)argus);
//	pthread_detach(serve_thread);



}




#endif /* TEAM_H_ */
