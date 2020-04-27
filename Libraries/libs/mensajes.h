/*
 * mensajes.h
 *
 *  Created on: 21 oct. 2019
 *      Author: utnso
 */

#ifndef MENSAJES_H_
#define MENSAJES_H_


#include <string.h>
#include <commons/string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <stdint.h>


typedef enum {
	NADA,
	SALUDO,
	CERRAR,
	RESERVAR,
	LIBERAR,
	OBTEBER,
	COPIAR,
	MAPEAR,
	SINCRO,
	DESMAP
} museOperacion;


int enviarInt(int destinatario, int loQueEnvio);
int recibirInt(int destinatario);

int enviarUint32_t(int destinatario, uint32_t loQueEnvio);
uint32_t recibirUint32_t(int destinatario);

int enviarSizet(int destinatario, size_t n);
size_t recibirSizet(int destinatario);

char* recibirString(int destinatario);
int enviarString(int destinatario, char* loQueEnvio);


#endif /* MENSAJES_H_ */
