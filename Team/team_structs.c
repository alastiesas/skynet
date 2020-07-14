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
	if(trainer->target != NULL)
		printf("trainer->target: [pokemon: %s, posicion: (%d, %d), catching: %d, trainer_id: %d]\n", trainer->target->pokemon, trainer->target->position->x, trainer->target->position->y, trainer->target->catching, trainer->target->trainer_id);
	else
		printf("sin target\n");

	printf("trainer->posicion: (%d, %d)\n", trainer->position->x, trainer->position->y);
	uint32_t i = 0;
	printf("trainer->objetivos: [");
	while(trainer->objectives[i] != NULL) {
		printf(" \"%s\" ", trainer->objectives[i]);
		i++;
	}
	printf("]\n");

	i = 0;
	printf("trainer->pokemones: [");
	while(trainer->pokemons[i] != NULL) {
		printf(" \"%s\" ", trainer->pokemons[i]);
		i++;
	}
	printf("]\n");
	printf("**FIN DEBUG**\n\n");
}

t_trainer* create_trainer(uint32_t id, t_position* position, char** objectives, char** pokemons) {

	t_trainer* trainer = malloc(sizeof(t_trainer));
	trainer->id = id;
	trainer->action = FREE;
	trainer->target = NULL;
	trainer->burst = 0;
	trainer->quantum = 0;
	trainer->burst_estimate = 0;
	trainer->action_burst = 0;
	sem_init(&trainer->sem_thread, 0, 0);
	trainer->position = position;
	trainer->objectives = objectives;
	uint32_t inventory_size = string_list_size(objectives);
	trainer->pokemons = calloc(inventory_size+1, sizeof(char*));
	uint32_t i = 0;
	while(pokemons[i] != NULL) {
		trainer->pokemons[i] = pokemons[i];
		i++;
	}//PRUEBA CALLOC A VER SI ANDA
	//trainer->pokemons = pokemons;
	debug_trainer(trainer);
	return trainer;
}

t_trainer* create_trainer_from_config(uint32_t id, char* config_position, char* config_objectives, char* config_pokemons) {

	t_position* position = create_position_from_config(config_position);
	char** objectives = string_split(config_objectives, "|");
	char** pokemons = string_split(config_pokemons, "|");
	return create_trainer(id, position, objectives, pokemons);

}

