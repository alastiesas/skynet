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

typedef enum
{
	SALUDO=1,
	NEW=2,
	APPEARED=3,
	GET=4,
	LOCALIZED=5,
	CATCH=6,
	CAUGHT=7,
	SUSCRIPCION=8,
	CONFIRMACION=9
}op_code;

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
	t_queue* NEW;
	t_queue* APPEARED;
	t_queue* GET;
	t_queue* LOCALIZED;
	t_queue* CATCH;
	t_queue* CAUGHT;

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
	pthread_mutex_t mutex_cola_new;
	sem_t nuevo_new;

} t_semaforos;

typedef struct
{
	t_list* procesos_enviados;
	t_list* procesos_confirmados;
	t_new* new;

} t_mensaje;



#endif /* STRUCTS_H_ */
