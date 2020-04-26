/*
 * broker.c
 *
 *  Created on: 11 abr. 2020
 *      Author: utnso
 */

#include <stdio.h>
#include <libs/conexion.h>


int main(int argc, char **argv) {
	char* string = "Broker";

	int peso = pesoString(string);

	printf("el peso es %d. \n", peso);
}
