/*
 * serverUtils.h
 *
 *  Created on: 24 abr. 2020
 *      Author: utnso
 */

#ifndef SERVERUTILS_H_
#define SERVERUTILS_H_

#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<unistd.h>
#include<netdb.h>
#include<commons/log.h>
#include<commons/collections/list.h>
#include<string.h>
#include<pthread.h>

pthread_t thread;

#define IP "127.0.0.1"
#define PUERTO "4444"

typedef enum
{
	MENSAJE=1
}op_code;

typedef struct
{
	int size;
	void* stream;
} t_buffer;

typedef struct
{
	op_code codigo_operacion;
	t_buffer* buffer;
} t_paquete;



void iniciar_servidor(char * puerto);
void esperar_cliente(int socket_servidor);
void serve_client(int* socket);
void process_request(int cod_op, int cliente_fd);
void* recibir_mensaje(int socket_cliente, int* size);
void* serializar_paquete(t_paquete* paquete, int bytes);
void devolver_mensaje(void* payload, int size, int socket_cliente);





#endif /* SERVERUTILS_H_ */
