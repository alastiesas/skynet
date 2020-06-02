/*
 * process.c
 *
 *  Created on: 1 jun. 2020
 *      Author: utnso
 */

#include "broker.h"

void agregar_Acola(t_list* cola, t_list* colaIds, t_pending* t_mensaje, pthread_mutex_t mutex, t_log* logger, t_semaforos* semaforos, uint32_t* total_queue_messages){

    pthread_mutex_lock(&(mutex));
    /* do something that might make condition true */
    	list_add(cola, t_mensaje);
    	list_add(colaIds, t_mensaje->ID_mensaje);
    	(*total_queue_messages)++;		//solo se suma, no se resta al eliminar mensajes.
    	pthread_cond_broadcast(&(semaforos->broadcast));
    pthread_mutex_unlock(&(mutex));
}

void first_process(operation_code cod_op, int32_t socket_cliente, t_log* logger, t_queues* colas) {

	t_semaforos* my_semaphores;
	uint32_t* count;

		switch (cod_op) {

		case OPERATION_NEW:
			my_semaphores = semaphores_new;
			count = &total_new_messages;
			process_receive_message(socket_cliente, logger, colas->NEW_POKEMON, colas->NEW_POKEMON_IDS, my_semaphores, count);
			log_debug(logger, "Se notifico el mensaje new recibido");

			break;

		case OPERATION_APPEARED:
			my_semaphores = semaphores_appeared;
			count = &total_appeared_messages;
			process_receive_message(socket_cliente, logger, colas->APPEARED_POKEMON, colas->APPEARED_POKEMON_IDS, my_semaphores, count);

			break;

		case OPERATION_GET:
			my_semaphores = semaphores_get;
			count = &total_get_messages;
			process_receive_message(socket_cliente, logger, colas->GET_POKEMON, colas->GET_POKEMON_IDS, my_semaphores, count);

			break;

		case OPERATION_LOCALIZED:
			my_semaphores = semaphores_localized;
			count = &total_localized_messages;
			process_receive_message(socket_cliente, logger, colas->LOCALIZED_POKEMON, colas->LOCALIZED_POKEMON_IDS, my_semaphores, count);

			break;

		case OPERATION_CATCH:
			my_semaphores = semaphores_catch;
			count = &total_catch_messages;
			process_receive_message(socket_cliente, logger, colas->CATCH_POKEMON, colas->CATCH_POKEMON_IDS, my_semaphores, count);

			break;

		case OPERATION_CAUGHT:
			my_semaphores = semaphores_caught;
			count = &total_caught_messages;
			process_receive_message(socket_cliente, logger, colas->CAUGHT_POKEMON, colas->CAUGHT_POKEMON_IDS, my_semaphores, count);

			break;

		default:
			log_warning(logger, "Error de numero de cod_op, finaliza el thread de conexion", cod_op);
			pthread_exit(NULL);
		}
}

t_pending* broker_receive_mensaje(uint32_t socket_cliente, uint32_t* size, t_log* logger){

	t_pending* t_mensaje = malloc(sizeof(t_pending));
	t_mensaje->subs_confirmados = list_create();
	t_mensaje->subs_enviados = list_create();

	log_info(logger, "Esperando recibir tamanio del stream\n");

	if(recv(socket_cliente, size, sizeof(uint32_t), MSG_WAITALL) == -1)
		log_error(logger, "Error al recibir el tamanio del stream");
	else
		log_info(logger, "Se solicito recibir un tamanio de stream de: %d\n", *size);

	//recibir id de new. (El cual va a ignorar, porque setea el suyo propio luego)
	if(recv(socket_cliente, &(t_mensaje->ID_mensaje), sizeof(t_mensaje->ID_mensaje), MSG_WAITALL) == -1)
		log_error(logger, "Error al recibir el id de new");
	else
		log_info(logger, "id de new recibido: %d (no se usa ese ID)", t_mensaje->ID_mensaje);

	uint32_t size_ID = sizeof(uint32_t);
	uint32_t size_datos = *size - size_ID;
	t_mensaje->datos_mensaje = malloc(size_datos);

	//recibir t0do el resto de datos del mensaje
	int32_t bytes_received = recv(socket_cliente, t_mensaje->datos_mensaje, size_datos, MSG_WAITALL);
	if(bytes_received == -1)
		log_error(logger, "Error al recibir los datos del mensaje");
	else
		log_info(logger, "Datos del mensaje recibidos. (%d bytes de un total de %d)", bytes_received, size_datos);

	t_mensaje->bytes = size_datos;

	log_debug(logger, "Se guardo un mensaje de %d bytes (+4 bytes ID)", t_mensaje->bytes);

	if(*size != size_ID + size_datos)
		log_error(logger, "Tamanio erroneo");

	return t_mensaje;
}

void process_receive_message(int32_t socket_cliente, t_log* logger, t_list* queue, t_list* queueIds, t_semaforos* semaforos, uint32_t* total_queue_messages){
	uint32_t size;
	t_pending* t_mensaje;

	t_mensaje = broker_receive_mensaje(socket_cliente, &size, logger);

	//Generar ID del mensaje
	pthread_mutex_lock(&mutex_ID_global);
		t_mensaje->ID_mensaje = ID_GLOBAL;
		ID_GLOBAL++;
	pthread_mutex_unlock(&mutex_ID_global);

	//Enviar ID del mensaje
	send_ID(t_mensaje->ID_mensaje, socket_cliente, logger);

	//Recibir confirmacion de haber recibido la ID
	receive_ACK(socket_cliente, logger);

	//Agregar mensaje a cola correspondiente
	agregar_Acola(queue, queueIds, t_mensaje, semaforos->mutex_cola, logger, semaforos, total_queue_messages);

}

