/*
 * Serializer.c
 *
 *  Created on: 24 abr. 2020
 *      Author: utnso
 */


#include "serializer.h"
#include <commons/collections/list.h>


//constructores
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


t_get* construct_get(uint32_t message_id, char* pokemon) {
	t_get* get = malloc(sizeof(t_get));
	get->operation_code = 3;//codigo de operacion de GET
	get->message_id = 0;//El Broker se encarga de generar este dato
	get->size_pokemon = strlen(pokemon) + 1;//ya incluye el +1, se usa asi directo para el stream
	get->pokemon = malloc(get->size_pokemon);
	strcpy(get->pokemon, pokemon);
	return get;
}

t_localized* construct_localized(uint32_t message_id, uint32_t correlative_id, char* pokemon, uint32_t position_amount, t_position* positions) {
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

t_catch* construct_catch(uint32_t message_id, char* pokemon, t_position position) {
	t_catch* catch = malloc(sizeof(t_catch));
	catch->operation_code = 5;//codigo de operacion de LOCALIZED
	catch->message_id = 0;//El Broker se encarga de generar este dato
	catch->size_pokemon = strlen(pokemon) + 1;//ya incluye el +1, se usa asi directo para el stream
	catch->pokemon = malloc(catch->size_pokemon);
	strcpy(catch->pokemon, pokemon);
	catch->position = position;
	return catch;
}
t_catch* construct_catch_long(uint32_t message_id, char* pokemon, uint32_t posx, uint32_t posy) {
	t_position position;
		position.x = posx;
		position.y = posy;
		return construct_catch(message_id, pokemon, position);
}

t_caught* construct_caught(uint32_t message_id, uint32_t correlative_id, uint32_t result) {
	t_caught* caught = malloc(sizeof(t_caught));
	caught->operation_code = 6;//codigo de operacion de LOCALIZED
	caught->message_id = 0;//El Broker se encarga de generar este dat
	caught->correlative_id = correlative_id;//El que responde se encarga de generar este dato
	caught->result = result;
	return caught;
}
//fin constructores


//serializadores
void* serialize_new(t_new* new, uint32_t* bytes) {
	*bytes = sizeof(uint32_t)*6 + sizeof(char) * (new->size_pokemon);
	void* serialized = malloc(*bytes);
	uint32_t offset = 0;
	uint32_t size = 0;
	//operation code
	size = sizeof(uint32_t);
	memcpy(serialized + offset, &new->operation_code, size);
	offset += size;
	//message_id
	memcpy(serialized + offset, &new->message_id, size);
	offset += size;
	//size_pokemon
	memcpy(serialized + offset, &new->size_pokemon, size);
	offset += size;
	//pokemon
	size = sizeof(char) * new->size_pokemon;
	memcpy(serialized + offset, new->pokemon, size);
	/*printf("--------->pokemon = %s\n", new->pokemon);//DEBUG*/
	offset += size;
	//position x
	size = sizeof(uint32_t);
	memcpy(serialized + offset, &new->location.position.x, size);
	offset += size;
	//position y
	memcpy(serialized + offset, &new->location.position.y, size);
	offset += size;
	//cantidad
	memcpy(serialized + offset, &new->location.amount, size);
	offset += size;

	return serialized;
}

/*
void* serialize_appeared(t_appeared* message, uint32_t *bytes);

void* serialize_get(t_get* message, uint32_t *bytes);

void* serialize_localized(t_localized* message, uint32_t *bytes);

void* serialize_catch(t_catch* message, uint32_t *bytes);

void* serialize_caught(t_caught* message, uint32_t *bytes);//*/
//fin serializadores





//deserializadores ESTOS SE VAN A BORRAR, ESTAN HACIENDO EL RCV DIRECTO DONDE LO QUIEREN
uint32_t deserializeint(void* from, uint32_t* to) {
	memcpy(to, from, sizeof(uint32_t));
	return *to;

}
char* deserializestring(void* from, char* to, uint32_t size) {
	to = malloc(sizeof(char)*size);
	memcpy(to, from, size*sizeof(char));
	//printf("deserializestring string = %s\n", to);
	return to;
}
void deserialize_new_message_id(void* from, t_new* to) {
	memcpy(&to->message_id, from, sizeof(uint32_t));
}
void deserialize_new_size_pokemon(void* from, t_new* to) {
	memcpy(&to->size_pokemon, from, sizeof(uint32_t));
	printf("deserialize_new_size_pokemon = %d\n", to->size_pokemon);
}
void deserialize_new_pokemon(void* from, t_new* to) {
	uint32_t size = to->size_pokemon * sizeof(char);
	realloc(to->pokemon, size);
	memcpy(to->pokemon, from, size);
}


/*
t_new* deserialize_new(void* stream, uint32_t* bytes){

}

t_appeared* deserialize_appeared(void* stream, uint32_t* bytes);

t_get* deserialize_get(void* stream, uint32_t* bytes);

t_localized* deserialize_localized(void* stream, uint32_t* bytes);

t_catch* deserialize_catch(void* stream, uint32_t* bytes);

t_caught* deserialize_caught(void* stream, uint32_t* bytes);
//findeserializadores*/

