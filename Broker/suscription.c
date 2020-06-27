/*
 * suscription.c
 *
 *  Created on: 1 jun. 2020
 *      Author: utnso
 */

#include "broker.h"

void agregar_Asubs(t_suscriber* suscriber, int32_t socket, queue_code cola, t_list* lista_subs, pthread_mutex_t mutex, t_log* logger){

	suscriber->suscribed_queue = cola;
	suscriber->connected = true;
	suscriber->socket = socket;

	pthread_mutex_lock(&mutex);
		list_add(lista_subs, suscriber);
		pthread_mutex_unlock(&mutex);
	log_info(logger, "Se agrego el socket '%d' a suscriptores\n", socket);

}

void process_suscripcion(operation_code cod_op, int32_t socket_cliente, t_log* logger, t_suscribers* suscriptores) {

	//ya recibi la cod_op
	//recibir el size del stream
	uint32_t size;
	size = receive_size(socket_cliente, logger);

	//recibir el ID del proceso
	uint32_t ID_proceso;
	ID_proceso = receive_ID_proceso(socket_cliente, logger);

	//recibir la cola a suscribirse
	queue_code cola;
	cola = receive_cola(socket_cliente, logger);

	if(size != sizeof(ID_proceso) + sizeof(cola))
		log_error(logger, "Tamanio erroneo");

	t_suscriber* suscriber;
	char* queue_name;

	t_semaforos* my_semaphores;
	t_list* my_queue;
	t_list* my_queueIDs;
	uint32_t* count;
//verificar si es un proceso que ya se habia suscrito antes,
	switch(cola){
	case COLA_NEW:
		//buscar sub en la lista de subs de la cola elegida. (un ID de proceso se puede suscribir a todas las colas independientemente)
		suscriber = find_suscriber_given_ID((void*) ID_proceso, suscribers->NEW, semaphores_new->mutex_subs);
		my_semaphores = semaphores_new;
		my_queue = queues->NEW_POKEMON;
		my_queueIDs = queues->NEW_POKEMON_IDS;
		count = &total_new_messages;
		queue_name = "NEW";
		break;
	case COLA_APPEARED:
		suscriber = find_suscriber_given_ID((void*) ID_proceso, suscribers->APPEARED, semaphores_appeared->mutex_subs);
		my_semaphores = semaphores_appeared;
		my_queue = queues->APPEARED_POKEMON;
		my_queueIDs = queues->APPEARED_POKEMON_IDS;
		count = &total_appeared_messages;
		queue_name = "APPEARED";
		break;
	case COLA_GET:
		suscriber = find_suscriber_given_ID((void*) ID_proceso, suscribers->GET, semaphores_get->mutex_subs);
		my_semaphores = semaphores_get;
		my_queue = queues->GET_POKEMON;
		my_queueIDs = queues->GET_POKEMON_IDS;
		count = &total_get_messages;
		queue_name = "GET";
		break;
	case COLA_LOCALIZED:
		suscriber = find_suscriber_given_ID((void*) ID_proceso, suscribers->LOCALIZED, semaphores_localized->mutex_subs);
		my_semaphores = semaphores_localized;
		my_queue = queues->LOCALIZED_POKEMON;
		my_queueIDs = queues->LOCALIZED_POKEMON_IDS;
		count = &total_localized_messages;
		queue_name = "LOCALIZED";
		break;
	case COLA_CATCH:
		suscriber = find_suscriber_given_ID((void*) ID_proceso, suscribers->CATCH, semaphores_catch->mutex_subs);
		my_semaphores = semaphores_catch;
		my_queue = queues->CATCH_POKEMON;
		my_queueIDs = queues->CATCH_POKEMON_IDS;
		count = &total_catch_messages;
		queue_name = "CATCH";
		break;
	case COLA_CAUGHT:
		suscriber = find_suscriber_given_ID((void*) ID_proceso, suscribers->CAUGHT, semaphores_caught->mutex_subs);
		my_semaphores = semaphores_caught;
		my_queue = queues->CAUGHT_POKEMON;
		my_queueIDs = queues->CAUGHT_POKEMON_IDS;
		count = &total_caught_messages;
		queue_name = "CAUGHT";
		break;
	default:
		break;
	}

	if(suscriber == NULL){
		log_info(logger, "Se suscribe por primera vez a la cola %s, el proceso %d", queue_name, (uint32_t) ID_proceso);
		//camino normal

		//crear el t_suscriber
		suscriber = malloc(sizeof(t_suscriber));
		suscriber->ID_suscriber = ID_proceso;
		suscriber->sent_messages = list_create();

		//asignar la cola al t_suscriber y guardar en la cola de suscritos
		switch(cola){

		case COLA_NEW:
			log_info(logger, "Por suscribir al socket '%d' a la cola de NEW", socket_cliente);
			agregar_Asubs(suscriber, socket_cliente, cola, suscriptores->NEW, my_semaphores->mutex_subs, logger);
			break;

		case COLA_APPEARED:
			log_info(logger, "Por suscribir al socket '%d' a la cola de APPEARED", socket_cliente);
			agregar_Asubs(suscriber, socket_cliente, cola, suscriptores->APPEARED, my_semaphores->mutex_subs, logger);
			break;

		case COLA_CATCH:
			log_info(logger, "Por suscribir al socket '%d' a la cola de CATCH", socket_cliente);
			agregar_Asubs(suscriber, socket_cliente, cola, suscriptores->CATCH, my_semaphores->mutex_subs, logger);
			break;

		case COLA_CAUGHT:
			log_info(logger, "Por suscribir al socket '%d' a la cola de CAUGHT", socket_cliente);
			agregar_Asubs(suscriber, socket_cliente, cola, suscriptores->CAUGHT, my_semaphores->mutex_subs, logger);
			break;

		case COLA_GET:
			log_info(logger, "Por suscribir al socket '%d' a la cola de GET", socket_cliente);
			agregar_Asubs(suscriber, socket_cliente, cola, suscriptores->GET, my_semaphores->mutex_subs, logger);
			break;

		case COLA_LOCALIZED:
			log_info(logger, "Por suscribir al socket '%d' a la cola de LOCALIZED", socket_cliente);
			agregar_Asubs(suscriber, socket_cliente, cola, suscriptores->LOCALIZED, my_semaphores->mutex_subs, logger);
			break;

		default:
			log_error(logger, "Aun no puedo suscribir a nadie en ese tipo de cola\n");
			return;
		}

		//crear logger para el suscriptor
		char* nombre = string_new();
		string_append(&nombre, "sub-");
		char* ID_char = string_itoa(suscriber->ID_suscriber); //TODO string_itoa sigue perdiendo bytes...
		string_append(&nombre, ID_char);
		free(ID_char);
		char* file = string_duplicate(nombre);
		string_append(&nombre, ".log");
		suscriber->log = log_create(nombre, file, LOG_CONSOLE, LOG_LEVEL_TRACE);
		free(nombre);
		free(file);

	}
	else if(suscriber->connected == false){
		log_info(logger, "Se esta reconectando a la cola %s, el proceso %d", queue_name, (uint32_t) ID_proceso);
		//Solo actualizar que esta conectado (socket, flag y logger), y tomar la referencia de suscriber
		suscriber->socket = socket_cliente;
		suscriber->connected = true;
		//el logger ya estaba creado, no hace falta actualizar
	}
	else{
		/*
		log_error(logger, "Ya esta conectado a la cola %s, el proceso %d. Debe desconectar primero.", queue_name, (uint32_t) ID_proceso);
	//enviar falla en la confirmacion. (ack de error)
		send_ACK_failure(socket_cliente, logger);
		pthread_exit(NULL);
		*/

		log_warning(logger, "Ya esta conectado a la cola %s, el proceso %d. Se toma el nuevo socket del suscriptor. (El viejo queda inutilizado)", queue_name, (uint32_t) ID_proceso);
		suscriber->socket = socket_cliente;

	}


	//responder al proceso que ya esta suscrito
	send_ACK(socket_cliente, logger);

	log_info(obligatorio, "Se suscribe un proceso %d a la cola: %s", suscriber->ID_suscriber, queue_to_string(suscriber->suscribed_queue));

	send_received_message(suscriber, my_semaphores, my_queue, my_queueIDs, count);	//loop infinito

}