t_position* create_position_from_config(char* config_position) {
	t_position* position = malloc(sizeof(t_position));
	char ** positions_split = string_split(config_position, "|");
	position->x = atoi(positions_split[0]);
	position->y = atoi(positions_split[1]);


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

void destroy_target(t_target* target) {
	if(target != NULL) {
		free(target->pokemon);
		free(target->position);
		//free(target);
	}
}

void trainer_set_target(t_trainer* trainer, t_target* target) {
	destroy_target(trainer->target);
	trainer->target = target;
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
	bool response = false;
	if(string_list_size(trainer->pokemons) >=  string_list_size(trainer->objectives))
		response = true;
	return response;
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
//			printf("LA POSICION DEL ENTRANDOR ES (%d,%d)\n",((t_trainer*)element->data)->position->x,((t_trainer*)element->data)->position->y);
//			printf("distance actual de este entrenador: %d\n" ,distance_aux);
//			printf("distance minima: %d\n" ,closest_distance);
//			printf("el actions es (0=libre): %d\n", ((t_trainer*)element->data)->action);
//			printf("teiene espacio en inventario: %d\n" ,trainer_free_space(((t_trainer*)element->data)));
			//debug_trainer(((t_trainer*)element->data));
			bool trainer_free_space2 = 1;
			if(strcmp(channel,"job") == 0)
				trainer_free_space2 = trainer_free_space(((t_trainer*)element->data));

			if(((t_trainer*)element->data)->action == FREE && trainer_free_space2 && (distance_aux < closest_distance || closest_distance < 0)){
				closest_distance = distance_aux;
				//trainer = (t_trainer*) element->data;
				index = i;
//				printf("-> SELECCIONADO %d\n",((t_trainer*)element->data)->id);
			}
			else
//				printf("-> NO SELECCIONADO %d\n",((t_trainer*)element->data)->id);
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

void add_pokemon(t_trainer* trainer, char*pokemon)
{
	uint32_t i = 0;
	while(trainer->pokemons[i] != NULL)
	{
		i++;
	}
	//printf("ROMPE EN REALLOC\n");
	//array, array_size * sizeof(*array)
	// EL POBLEMA ESTA EN ESE REALLOC
	//trainer->pokemons = (char**)realloc(trainer->pokemons,new_size*sizeof(char*));//PRUEBA CALLOC
	//trainer->pokemons[i] = malloc(strlen(pokemon)+1);
	//memcpy(trainer->pokemons[i], pokemon, strlen(pokemon)+1);
	trainer->pokemons[i] = create_copy_string(pokemon);
}

bool trainer_success_objective(t_trainer* trainer)
{
	//EL DICCIONARIO KEY->(cant,cantInv)
	bool success = 1;//true
	t_dictionary* dictionary = dictionary_create();
	typedef struct
	{
		uint32_t count;
		uint32_t caught;
	} t_objective_aux;

  uint32_t i = 0;
	while(trainer->objectives[i] != NULL){
		if(dictionary_has_key(dictionary,trainer->objectives[i])){
			t_objective_aux* objective_aux = (t_objective_aux*) dictionary_get(dictionary,trainer->objectives[i]);
			objective_aux->count++;
		}
		else{
			t_objective_aux* objective_aux = malloc(sizeof(t_objective_aux));
			objective_aux->count = 1;
			objective_aux->caught = 0;
			dictionary_put(dictionary, trainer->objectives[i], objective_aux);
		}
		i++;
	}
	i=0;

	while(trainer->pokemons[i] != NULL){
		if(dictionary_has_key(dictionary,trainer->pokemons[i])){
			t_objective_aux* objective_aux = (t_objective_aux*) dictionary_get(dictionary,trainer->pokemons[i]);
			objective_aux->caught++;
		}
		i++;
	}
	i=0;
	while(trainer->objectives[i] != NULL){
		t_objective_aux* objective_aux = (t_objective_aux*) dictionary_get(dictionary,trainer->objectives[i]);
		if(objective_aux->caught != objective_aux->count)
			success = 0;//false
		i++;
	}
	//dictionary_destroy_and_destroy_elements(dictionary, free);
	//ACA HAY QUE LIMPIAR EL DICCIONARIO TODO
	return success;
}


bool trainer_needs(t_trainer* trainer, char* pokemon) {
	bool needs = false;
	int32_t count_objective= 0;
	int32_t count_pokemon= 0;
	int32_t i= 0;

	while(trainer->objectives[i] != NULL) {
		if(strcmp(trainer->objectives[i], pokemon) == 0) {
			count_objective++;
		}
			i++;
	}
	i = 0;
	while(trainer->pokemons[i] != NULL) {
		if(strcmp(trainer->pokemons[i], pokemon) == 0) {
			count_pokemon++;
		}
			i++;
	}
	needs = count_objective > count_pokemon;
	return needs;
}

bool trainer_locked(t_trainer* trainer) {
	bool locked = false;
	//FREE
	if(trainer->action == FREE){
		if(trainer_full(trainer)) {
			if(!trainer_success_objective(trainer)) {
				locked = true;
				printf("trainer[%d] está bloqueado\n", trainer->id);
			}else {
				printf("trainer[%d] no está bloqueado porque ya cumplio sus objetivos\n", trainer->id);
			}
		}else {
			printf("trainer[%d] no está bloqueado porque no tiene el inventario lleno\n", trainer->id);
		}
	}else {
		printf("trainer[%d] no está bloqueado porque no está en FREE\n", trainer->id);
	}
	return locked;
}

//*
t_list* trainer_held_pokemons(t_trainer* trainer) {
	int i = 0;
	bool condition(void* pokemon) {
		return (strcmp(trainer->objectives[i], pokemon) == 0);
	}

	t_list* held_pokemons = list_create();

	while(trainer->pokemons[i] != NULL) {

		list_add(held_pokemons, create_copy_string(trainer->pokemons[i]));
		i++;
	}

	i = 0;

	while(trainer->objectives[i] != NULL) {
		list_remove_by_condition(held_pokemons, &condition);
		i++;
	}//resta los objetivos de la lista

	return held_pokemons;
}

t_list* trainer_waiting_pokemons(t_trainer* trainer) {
	int i = 0;
	bool condition(void* pokemon) {
		return (strcmp(trainer->pokemons[i], pokemon) == 0);
	}

	t_list* waiting_pokemons = list_create();

	while(trainer->objectives[i] != NULL) {

		list_add(waiting_pokemons, create_copy_string(trainer->objectives[i]));
		i++;
	}

	i = 0;

	while(trainer->pokemons[i] != NULL) {
		char* removed = list_remove_by_condition(waiting_pokemons, &condition);
		i++;
	}

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
		printf("no se leyo el algoritmo en el config, default: FIFO\n");
	}
	return algorithm;
}
//*/









