/*
 * team_structs.c
 *
 *  Created on: 9 jun. 2020
 *      Author: utnso
 */

#include "team_structs.h"


void debug_trainer(t_trainer* trainer) {
	printf("\n**DEBUG DEL ENTRENADOR**\n");
	printf("trainer->id: %d\n", trainer->id);
	printf("trainer->action: %d\n", trainer->action);

	if(trainer->target != NULL){
		printf("trainer->target: [pokemon: %s, posicion: (%d, %d), catching: %d, trainer_id: %d]\n", trainer->target->pokemon, trainer->target->position->x, trainer->target->position->y, trainer->target->catching, trainer->target->trainer_id);
	}
	else {
		printf("sin target\n");
	}

	printf("trainer->posicion: (%d, %d)\n", trainer->position->x, trainer->position->y);
	uint32_t i = 0;


	void debug_pokemon(char* string, uint32_t* count) {
		printf(" (%s: %d) ", string, *count);
	}

	printf("trainer->objetivos: [");
	dictionary_iterator(trainer->objectives, &debug_pokemon);
	printf("]\n");
	printf("trainer->pokemons: [");
	dictionary_iterator(trainer->pokemons, &debug_pokemon);
	printf("]\n");

	printf("**FIN DEBUG**\n\n");
}

t_trainer* create_trainer(uint32_t id, t_position* position, t_dictionary* objectives, t_dictionary* pokemons, uint32_t inventory_size) {

	t_trainer* trainer = malloc(sizeof(t_trainer));
	trainer->id = id;
	trainer->action = FREE;
	trainer->cpu_cycles = 0;
	trainer->burst = 0;
	trainer->quantum = 0;
	trainer->burst_estimate = 0;
	trainer->action_burst = 0;
	trainer->target = NULL;
	sem_init(&trainer->sem_thread, 0, 0);
	trainer->position = position;
	trainer->objectives = objectives;
	trainer->pokemons = pokemons;
	trainer->inventory_size = inventory_size;

//	debug_trainer(trainer);
	return trainer;
}

t_trainer* create_trainer_from_config(uint32_t id, char* config_position, char* config_objectives, char* config_pokemons) {

	t_position* position = create_position_from_config(config_position);
	char** objectives_string = string_split(config_objectives, "|");
	char** pokemons_string = NULL;
	if(config_pokemons != NULL) {
		pokemons_string = string_split(config_pokemons, "|");
	}
	t_dictionary* objectives = dictionary_create();
	t_dictionary* pokemons = dictionary_create();

	int i = 0;
	if(objectives_string != NULL) {
		while(objectives_string[i] != NULL) {
			add_one_to_dictionary(objectives, objectives_string[i]);
			i++;
		}
	}
	uint32_t inventory_size = i;
	i = 0;
	if(config_pokemons != NULL) {
		while(pokemons_string[i] != NULL) {
			add_one_to_dictionary(pokemons, pokemons_string[i]);
			i++;
		}
		free_string_list(pokemons_string);
		free(pokemons_string);
	}
	if(i > inventory_size){
		printf("ERROR DE CONFIG: pokemons > objetivos en trainer[%d]", id);
		exit(-1);
	}

	free_string_list(objectives_string);
	free(objectives_string);

	//tira error si intento borrar los punteros split
	return create_trainer(id, position, objectives, pokemons, inventory_size);

}

t_position* create_position_from_config(char* config_position) {
	t_position* position = malloc(sizeof(t_position));
	char ** positions_split = string_split(config_position, "|");
	position->x = atoi(positions_split[0]);
	position->y = atoi(positions_split[1]);
	free_string_list(positions_split);
	free(positions_split);


	return position;
}

t_target* create_target(char* pokemon, t_position* position, uint32_t trainer_id, bool catching) {
	t_target* target = malloc(sizeof(t_target));
	target->pokemon = create_copy_string(pokemon);
	target->position = create_position(position->x, position->y);
	target->trainer_id = trainer_id;
	target->catching = catching;
	return target;
}

void trainer_set_target(t_trainer* trainer, t_target* target) {
	destroy_target(trainer->target);
	trainer->target = target;
}

void add_pokemon(t_trainer* trainer, char* pokemon) {
	add_one_to_dictionary(trainer->pokemons, pokemon);
}

void sub_pokemon(t_trainer* trainer, char* pokemon) {
	sub_one_from_dicionary(trainer->pokemons, pokemon);
}


void destroy_trainer(t_trainer* trainer) {
	if(trainer != NULL) {
		destroy_target(trainer->target);
		free(trainer->position);

		dictionary_destroy_and_destroy_elements(trainer->objectives, free);
		dictionary_destroy_and_destroy_elements(trainer->pokemons, free);

		free(trainer);
	}
}

void destroy_target(t_target* target) {
	if(target != NULL) {
		free(target->pokemon);
		free(target->position);
		free(target);
	}
}


bool success_objective(t_objective* objective) {
	return objective->count == objective->caught;
}

uint32_t distance(t_position* current, t_position* destiny) {
	uint32_t distance_x = abs(current->x-destiny->x);
	uint32_t distance_y = abs(current->y-destiny->y);
	return distance_x + distance_y;
}

bool trainer_full(t_trainer* trainer) {

	uint32_t pokemons_in_inventory = 0;
	void count_pokemons(char* pokemon, uint32_t* count) {
		pokemons_in_inventory += *count;
	}

	dictionary_iterator(trainer->pokemons, &count_pokemons);

	return pokemons_in_inventory == trainer->inventory_size;

}

bool trainer_free_space(t_trainer* trainer) {
	return !trainer_full(trainer);
}

bool first_closer(t_trainer* trainer, t_trainer* trainer2,t_position* position){
	return  distance(trainer->position, position) <= distance(trainer2->position, position);
}

