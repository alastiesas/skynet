/*
 ============================================================================
 Name        : Team.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include "serializer.h"

int main(void) {
	printf("\nDEBUG ESTRUCTURAS\n");

	printf("struct 1 - NEW\n");
	t_new *new = construct_new_long("pikachu", 1, 2, 3);
	printf("operation_code = %d\n", new->operation_code);
	printf("message_id = %d\n", new->message_id);
	printf("size_pokemon = %d\n", new->size_pokemon);
	printf("pokemon = %s\n", new->pokemon);
	printf("position = (%d, %d)\n", new->location->position->x, new->location->position->y);
	printf("amount = %d\n", new->location->amount);


	printf("\nstruct 2 - APPEARED\n");
	t_appeared *appeared = construct_appeared_long("pikachu", 1, 2);
	printf("operation_code = %d\n", appeared->operation_code);
	printf("message_id = %d\n", appeared->message_id);
	printf("size_pokemon = %d\n", appeared->size_pokemon);
	printf("pokemon = %s\n", appeared->pokemon);
	printf("position = (%d, %d)\n\n\n", appeared->position->x, appeared->position->y);


	printf("struct 3 - GET\n");
	t_get *get = construct_get("pikachu");
	printf("operation_code = %d\n", get->operation_code);
	printf("message_id = %d\n", get->message_id);
	printf("size_pokemon = %d\n", get->size_pokemon);
	printf("pokemon = %s\n", get->pokemon);

	printf("\nstruct 4 - LOCALIZED\n");
	t_position* positions = malloc(sizeof(t_position)*2);
	positions->x = 1;
	positions->y = 2;
	(positions+1)->x = 3;
	(positions+1)->y = 4;
	t_localized *localized = construct_localized(0, "pikachu", 2, positions);
	printf("operation_code = %d\n", localized->operation_code);
	printf("message_id = %d\n", localized->message_id);
	printf("size_pokemon = %d\n", localized->size_pokemon);
	printf("pokemon = %s\n", localized->pokemon);
	printf("position_amount = %d\n", localized->position_amount);
	int i = 0;
	for(i = 0;i<localized->position_amount;i++) {
		printf("posicion %d = (%d, %d)\n", (i+1), (localized->positions+i)->x, (localized->positions+i)->y);
	};


	printf("\nstruct 5 - CATCH\n");
	t_catch *catch = construct_catch_long("pikachu", 1, 2);
	printf("operation_code = %d\n", catch->operation_code);
	printf("message_id = %d\n", catch->message_id);
	printf("size_pokemon = %d\n", catch->size_pokemon);
	printf("pokemon = %s\n", catch->pokemon);
	printf("position = (%d, %d)\n", catch->position->x, catch->position->y);


	printf("\nstruct 6 - CAUGHT\n");
	t_caught *caught = construct_caught(0, true);
	printf("operation_code = %d\n", caught->operation_code);
	printf("message_id = %d\n", caught->message_id);
	printf("correlative_id = %d\n", caught->correlative_id);
	printf("result = %d\n", caught->result);




	printf("\n\n\nSERIALIZANDO\n");

	uint32_t *bytes = malloc(sizeof(uint32_t));
	*bytes = 0;
	printf("*bytes inicializado en: %d\n", *bytes);

	printf("\nserialize_new\n");
	void* stream_new = serialize_new(new, bytes);
	printf("bytes = %d\n", *bytes);
	uint32_t offset = 0;
	printf("operation_code = %d\n", *(uint32_t*)(stream_new+offset));
	offset += sizeof(uint32_t);
	printf("message_id = %d\n", *(uint32_t*)(stream_new+offset));
	offset += sizeof(uint32_t);
	printf("size_pokemon = %d\n", *(uint32_t*)(stream_new+offset));
	offset += sizeof(uint32_t);
	printf("pokemon = %s\n", (char*)(stream_new+offset));
	offset += sizeof(char)*8;
	printf("posx = %d\n", *(uint32_t*)(stream_new+offset));
	offset += sizeof(uint32_t);
	printf("posy = %d\n", *(uint32_t*)(stream_new+offset));
	offset += sizeof(uint32_t);
	printf("amount = %d\n", *(uint32_t*)(stream_new+offset));
	offset += sizeof(uint32_t);

	printf("\nserialize_appeared\n");
	void* stream_appeared = serialize_appeared(appeared, bytes);
	printf("bytes = %d\n", *bytes);
	offset = 0;
	printf("operation_code = %d\n", *(uint32_t*)(stream_appeared+offset));
	offset += sizeof(uint32_t);
	printf("message_id = %d\n", *(uint32_t*)(stream_appeared+offset));
	offset += sizeof(uint32_t);
	printf("size_pokemon = %d\n", *(uint32_t*)(stream_appeared+offset));
	offset += sizeof(uint32_t);
	printf("pokemon = %s\n", (char*)(stream_appeared+offset));
	offset += sizeof(char)*8;
	printf("posx = %d\n", *(uint32_t*)(stream_appeared+offset));
	offset += sizeof(uint32_t);
	printf("posy = %d\n", *(uint32_t*)(stream_appeared+offset));
	offset += sizeof(uint32_t);

	printf("\nserialize_get\n");
	void* stream_get = serialize_get(get, bytes);
	printf("bytes = %d\n", *bytes);
	offset = 0;
	printf("operation_code = %d\n", *(uint32_t*)(stream_get+offset));
	offset += sizeof(uint32_t);
	printf("message_id = %d\n", *(uint32_t*)(stream_get+offset));
	offset += sizeof(uint32_t);
	printf("size_pokemon = %d\n", *(uint32_t*)(stream_get+offset));
	offset += sizeof(uint32_t);
	printf("pokemon = %s\n", (char*)(stream_get+offset));
	offset += sizeof(char)*8;

	printf("\nserialize_localized\n");
	void* stream_localized = serialize_localized(localized, bytes);
	printf("bytes = %d\n", *bytes);
	offset = 0;
	printf("operation_code = %d\n", *(uint32_t*)(stream_localized+offset));
	offset += sizeof(uint32_t);
	printf("message_id = %d\n", *(uint32_t*)(stream_localized+offset));
	offset += sizeof(uint32_t);
	printf("correlative_id = %d\n", *(uint32_t*)(stream_localized+offset));
	offset += sizeof(uint32_t);
	printf("size_pokemon = %d\n", *(uint32_t*)(stream_localized+offset));
	offset += sizeof(uint32_t);
	printf("pokemon = %s\n", (char*)(stream_localized+offset));
	offset += sizeof(char)*8;
	printf("position_amount = %d\n", *(uint32_t*)(stream_localized+offset));
	offset += sizeof(uint32_t);
	for(int i = 0; i < 2; i++){
		printf("position = (%d, ", *(uint32_t*)(stream_localized+offset));
		offset += sizeof(uint32_t);
		printf("%d)\n", *(uint32_t*)(stream_localized+offset));
		offset += sizeof(uint32_t);
	}

	printf("\nserialize_catch\n");
	void* stream_catch = serialize_catch(catch, bytes);
	printf("bytes = %d\n", *bytes);
	offset = 0;
	printf("operation_code = %d\n", *(uint32_t*)(stream_catch+offset));
	offset += sizeof(uint32_t);
	printf("message_id = %d\n", *(uint32_t*)(stream_catch+offset));
	offset += sizeof(uint32_t);
	printf("size_pokemon = %d\n", *(uint32_t*)(stream_catch+offset));
	offset += sizeof(uint32_t);
	printf("pokemon = %s\n", (char*)(stream_catch+offset));
	offset += sizeof(char)*8;
	printf("posx = %d\n", *(uint32_t*)(stream_catch+offset));
	offset += sizeof(uint32_t);
	printf("posy = %d\n", *(uint32_t*)(stream_catch+offset));
	offset += sizeof(uint32_t);

	printf("\nserialize_localized\n");
	void* stream_caught = serialize_caught(caught, bytes);
	printf("bytes = %d\n", *bytes);
	offset = 0;
	printf("operation_code = %d\n", *(uint32_t*)(stream_caught+offset));
	offset += sizeof(uint32_t);
	printf("message_id = %d\n", *(uint32_t*)(stream_caught+offset));
	offset += sizeof(uint32_t);
	printf("correlative_id = %d\n", *(uint32_t*)(stream_caught+offset));
	offset += sizeof(uint32_t);
	printf("resut = %d\n", *(uint32_t*)(stream_caught+offset));
	offset += sizeof(uint32_t);

	return EXIT_SUCCESS;
}

