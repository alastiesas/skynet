/*
 * team_structs.c
 *
 *  Created on: 9 jun. 2020
 *      Author: utnso
 */

#include "team_structs.h"

t_trainer* create_trainer(t_position* position, char** objectives, char** pokemons) {

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
	trainer->position = position;
	trainer->objectives = objectives;
	trainer->pokemons = pokemons;
	return trainer;
}

t_trainer* create_trainer_from_config(char* config_position, char* config_objectives, char* config_pokemons) {

	t_position* position = create_position_from_config(config_position);
	char** objectives = string_split(config_objectives, "|");
	char** pokemons = string_split(config_pokemons, "|");
	return create_trainer(position, objectives, pokemons);

}

t_position* create_position_from_config(char* config_position) {
	t_position* position = malloc(sizeof(t_position));
	char ** positions_split = string_split(config_position, "|");
	position->x = atoi(positions_split[0]);
	position->y = atoi(positions_split[1]);


	return position;
}

bool success_objective(t_objective* objective) {
	return objective->count == objective->caught;
}

uint32_t dinstance(t_position* current, t_position* destiny) {
	uint32_t distance_x = abs(current->x-destiny->x);
	uint32_t distance_y = abs(current->y-destiny->y);

	return distance_x + distance_y;
}

bool trainer_full(t_trainer* trainer) {
	bool response = false;
	if(string_list_size(trainer->pokemons) ==  string_list_size(trainer->objectives))
		response = true;
	return response;
}

bool trainer_free_space(t_trainer* trainer) {
	return !trainer_full(trainer);
}

bool first_closer(t_trainer* trainer, t_trainer* trainer2,t_position* position){
	return  dinstance(trainer->position, position) <= dinstance(trainer2->position, position);
}
