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




#define TIEMPO_CHECK 15
#define LOG_CONSOLE true


//Recibe un mensaje char*. (Reservando la memoria necesaria)
void* recibir_mensaje(uint32_t socket_cliente, uint32_t* size, t_log* logger);

int32_t enviar_mensaje(char* mensaje, uint32_t socket_cliente, t_log* logger);

//uint32_t receive_ACK();	//TODO recibir confirmacion de que recibio el mensaje, tiene que ser una estructura, no un int. Por ejemplo si le mando al broker, me devuelve el id del mensaje.


int32_t send_catch(t_message_catch* catch, uint32_t socket, t_log* logger);
//Recibe una estructura t_catch. (Reservando la memoria necesaria)
t_message_catch* receive_catch(uint32_t socket_cliente, uint32_t* size, t_log* logger);


int32_t send_ID(int32_t ID, uint32_t socket, t_log* logger);
int32_t receive_ID(uint32_t socket, t_log* logger);
int32_t send_ACK(uint32_t socket, t_log* logger);
int32_t send_ACK_failure(uint32_t socket, t_log* logger);
int32_t receive_ACK(uint32_t socket, t_log* logger);
uint32_t receive_size(uint32_t socket, t_log* logger);
uint32_t receive_ID_proceso(uint32_t socket, t_log* logger);

t_message_new* receive_new(uint32_t socket_cliente, uint32_t* size, t_log* logger);
t_message_new* receive_appeared(uint32_t socket_cliente, uint32_t* size, t_log* logger);

t_log* initialize_thread(char * mi_nombre, char * proceso_a_conectar, pthread_t mi_thread);
void enviar_muchos_mensajes(char* yo, char* el, uint32_t socket, t_log* logger);
operation_code receive_cod_op(uint32_t socket, t_log* logger);










#endif /* MENSAJES_H_ */
