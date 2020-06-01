/*
 * structs.h
 *
 *  Created on: 24 abr. 2020
 *      Author: utnso
 */

#ifndef STRUCTS_H_
#define STRUCTS_H_


#include <stdint.h>		//para uint32_t
#include<commons/collections/queue.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdio.h>


typedef enum {
	OPERATION_NEW = 1,
	OPERATION_APPEARED = 2,
	OPERATION_GET = 3,
	OPERATION_LOCALIZED = 4,
	OPERATION_CATCH = 5,
	OPERATION_CAUGHT = 6,
	OPERATION_SUSCRIPTION = 7,
	OPERATION_CONFIRMATION = 8,
	OPERATION_END = 9,
	PRUEBA = 10
} operation_code;

typedef enum
{
	COLA_NEW=1,
	COLA_APPEARED=2,
	COLA_GET=3,
	COLA_LOCALIZED=4,
	COLA_CATCH=5,
	COLA_CAUGHT=6,

}queue_code;

typedef enum
{
	BROKER=1,
	TEAM=2,
	GAMECARD=3,
	GAMEBOY=4,
	SUSCRIPTOR=5 //lo usa el gameboy para suscribirse al broker

}process_code;

//-----------------------------------------------

typedef struct
{
	int32_t size;
	void* stream;
} t_buffer;

typedef struct {
	operation_code operation_code;
	t_buffer* buffer;
} t_package;

//-----------------------------------------------

typedef struct {
	uint32_t x;
	uint32_t y;
} t_position;

typedef struct {
	t_position* position;
	uint32_t amount;
} t_location;

//---------------------------------------------Mensajes

typedef struct {
	uint32_t id;
	uint32_t size_pokemon_name;
	char* pokemon_name;
	t_location* location;
} t_message_new;

typedef struct {
	uint32_t id;
	uint32_t correlative_id;
	uint32_t size_pokemon_name;
	char* pokemon_name;
	t_position* position;
} t_message_appeared;

typedef struct {
	uint32_t id;
	uint32_t size_pokemon_name;
	char* pokemon_name;
} t_message_get;

//pending
typedef struct {
	uint32_t id;
	uint32_t correlative_id;
	uint32_t size_pokemon_name;
	char* pokemon_name;
	uint32_t position_amount; //cantidad de locaciones -> sizeof(t_location)*location_amount para el tamaño de la lista en bytes
	t_position* positions; //lista de locaciones (posicion + cantidad)
} t_message_localized;

typedef struct {
	uint32_t id;
	uint32_t size_pokemon_name;
	char* pokemon_name;
	t_position* position;
} t_message_catch;

typedef struct {
	uint32_t id;
	uint32_t correlative_id;
	bool result;
} t_message_caught;


t_message_appeared* create_message_appeared(char* pokemon_name, t_position* position);
t_message_appeared* create_message_appeared_long(char* pokemon_name, uint32_t position_x, uint32_t position_y);
t_message_catch* create_message_catch(char* pokemon_name, t_position* position);
t_message_catch* create_message_catch_long(char* pokemon, uint32_t posx, uint32_t posy);
t_message_caught* create_message_caught(uint32_t correlative_id, bool result);
t_message_get* create_message_get(char* pokemon_name);
t_message_localized* create_message_localized(uint32_t correlative_id, char* pokemon_name, uint32_t position_amount, t_position* positions);
t_message_new* create_message_new(char* pokemon_name, t_location* location);
t_message_new* create_message_new_long(char* pokemon_name, uint32_t position_x, uint32_t position_y, uint32_t amount);

void destroy_message_appeared(t_message_appeared* message_appeared);
void destroy_message_catch(t_message_catch* message_catch);
void destroy_message_caught(t_message_caught* message_caught);
void destroy_message_get(t_message_get* message_get);
void destroy_message_localized(t_message_localized* message_localized);
void destroy_message_new(t_message_new* message_new);

#endif /* STRUCTS_H_ */
