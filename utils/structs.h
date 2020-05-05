/*
 * structs.h
 *
 *  Created on: 24 abr. 2020
 *      Author: utnso
 */

#ifndef STRUCTS_H_
#define STRUCTS_H_


#include <stdint.h>		//para uint32_t

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





#endif /* STRUCTS_H_ */
