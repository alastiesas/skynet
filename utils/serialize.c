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
int32_t send_paquete(int32_t socket, t_package* paquete){
	int32_t result;
	uint32_t bytes = sizeof(int32_t)*2 + paquete->buffer->size;
	printf("Bytes a enviar: %d\n", bytes);

	//meto el cod_op + size + mensaje t0d0 en un stream de datos
	char* a_enviar = malloc(bytes);
	int offset = 0;

	memcpy(a_enviar, &(paquete->operation_code), sizeof(int32_t));
	offset += sizeof(int32_t);
	memcpy(a_enviar + offset, &(paquete->buffer->size), sizeof(int32_t));
	offset += sizeof(int32_t);
	memcpy(a_enviar + offset, paquete->buffer->stream, paquete->buffer->size);


	printf("Se va a enviar: %s\n", (char *)a_enviar);
	result = send(socket, a_enviar, bytes, 0);
													//TODO hacer reintento de envio si se enviaron una cantidad parcial de bytes
	free(a_enviar);
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);

return result;
}

t_package* serialize_suscripcion(uint32_t ID_proceso, queue_code cola){

	t_buffer* ptr_buffer = malloc(sizeof(t_buffer));
	t_package* paquete = malloc(sizeof(t_package));

	//meto la cod_op en el paquete
	paquete->operation_code = OPERATION_SUSCRIPTION;
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

t_package* serialize_message_new(t_message_new* message_new) {

	t_buffer* buffer = malloc(sizeof(t_buffer));
	t_package* package = malloc(sizeof(t_package));

	uint32_t buffer_size = sizeof(message_new->id)
			+ sizeof(message_new->size_pokemon_name)
			+ message_new->size_pokemon_name
			+ sizeof(message_new->location->position->x)
			+ sizeof(message_new->location->position->y)
			+ sizeof(message_new->location->amount);

	package->operation_code = OPERATION_NEW;
	package->buffer = buffer;
	package->buffer->size = buffer_size;
	package->buffer->stream = malloc(package->buffer->size);

	int offset = 0;
	memcpy(package->buffer->stream + offset, &(message_new->id), sizeof(message_new->id));
	offset += sizeof(message_new->id);
	memcpy(package->buffer->stream + offset, &(message_new->size_pokemon_name),	sizeof(message_new->size_pokemon_name));
	offset += sizeof(message_new->size_pokemon_name);
	memcpy(package->buffer->stream + offset, message_new->pokemon_name,	message_new->size_pokemon_name);
	offset += message_new->size_pokemon_name;
	memcpy(package->buffer->stream + offset, &(message_new->location->position->x),	sizeof(message_new->location->position->x));
	offset += sizeof(message_new->location->position->x);
	memcpy(package->buffer->stream + offset, &(message_new->location->position->y),	sizeof(message_new->location->position->y));
	offset += sizeof(message_new->location->position->y);
	memcpy(package->buffer->stream + offset, &(message_new->location->amount), sizeof(message_new->location->amount));
	offset += sizeof(message_new->location->amount);

	return package;
}

t_package* serialize_appeared(t_message_appeared* message) {
	t_buffer* buffer = malloc(sizeof(t_buffer));
	t_package* package = malloc(sizeof(t_package));

	uint32_t buffer_size = sizeof(uint32_t)*5 + sizeof(char) * (message->size_pokemon_name);

	package->operation_code = OPERATION_APPEARED;
	package->buffer = buffer;
	package->buffer->size = buffer_size;
	package->buffer->stream = malloc(package->buffer->size);
	void* serialized = package->buffer->stream;

	uint32_t size = 0;
	size = sizeof(uint32_t);
	uint32_t offset = 0;

	//message_id
	memcpy(serialized + offset, &message->id, size);
	offset += size;
//ID CORRELATIVO
	memcpy(serialized + offset, &message->correlative_id, size);
	offset += size;
	//size_pokemon
	memcpy(serialized + offset, &message->size_pokemon_name, size);
	offset += size;
	//pokemon
	size = sizeof(char) * message->size_pokemon_name;
	memcpy(serialized + offset, message->pokemon_name, size);
	offset += size;
	//position x
	size = sizeof(uint32_t);
	memcpy(serialized + offset, &message->position->x, size);
	offset += size;
	//position y
	memcpy(serialized + offset, &message->position->y, size);
	offset += size;

	return package;
}

t_package* serialize_get(t_message_get* message) {
	t_buffer* buffer = malloc(sizeof(t_buffer));
	t_package* package = malloc(sizeof(t_package));

	uint32_t buffer_size = sizeof(uint32_t)*2 + sizeof(char) * (message->size_pokemon_name);

	package->operation_code = OPERATION_GET;
	package->buffer = buffer;
	package->buffer->size = buffer_size;
	package->buffer->stream = malloc(package->buffer->size);
	void* serialized = package->buffer->stream;

	uint32_t offset = 0;
	uint32_t size = 0;
	size = sizeof(uint32_t);

	//message_id
	memcpy(serialized + offset, &message->id, size);
	offset += size;
	//size_pokemon
	memcpy(serialized + offset, &message->size_pokemon_name, size);
	offset += size;
	//pokemon
	size = sizeof(char) * message->size_pokemon_name;
	memcpy(serialized + offset, message->pokemon_name, size);
	offset += size;

	return package;
}

t_package* serialize_localized(t_message_localized* message) {
	t_buffer* buffer = malloc(sizeof(t_buffer));
	t_package* package = malloc(sizeof(t_package));

	uint32_t buffer_size = sizeof(uint32_t)*4 + sizeof(char) * (message->size_pokemon_name) + sizeof(uint32_t)*2*(message->position_amount);

	package->operation_code = OPERATION_LOCALIZED;
	package->buffer = buffer;
	package->buffer->size = buffer_size;
	package->buffer->stream = malloc(package->buffer->size);
	void* serialized = package->buffer->stream;

	uint32_t offset = 0;
	uint32_t size = 0;
	size = sizeof(uint32_t);

	//message_id
	memcpy(serialized + offset, &message->id, size);
	offset += size;
	//correlative_id
	memcpy(serialized + offset, &message->correlative_id, size);
	offset += size;
	//size_pokemon
	memcpy(serialized + offset, &message->size_pokemon_name, size);
	offset += size;
	//pokemon
	size = sizeof(char) * message->size_pokemon_name;
	memcpy(serialized + offset, message->pokemon_name, size);
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

	return package;
}

t_package* serialize_catch(t_message_catch* catch){

	if(catch->pokemon_name == NULL)
		printf("ERROR FALTA COMPLETAR CAMPOS DEL MENSAJE CATCH");	//no se pueden comprobar enteros sin inicializar

	t_buffer* buffer = malloc(sizeof(t_buffer));
	t_package* paquete = malloc(sizeof(t_package));

	uint32_t buffer_size = sizeof(catch->id) + sizeof(catch->size_pokemon_name) + catch->size_pokemon_name + sizeof(catch->position->x) + sizeof(catch->position->y);

	//meto la cod_op en el paquete
	paquete->operation_code = OPERATION_CATCH;
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
	memcpy(paquete->buffer->stream + offset, &(catch->size_pokemon_name), sizeof(catch->size_pokemon_name));
	offset += sizeof(catch->size_pokemon_name);
	memcpy(paquete->buffer->stream + offset, catch->pokemon_name, catch->size_pokemon_name);
	offset += catch->size_pokemon_name;
	memcpy(paquete->buffer->stream + offset, &(catch->position->x), sizeof(catch->position->x));
	offset += sizeof(catch->position->x);
	memcpy(paquete->buffer->stream + offset, &(catch->position->y), sizeof(catch->position->y));
	offset += sizeof(catch->position->y);


	return paquete;
}

t_package* serialize_caught(t_message_caught* message) {
	t_buffer* buffer = malloc(sizeof(t_buffer));
	t_package* package = malloc(sizeof(t_package));

	uint32_t buffer_size = sizeof(uint32_t)*2 + sizeof(bool);

	package->operation_code = OPERATION_CAUGHT;
	package->buffer = buffer;
	package->buffer->size = buffer_size;
	package->buffer->stream = malloc(package->buffer->size);
	void* serialized = package->buffer->stream;

	uint32_t offset = 0;
	uint32_t size = 0;
	size = sizeof(uint32_t);

	//message_id
	memcpy(serialized + offset, &message->id, size);
	offset += size;
	//correlative_id
	memcpy(serialized + offset, &message->correlative_id, size);
	offset += size;
	//result
	size = sizeof(bool);
	memcpy(serialized + offset, &message->result, size);
	offset += size;


	return package;
}

/*
void* serializar_paquete(t_package* paquete, int bytes)
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
*/

t_package* serialize_saludo(char* mensaje){

	t_buffer* ptr_buffer = malloc(sizeof(t_buffer));
	t_package* paquete = malloc(sizeof(t_package));

	//meto la cod_op en el paquete
	paquete->operation_code = PRUEBA;
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
