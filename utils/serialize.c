/*
 * serialize.c
 *
 *  Created on: 24 abr. 2020
 *      Author: utnso
 */
#include "serialize.h"
#include <stdio.h>
#include <stdint.h>		//para uint32_t


//Recibe un paquete. Envia el cod_op, seguido del size_stream, seguido del stream
int32_t send_paquete(int32_t socket, t_paquete* paquete){
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


t_paquete* serialize_message(char* mensaje){

	t_buffer* ptr_buffer = malloc(sizeof(t_buffer));
	t_paquete* paquete = malloc(sizeof(t_paquete));

	//meto la cod_op en el paquete
	paquete->codigo_operacion = SALUDO;
	//asigno el buffer que previamente reserve memoria
	paquete->buffer = ptr_buffer;
	//asigno el size del buffer
	paquete->buffer->size = strlen(mensaje) + 1;
	//Con el size calculado, reservo memoria para el payload
	paquete->buffer->stream = malloc(paquete->buffer->size);
	//con memcpy() lleno el stream
	memcpy(paquete->buffer->stream, mensaje, paquete->buffer->size);

	return paquete;
}

t_paquete* serialize_suscripcion(uint32_t ID_proceso, queue_code cola){

	t_buffer* ptr_buffer = malloc(sizeof(t_buffer));
	t_paquete* paquete = malloc(sizeof(t_paquete));

	//meto la cod_op en el paquete
	paquete->codigo_operacion = SUSCRIPCION;
	//asigno el buffer que previamente reserve memoria
	paquete->buffer = ptr_buffer;
	//asigno el size del buffer
	paquete->buffer->size = sizeof(uint32_t) + sizeof(queue_code);
	//Con el size calculado, reservo memoria para el payload
	paquete->buffer->stream = malloc(paquete->buffer->size);
	//con memcpy() lleno el stream
	memcpy(paquete->buffer->stream, &ID_proceso, sizeof(uint32_t));
	memcpy(paquete->buffer->stream + sizeof(uint32_t), &cola, sizeof(queue_code));

	return paquete;
}

t_paquete* serialize_new(t_new* new){

	if(new->nombre == NULL)
		printf("ERROR FALTA COMPLETAR CAMPOS DEL MENSAJE NEW");	//no se pueden comprobar enteros sin inicializar

	t_buffer* buffer = malloc(sizeof(t_buffer));
	t_paquete* paquete = malloc(sizeof(t_paquete));
//	void* stream = paquete->buffer->stream;

	int32_t buffer_size = sizeof(new->id) + sizeof(new->size_nombre) + new->size_nombre + sizeof(new->posX) + sizeof(new->posY) + sizeof(new->cantidad);

	//meto la cod_op en el paquete
	paquete->codigo_operacion = NEW;
	//asigno el buffer que previamente reserve memoria
	paquete->buffer = buffer;
	//asigno el size del buffer
	paquete->buffer->size = buffer_size;
	//Con el size calculado, reservo memoria para el payload
	paquete->buffer->stream = malloc(paquete->buffer->size);

	int offset = 0;
	//con memcpy() lleno el stream
	memcpy(paquete->buffer->stream + offset, &(new->id), sizeof(new->id));
	offset += sizeof(new->id);
	memcpy(paquete->buffer->stream + offset, &(new->size_nombre), sizeof(new->size_nombre));
	offset += sizeof(new->size_nombre);
	memcpy(paquete->buffer->stream + offset, new->nombre, new->size_nombre);
	offset += new->size_nombre;
	memcpy(paquete->buffer->stream + offset, &(new->posX), sizeof(new->posX));
	offset += sizeof(new->posX);
	memcpy(paquete->buffer->stream + offset, &(new->posY), sizeof(new->posY));
	offset += sizeof(new->posY);
	memcpy(paquete->buffer->stream + offset, &(new->cantidad), sizeof(new->cantidad));
	offset += sizeof(new->cantidad);

	return paquete;
}


t_paquete* serialize_catch(t_catch* catch){

	if(catch->nombre == NULL)
		printf("ERROR FALTA COMPLETAR CAMPOS DEL MENSAJE CATCH");	//no se pueden comprobar enteros sin inicializar

	t_buffer* buffer = malloc(sizeof(t_buffer));
	t_paquete* paquete = malloc(sizeof(t_paquete));
//	void* stream = paquete->buffer->stream;

	int32_t buffer_size = sizeof(catch->id) + sizeof(catch->size_nombre) + catch->size_nombre + sizeof(catch->posX) + sizeof(catch->posY);

	//meto la cod_op en el paquete
	paquete->codigo_operacion = CATCHS;
	//asigno el buffer que previamente reserve memoria
	paquete->buffer = buffer;
	//asigno el size del buffer
	paquete->buffer->size = buffer_size;
	//Con el size calculado, reservo memoria para el payload
	paquete->buffer->stream = malloc(paquete->buffer->size);

	int offset = 0;
	//con memcpy() lleno el stream
	memcpy(paquete->buffer->stream + offset, &(catch->id), sizeof(catch->id));
	offset += sizeof(catch->id);
	memcpy(paquete->buffer->stream + offset, &(catch->size_nombre), sizeof(catch->size_nombre));
	offset += sizeof(catch->size_nombre);
	memcpy(paquete->buffer->stream + offset, catch->nombre, catch->size_nombre);
	offset += catch->size_nombre;
	memcpy(paquete->buffer->stream + offset, &(catch->posX), sizeof(catch->posX));
	offset += sizeof(catch->posX);
	memcpy(paquete->buffer->stream + offset, &(catch->posY), sizeof(catch->posY));
	offset += sizeof(catch->posY);


	return paquete;
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


//FROM serializer.c


/*

//serializadores
void* serialize_new(t_new* message, uint32_t* bytes) {
	*bytes = sizeof(uint32_t)*6 + sizeof(char) * (message->size_pokemon);
	void* serialized = malloc(*bytes);
	uint32_t offset = 0;
	uint32_t size = 0;
	//operation code
	size = sizeof(uint32_t);
	memcpy(serialized + offset, &message->operation_code, size);
	offset += size;
	//message_id
	memcpy(serialized + offset, &message->message_id, size);
	offset += size;
	//size_pokemon
	memcpy(serialized + offset, &message->size_pokemon, size);
	offset += size;
	//pokemon
	size = sizeof(char) * message->size_pokemon;
	memcpy(serialized + offset, message->pokemon, size);
	offset += size;
	//position x
	size = sizeof(uint32_t);
	memcpy(serialized + offset, &message->location->position->x, size);
	offset += size;
	//position y
	memcpy(serialized + offset, &message->location->position->y, size);
	offset += size;
	//cantidad
	memcpy(serialized + offset, &message->location->amount, size);
	offset += size;

	return serialized;
}

void* serialize_appeared(t_appeared* message, uint32_t *bytes) {
	*bytes = sizeof(uint32_t)*5 + sizeof(char) * (message->size_pokemon);
	void* serialized = malloc(*bytes);
	uint32_t offset = 0;
	uint32_t size = 0;
	//operation code
	size = sizeof(uint32_t);
	memcpy(serialized + offset, &message->operation_code, size);
	offset += size;
	//message_id
	memcpy(serialized + offset, &message->message_id, size);
	offset += size;
	//size_pokemon
	memcpy(serialized + offset, &message->size_pokemon, size);
	offset += size;
	//pokemon
	size = sizeof(char) * message->size_pokemon;
	memcpy(serialized + offset, message->pokemon, size);
	offset += size;
	//position x
	size = sizeof(uint32_t);
	memcpy(serialized + offset, &message->position->x, size);
	offset += size;
	//position y
	memcpy(serialized + offset, &message->position->y, size);
	offset += size;

	return serialized;
}

void* serialize_get(t_get* message, uint32_t *bytes) {
	*bytes = sizeof(uint32_t)*3 + sizeof(char) * (message->size_pokemon);
	void* serialized = malloc(*bytes);
	uint32_t offset = 0;
	uint32_t size = 0;
	//operation code
	size = sizeof(uint32_t);
	memcpy(serialized + offset, &message->operation_code, size);
	offset += size;
	//message_id
	memcpy(serialized + offset, &message->message_id, size);
	offset += size;
	//size_pokemon
	memcpy(serialized + offset, &message->size_pokemon, size);
	offset += size;
	//pokemon
	size = sizeof(char) * message->size_pokemon;
	memcpy(serialized + offset, message->pokemon, size);
	offset += size;

	return serialized;
}

void* serialize_localized(t_localized* message, uint32_t *bytes) {
	*bytes = sizeof(uint32_t)*5 + sizeof(char) * (message->size_pokemon) + sizeof(uint32_t)*2*(message->position_amount);
	void* serialized = malloc(*bytes);
	uint32_t offset = 0;
	uint32_t size = 0;
	//operation code
	size = sizeof(uint32_t);
	memcpy(serialized + offset, &message->operation_code, size);
	offset += size;
	//message_id
	memcpy(serialized + offset, &message->message_id, size);
	offset += size;
	//correlative_id
	memcpy(serialized + offset, &message->correlative_id, size);
	offset += size;
	//size_pokemon
	memcpy(serialized + offset, &message->size_pokemon, size);
	offset += size;
	//pokemon
	size = sizeof(char) * message->size_pokemon;
	memcpy(serialized + offset, message->pokemon, size);
	offset += size;
	//position_aomunt
	size = sizeof(uint32_t);
	memcpy(serialized + offset, &message->position_amount, size);
	offset += size;
	for(int i = 0; i < message->position_amount; i++){
		memcpy(serialized + offset, &message->positions[i].x, size);
		offset += size;
		memcpy(serialized + offset, &message->positions[i].y, size);
		offset += size;
	}

	return serialized;
}

void* serialize_catch(t_catch* message, uint32_t *bytes) {
	*bytes = sizeof(uint32_t)*5 + sizeof(char) * (message->size_pokemon);
	void* serialized = malloc(*bytes);
	uint32_t offset = 0;
	uint32_t size = 0;
	//operation code
	size = sizeof(uint32_t);
	memcpy(serialized + offset, &message->operation_code, size);
	offset += size;
	//message_id
	memcpy(serialized + offset, &message->message_id, size);
	offset += size;
	//size_pokemon
	memcpy(serialized + offset, &message->size_pokemon, size);
	offset += size;
	//pokemon
	size = sizeof(char) * message->size_pokemon;
	memcpy(serialized + offset, message->pokemon, size);
	offset += size;
	//position x
	size = sizeof(uint32_t);
	memcpy(serialized + offset, &message->position->x, size);
	offset += size;
	//position y
	memcpy(serialized + offset, &message->position->y, size);
	offset += size;

	return serialized;
}


void* serialize_caught(t_caught* message, uint32_t *bytes) {
	*bytes = sizeof(uint32_t)*3 + sizeof(bool);
	void* serialized = malloc(*bytes);
	uint32_t offset = 0;
	uint32_t size = 0;
	//operation code
	size = sizeof(uint32_t);
	memcpy(serialized + offset, &message->operation_code, size);
	offset += size;
	//message_id
	memcpy(serialized + offset, &message->message_id, size);
	offset += size;
	//correlative_id
	memcpy(serialized + offset, &message->correlative_id, size);
	offset += size;
	//result
	size = sizeof(bool);
	memcpy(serialized + offset, &message->result, size);
	offset += size;


	return serialized;
}
*/
