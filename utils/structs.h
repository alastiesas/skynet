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

//
#include <stdio.h>
//

typedef enum
{
	NEW=1,
	APPEARED=2,
	GET=3,
	LOCALIZED=4,
	CATCHS=5,
	CAUGHT=6,
	SUSCRIPCION=7,
	CONFIRMACION=8,
	SALUDO=9
}op_code;

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

typedef struct
{
	int32_t size;
	void* stream;
} t_buffer;

typedef struct
{
	op_code codigo_operacion; //operation_code
	t_buffer* buffer;
} t_paquete; //t_package


typedef struct
{
		//op_code CATCH
	uint32_t id;
	uint32_t size_nombre;
	char* nombre;
	uint32_t posX;
	uint32_t posY;

} t_catch;

typedef struct
{
		//op_code NEW
	uint32_t id;
	uint32_t size_nombre;
	char* nombre;
	uint32_t posX;
	uint32_t posY;
	uint32_t cantidad;

} t_new;

typedef struct
{
	t_list* NEW_POKEMON;		//asi nombra a las colas en t0do el TP
	t_list* APPEARED_POKEMON;
	t_list* GET_POKEMON;
	t_list* LOCALIZED_POKEMON;
	t_list* CATCH_POKEMON;
	t_list* CAUGHT_POKEMON;

	t_list* NEW_POKEMON_IDS;
	t_list* APPEARED_POKEMON_IDS;
	t_list* GET_POKEMON_IDS;
	t_list* LOCALIZED_POKEMON_IDS;
	t_list* CATCH_POKEMON_IDS;
	t_list* CAUGHT_POKEMON_IDS;

} t_colas;

typedef struct
{
	t_list* NEW;
	t_list* APPEARED;
	t_list* GET;
	t_list* LOCALIZED;
	t_list* CATCH;
	t_list* CAUGHT;

} t_suscriptores;


/*
 *
 *
 */
// FROM serializer.h (team)

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


/*
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
 *
 */

#endif /* STRUCTS_H_ */
