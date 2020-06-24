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
#include "conexion.h"


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
t_package* serialize_saludo(char* mensaje);

//recibe una cola a suscribirse y lo convierte a paquete
//se
t_package* serialize_suscripcion(uint32_t ID_PROCESO, queue_code cola);

//Recibe un t_new y lo convierte en estructura paquete. (Reserva memoria para el nuevo paquete)
t_package* serialize_message_new(t_message_new* new);
t_package* serialize_appeared(t_message_appeared* new);
t_package* serialize_get(t_message_get* new);
t_package* serialize_localized(t_message_localized* new);
t_package* serialize_catch(t_message_catch* new);
t_package* serialize_caught(t_message_caught* new);

//Recibe un t_catch y lo convierte en estructura paquete. (Reserva memoria para el nuevo paquete)
t_package* serialize_catch(t_message_catch* catch);




//void* serializar_paquete(t_package* paquete, int bytes);

#endif /* SERIALIZE_H_ */
