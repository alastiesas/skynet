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
#include "serializer.h"
#include "utils.h"
#include <stdbool.h>
#include <pthread.h>
#include <semaphore.h>

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

typedef enum {
	EMPTY = 0,
	FIFO= 1,
	RR = 2,
	SJFS = 3,
	SJFC = 4,
}t_algorithm;

t_algorithm algorithm = FIFO;

typedef enum {
	FREE= 0,
	MOVE = 1,
	CATCHING = 2,
	TRADE = 3,
}t_action;

typedef struct
{
	pthread_t tid;
	//pthread_attr_t attr;
	sem_t sem_thread;
	t_action action;
	uint32_t quantum;
	uint32_t burst;
	uint32_t action_burst;
	t_position* move_destiny;
	//t_position* action_position;
	char* action_pokemon;

	t_position* position;
	char** objectives;
	char** pokemons;
} t_trainer;

typedef struct
{
	char* pokemon;
	uint32_t count;
	uint32_t caught;
} t_objective;

typedef struct
{
	char* string;
	t_objective* objective;
} t_index;


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
void *trainer_thread(t_trainer* trainer);
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


t_trainer* construct_trainer(char* positions, char** objectives, char** pokemons)
{
	t_trainer* trainer = malloc(sizeof(t_trainer));
	trainer->action = FREE;
	trainer->move_destiny = NULL;
	trainer->burst = 0;
	trainer->quantum = 0;
	trainer->action_burst = 0;
	sem_init(&trainer->sem_thread, 0, 0);
	trainer->position = construct_position(positions);
	trainer->objectives = string_split(objectives, "|");
	trainer->pokemons = string_split(pokemons, "|");
	pthread_create(&(trainer->tid), NULL, trainer_thread, trainer);

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
		list_add(list,(void*)objective);
	}
}

void add_caught(t_list* list, char* pokemon)
{
	t_objective* objective = find_key(list, pokemon);
	if(objective != NULL)
		objective->caught++;
	else
		printf("Lo rompiste todo, maldito idiota\n");
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

void *trainer_thread(t_trainer* trainer)
{
	//if(/*si es 0 menor*/)
		//funcion cambiar valor global de variable interrumption

		sem_wait(&trainer->sem_thread);
	switch(trainer->action){
		case MOVE:
			printf("Me estoy moviendo, comando MOVE\n");
			printf("Arranca con (%d,%d)\n", trainer->position->x,trainer->position->y);
			while(trainer->position->x != trainer->move_destiny->x || trainer->position->y != trainer->move_destiny->y)
				move(trainer);
			printf("Llegue a (%d,%d)\n", trainer->position->x,trainer->position->y);
			break;
		case CATCHING:
			printf("Estoy atrapando pokemon, comando CATCHING\n");
			break;
		case TRADE:
			printf("Estoy tradeando pokemon, comando TRADE\n");
			break;
		default:
			printf("No hago nada\n");
			break;
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

void trainer_assign_job(char* pokemon, t_list* positions)
{
	t_link_element* element = positions->head;
	t_position* position;
	int32_t i = -1;
	while(element != NULL) {
		position = (t_position*) element->data;
		printf("LA POSICION SIEMPRE ES (%d,%d)\n", position->x,position->y);
		// se remplaza la position por lo que devuelva del diccionario
		int32_t closest_from_new = closest_free_trainer(new_list, position);
		printf("aca llleoogoogogoogog\n");
		int32_t closest_from_block = closest_free_trainer(block_list, position);

		if(closest_from_new != -1 && closest_from_block != -1){
			if(closest_from_new <= closest_from_block){
				//transicion de new a ready
				t_trainer* trainer = (t_trainer*) list_get(new_list, closest_from_new);
				trainer->action_pokemon = pokemon;
				trainer->action = MOVE;
				trainer->move_destiny = position;
				transition_new_to_ready(closest_from_new);
			}
			else
			{
				// transicion de block a ready
				t_trainer* trainer = (t_trainer*) list_get(block_list, closest_from_block);
				trainer->action_pokemon = pokemon;
				trainer->action = MOVE;
				trainer->move_destiny = position;
				transition_block_to_ready(closest_from_block);
			}
		}
		else if(closest_from_new != -1){
			//transicion de new a ready
			t_trainer* trainer = (t_trainer*) list_get(new_list, closest_from_new);
			trainer->action_pokemon = pokemon;
			trainer->action = MOVE;
			trainer->move_destiny = position;
			transition_new_to_ready(closest_from_new);
		}
		else if(closest_from_block != -1){
			//transicion de block a ready
			t_trainer* trainer = (t_trainer*) list_get(block_list, closest_from_block);
			trainer->action_pokemon = pokemon;
			trainer->action = MOVE;
			trainer->move_destiny = position;
			transition_block_to_ready(closest_from_block);
		}
		else{
			//nada
			printf("no hay entrenadores en las listas de new ni block \n");
		}

		element = element->next;

		// NO OLVIDAR BORRAR LA POSICION QUE YA SE USO
		i++;
	}

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
	if(trainer->move_destiny->x > trainer->position->x)
		move_right(trainer);
	else if(trainer->move_destiny->x < trainer->position->x)
		move_left(trainer);
	else if(trainer->move_destiny->y > trainer->position->y)
		move_up(trainer);
	else if(trainer->move_destiny->y < trainer->position->y)
		move_down(trainer);
}
//void* list_fold(t_list* self, void* seed, void*(*operation)(void*, void*));


#endif /* TEAM_H_ */
