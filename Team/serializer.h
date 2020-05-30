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

typedef enum {
	NEW = 1,
	APPEARED = 2,
	GET = 3,
	LOCALIZED = 4,
	CATCH = 5,
	CAUGHT = 6,
}op_code;

//estructuras utiles
typedef struct {
	uint32_t x;
	uint32_t y;
}t_position;//posicion (x, y)
typedef struct {
	t_position* position;
	uint32_t amount;
}t_location;//Localizacion (posicion + cantidad)
//--fin estructuras utiles--


//--estructuras de mensajes
//--estrcutruas para envio y recepcion de mensajes--
typedef struct {
	uint32_t size;
	void* stream;
}t_buffer;
typedef struct {
	op_code operation_code;
	t_buffer* buffer;
}t_package;


//NEW = {1, id_mensaje, tamaño_pokemon, pokemon, posx, posy, cantidad}
typedef struct {
	op_code operation_code;
	uint32_t message_id;
	uint32_t size_pokemon;
	char* pokemon;
	t_location* location;
}t_new;

//APPEARED = {2, id_mensaje, tamaño_pokeon, pokemon, posx, posy}
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



//constructores de mensajes
t_new* construct_new(char* pokemon, t_location* location);
t_new* construct_new_long(char* pokemon, uint32_t posx,uint32_t posy,uint32_t amount);
t_appeared* construct_appeared(char* pokemon, t_position* position);
t_appeared* construct_appeared_long(char* pokemon, uint32_t posx, uint32_t posy);
t_get* construct_get(char* pokemon);
t_localized* construct_localized(uint32_t correlative_id, char* pokemon, uint32_t position_amount, t_position* positions);
//t_localized* construct_localized_long(uint32_t message_id, uint32_t correlative_id, char* pokemon, uint32_t position_amount, uint32_t** positions);
t_catch* construct_catch(char* pokemon, t_position* position);
t_caught* construct_caught(uint32_t correlative_id, bool result);
//fin constructores

//destructores
void destroy_new(t_new* message);
void destroy_appeared(t_appeared* message);
void destroy_get(t_get* message);
void destroy_localized(t_localized* message);
void destroy_catch(t_catch* message);
void destroy_caught(t_caught* message);
//fin destructores

#endif /* SERIALIZER_H_ */
