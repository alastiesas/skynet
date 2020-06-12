/*
 * team_structs.h
 *
 *  Created on: 9 jun. 2020
 *      Author: utnso
 */

#ifndef TEAM_STRUCTS_H_
#define TEAM_STRUCTS_H_

#include"includes.h"

typedef enum {
	EMPTY = 0,
	FIFO= 1,
	RR = 2,
	SJFS = 3,
	SJFC = 4,
} t_algorithm;



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
	void(*callback)(t_trainer*);

} t_callback;


typedef struct
{
	pthread_t tid;
	t_message_catch* message;

} t_catch;

typedef struct
{
	pthread_t tid;
	char* pokemon;
	t_position* position;
} t_message_team;

typedef struct
{
	pthread_t tid;
	uint32_t message_id;
} t_message_team_receive;


//constructores
t_trainer* create_trainer(t_position* position, char** objectives, char** pokemons);
t_trainer* create_trainer_from_config(char* config_position, char* config_objectives, char* config_pokemons);
t_position* create_position_from_config(char* positions);
//FIN constructores

//consultas
bool success_objective(t_objective* objective);
uint32_t dinstance(t_position* current, t_position* destiny);
bool trainer_full(t_trainer* trainer);
bool trainer_free_space(t_trainer* trainer);
bool first_closer(t_trainer* trainer, t_trainer* trainer2,t_position* position);
//FIN consultas

#endif /* TEAM_STRUCTS_H_ */
