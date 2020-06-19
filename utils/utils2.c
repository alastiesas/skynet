/*
 * utils2.c
 *
 *  Created on: 16 jun. 2020
 *      Author: utnso
 */

#include "utils.h"


void* string_to_void(char* string, uint32_t* size){

	*size = strlen(string);		//afuera de la funcion voy a necesitar saber el tamano del buffer
	void* buffer = malloc(*size);
	memcpy(buffer, string, *size);
	//falta hacer fuera de la funcion el free del string
	return buffer;
}

char* void_to_string(void* buffer, uint32_t size){
						//necesito el size del buffer para crear el string
	char* string = malloc(size + 1);
	memcpy(string, buffer, size);
	char caracter = '\0';
	memcpy(string + size, &caracter, 1);
	//falta hacer fuera de la funcion el free del buffer
	return string;
}

void* generate_void(uint32_t size){

	void* data = malloc(size);
	FILE* random;
	random = fopen("/dev/urandom", "r");
	fread(data, 1, size, random);
	fclose(random);

	return data;
}

