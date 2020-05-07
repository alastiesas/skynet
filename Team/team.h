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

typedef struct
{
	uint32_t id;
	t_position* position;
	char** objectives;
	char** pokemons;
} t_trainer;

int size_array (char*);
t_trainer* construct_trainer(char* positions, char**, char**);
t_position* construct_position(char*);
t_list * initialize_trainers(char**,char**,char**);
void state_change(int index, t_list* from,t_list* to);




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

t_list * initialize_trainers(char** positions_config,char** objectives_config,char** pokemons_config)
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


#endif /* TEAM_H_ */