void send_received_message(t_suscriber* suscriber, t_semaforos* semaforos, t_list* cola, t_list* colaIDs, uint32_t* total_queue_messages){

	t_list* current_global_message_ids = list_create();
	t_list* not_sent_ids = list_create();
	void* elemento;
	uint32_t current_total_count;
	uint32_t current_count;
	t_pending* mensaje;
	void* message_data;
	uint32_t bytes;
	t_package* paquete;
	int32_t result;
	uint32_t not_sent_size;
	uint32_t id_co;

	log_debug(suscriber->log, "Empieza el envio de mensajes al proceso: %d", suscriber->ID_suscriber);

	while(1){

		list_clean(current_global_message_ids);
		//--------------Chequeo de mensajes nuevos (siempre se envian mesajes cacheados, la primera vez, y las siguientes)

		log_debug(suscriber->log, "Nuevo chequeo de envio");
		//obtener lista global de ids
		pthread_mutex_lock(&(semaforos->mutex_cola));
			list_add_all(current_global_message_ids, colaIDs);
			current_count = list_size(colaIDs);
			current_total_count = (*total_queue_messages);
		pthread_mutex_unlock(&(semaforos->mutex_cola));
		log_debug(suscriber->log, "Hay un total de %d mensajes actualmente en la cola", current_count);

		no_enviados_lista(current_global_message_ids, suscriber->sent_messages, &not_sent_ids);
		not_sent_size = list_size(not_sent_ids);
		log_debug(suscriber->log, "Hay %d mensajes pendientes a enviar", not_sent_size);

		//-----------------------------------------------------------------------------------------------------------

		log_debug(suscriber->log, "Entrando al semaforo");
		pthread_mutex_lock(&(semaforos->mutex_cola));
			while (((*total_queue_messages) == current_total_count) && not_sent_size == 0)	//si cumple la condicion, pasa de largo, y sigue ejecutando el programa. Si no cumple, se bloquea.
				pthread_cond_wait(&(semaforos->broadcast), &(semaforos->mutex_cola));
		/* do something that requires holding the mutex and condition is true */
		pthread_mutex_unlock(&(semaforos->mutex_cola));
		log_debug(suscriber->log, "Aparecieron nuevos mensajes a enviar");

		if((*total_queue_messages) < current_total_count)
			printf("ERROR imposible, nunca puede ser menor\n");

		//-----------------------------------------------------------------------------------------------

		while(!list_is_empty(not_sent_ids)){
			//tomar el primer ID de mensaje que falte enviar, y sacarlo de la lista
			elemento = list_remove(not_sent_ids, 0);
			log_debug(suscriber->log, "Se va a enviar el mensaje de ID: %d", (int)elemento);

			//obtener el mensaje con ese ID
			//Buscar el t_pending adentro de la cola, siempre se hace.
			mensaje = find_element_given_ID(elemento, cola, semaforos->mutex_cola, &bytes, &id_co, &message_data, suscriber->log);
			//buscar los datos en la cache solo en el caso de (particiones o buddy)
			if(strcmp(memory_algorithm, "PARTICIONES") == 0 || strcmp(memory_algorithm, "BS") == 0)
				message_data = find_cache_element_given_ID(elemento, &bytes, suscriber->log);

			if(mensaje != NULL && message_data != NULL){	//en modo sin memoria, siempre va a encontrar el mensaje
				paquete = broker_serialize(suscriber->suscribed_queue, (uint32_t) elemento, id_co, &message_data, bytes);

				//enviar el mensaje
				result = send_paquete(suscriber->socket, paquete);

				//si falla el envio, cambiar el flag a desconectado, y cerrar el hilo.
				if((result == -1) || (result == 0)){
					close_suscriber_thread(suscriber);
				}
				else{
					log_debug(suscriber->log, "Se envio el mensaje de ID %d al suscriptor %d. El send dio: %d (no asegura que lo haya recibido)", (uint32_t) elemento, (uint32_t) suscriber->ID_suscriber, result);
					log_info(obligatorio, "Se envio el mensaje de ID %d al suscriptor %d", (uint32_t) elemento, (uint32_t) suscriber->ID_suscriber);
				}
				//agregar el ID del mensaje como enviado en suscriber->sent_messages
				list_add(suscriber->sent_messages, elemento);

				//Agregar el ID suscriptor en el mensaje de la cola, como que ya fue enviado a este
				pthread_mutex_lock(&(semaforos->mutex_cola));
				list_add(mensaje->subs_enviados, suscriber->ID_suscriber);
				//verificar si el mensaje ya fue enviado a todos para borrar de la cola. (sigue en la cache)
				pthread_mutex_unlock(&(semaforos->mutex_cola));

				//esperar confirmacion del mensaje
				result = receive_ACK(suscriber->socket, suscriber->log);

				if((result == -1) || (result == 0)){
					close_suscriber_thread(suscriber);
				}
				else{
					log_info(logger, "El suscriptor confirma haber recibido el mensaje");
					log_info(obligatorio, "El suscriptor %d confirma haber recibido el mensaje %d", (uint32_t) suscriber->ID_suscriber, (uint32_t) elemento);
				}

				//Agregar el ID suscriptor en el mensaje de la cola, como que ya fue confirmado para este
				pthread_mutex_lock(&(semaforos->mutex_cola));
				list_add(mensaje->subs_confirmados, suscriber->ID_suscriber);
				pthread_mutex_unlock(&(semaforos->mutex_cola));

				free(message_data);
			}
			else{
				log_info(suscriber->log, "El mensaje de ID %d ya no se encuentra en la memoria", (uint32_t)elemento);
			}
		}



	}
}

void close_suscriber_thread(t_suscriber* suscriber){
	suscriber->connected = false;
	log_info(suscriber->log, "No se encuentra conectado el suscriptor %d\n", suscriber->ID_suscriber);
	close(suscriber->socket);
	suscriber->socket = 0;
	pthread_exit(NULL);
}

queue_code receive_cola(uint32_t socket, t_log* logger){

	queue_code cola;
	int32_t resultado;
	if((resultado = recv_with_retry(socket, &cola, sizeof(queue_code), MSG_WAITALL)) == (-1 || 0)){
		log_error(logger, "Error al recibir la cola a suscribirse\n");
		return -1; //failure
	}
	else
		log_debug(logger, "Se recibio la cola a suscribirse: %d\n", cola);

	//verificar que sea una cola valida

	return cola;

}