int32_t closest_free_trainer(t_list* list_trainer, t_position* destiny, char* channel)
{
	//printf("elements count %d\",list_trainer->elements_count);
	int32_t i = -1;
	int32_t index = -1;
//	printf("LA POSICION DE DESTINO ES (%d,%d)\n",destiny->x,destiny->y);
//	printf("el size de la lista de entrenadores es %d\n", list_size(list_trainer));

	if(list_size(list_trainer) != 0)
	{
		t_link_element* element = list_trainer->head;
		uint32_t closest_distance = -1;
		//uint32_t distance = dinstance(((t_trainer*)element->data)->position, destiny);
		//t_trainer* trainer = (t_trainer*) element->data;

		//element = element->next;
		i = 0;
		while(element != NULL) {
			uint32_t distance_aux = distance(((t_trainer*)element->data)->position, destiny);
			bool trainer_free_space2 = true;
			if(strcmp(channel,"job") == 0)
				trainer_free_space2 = trainer_free_space(((t_trainer*)element->data));

			if(((t_trainer*)element->data)->action == FREE && trainer_free_space2 && (distance_aux < closest_distance || closest_distance < 0)){
				closest_distance = distance_aux;
				//trainer = (t_trainer*) element->data;
				index = i;
			}
			element = element->next;
			i++;
		}
	}
//	printf("El trainer seleccionado fue: %d\n(indice en la lista actual)\n\n",index);
	return index;
}

int32_t closest_free_trainer_job(t_list* list_trainer, t_position* destiny)
{
	return closest_free_trainer(list_trainer, destiny, "job");
}

int32_t closest_free_trainer_deadlock(t_list* list_trainer, t_position* destiny)
{
	return closest_free_trainer(list_trainer, destiny, "deadlock");
}

bool trainer_success_objective(t_trainer* trainer)
{
	//EL DICCIONARIO KEY->(cant,cantInv)
	bool success = true;//true
	void single_objective_succes(char* pokemon_needed, uint32_t* amount_needed){

		if(dictionary_has_key(trainer->pokemons, pokemon_needed)) {
			uint32_t* count = dictionary_get(trainer->pokemons, pokemon_needed);
			if(*count < *amount_needed) {
				success = false;
			} else {
			}
		} else {
			success = false;
		}

	}

	dictionary_iterator(trainer->objectives, &single_objective_succes);
	return success;



}

bool trainer_needs(t_trainer* trainer, char* pokemon) {
	bool needs = true;
	if(dictionary_has_key(trainer->objectives, pokemon)) {
		if(dictionary_has_key(trainer->pokemons, pokemon)) {
			uint32_t* amount_needed = dictionary_get(trainer->objectives, pokemon);
			uint32_t* count = dictionary_get(trainer->pokemons, pokemon);
			needs = (*count < *amount_needed);//si tiene menos de los que necesita, entonces necesita
		}
	} else {
		needs = false;
	}
	return needs;
}

bool trainer_locked(t_trainer* trainer) {
	bool locked = false;
	//FREE
	if(trainer->action == FREE){
		if(trainer_full(trainer)) {
			if(!trainer_success_objective(trainer)) {
				locked = true;
			}
		}
	}
	return locked;
}

//*
t_list* trainer_held_pokemons(t_trainer* trainer) {
	t_list* held_pokemons = list_create();

	bool holds(void* pokemon, uint32_t* has) {
		bool answer = true;
		if(dictionary_has_key(trainer->objectives, pokemon)){//si no lo tiene en objetivos, no lo necesita -> lo retiene
			uint32_t* needs = dictionary_get(trainer->objectives, pokemon);
			answer = *has > *needs;//si lo tiene en objetivos, lo retiene si tiene más de los que necesita
		}
		return answer;
	}
	void list_add_if_holds(char* pokemon, uint32_t* count) {

		if(holds(pokemon, count)) {//solo agrega si esta reteniendo
		list_add(held_pokemons, create_copy_string(pokemon));//suma el pokemon una vez, si retiene mas de uno es redundante, porque cuando lo pasen a intercambiar ya no va ser candidato de nuevo

		}
	}
	dictionary_iterator(trainer->pokemons, &list_add_if_holds);

	return held_pokemons;
}

t_list* trainer_waiting_pokemons(t_trainer* trainer) {
	int i = 0;
	t_list* waiting_pokemons = list_create();

	bool waits(void* pokemon) {
		return trainer_needs(trainer, pokemon);
	}

	void list_add_if_waits(void* pokemon, uint32_t* needs) {
		if(waits(pokemon)) {
			list_add(waiting_pokemons, create_copy_string(pokemon));
		}

	}
	dictionary_iterator(trainer->objectives, &list_add_if_waits);


	return waiting_pokemons;

}



bool trainer_full_quantum(t_trainer* trainer, uint32_t quantum){

	return trainer->burst == quantum;

}

uint32_t trainer_burst_estimate(t_trainer* trainer) {
	return max(trainer->burst_estimate - trainer->burst, 0);
}

t_algorithm read_algorithm(char* config_algorithm) {
	t_algorithm algorithm = EMPTY;
	if(strcmp(config_algorithm, "FIFO") == 0) {
		algorithm = FIFO;
	}else if(strcmp(config_algorithm, "RR") == 0) {
		algorithm = RR;
	}else if(strcmp(config_algorithm, "SJF-SD") == 0) {
		algorithm = SJFS;
	}else if(strcmp(config_algorithm, "SJF-CD") == 0) {
		algorithm = SJFC;
	}else {
//		printf("no se leyo el algoritmo en el config, default: FIFO\n");
	}
	return algorithm;
}
//*/









