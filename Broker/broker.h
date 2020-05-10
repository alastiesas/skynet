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


t_log* log;
t_config* config;

t_colas* queues;
t_suscriptores* suscribers;
t_semaforos* semaphores;

pthread_t hilo_LISTEN;

pthread_t hilo_NEW;
pthread_t hilo_APPEARED;

pthread_t hilo_GET;
pthread_t hilo_LOCALIZED;

pthread_t hilo_CATCH;
pthread_t hilo_CAUGHT;

t_config* config;

void cola_NEW();
void cola_APPEARED();
void cola_GET();
void cola_LOCALIZED();
void cola_CATCH();
void cola_CAUGHT();

//inicia el servidor del broker con las referencias a colas y semaforos necesarias
void iniciar_servidor_broker(char* puerto, t_log* logger, t_colas* colas, t_suscriptores* suscriptores, t_semaforos* semaforos);

//atiende clientes en un nuevo hilo con la funcion broker_server_client()
void esperar_clientes(int32_t socket_servidor, t_log* logger, t_colas* colas, t_suscriptores* suscriptores, t_semaforos* semaforos);

//verifica si lo que se recibe es una suscripcion o un mensaje, y lo procesa segun corresponda
void broker_serves_client(void* input);


void process_suscripcion(op_code cod_op, int32_t socket_cliente, t_log* logger, t_suscriptores* suscriptores, t_semaforos* semaforos);


void process_mensaje(op_code cod_op, int32_t socket_cliente, t_log* logger, t_colas* colas, t_semaforos* semaforos);

//mutex funciona sin pasarlo como puntero??
void agregar_Asubs(int32_t socket, t_list* lista_subs, pthread_mutex_t mutex, t_log* logger);

//agrega a una cola del broker un t_pending, dada una estructura t_mensaje (new, catch, etc..)
//mutex funciona sin pasarlo como puntero??
void agregar_Acola(t_queue* cola, void* t_mensaje, pthread_mutex_t mutex, t_log* logger);

//Recibe el size del stream. Recibe un queue_code.
queue_code receive_cola(uint32_t socket, t_log* logger);

void initialization();

void general_initialization(char* process_name);

void specific_initialization();

void behavior();

void listening();

void sending();

void termination();

void specific_termination();

#endif /* BROKER_H_ */
