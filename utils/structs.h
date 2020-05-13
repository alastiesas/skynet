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
	op_code codigo_operacion;
	t_buffer* buffer;
} t_paquete;


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
	t_queue* NEW_POKEMON;		//asi nombra a las colas en t0do el TP
	t_queue* APPEARED_POKEMON;
	t_queue* GET_POKEMON;
	t_queue* LOCALIZED_POKEMON;
	t_queue* CATCH_POKEMON;
	t_queue* CAUGHT_POKEMON;

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

typedef struct
{
	uint32_t ID_suscriber;
	bool connected;
	int32_t socket;

} t_suscriber;

typedef struct
{
	pthread_mutex_t mutex_ID_global;

	pthread_mutex_t mutex_cola_new;
	pthread_mutex_t mutex_subs_new;
	sem_t nuevo_new;

	pthread_mutex_t mutex_cola_appeared;
	pthread_mutex_t mutex_subs_appeared;
	sem_t nuevo_appeared;

	pthread_mutex_t mutex_cola_get;
	pthread_mutex_t mutex_subs_get;
	sem_t nuevo_get;

	pthread_mutex_t mutex_cola_localized;
	pthread_mutex_t mutex_subs_localized;
	sem_t nuevo_localized;

	pthread_mutex_t mutex_cola_catch;
	pthread_mutex_t mutex_subs_catch;
	sem_t nuevo_catch;

	pthread_mutex_t mutex_cola_caught;
	pthread_mutex_t mutex_subs_caught;
	sem_t nuevo_caught;

} t_semaforos;

typedef struct
{
	uint32_t ID_mensaje;
	t_list* subs_enviados;
	t_list* subs_confirmados;
	void* datos_mensaje;

} t_pending;



#endif /* STRUCTS_H_ */
