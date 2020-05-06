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
	t_position position;
	char** objectives;
	char** pokemons;
} t_trainer;

int size_array (char*);
t_trainer* construct_trainer(*t_position, char**, char**);
t_position* construct_position(uint32_t, uint32_t);

int size_array (char* array){
	return char_count(array,'|');
}



t_trainer* construct_trainer(t_position* position, char** objectives, char** pokemons){
	t_trainer* trainer = malloc(sizeof(t_trainer));
	trainer->position = position;
	trainer->objectives = objectives;
	trainer->pokemons = pokemons;

	return trainer;
}

t_position* construct_position(uint32_t x , uint32_t y){
	t_position* position = malloc(sizeof(t_position));
	position->x = x;
	position->y = y;

	return position;
}

#endif /* TEAM_H_ */
