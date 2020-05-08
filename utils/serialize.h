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


//Recibe una estructura t_paquete. Lo convierte en el stream |cod_op|size_buffer|buffer| y lo manda. Libera el paquete.
int32_t send_paquete(int32_t socket, t_paquete* paquete);

//Recibe un char* y lo convierte en estructura paquete. (Reserva memoria para el nuevo paquete)
t_paquete* serialize_message(char* mensaje);

//recibe una cola a suscribirse y lo convierte a paquete
//se
t_paquete* serialize_suscripcion(queue_code cola);

//Recibe un t_new y lo convierte en estructura paquete. (Reserva memoria para el nuevo paquete)
t_paquete* serialize_new(t_new* new);

//Recibe un t_catch y lo convierte en estructura paquete. (Reserva memoria para el nuevo paquete)
t_paquete* serialize_catch(t_catch* catch);




//---Funciones sin usar

void* serializar_paquete(t_paquete* paquete, int bytes);



#endif /* SERIALIZE_H_ */
