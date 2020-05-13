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


char* IP_BROKER;
char* PORT;

uint32_t ID_GLOBAL;
uint32_t size_subs_new;			//para mantener la cantidad de suscriptores tambien hay que usar semaforos?
uint32_t size_subs_appeared;
uint32_t size_subs_get;
uint32_t size_subs_localized;
uint32_t size_subs_catch;
uint32_t size_subs_caught;

t_log* logger;
t_config* config;

t_colas* queues;
t_suscriptores* suscribers;
t_semaforos* semaphores;

pthread_t listening_thread;

void cola_NEW();
void cola_APPEARED();
void cola_GET();
void cola_LOCALIZED();
void cola_CATCH();
void cola_CAUGHT();

//inicia el servidor del broker con las referencias a colas y semaforos necesarias
void iniciar_servidor_broker();

//atiende clientes en un nuevo hilo con la funcion broker_server_client()
void esperar_clientes(int32_t socket_servidor, t_log* logger, t_colas* colas, t_suscriptores* suscriptores, t_semaforos* semaforos);

//verifica si lo que se recibe es una suscripcion o un mensaje, y lo procesa segun corresponda
void broker_serves_client(void* input);


void process_suscripcion(op_code cod_op, int32_t socket_cliente, t_log* logger, t_suscriptores* suscriptores, t_semaforos* semaforos);


void process_mensaje(op_code cod_op, int32_t socket_cliente, t_log* logger, t_colas* colas, t_semaforos* semaforos);

//TODO
void send_received_message(uint32_t ID_proceso);

//mutex funciona sin pasarlo como puntero??
void agregar_Asubs(uint32_t ID_proceso, int32_t socket, queue_code cola, t_list* lista_subs, pthread_mutex_t mutex, t_log* logger);

//agrega a una cola del broker un t_pending, dada una estructura t_mensaje (new, catch, etc..)
//mutex funciona sin pasarlo como puntero??
void agregar_Acola(t_queue* cola, t_pending* t_mensaje, pthread_mutex_t mutex, t_log* logger);

//Recibe el size del stream. Recibe un queue_code.
queue_code receive_cola(uint32_t socket, t_log* logger);

t_pending* broker_receive_mensaje(uint32_t socket_cliente, uint32_t* size, t_log* logger);

void process_NEW(int32_t socket_cliente, t_log* logger, t_queue* queue_NEW, t_semaforos* semaforos);
void process_APPEARED(int32_t socket_cliente, t_log* logger, t_queue* queue_NEW, t_semaforos* semaforos);
void process_CATCH(int32_t socket_cliente, t_log* logger, t_queue* queue_NEW, t_semaforos* semaforos);
void process_CAUGHT(int32_t socket_cliente, t_log* logger, t_queue* queue_NEW, t_semaforos* semaforos);
void process_GET(int32_t socket_cliente, t_log* logger, t_queue* queue_NEW, t_semaforos* semaforos);
void process_LOCALIZED(int32_t socket_cliente, t_log* logger, t_queue* queue_NEW, t_semaforos* semaforos);

void initialization();

void generic_initialization();

void specific_initialization();

void behavior();

void listening();

void sending();

void termination();

void specific_termination();

#endif /* BROKER_H_ */
