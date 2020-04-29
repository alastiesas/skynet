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


//recibe un t_paquete paquete, lo serializa, y lo manda
uint32_t send_paquete(int32_t socket, t_paquete* paquete);

void* serializar_paquete(t_paquete* paquete, int bytes);



#endif /* SERIALIZE_H_ */
