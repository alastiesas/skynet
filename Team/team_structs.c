/*
 * team_structs.c
 *
 *  Created on: 9 jun. 2020
 *      Author: utnso
 */

#include "team_structs.h"


void debug_trainer(t_trainer* trainer) {
	printf("\n**DEBUG DEL ENTRENADOR**\n");
	printf("id: %d\n", trainer->id);
	printf("action: %d\n", trainer->action);
	if(trainer->target->position != NULL)
		printf("target: [pokemon: %s, posicion: (%d, %d), catching: %d]\n", trainer->target->pokemon, trainer->target->position->x, trainer->target->position->y, trainer->target->catching);
	else
		printf("sin target\n");

	printf("posicion: (%d, %d)\n", trainer->position->x, trainer->position->y);
	uint32_t i = 0;
	printf("objetivos: [");
	while(trainer->objectives[i] != NULL) {
		printf(" \"%s\" ", trainer->objectives[i]);
		i++;
	}
	printf("]\n");

	i = 0;
	printf("pokemones: [");
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
	if(string_list_size(trainer->pokemons) >=  string_list_size(trainer->objectives))
		response = true;
	return response;
}

bool trainer_free_space(t_trainer* trainer) {
	return !trainer_full(trainer);
}

bool first_closer(t_trainer* trainer, t_trainer* trainer2,t_position* position){
	return  dinstance(trainer->position, position) <= dinstance(trainer2->position, position);
}

int32_t closest_free_trainer(t_list* list_trainer, t_position* destiny)
{
	//printf("elements count %d\",list_trainer->elements_count);
	int32_t i = -1;
	int32_t index = -1;
	printf("LA POSICION DE DESTINO ES (%d,%d)\n",destiny->x,destiny->y);
	printf("el size de la lista de entrenadores es %d\n", list_size(list_trainer));

	if(list_size(list_trainer) != 0)
	{

		t_link_element* element = list_trainer->head;
		uint32_t distance = -1;
		//uint32_t distance = dinstance(((t_trainer*)element->data)->position, destiny);
		//t_trainer* trainer = (t_trainer*) element->data;

		//element = element->next;
		i = 0;
		while(element != NULL) {
			uint32_t distance_aux = dinstance(((t_trainer*)element->data)->position, destiny);
			printf("LA POSICION DEL ENTRANDOR ES (%d,%d)\n",((t_trainer*)element->data)->position->x,((t_trainer*)element->data)->position->y);
			printf("distance actual de este entrenador: %d\n" ,distance_aux);
			printf("distance minima: %d\n" ,distance);
			printf("el actions es (0=libre): %d\n", ((t_trainer*)element->data)->action);
			printf("teiene espacio en inventario: %d\n" ,trainer_free_space(((t_trainer*)element->data)));

			if(((t_trainer*)element->data)->action == FREE && trainer_free_space(((t_trainer*)element->data)) && (distance_aux < distance || distance < 0)){
				distance = distance_aux;
				//trainer = (t_trainer*) element->data;
				index = i;
				printf("-> SELECCIONADO %d\n",((t_trainer*)element->data)->id);
			}
			else
				printf("-> NO SELECCIONADO %d\n",((t_trainer*)element->data)->id);
			element = element->next;
			i++;
		}
	}
	printf("El trainer seleccionado fue: %d\n(indice en la lista actual)\n\n",index);
	return index;
}

void add_pokemon(t_trainer* trainer, char*pokemon)
{
	realloc(trainer->pokemons,sizeof(trainer->pokemons)+1);
	uint32_t i = 0;
	while(trainer->pokemons[i] != NULL)
	{
		i++;
	}
	strcpy(&trainer->pokemons[i],&pokemon);
	trainer->pokemons[i+1] = NULL;
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
