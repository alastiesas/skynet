/*
 * serialize.c
 *
 *  Created on: 24 abr. 2020
 *      Author: utnso
 */
#include "serialize.h"
#include <stdio.h>


uint32_t send_paquete(int32_t socket, t_paquete* paquete){
	int32_t result;
	uint32_t bytes = sizeof(int32_t)*2 + paquete->buffer->size;
	printf("Bytes a enviar: %d\n", bytes);

	//meto el cod_op + size + mensaje t0d0 en un stream de datos
	char* a_enviar = malloc(bytes);
	int offset = 0;

	memcpy(a_enviar, &(paquete->codigo_operacion), sizeof(int32_t));
	offset += sizeof(int32_t);
	memcpy(a_enviar + offset, &(paquete->buffer->size), sizeof(int32_t));
	offset += sizeof(int32_t);
	memcpy(a_enviar + offset, paquete->buffer->stream, paquete->buffer->size);


	printf("Se va a enviar: %s\n", (char *)a_enviar);
	result = send(socket, a_enviar, bytes, 0);

	free(a_enviar);
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);

return result;
}

void* serializar_paquete(t_paquete* paquete, int bytes)
{
	void * magic = malloc(bytes);
	int desplazamiento = 0;

	memcpy(magic + desplazamiento, &(paquete->codigo_operacion), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(magic + desplazamiento, &(paquete->buffer->size), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(magic + desplazamiento, paquete->buffer->stream, paquete->buffer->size);
	desplazamiento+= paquete->buffer->size;

	return magic;
}
