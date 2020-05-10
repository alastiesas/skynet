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

typedef struct
{
	uint32_t id;
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
t_trainer* construct_trainer(char* positions, char**, char**);
t_position* construct_position(char*);
t_list * initialize_trainers(char**,char**,char**);
void state_change(int index, t_list* from,t_list* to);
t_index* search_index(t_index* index ,t_objective* objective);
t_list* add_trainer_to_objective(t_list* list_global_objectives, t_trainer* trainer);
t_list* initialize_global_objectives(t_list* list);



int size_array (char* array)
{
	return char_count(array,'|');
}


t_trainer* construct_trainer(char* positions, char** objectives, char** pokemons)
{
	t_trainer* trainer = malloc(sizeof(t_trainer));

	trainer->position = construct_position(positions);
	trainer->objectives = string_split(objectives, "|");
	trainer->pokemons = string_split(pokemons, "|");
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

t_list* initialize_trainers(char** positions_config,char** objectives_config,char** pokemons_config)
{
	int k = 0;
	t_list * trainers_list = list_create();
	while(positions_config[k]){
		t_trainer* test_entrenador = construct_trainer(positions_config[k], objectives_config[k], pokemons_config[k]);
		list_add(trainers_list, test_entrenador);
		k++;
	}

	return trainers_list;
}

void state_change(int index, t_list* from,t_list* to)
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

t_list* add_trainer_to_objective(t_list* list_global_objectives,t_trainer* trainer)
{
	int i = 0;
	while(trainer->objectives[i]!= NULL){
		add_objective(list_global_objectives, trainer->objectives[i]);
		i++;
	}
	return list_global_objectives;
}


t_list* initialize_global_objectives(t_list* list)
{

	t_list* list_global_objectives = list_create();
	//void*(*function)(void*, void*) = &add_trainer_to_objective;
	list_global_objectives = (t_list*) list_fold( list,(void*)list_global_objectives,(void*)&add_trainer_to_objective);
	//t_objective global_objectives = malloc(sizeof(t_objective));
	//list_iterate(list, void(*closure)(void*))
	return list_global_objectives;
}

//void* list_fold(t_list* self, void* seed, void*(*operation)(void*, void*));


#endif /* TEAM_H_ */
