/*
 * gamecard.c
 *
 *  Created on: 11 abr. 2020
 *      Author: utnso
 */

#include <libs/conexion.h>


int main(int argc, char **argv) {
	char* string = "GameCard";

	int peso = pesoString(string);

	printf("el peso es %d. \n", peso);
}
