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

#endif /* TEAM_H_ */
