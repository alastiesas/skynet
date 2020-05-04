/*
 * Serializer.c
 *
 *  Created on: 24 abr. 2020
 *      Author: utnso
 */


#include "serializer.h"
#include <commons/collections/list.h>

//retorna un mensaje NEW.
t_new *construct_new(uint32_t message_id, char* pokemon, t_location location) {
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
t_new *construct_new_long(uint32_t message_id, char* pokemon, uint32_t posx,uint32_t posy,uint32_t amount) {
	t_location location;
	location.position.x = posx;
	location.position.y = posy;
	location.amount = amount;
	return construct_new(message_id, pokemon, location);
	//return new;
}


//retorna un mensaje APPEARED.
t_appeared* construct_appeared(uint32_t message_id, char* pokemon, t_position position) {
	t_appeared* appeared = malloc(sizeof(t_appeared));
	appeared->operation_code = 2;//codigo de operacion de APPEARED
	appeared->message_id = 0;//El Broker se encarga de generar este dato
	appeared->size_pokemon = strlen(pokemon) + 1;//ya incluye el +1, se usa asi directo para el stream
	appeared->pokemon = malloc(appeared->size_pokemon);
	strcpy(appeared->pokemon, pokemon);
	appeared->position = position;
	return appeared;
}
t_appeared* construct_appeared_long(uint32_t message_id, char* pokemon, uint32_t posx, uint32_t posy) {
	t_position position;
	position.x = posx;
	position.y = posy;
	return construct_appeared(message_id, pokemon, position);

}

//retorna un mensaje GET.
t_get* construct_get(uint32_t message_id, char* pokemon) {
	t_get* get = malloc(sizeof(t_get));
	get->operation_code = 3;//codigo de operacion de GET
	get->message_id = 0;//El Broker se encarga de generar este dato
	get->size_pokemon = strlen(pokemon) + 1;//ya incluye el +1, se usa asi directo para el stream
	get->pokemon = malloc(get->size_pokemon);
	strcpy(get->pokemon, pokemon);
	return get;
}
//retorna un mensaje LOCALIZED.
t_localized* construct_localized(uint32_t message_id, uint32_t correlative_id, char* pokemon, uint32_t position_amount, t_position* positions) {
	t_localized localized = malloc(sizeof(t_localized));
	localized->operation_code = 4;//codigo de operacion de LOCALIZED
	localized->message_id = 0;//El Broker se encarga de generar este dato
	localized->size_pokemon = strlen(pokemon) + 1;//ya incluye el +1, se usa asi directo para el stream
	localized->pokemon = malloc(localized->size_pokemon);

}
/*
//retorna un mensaje CATCH.
t_catch* construct_catch(uint32_t message_id, char* pokemon, t_position position) {
	//TODO
}
//retorna un mensaje CAUGHT.
t_caught* construct_caught(uint32_t message_id, uint32_t correlative_id, uint32_t result) {
	//TODO
}
//*/
