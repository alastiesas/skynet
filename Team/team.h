/*
 * team.h
 *
 *  Created on: 19 abr. 2020
 *      Author: utnso
 */

#ifndef TEAM_H_
#define TEAM_H_

#include<stdio.h>
#include<stdlib.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<commons/collections/list.h>
#include<readline/readline.h>
#include <utils.h>
#include <stdbool.h>
#include <pthread.h>
#include <semaphore.h>
#include <conexion.h>
#include <mensajes.h>
#include <structs.h>


t_config* config;
t_log* log;

t_list* new_list;
t_list* ready_list;
t_list* block_list;
t_list* exec_list;
t_list* exit_list;
uint32_t context_changes = 0;
uint32_t cpu_cycles = 0;
t_list* objetives_list;
uint32_t time_delay = 1; // TIENE QUE LEVANTAR DATO DEL CONFIG
t_dictionary* poke_map;
sem_t sem_exec;
t_list* messages_list;
sem_t sem_messages_list;
sem_t sem_messages;

typedef enum {
	EMPTY = 0,
	FIFO= 1,
	RR = 2,
	SJFS = 3,
	SJFC = 4,
} t_algorithm;

t_algorithm algorithm = FIFO;

typedef enum {
	FREE= 0,
	MOVE = 1,
	CATCHING = 2,
	TRADE = 3,
} t_action;

typedef struct {
	char* pokemon;
	t_position* position;
	uint32_t distance;
	bool catching;
} t_target;

typedef struct
{
	pthread_t tid;
	//pthread_attr_t attr;
	sem_t sem_thread;
	t_action action;
	uint32_t quantum;
	uint32_t burst;
	uint32_t action_burst;
	//esto reemplaza a target->position y target->pokemon
	//t_objective* objetivo ( target->position, nombre del pokemon y la distancia hasta el pokemon)
	//t_position* action_position;
	t_target* target;

	t_position* position;
	char** objectives;
	char** pokemons;
} t_trainer;

typedef struct
{
	char* pokemon;
	uint32_t count;
	uint32_t caught;
	uint32_t catching;
} t_objective;

typedef struct
{
	char* string;
	t_objective* objective;
} t_index;

typedef struct
{
	t_trainer* trainer;
	void(*callback)(void*);

} t_callback;


typedef struct
{
	pthread_t tid;
	t_message_catch* message;

} t_catch;



int size_array (char*);
int char_count(char* array, char parameter);
int size_array_config(char** array);
t_trainer* construct_trainer(char* positions, char**, char**);
t_position* construct_position(char*);
void initialize_trainers(char**,char**,char**);
void state_change(uint32_t index, t_list* from,t_list* to);
t_index* search_index(t_index* index ,t_objective* objective);
t_list* add_trainer_to_objective(t_list* list_global_objectives, t_trainer* trainer);
t_list* initialize_global_objectives(t_list* list);
void add_caught(t_list* list, char* pokemon);
bool success_objective(t_objective* objective);
bool success_global_objective(t_list* global_objectives);
void *trainer_thread(t_callback* callback_thread);
uint32_t dinstance(t_position* current, t_position* destiny);
int32_t closest_free_trainer(t_list* entrenadores, t_position* posicion);
bool trainer_full(t_trainer* trainer);
bool trainer_free_space(t_trainer* trainer);
void transition_new_to_ready(uint32_t index);
void transition_ready_to_exec(uint32_t index);
void transition_exec_to_ready();
void transition_exec_to_block();
void transition_exec_to_exit();
void transition_block_to_ready(uint32_t index);
void transition_block_to_exit(uint32_t index);
void fifo_algorithm();
void rr_algorithm();
void sjfs_algorithm();
void sjfc_algorithm();
void move_up(t_trainer* trainer);
void move_down(t_trainer* trainer);
void move_right(t_trainer* trainer);
void move_left(t_trainer* trainer);
void move(t_trainer* trainer);
void trainer_assign_job(char* pokemon, t_list* positions);
void long_term_scheduler();
void trainer_assign_move(char* type,char* pokemon, uint32_t index, t_position* position, bool catching);
bool first_closer(t_trainer* trainer, t_trainer* trainer2,t_position* position);
void callback_fifo(t_trainer* trainer);
void* exec_thread();
void add_catching(t_list* list, char* pokemon);
bool pokemon_is_needed(char* pokemon);
void* sender_thread();



