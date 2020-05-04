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
	printf("!!!Hello World!!! %d", NEW); /* prints !!!Hello World!!! */

	t_position pos1;// = malloc(sizeof(t_position));
	t_position pos2;// = malloc(sizeof(t_position));
	pos1.x = 1;
	pos1.y = 2;
	printf("\nx = %d\ty = %d", pos1.x, pos1.y);

	pos2 = pos1;
	printf("\nx = %d\ty = %d\n\n", pos2.x, pos2.y);

	t_new *new = construct_new_long(0, "pikachu", 1, 2, 3);
	printf("operation_code = %d\n", new->operation_code);
	printf("message_id = %d\n", new->message_id);
	printf("size_pokemon = %d\n", new->size_pokemon);
	printf("pokemon = %s\n", new->pokemon);
	printf("position = (%d, %d)\n", new->location.position.x, new->location.position.y);
	printf("amount = %d\n\n\n", new->location.amount);

	t_appeared *appeared = construct_appeared_long(0, "pikachu", 1, 2);
	printf("operation_code = %d\n", appeared->operation_code);
	printf("message_id = %d\n", appeared->message_id);
	printf("size_pokemon = %d\n", appeared->size_pokemon);
	printf("pokemon = %s\n", appeared->pokemon);
	printf("position = (%d, %d)\n\n\n", appeared->position.x, appeared->position.y);

	t_get *get = construct_get(0, "pikachu");
	printf("operation_code = %d\n", get->operation_code);
	printf("message_id = %d\n", get->message_id);
	printf("size_pokemon = %d\n", get->size_pokemon);
	printf("pokemon = %s\n\n\n", get->pokemon);



	return EXIT_SUCCESS;
}

