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
	NO_CHANGE = 0,
	BURST = 1,
	CATCH_PETITION = 2,
	TRADE_WAITING = 3,
	FULL_QUANTUM = 4,
	SJF_PRIORITY = 5,
	EXIT = 6,
} t_state_change_reason;


typedef enum {
	FREE= 0,
	MOVE = 1,
	CATCH = 2,
	CATCHING = 3,
	TRADE = 4,
	FINISH = 5,
} t_action;

typedef struct {
	char* pokemon;
	t_position* position;
	uint32_t trainer_id;
	bool catching;
} t_target;

typedef struct
{
	pthread_t tid;
	uint32_t id;

	//pthread_attr_t attr;
	sem_t sem_thread;
	t_action action;
	uint32_t quantum;
	uint32_t burst;
	uint32_t burst_estimate;
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
	t_trainer* trainer;
	char* pokemon;
	t_position* position;
} t_message_team;


void debug_trainer(t_trainer* trainer);
//constructores
t_trainer* create_trainer(uint32_t id, t_position* position, char** objectives, char** pokemons);
t_trainer* create_trainer_from_config(uint32_t id, char* config_position, char* config_objectives, char* config_pokemons);
t_position* create_position_from_config(char* positions);
t_target* create_target(char* pokemon, t_position* position, uint32_t trainer_id, bool catching);
void destroy_target(t_target* target);
void trainer_set_target(t_trainer* trainer, t_target* target);
void add_pokemon(t_trainer* trainer, char*pokemon);
//FIN constructores

//destructores
void destroy_trainer(t_trainer* trainer);
//FIN destructores

//consultas
bool success_objective(t_objective* objective);
uint32_t distance(t_position* current, t_position* destiny);
bool trainer_full(t_trainer* trainer);
bool trainer_free_space(t_trainer* trainer);
bool first_closer(t_trainer* trainer, t_trainer* trainer2,t_position* position);
int32_t closest_free_trainer(t_list* list_trainer, t_position* destiny, char* channel);
int32_t closest_free_trainer_job(t_list* list_trainer, t_position* destiny);
int32_t closest_free_trainer_deadlock(t_list* list_trainer, t_position* destiny);
bool trainer_success_objective(t_trainer* trainer);
bool trainer_needs(t_trainer* trainer, char* pokemon);
bool trainer_locked(t_trainer* trainer);
t_list* trainer_held_pokemons(t_trainer* trainer);
t_list* trainer_waiting_pokemons(t_trainer* trainer);
bool trainer_full_quantum(t_trainer* trainer, uint32_t quantum);
uint32_t trainer_burst_estimate(t_trainer* trainer);
//*/

t_algorithm read_algorithm(char* config_algorithm);

//FIN consultas

#endif /* TEAM_STRUCTS_H_ */