int size_array (char* array)
{
	return char_count(array,'|');
}

int char_count(char* array, char parameter)
{
	int size_test =  strlen (array);
	int count = 1;
	for(int i = 0; i< size_test ; i++)
		if(array[i] == parameter)
					count++;
	return count;
}

int size_array_config(char** array)
{
	int j = 0;
		while(array[j] != NULL){
			//printf("test_objetivos %s\n", array[j]);
			j++;
		}
		return j;
}


void callback_fifo(t_trainer* trainer){
	if(trainer->action == CATCHING){
			//llama funcion para enviar mensaje, recibe entrenador por parametro y hace post a hilo de sender
			//sem_post(&sem_sender)
			printf("aca no llega NUNCA\n");
			sem_post(&sem_exec);
			//signal a semaforo de exec
		}
	else if(trainer->action == FREE){
		sem_post(&sem_exec);
		//signal a semaforo de exec
	}
	else
		sem_post(&trainer->sem_thread);

}

t_trainer* construct_trainer(char* positions, char** objectives, char** pokemons)
{
	t_trainer* trainer = malloc(sizeof(t_trainer));
	trainer->action = FREE;
	trainer->target = malloc(sizeof(t_target));
	trainer->target->position = NULL;
	trainer->target->distance = NULL;
	trainer->target->pokemon = NULL;
	trainer->burst = 0;
	trainer->quantum = 0;
	trainer->action_burst = 0;
	sem_init(&trainer->sem_thread, 0, 0);
	trainer->position = construct_position(positions);
	trainer->objectives = string_split(objectives, "|");
	trainer->pokemons = string_split(pokemons, "|");
	//struct x
	// LO QUE HABLAMOS CON AYUDANTE 2

	// ACA VA CALLBACK
	t_callback* callback_thread = malloc(sizeof(t_callback));
	callback_thread->trainer = trainer;
	//ACA VA SEGUN ALGORITMO
	callback_thread->callback = &callback_fifo;
	pthread_create(&(trainer->tid), NULL, trainer_thread, callback_thread);

	/*
	printf("test debug pokemon %d\n",trainer->position->x);
	printf("test debug pokemon %d\n",trainer->position->y);
	printf("test debug pokemon %s\n",trainer->objectives[0]);
	printf("test debug pokemon %s\n",trainer->pokemons[2]);
	*/
	return trainer;
}

t_position* construct_position(char* positions)
{
	t_position* position = malloc(sizeof(t_position));
	char ** positions_split = string_split(positions, "|");
	position->x = atoi(positions_split[0]);
	position->y = atoi(positions_split[1]);


	return position;
}

void initialize_trainers(char** positions_config,char** objectives_config,char** pokemons_config)
{
	int k = 0;
	while(positions_config[k]){
		t_trainer* test_entrenador = construct_trainer(positions_config[k], objectives_config[k], pokemons_config[k]);
		list_add(new_list, test_entrenador);
		k++;
	}
}

void state_change(uint32_t index, t_list* from,t_list* to)
{
	void* element = list_remove(from, index);
	list_add(to, element);
}

//void *list_find(t_list *, bool(*closure)(void*));

t_index* search_index(t_index* index ,t_objective* objective)
{
	if(0 == strcmp(objective->pokemon, index->string)){
		index->objective = objective;
	}
	return index;
}

t_objective* find_key(t_list* list, char* key)
{
	t_index* index = malloc(sizeof(t_index));
	index->string = key;
	index->objective = NULL;
	//void*(*function)(void*, void*) = &search_index;
	index = (t_index*) list_fold(list,(void*)index,(void*)&search_index);
	t_objective* objective = index->objective;
	free(index);
	return objective;
}

