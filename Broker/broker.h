/*
 * broker.h
 *
 *  Created on: 6 may. 2020
 *      Author: utnso
 */

#ifndef BROKER_H_
#define BROKER_H_
#include<commons/config.h>
#include<commons/collections/queue.h>
#include <conexion.h>
#include <mensajes.h>


#define IP "127.0.0.1"



t_colas* colas;
t_suscriptores* suscriptores;
t_semaforos* semaforos;

pthread_t hilo_LISTEN;

pthread_t hilo_NEW;
pthread_t hilo_APPEARED;

pthread_t hilo_GET;
pthread_t hilo_LOCALIZED;

pthread_t hilo_CATCH;
pthread_t hilo_CAUGHT;

t_config* config;

void broker_LISTEN();
void cola_NEW();
void cola_APPEARED();
void cola_GET();
void cola_LOCALIZED();
void cola_CATCH();
void cola_CAUGHT();




#endif /* BROKER_H_ */
