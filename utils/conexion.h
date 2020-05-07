/*
 * clientUtils.h
 *
 *  Created on: 24 abr. 2020
 *      Author: utnso
 */

#ifndef CONEXION_H_
#define CONEXION_H_

#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include<pthread.h>
#include<commons/log.h>
#include<commons/string.h>
#include "serialize.h"
#include "structs.h"

#define TIEMPO_REINTENTO 10

pthread_t thread;

struct thread_args {
    int32_t socket;
    t_log* logger;
    t_colas* colas;
    t_suscriptores* suscriptores;
    t_semaforos* semaforos;
};

void iniciar_servidor(char* puerto, t_log* logger);

void esperar_cliente(int32_t socket_servidor, t_log* logger);

void recibir_muchos_mensajes(void* input);
void process_request(op_code cod_op, int32_t socket, t_log* logger);


void process_NEW(int32_t socket_cliente, t_log* logger, t_queue* queue_NEW, t_semaforos* semaforos);


int32_t connect_to_server(char * ip, char * puerto, t_log* logger);


#endif /* CONEXION_H_ */