void add_objective(t_list* list, char* pokemon)
{
	t_objective* objective = find_key(list, pokemon);
	if(objective != NULL)
		objective->count++;
	else{
		objective = malloc(sizeof(t_objective));
		objective->pokemon = pokemon;
		objective->count = 1;
		objective->caught = 0;
		objective->catching = 0;
		list_add(list,(void*)objective);
	}
}

void add_caught(t_list* list, char* pokemon)
{
	t_objective* objective = find_key(list, pokemon);
	if(objective != NULL){
		objective->caught++;
		objective->catching--;
	}
	else
		printf("Lo rompiste todo, maldito idiota\n");
}

void add_catching(t_list* list, char* pokemon)
{
	t_objective* objective = find_key(list, pokemon);
	if(objective != NULL){
		objective->catching++;
	}
	else
		printf("Lo rompiste todo, maldito idiota\n");
}

bool pokemon_is_needed(char* pokemon)
{
	printf("LN 301 the pokemonn is %s\n", pokemon);
	t_objective* test = (t_objective*) list_get(objetives_list,0);
	printf("LN 301 the pokemon OBJECTIVE is %s\n", test->pokemon);
	t_objective* objective = find_key(objetives_list, pokemon);
	printf("holaaaaaaaaaa\n");

	if(objective == NULL)
		printf("la concha de la lora\n");
	//return (objective->count > (objective->caught + objective->catching));
	return objective->count > (objective->caught + objective->catching);
}

t_list* add_trainer_to_objective(t_list* list_global_objectives,t_trainer* trainer)
{
	int i = 0;
	while(trainer->objectives[i]!= NULL){
		add_objective(list_global_objectives, trainer->objectives[i]);
		i++;
	}
	i = 0;
	while(trainer->pokemons[i]!= NULL){
		add_caught(list_global_objectives, trainer->pokemons[i]);
		i++;
	}
	return list_global_objectives;
}


t_list* initialize_global_objectives(t_list* list)
{
	t_list* list_global_objectives = list_create();
	list_global_objectives = (t_list*) list_fold( list,(void*)list_global_objectives,(void*)&add_trainer_to_objective);
	return list_global_objectives;
}

bool success_objective(t_objective* objective)
{
	return objective->count == objective->caught;
}

bool success_global_objective(t_list* global_objectives)
{
	return (bool) list_all_satisfy(global_objectives,&success_objective);
}

void *trainer_thread(t_callback* callback_thread)
{
	//if(/*si es 0 menor*/)
		//funcion cambiar valor global de variable interrumption
		//COMO SE DESALOJA A UN HILO DE ENTENADOR, COMO SE ENTERA EL PLANIFICADOR O EL HILO DE EJECUCION!!
		//while(1) agregar
	t_trainer* trainer = callback_thread->trainer;

	printf("hola soy el entrenador %d\n", trainer->tid);
	printf("aca no esta llegando123\n");
	while(1){
		printf("ACA PASO 1\n");
		sem_wait(&trainer->sem_thread);
		printf("ACA PASO 2\n");
		switch(trainer->action){
			case MOVE:
				printf("Me estoy moviendo, comando MOVE\n");
				printf("Arranca con (%d,%d)\n", trainer->position->x,trainer->position->y);
				//aca va un if, no un while
				if(trainer->position->x != trainer->target->position->x || trainer->position->y != trainer->target->position->y)
					move(trainer);
				else if(trainer->target->catching)
					trainer->action = CATCHING;
				else
					trainer->action = TRADE;
				printf("Llegue a (%d,%d)\n", trainer->position->x,trainer->position->y);
				break;
			case CATCHING:
				printf("Estoy atrapando pokemon, comando CATCHING\n");
				//llamada el broker
				//block
				//vuelve block
				//si es positivo, modifca agrega inventario y objetivo global, o si pasa exit
				//si no, no?, pasa a free de nuevo
				break;
			case TRADE:
				printf("Estoy tradeando pokemon, comando TRADE\n");
				break;
			default:
				printf("No hago nada\n");
				trainer->action = FREE;
				break;
		}
		//aca va el callback
		callback_thread->callback(trainer);
	}

	printf("HILO debug del entrenador %s\n", trainer->objectives[0]);
	//pthread_mutex_unlock(trainer->semThread);
	//sem_post(&trainer->sem_thread);
	return NULL;
}

