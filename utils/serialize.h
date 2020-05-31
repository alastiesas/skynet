/*
 * serialize.h
 *
 *  Created on: 24 abr. 2020
 *      Author: utnso
 */
#ifndef SERIALIZE_H_
#define SERIALIZE_H_

#include <stdlib.h>	//para malloc
#include <string.h>	//para memcpy
#include<sys/socket.h>
#include<netdb.h>


#include "structs.h"
/*
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <serialize.h>
*/

//Recibe una estructura t_paquete. Lo convierte en el stream |cod_op|size_buffer|buffer| y lo manda. Libera el paquete.
int32_t send_paquete(int32_t socket, t_package* paquete);

//Recibe un char* y lo convierte en estructura paquete. (Reserva memoria para el nuevo paquete)
t_package* serialize_message(char* mensaje);

//recibe una cola a suscribirse y lo convierte a paquete
//se
t_package* serialize_suscripcion(uint32_t ID_PROCESO, queue_code cola);

//Recibe un t_new y lo convierte en estructura paquete. (Reserva memoria para el nuevo paquete)
t_package* serialize_new(t_new* new);

//Recibe un t_catch y lo convierte en estructura paquete. (Reserva memoria para el nuevo paquete)
t_package* serialize_catch(t_catch* catch);




//---Funciones sin usar

void* serializar_paquete(t_package* paquete, int bytes);


//FROM serializer.h
/*
void* serialize_new(t_new* message, uint32_t* bytes);
void* serialize_appeared(t_appeared* message, uint32_t *bytes);
void* serialize_get(t_get* message, uint32_t *bytes);
void* serialize_localized(t_localized* message, uint32_t *bytes);
void* serialize_catch(t_catch* message, uint32_t *bytes);
void* serialize_caught(t_caught* message, uint32_t *bytes);
*/

#endif /* SERIALIZE_H_ */
