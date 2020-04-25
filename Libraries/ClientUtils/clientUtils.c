/*
 * clientUtils.c
 *
 *  Created on: 24 abr. 2020
 *      Author: utnso
 */
#include "clientUtils.h"

#define TIEMPO_REINTENTO 10


int crear_conexion(char *ip, char* puerto)
{
	int socket_cliente;
	char modulo[16];
	int tid = pthread_self();
	pthread_getname_np(tid, modulo, 16);
	int conexion = -2;
	while (conexion < 0){
		if (conexion == -1){
			printf("Reintentando en %d segundos\n", TIEMPO_REINTENTO);
				sleep(TIEMPO_REINTENTO);
		}

	struct addrinfo hints;
	struct addrinfo *server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, puerto, &hints, &server_info);

	socket_cliente = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);

	conexion = connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen);
	if(conexion == -1)
		printf("error de conexion con el %s\n", modulo);

	freeaddrinfo(server_info);
	}

	return socket_cliente;
}

void enviar_mensaje(char* mensaje, int socket_cliente)
{
	//Quiero mandar el stream de datos	|cod_op|size|mensaje|

	t_buffer2 *ptr_buffer = malloc(sizeof(t_buffer2));
	t_paquete2 *paquete = malloc(sizeof(t_paquete2));

	//meto la cod_op en el paquete
	paquete->codigo_operacion = MENSAJE2;
	//asigno el buffer que previamente reserve memoria
	paquete->buffer = ptr_buffer;
	//asigno el size del buffer
	paquete->buffer->size = strlen(mensaje) + 1;
	//Con el size calculado, reservo memoria para el payload
	paquete->buffer->stream = malloc(paquete->buffer->size);
	//con memcpy() lleno el stream
	memcpy(paquete->buffer->stream, mensaje, paquete->buffer->size);


	int bytes = sizeof(int)*2 + ptr_buffer->size;

	//meto el cod_op + size + mensaje todo en un stream de datos
	void* a_enviar = malloc(bytes);
	int offset = 0;

	memcpy(a_enviar, &(paquete->codigo_operacion), sizeof(int));
	offset += sizeof(int);
	memcpy(a_enviar + offset, &(paquete->buffer->size), sizeof(int));
	offset += sizeof(int);
	memcpy(a_enviar + offset, paquete->buffer->stream, paquete->buffer->size);

	printf("Intentando enviar\n");
	if(send(socket_cliente, a_enviar, bytes, 0) == -1)
		printf("Error al enviar\n");
	else
		printf("Enviado\n");

	free(a_enviar);
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);

}