uint32_t dinstance(t_position* current, t_position* destiny)
{
	uint32_t distance_x = abs(current->x-destiny->x);
	uint32_t distance_y = abs(current->y-destiny->y);

	return distance_x + distance_y;
}

int32_t closest_free_trainer(t_list* list_trainer, t_position* destiny)
{
	//printf("elements count %d\",list_trainer->elements_count);
	int32_t i = -1;
	int32_t index = -1;
	printf("LA POSICION DE DESTINO ES (%d,%d)\n",destiny->x,destiny->y);
	printf("el size de la lista es %d\n", list_size(list_trainer));

	if(list_size(list_trainer) != 0)
	{

		t_link_element* element = list_trainer->head;
		uint32_t distance = -1;
		//uint32_t distance = dinstance(((t_trainer*)element->data)->position, destiny);
		//t_trainer* trainer = (t_trainer*) element->data;

		//element = element->next;
		i = 0;
		while(element != NULL) {
			uint32_t distance_aux = dinstance(((t_trainer*)element->data)->position, destiny);
			printf("LA POSICION DEL ENTRANDOR ES (%d,%d)\n",((t_trainer*)element->data)->position->x,((t_trainer*)element->data)->position->y);
			printf("distance actual %d\n" ,distance_aux);
			printf("distance minima %d\n" ,distance);
			printf("el actions es %d\n", ((t_trainer*)element->data)->action);
			printf("is free  %d\n" ,trainer_free_space(((t_trainer*)element->data)));

			if(((t_trainer*)element->data)->action == FREE && trainer_free_space(((t_trainer*)element->data)) && (distance_aux < distance || distance < 0)){
				distance = distance_aux;
				//trainer = (t_trainer*) element->data;
				index = i;
				printf("->SELECCIONADO %d\n",i);
			}
			else
				printf("->NO SELECCIONADO %d\n",i);
			element = element->next;
			i++;
		}
	}
	printf("El index que retorna %d\n",index);
	return index;
}

//encuentra al entrenador mas cerca de la posicion en ambas listas

//compara la posicion de los 2 entrenadores y la lista.

//setea al entrenador, el pokemon y la distancia objetivo

//dependiendo de la lista en la que este hace el transition.

//VAMOS POR ACA FEDE!!!!!!!!! 13/05/2020
//large_term_scheduler
//void 		  dictionary_iterator(t_dictionary *, void(*closure)(char*,void*));

void long_term_scheduler(){
	dictionary_iterator(poke_map, &trainer_assign_job);
}

void trainer_assign_move(char* type,char* pokemon, uint32_t index, t_position* position, bool catching)
{
	if(strcmp(type,"NEW") == 0){
		t_trainer* trainer = (t_trainer*) list_get(new_list, index);
		trainer->target->pokemon = pokemon;
		trainer->action = MOVE;
		trainer->target->position = position;
		trainer->target->catching = catching;
		printf("aca el ACTION ES %d\n", trainer->action);
		transition_new_to_ready(index);
	}
	else if(strcmp(type,"BLOCK") == 0){
		t_trainer* trainer = (t_trainer*) list_get(block_list, index);
		trainer->target->pokemon = pokemon;
		trainer->action = MOVE;
		trainer->target->position = position;
		trainer->target->catching = catching;
		transition_block_to_ready(index);
	}
}

bool first_closer(t_trainer* trainer, t_trainer* trainer2,t_position* position){
	return  dinstance(trainer->position, position) <= dinstance(trainer2->position, position);
}

