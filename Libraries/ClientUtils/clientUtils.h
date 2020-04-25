/*
 * clientUtils.h
 *
 *  Created on: 24 abr. 2020
 *      Author: utnso
 */

#ifndef CLIENTUTILS_H_
#define CLIENTUTILS_H_

#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>


typedef enum
{
	MENSAJE2=1
}op_code2;

typedef struct
{
	int size;
	void* stream;
} t_buffer2;

typedef struct
{
	op_code2 codigo_operacion;
	t_buffer2* buffer;
} t_paquete2;


int crear_conexion(char *ip, char* puerto);
void enviar_mensaje(char* mensaje, int socket_cliente);


#endif /* CLIENTUTILS_H_ */
