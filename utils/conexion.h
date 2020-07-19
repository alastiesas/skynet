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
#include "mensajes.h"

#define TIEMPO_REINTENTO 10

pthread_t thread;

struct thread_args {
    int32_t socket;
    t_log* logger;
    void (*function)(void*);//agrego una funcion que debera ser definida por cada proceso.

};

typedef struct {
	operation_code op_code;
	void* message;

}serve_thread_args;


void iniciar_servidor(char* puerto, t_log* logger);

void esperar_cliente(int32_t socket_servidor, t_log* logger);

int32_t listen_messages(void* input);
void* process_request(operation_code cod_op, int32_t socket, t_log* logger);

int32_t send_with_retry(int32_t socket, void* a_enviar, size_t bytes, int32_t flag);
int32_t recv_with_retry(int32_t socket, void* a_recibir, size_t bytes, int32_t flag);


int32_t connect_to_server(char * ip, char * puerto, uint32_t retry_time, uint32_t retry_amount, t_log* logger);


#endif /* CONEXION_H_ */