void trainer_assign_job(char* pokemon, t_list* positions)
{
	t_link_element* element = positions->head;
	t_position* position;
	int32_t i = -1;
	if(pokemon_is_needed(pokemon)){
		while(element != NULL) {
			position = (t_position*) element->data;
			printf("LA POSICION SIEMPRE ES (%d,%d)\n", position->x,position->y);
			// se remplaza la position por lo que devuelva del diccionario
			t_trainer* trainer_new = NULL;
			t_trainer* trainer_block = NULL;
			int32_t closest_from_new = closest_free_trainer(new_list, position);
			printf("aca llleoogoogogoogog\n");
			int32_t closest_from_block = closest_free_trainer(block_list, position);

			if(closest_from_new >= 0){
				trainer_new = list_get(new_list,closest_from_new);
			}
			if(closest_from_block >= 0){
				trainer_block = list_get(block_list,closest_from_block);
			}

			//bool first_closer(t_trainer* trainer, t_trainer* trainer2,t_position* position)

			if(trainer_new != NULL && (trainer_block == NULL || first_closer(trainer_new, trainer_block, position))){
				add_catching(objetives_list, pokemon);
				trainer_assign_move("NEW",pokemon, closest_from_new,position,1);
				list_remove(positions, (i+1));
				//aca deberia sacar la posicion de la lista de posiciones del pokemon, solo sacarla NO! borrarla
			}
			else if(trainer_block != NULL && (trainer_new == NULL || first_closer(trainer_block, trainer_new, position))){
				add_catching(objetives_list, pokemon);
				trainer_assign_move("BLOCK",pokemon, closest_from_block,position,1);
				list_remove(positions, (i+1));
				//aca deberia sacar la posicion de la lista de posiciones del pokemon, solo sacarla NO! borrarla
			}
			else{
				printf("no hay entrenadores en las listas de new ni block \n");
			}

			if(list_size(positions) == 0){
				printf("lasti was here\n");
				dictionary_remove_and_destroy(poke_map, pokemon,&list_destroy);
			}

			//si el size de positions es , deberia eliminar el pokemon del mapa
			element = element->next;

			// NO OLVIDAR BORRAR LA POSICION QUE YA SE USO
			// Si tiene una sola posicion, entonces se borra el pokemon del pokemap ??
			i++;
		}
	}
	else
		printf("The pokemon is already full\n");


}

void add_to_poke_map(char* pokemon, t_position* position)
{
	//CASO DE QUE NO ESTE EL POKEMON EN EL MAP
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

	//EL CASO DE QUE ESTE EL POKEMON EN EL MAPA
	//buscar el pokemon en la lista y retornar el indice


}


bool trainer_full(t_trainer* trainer)
{
	bool response = false;
	if(size_array_config(trainer->pokemons) ==  size_array_config(trainer->objectives))
		response = true;
	return response;
}

bool trainer_free_space(t_trainer* trainer)
{
	return !trainer_full(trainer);
}

transition_new_to_ready(uint32_t index)
{
	state_change(index,new_list,ready_list);
	context_changes++;
}

transition_ready_to_exec(uint32_t index)
{
	state_change(index,ready_list,exec_list);
	context_changes++;
	t_trainer* trainer = list_get(exec_list,0);
	printf("ACA HACEMOS EL POST DE HILO TRAINER\n");
	sem_post(&trainer->sem_thread);
}

transition_exec_to_ready()
{
	state_change(0,exec_list,ready_list);
	context_changes++;
}

transition_exec_to_block()
{
	state_change(0,exec_list,block_list);
	context_changes++;
}

transition_exec_to_exit()
{
	state_change(0,exec_list,exit_list);
	context_changes++;
}

transition_block_to_ready(uint32_t index)
{
	state_change(index,block_list,ready_list);
	context_changes++;
}

transition_block_to_exit(uint32_t index)
{
	state_change(index,block_list,exit_list);
	context_changes++;
}

void fifo_algorithm()
{
	printf("estoy en fifo\n");
	if(list_size(exec_list) > 0)
		transition_exec_to_block();
	if(list_size(ready_list) > 0)
		transition_ready_to_exec(0);
}

void rr_algorithm()
{
	printf("Estoy en RR\n");
}

void sjfs_algorithm()
{
	printf("Estoy en SJFS\n");
}

void sjfc_algorithm()
{
	printf("Estoy en SJFC\n");
}

