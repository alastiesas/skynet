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


//Recibe un mensaje char*. (Reservando la memoria necesaria)
void* recibir_mensaje(uint32_t socket_cliente, uint32_t* size, t_log* logger);

int32_t enviar_mensaje(char* mensaje, uint32_t socket_cliente, t_log* logger);

//uint32_t receive_ACK();	//TODO recibir confirmacion de que recibio el mensaje, tiene que ser una estructura, no un int. Por ejemplo si le mando al broker, me devuelve el id del mensaje.


int32_t send_catch(t_catch* catch, uint32_t socket, t_log* logger);
//Recibe una estructura t_catch. (Reservando la memoria necesaria)
t_catch* receive_catch(uint32_t socket_cliente, uint32_t* size, t_log* logger);


t_log* initialize_thread(char * mi_nombre, char * proceso_a_conectar, pthread_t mi_thread);
void enviar_muchos_mensajes(char* yo, char* el, uint32_t socket, t_log* logger);
op_code receive_cod_op(uint32_t socket, t_log* logger);






#endif /* MENSAJES_H_ */
