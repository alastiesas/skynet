/*
 * structs.c
 *
 *  Created on: 31 may. 2020
 *      Author: utnso
 */

#include "structs.h"
#include <commons/collections/list.h>


//constructores
/*
t_new *construct_new(char* pokemon, t_location* location) {
	//DONE
	t_new* new = malloc(sizeof(t_new));
	new->operation_code = 1;//codigo de operacion de NEW
	new->message_id = 0;//El Broker se encarga de generar este dato
	new->size_pokemon = strlen(pokemon) + 1;//ya incluye el +1, se usa asi directo para el stream
	new->pokemon = malloc(new->size_pokemon);
	strcpy(new->pokemon, pokemon);
	new->location = location;
	return new;

}
t_new *construct_new_long(char* pokemon, uint32_t posx,uint32_t posy,uint32_t amount) {
	t_location* location = malloc(sizeof(t_location));
	location->position = malloc(sizeof(t_position));
	location->position->x = posx;
	location->position->y = posy;
	location->amount = amount;
	return construct_new(pokemon, location);
	//return new;
}

t_appeared* construct_appeared(char* pokemon, t_position* position) {
	t_appeared* appeared = malloc(sizeof(t_appeared));
	appeared->operation_code = 2;//codigo de operacion de APPEARED
	appeared->message_id = 0;//El Broker se encarga de generar este dato
	appeared->size_pokemon = strlen(pokemon) + 1;//ya incluye el +1, se usa asi directo para el stream
	appeared->pokemon = malloc(appeared->size_pokemon);
	strcpy(appeared->pokemon, pokemon);
	appeared->position = position;
	return appeared;
}
t_appeared* construct_appeared_long(char* pokemon, uint32_t posx, uint32_t posy) {
	t_position* position = malloc(sizeof(t_position));
	position->x = posx;
	position->y = posy;
	return construct_appeared(pokemon, position);

}


t_get* construct_get(char* pokemon) {
	t_get* get = malloc(sizeof(t_get));
	get->operation_code = 3;//codigo de operacion de GET
	get->message_id = 0;//El Broker se encarga de generar este dato
	get->size_pokemon = strlen(pokemon) + 1;//ya incluye el +1, se usa asi directo para el stream
	get->pokemon = malloc(get->size_pokemon);
	strcpy(get->pokemon, pokemon);
	return get;
}

t_localized* construct_localized(uint32_t correlative_id, char* pokemon, uint32_t position_amount, t_position* positions) {
	t_localized* localized = malloc(sizeof(t_localized));
	localized->operation_code = 4;//codigo de operacion de LOCALIZED
	localized->message_id = 0;//El Broker se encarga de generar este dato
	localized->correlative_id = correlative_id;//El que responde se encarga de generar este dato
	localized->size_pokemon = strlen(pokemon) + 1;//ya incluye el +1, se usa asi directo para el stream
	localized->pokemon = malloc(localized->size_pokemon);
	strcpy(localized->pokemon, pokemon);
	localized->position_amount = position_amount;
	localized->positions = malloc(position_amount*sizeof(t_position));
	localized->positions = positions;
	return localized;

}

t_catch* construct_catch(char* pokemon, t_position* position) {
	t_catch* catch = malloc(sizeof(t_catch));
	catch->operation_code = 5;//codigo de operacion de LOCALIZED
	catch->message_id = 0;//El Broker se encarga de generar este dato
	catch->size_pokemon = strlen(pokemon) + 1;//ya incluye el +1, se usa asi directo para el stream
	catch->pokemon = malloc(catch->size_pokemon);
	strcpy(catch->pokemon, pokemon);
	catch->position = position;
	return catch;
}
t_catch* construct_catch_long(char* pokemon, uint32_t posx, uint32_t posy) {
	t_position *position = malloc(sizeof(t_position));
		position->x = posx;
		position->y = posy;
		return construct_catch(pokemon, position);
}

t_caught* construct_caught(uint32_t correlative_id, bool result) {
	t_caught* caught = malloc(sizeof(t_caught));
	caught->operation_code = 6;//codigo de operacion de LOCALIZED
	caught->message_id = 0;//El Broker se encarga de generar este dat
	caught->correlative_id = correlative_id;//El que responde se encarga de generar este dato
	caught->result = result;
	return caught;
}
//fin constructores

//destructores
void destroy_new(t_new* message) {
	free(message->pokemon);
	free(message);
}
void destroy_appeared(t_appeared* message) {
	free(message->pokemon);
	free(message->position);
	free(message);

}

void destroy_get(t_get* message) {
	free(message->pokemon);
	free(message);
}

void destroy_localized(t_localized* message) {
	free(message->pokemon);
	free(message->positions);
	free(message);
}

void destroy_catch(t_catch* message) {
	free(message->pokemon);
	free(message->position);
	free(message);
}

void destroy_caught(t_caught* message) {
	free(message);
}
*/
//fin destructores
