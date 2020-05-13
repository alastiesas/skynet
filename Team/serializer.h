/*
 * Serializer.h
 *
 *  Created on: 24 abr. 2020
 *      Author: utnso
 */

#ifndef SERIALIZER_H_
#define SERIALIZER_H_
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

// HAY QUE PASAR LOS uint32_t A Uuint32_t_32 que no se en que librería está. . .
typedef enum {
	NEW = 1,
	APPEARED = 2,
	GET = 3,
	LOCALIZED = 4,
	CATCH = 5,
	CAUGHT = 6,
}op_code;

//--estrcutruas para envio y recepcion de mensajes--
typedef struct {
	uint32_t size;
	void* stream;
}t_buffer;
typedef struct {
	op_code operation_code;
	t_buffer* buffer;
}t_package;
//--fin estrcutruas para envio y recepcion de mensajes--

//estructuras utiles
typedef struct {//posicion (x, y)
	uint32_t x;
	uint32_t y;
}t_position;
typedef struct {//Localizacion (posicion + cantidad)
	t_position* position;
	uint32_t amount;
}t_location;
//--fin estructuras utiles--


//--estructuras de mensajes

//NEW = {1, id_mensaje, tamaño_pokemon, pokemon, posx, posy, cantidad}
typedef struct {
	op_code operation_code;
	uint32_t message_id;
	uint32_t size_pokemon;
	char* pokemon;
	t_location* location;
}t_new;

//APPEARED = {2, id_mensaje, tamaño_pokeon, pokemon, posx, posy
typedef struct {
	op_code operation_code;
	uint32_t message_id;
	uint32_t size_pokemon;
	char* pokemon;
	t_position* position;
}t_appeared;

//GET = {3, id_mensaje, tamaño_pokemon, pokemon}
typedef struct {
	op_code operation_code;
	uint32_t message_id;
	uint32_t size_pokemon;
	char* pokemon;
}t_get;

//LOCALIZED = {4, id_mensaje, id_correlativo, tamaño_pokemon, pokemon, cantidad_localizaciones, lista_localizaciones}
typedef struct {
	op_code operation_code;
	uint32_t message_id;
	uint32_t correlative_id;
	uint32_t size_pokemon;
	char* pokemon;
	uint32_t position_amount;//cantidad de locaciones -> sizeof(t_location)*location_amount para el tamaño de la lista en bytes
	t_position* positions;//lista de locaciones (posicion + cantidad)
}t_localized;

//CATCH = {5, id_mensaje, tamaño_pokemon, pokemon, posicion}
typedef struct {
	op_code operation_code;
	uint32_t message_id;
	uint32_t size_pokemon;
	char* pokemon;
	t_position* position;
}t_catch;

//CAUGHT = {6, id_mensaje, id_correlativo, resultado}
typedef struct {
	op_code operation_code;
	uint32_t message_id;
	uint32_t correlative_id;
	bool result;//1 = si, 0 = no
}t_caught;
//--fin estructuras de mensajes



//constructores de mensajes, no requieren tamaños ni codigo de operación, lo calculan uint32_ternamente.
//retorna un mensaje NEW.
t_new* construct_new(char* pokemon, t_location* location);
t_new* construct_new_long(char* pokemon, uint32_t posx,uint32_t posy,uint32_t amount);
//retorna un mensaje APPEARED.
t_appeared* construct_appeared(char* pokemon, t_position* position);
t_appeared* construct_appeared_long(char* pokemon, uint32_t posx, uint32_t posy);
//retorna un mensaje GET.
t_get* construct_get(char* pokemon);
//retorna un mensaje LOCALIZED.
t_localized* construct_localized(uint32_t correlative_id, char* pokemon, uint32_t position_amount, t_position* positions);
//t_localized* construct_localized_long(uint32_t message_id, uint32_t correlative_id, char* pokemon, uint32_t position_amount, uint32_t** positions);
//retorna un mensaje CATCH.
t_catch* construct_catch(char* pokemon, t_position* position);
//retorna un mensaje CAUGHT.
t_caught* construct_caught(uint32_t correlative_id, bool result);
//fin constructores


//serializadores
void* serialize(t_package package, uint32_t *bytes);//NO SE USA
void* serialize_new(t_new* message, uint32_t *bytes);
void* serialize_appeared(t_appeared* message, uint32_t *bytes);
void* serialize_get(t_get* message, uint32_t *bytes);
void* serialize_localized(t_localized* message, uint32_t *bytes);
void* serialize_catch(t_catch* message, uint32_t *bytes);
void* serialize_caught(t_caught* message, uint32_t *bytes);
//fin serializadores


#endif /* SERIALIZER_H_ */
