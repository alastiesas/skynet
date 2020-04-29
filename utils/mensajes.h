/*
 * serverUtils.h
 *
 *  Created on: 24 abr. 2020
 *      Author: utnso
 */

#ifndef MENSAJES_H_
#define MENSAJES_H_

#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<unistd.h>
#include<netdb.h>
#include<commons/log.h>
#include<commons/collections/list.h>
#include<commons/string.h>
#include<string.h>
#include<pthread.h>
#include "serialize.h"
#include "structs.h"



#define IP "127.0.0.1"
#define PUERTO "4444"

#define TIEMPO_CHECK 15
#define LOG_CONSOLE true

void* recibir_mensaje(uint32_t socket_cliente, uint32_t* size, t_log* logger);



void enviar_mensaje(char* mensaje, uint32_t socket_cliente, t_log* logger);

//uint32_t receive_ACK();	//TODO recibir confirmacion de que recibio el mensaje, tiene que ser una estructura, no un int. Por ejemplo si le mando al broker, me devuelve el id del mensaje.

t_log* initialize_thread(char * mi_nombre, char * proceso_a_conectar, pthread_t mi_thread);
void enviar_muchos_mensajes(char* yo, char* el, uint32_t socket, t_log* logger);
op_code receive_cod_op(uint32_t socket, t_log* logger);










#endif /* MENSAJES_H_ */
