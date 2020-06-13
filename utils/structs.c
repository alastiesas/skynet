#include "structs.h"
#include <commons/collections/list.h>
#include <stdlib.h>
#include <string.h>

t_position* create_position(uint32_t position_x, uint32_t position_y) {
	t_position* position = malloc(sizeof(t_position));
	position->x = position_x;
	position->y = position_y;

	return position;
}

t_location* create_location(t_position* position, uint32_t amount) {
	t_location* location = malloc(sizeof(t_location));
	location->position = position;
	location->amount = amount;
	return location;
}

t_location* create_location_long(uint32_t position_x, uint32_t position_y, uint32_t amount) {
	return create_location(create_position(position_x, position_y), amount);
}

t_message_appeared* create_message_appeared(uint32_t correlative_id, char* pokemon_name, t_position* position) {
	t_message_appeared* appeared = malloc(sizeof(t_message_appeared));
	appeared->id = 0; //El Broker se encarga de generar este dato
	appeared->correlative_id = correlative_id;
	appeared->size_pokemon_name = strlen(pokemon_name) + 1; //ya incluye el +1, se usa asi directo para el stream
	appeared->pokemon_name = malloc(appeared->size_pokemon_name);
	strcpy(appeared->pokemon_name, pokemon_name);
	appeared->position = position;
	return appeared;
}
t_message_appeared* create_message_appeared_long(uint32_t correlative_id, char* pokemon_name, uint32_t position_x, uint32_t position_y) {
	return create_message_appeared(correlative_id, pokemon_name, create_position(position_x, position_y));
}

t_message_new* create_message_new(char* pokemon_name, t_location* location) {

	t_message_new* new = malloc(sizeof(t_message_new));
	new->id = 0; //El Broker se encarga de generar este dato
	new->size_pokemon_name = strlen(pokemon_name) + 1; //ya incluye el +1, se usa asi directo para el stream
	new->pokemon_name = malloc(new->size_pokemon_name);
	strcpy(new->pokemon_name, pokemon_name);
	new->location = location;
	return new;
}

t_message_new* create_message_new_long(char* pokemon_name, uint32_t position_x, uint32_t position_y, uint32_t amount) {

	t_location* location = malloc(sizeof(t_location));
	location->position = create_position(position_x, position_y);
	location->amount = amount;
	return create_message_new(pokemon_name, location);
}

t_message_get* create_message_get(char* pokemon_name) {
	t_message_get* get = malloc(sizeof(t_message_get));
	get->id = 0; //El Broker se encarga de generar este dato
	get->size_pokemon_name = strlen(pokemon_name) + 1; //ya incluye el +1, se usa asi directo para el stream
	get->pokemon_name = malloc(get->size_pokemon_name);
	strcpy(get->pokemon_name, pokemon_name);
	return get;
}

t_message_localized* create_message_localized(uint32_t correlative_id, char* pokemon_name, uint32_t position_amount, t_position* positions) {
	t_message_localized* localized = malloc(sizeof(t_message_localized));
	localized->id = 0; //El Broker se encarga de generar este dato
	localized->correlative_id = correlative_id; //El que responde se encarga de generar este dato
	localized->size_pokemon_name = strlen(pokemon_name) + 1; //ya incluye el +1, se usa asi directo para el stream
	localized->pokemon_name = malloc(localized->size_pokemon_name);
	strcpy(localized->pokemon_name, pokemon_name);
	localized->position_amount = position_amount;
	localized->positions = malloc(position_amount * sizeof(t_position));	//TODO ya te pasan la estructura t_position, hace  falta hacer malloc de otra?
	localized->positions = positions;
	return localized;

}

t_message_catch* create_message_catch(char* pokemon_name, t_position* position) {
	t_message_catch* catch = malloc(sizeof(t_message_catch));
	catch->id = 0; //El Broker se encarga de generar este dato
	catch->size_pokemon_name = strlen(pokemon_name) + 1; //ya incluye el +1, se usa asi directo para el stream
	catch->pokemon_name = malloc(catch->size_pokemon_name);
	strcpy(catch->pokemon_name, pokemon_name);
	catch->position = position;
	return catch;
}
t_message_catch* create_message_catch_long(char* pokemon, uint32_t position_x, uint32_t position_y) {
	return create_message_catch(pokemon, create_position(position_x, position_y));
}

t_message_caught* create_message_caught(uint32_t correlative_id, bool result) {
	t_message_caught* caught = malloc(sizeof(t_message_caught));
	caught->id = 0; //El Broker se encarga de generar este dat
	caught->correlative_id = correlative_id; //El que responde se encarga de generar este dato
	caught->result = result;
	return caught;
}

void destroy_message_appeared(t_message_appeared* message_appeared) {

	free(message_appeared->pokemon_name);
	free(message_appeared->position);
	free(message_appeared);
}

void destroy_message_catch(t_message_catch* message_catch) {

	free(message_catch->pokemon_name);
	free(message_catch->position);
	free(message_catch);
}

void destroy_message_caught(t_message_caught* message_caught) {

	free(message_caught);
}

void destroy_message_get(t_message_get* message_get) {

	free(message_get->pokemon_name);
	free(message_get);
}

void destroy_message_localized(t_message_localized* message_localized) {

	free(message_localized->pokemon_name);
	free(message_localized->positions);
	free(message_localized);
}

void destroy_message_new(t_message_new* message_new) {

	free(message_new->pokemon_name);
	free(message_new->location->position);
	free(message_new->location);
	free(message_new);
}
