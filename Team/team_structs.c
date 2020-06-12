/*
 * team_structs.c
 *
 *  Created on: 9 jun. 2020
 *      Author: utnso
 */

#include "team_structs.h"

t_trainer* create_trainer(t_position* position, char** objectives, char** pokemons) {

	t_trainer* trainer = malloc(sizeof(t_trainer));//create
	trainer->action = FREE;//create
	trainer->target = malloc(sizeof(t_target));//create
	trainer->target->position = NULL;//create
	trainer->target->distance = NULL;//create
	trainer->target->pokemon = NULL;//create
	trainer->burst = 0;//create
	trainer->quantum = 0;//create
	trainer->action_burst = 0;//create
	sem_init(&trainer->sem_thread, 0, 0);//create
	trainer->position = position;//create parametro
	trainer->objectives = objectives;//create parametro
	trainer->pokemons = pokemons;//create parametro
	return trainer;
}

t_trainer* create_trainer_from_config(char* config_position, char* config_objectives, char* config_pokemons) {

	t_position* position = create_position_from_config(config_position);//create parametro
	char** objectives = string_split(config_objectives, "|");//create parametro
	char** pokemons = string_split(config_pokemons, "|");//create parametro
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
	if(size_array_config(trainer->pokemons) ==  size_array_config(trainer->objectives))
		response = true;
	return response;
}

bool trainer_free_space(t_trainer* trainer) {
	return !trainer_full(trainer);
}

bool first_closer(t_trainer* trainer, t_trainer* trainer2,t_position* position){
	return  dinstance(trainer->position, position) <= dinstance(trainer2->position, position);
}