void short_term_scheduler()
{
	printf("aca llego bien\n");
	if(list_size(exec_list)>0){
		t_trainer* trainer_exec = (t_trainer*) list_get(exec_list,0);
			printf("aca llego bien12\n");
			//ACA CONSULTAMOS SI SALE POR I/0
			printf("the trainer exec has %s\n",trainer_exec->target->pokemon);
			printf("the trainer exec has %d\n",trainer_exec->target->position->x);
			printf("the trainer exec has %d\n",trainer_exec->target->position->y);
			printf("the trainer exec has %d\n",trainer_exec->target->catching);
			if(trainer_exec->target->catching){
				printf("aca llego bien11\n");
				sem_wait(&sem_messages_list);
				printf("aca llego bien2\n");
				t_message_catch* message = malloc(sizeof(t_message_catch));
				message->tid = trainer_exec->tid;
				//message->message = construct_catch_long(trainer_exec->target->pokemon, trainer_exec->target->position->x, trainer_exec->target->position->y);
				list_add(messages_list,message);
				sem_post(&sem_messages_list);
				printf("aca llego bien3\n");
				sem_post(&sem_messages);
				printf("aca llego bien4\n");


				//PENSAR QUE VA EN MESSAGE LIST ??
				//TARGET, tid del entrenador, correlative_id??

				transition_exec_to_block();
			}
	}


	switch(algorithm){
			case FIFO:
				fifo_algorithm();
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
}

void move_up(t_trainer* trainer)
{
	sleep(time_delay);
	trainer->position->y++;
	trainer->burst++;
	cpu_cycles++;
	printf("Se movio UP\n");
}

void move_down(t_trainer* trainer)
{
	sleep(time_delay);
	trainer->position->y--;
	trainer->burst++;
	cpu_cycles++;
	printf("Se movio DOWN\n");
}

void move_right(t_trainer* trainer)
{
	sleep(time_delay);
	trainer->position->x++;
	trainer->burst++;
	cpu_cycles++;
	printf("Se movio RIGHT\n");
}

void move_left(t_trainer* trainer)
{
	sleep(time_delay);
	trainer->position->x--;
	trainer->burst++;
	cpu_cycles++;
	printf("Se movio LEFT\n");
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


void* exec_thread()
{
	sem_init(&sem_exec, 0, 1);
	while(1){
			//sem_wait(&sem_exec); Este seria el unico semaforo, despues cambiar
			if(list_size(ready_list)>0 || list_size(exec_list)>0){
				printf("acallego?\n");
				sem_wait(&sem_exec);
				short_term_scheduler();
			}
			else{
				printf("aca pasa al planificador de largo plazo\n");
				//post al planificador de largo plazo
			}


			//printf("la lista de block queda %d\n",list_size(block_list));
			//printf("la lista de new queda %d\n",list_size(new_list));
			printf("cantidad de CPU %d\n", cpu_cycles);
			//if a funcion que consula objetivos globales
			//BREAK PARA CORTAR WHILE CUANDO TERMINAN OBJETIVOS
		}

		printf("cantidad de CPU %d\n", cpu_cycles);
}


void* sender_thread()
{
	sem_init(&sem_messages_list, 0, 0);
	sem_init(&sem_messages, 0, 0);
	while(1){
		sem_wait(&sem_messages);
	}
	//send al broker, (pokemon, posicion, el entrenador)

}

void subscribe(queue_code queue_code) {

	char* ip = config_get_string_value(config, "BROKER_IP");
	char* port = config_get_string_value(config, "BROKER_PORT");
	int32_t socket = connect_to_server(ip, port, log);

	uint32_t id = config_get_string_value(config, "ID");
	t_package* package = serialize_suscripcion(atoi(id), queue_code);

	send_paquete(socket, package); /*pending2*/
	if (receive_ACK(socket, log) == -1) {
		exit(EXIT_FAILURE);
	}


	//Quedarse recibiendo mensajes permanentemente, no hace falta otro hilo

	struct thread_args* args = malloc(sizeof(struct thread_args));
	args->socket = socket;
	args->logger = log;
	pthread_t thread;
	pthread_create(&thread, NULL, (void*) recibir_muchos_mensajes, args); /*pending2*/


	//Al completar el objetivo global, enviar tres mensajes al broker con ID de proceso e ID de cola asi puede liberar memoria
}


#endif /* TEAM_H_ */
