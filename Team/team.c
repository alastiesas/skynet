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


	t_position pos1;// = malloc(sizeof(t_position));
	t_position pos2;// = malloc(sizeof(t_position));
	pos1.x = 1;
	pos1.y = 2;
	printf("\nx = %d\ty = %d", pos1.x, pos1.y);

	pos2 = pos1;
	printf("\nx = %d\ty = %d\n\n", pos2.x, pos2.y);

	printf("struct 1 - NEW\n");
	t_new *new = construct_new_long(0, "pikachu", 1, 2, 3);
	printf("operation_code = %d\n", new->operation_code);
	printf("message_id = %d\n", new->message_id);
	printf("size_pokemon = %d\n", new->size_pokemon);
	printf("pokemon = %s\n", new->pokemon);
	printf("position = (%d, %d)\n", new->location.position.x, new->location.position.y);
	printf("amount = %d\n", new->location.amount);


	printf("new2 = new;\n");
	t_new *new2;//prueba de punteros struct
	new2 = new;
	printf("2operation_code = %d\n", new2->operation_code);
	printf("2message_id = %d\n", new2->message_id);
	printf("2size_pokemon = %d\n", new2->size_pokemon);
	printf("2pokemon = %s\n", new->pokemon);
	printf("2position = (%d, %d)\n", new2->location.position.x, new->location.position.y);
	printf("2amount = %d\n\n\n", new2->location.amount);


	printf("struct 2 - APPEARED\n");
	t_appeared *appeared = construct_appeared_long(0, "pikachu", 1, 2);
	printf("operation_code = %d\n", appeared->operation_code);
	printf("message_id = %d\n", appeared->message_id);
	printf("size_pokemon = %d\n", appeared->size_pokemon);
	printf("pokemon = %s\n", appeared->pokemon);
	printf("position = (%d, %d)\n\n\n", appeared->position.x, appeared->position.y);


	printf("struct 3 - GET\n");
	t_get *get = construct_get(0, "pikachu");
	printf("operation_code = %d\n", get->operation_code);
	printf("message_id = %d\n", get->message_id);
	printf("size_pokemon = %d\n", get->size_pokemon);
	printf("pokemon = %s\n\n\n", get->pokemon);

	printf("struct 4 - LOCALIZED\n");
	t_position* positions = malloc(sizeof(t_position)*2);
	positions->x = 1;
	positions->y = 2;
	(positions+1)->x = 3;
	(positions+1)->y = 4;
	printf("pos = (%d, %d)", positions->x, positions->y);
	printf("pos = (%d, %d)\n\n", (positions+1)->x, (positions+1)->y);
	//(uint32_t message_id, uint32_t correlative_id, char* pokemon, uint32_t position_amount, t_position* positions)
	t_localized *localized = construct_localized(0, 0, "pikachu", 2, positions);
	printf("operation_code = %d\n", localized->operation_code);
	printf("message_id = %d\n", localized->message_id);
	printf("size_pokemon = %d\n", localized->size_pokemon);
	printf("pokemon = %s\n", localized->pokemon);
	printf("position_amount = %d\n", localized->position_amount);
	int i = 0;
	for(i = 0;i<localized->position_amount;i++) {
		printf("posicion %d = (%d, %d)\n", (i+1), (localized->positions+i)->x, (localized->positions+i)->y);
	};


	printf("struct 5 - CATCH\n");
	t_catch *catch = construct_catch_long(0, "pikachu", 1, 2);
	printf("\n\noperation_code = %d\n", catch->operation_code);
	printf("message_id = %d\n", catch->message_id);
	printf("size_pokemon = %d\n", catch->size_pokemon);
	printf("pokemon = %s\n", catch->pokemon);
	printf("position = (%d, %d)\n", catch->position.x, catch->position.y);


	printf("struct 6 - CAUGHT\n");
	t_caught *caught = construct_caught(0, 0, 1);
	printf("\n\noperation_code = %d\n", caught->operation_code);
	printf("message_id = %d\n", caught->message_id);
	printf("correlative_id = %d\n", caught->correlative_id);
	printf("result = %d\n", caught->result);




	printf("\n\n\nSERIALIZANDO\n\n");

	uint32_t *bytes = 0;
	void* stream_new = serialize_new(new, &bytes);






	printf("tamaño del stream = %d\n", bytes);
	uint32_t offset = 0;
	//operation code


	uint32_t* opcode = malloc(sizeof(uint32_t));
	uint32_t testreturn = 0;
	testreturn = deserializeint(stream_new, opcode);
	printf("opcode = %d\n", *opcode);
	printf("return = %d\n", testreturn);
	offset += sizeof(uint32_t);

	uint32_t* message_id = malloc(sizeof(uint32_t));
	testreturn = deserializeint(stream_new + offset, message_id);
	printf("message_id = %d\n", *message_id);
	printf("return = %d\n", testreturn);
	offset += sizeof(uint32_t);


	uint32_t* size_pokemon = malloc(sizeof(uint32_t));
	testreturn = deserializeint(stream_new + offset, size_pokemon);
	printf("size_pokemon = %d\n", *size_pokemon);
	printf("return = %d\n", testreturn);
	offset += sizeof(uint32_t);

	char* pokemon;
	char* return_pokemon = deserializestring(stream_new + offset, pokemon, *size_pokemon);
	printf("after deserializestring\n");
	printf("return_pokemon = %s\n", return_pokemon);

	printf("prueba memcpy\n");
	uint32_t test_memcpy = 0;
	memcpy(&test_memcpy, stream_new, sizeof(uint32_t));
	printf("test_memcpy op code = %d\n", test_memcpy);



	t_new *new_test = construct_new_long(1, "pepe", 1, 2, 3);
	printf("pepe = %s\n", new_test->pokemon);

	deserialize_new_message_id(stream_new+sizeof(uint32_t), new_test);
	printf("test, id debe dar 0 = %d\n", new_test->message_id);

	deserialize_new_size_pokemon(stream_new + sizeof(uint32_t) + sizeof(uint32_t), new_test);
	printf("llega hasta aca1\n");
	printf("deserialize_new_size_pokemon = %d\n", new_test->size_pokemon);
	deserialize_new_pokemon(stream_new + offset, new_test);
	printf("llega hasta aca6\n");
	printf("test size = %d\n", new_test->size_pokemon);
	printf("test, debe dar pikachu = %s\n", new_test->pokemon);
	printf("llega hasta aca7\n");//*/









	/*
	//message_id
	memcpy(serialized + offset, &new->message_id, size);
	offset += size;
	//size_pokemon
	memcpy(serialized + offset, &new->size_pokemon, size);
	offset += size;
	//pokemon
	size = sizeof(char) * new->size_pokemon;
	memcpy(serialized + offset, &new->pokemon, size);
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
	offset += size;//*/






	return EXIT_SUCCESS;
}

