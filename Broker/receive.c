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
    	list_add(colaIds, (void*)(t_mensaje->ID_mensaje));
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
			process_receive_message(socket_cliente, logger, colas->NEW_POKEMON, colas->NEW_POKEMON_IDS, my_semaphores, count, false, COLA_NEW);
			log_debug(logger, "Se notifico el mensaje new recibido");

			break;

		case OPERATION_APPEARED:
			my_semaphores = semaphores_appeared;
			count = &total_appeared_messages;
			process_receive_message(socket_cliente, logger, colas->APPEARED_POKEMON, colas->APPEARED_POKEMON_IDS, my_semaphores, count, true, COLA_APPEARED);

			break;

		case OPERATION_GET:
			my_semaphores = semaphores_get;
			count = &total_get_messages;
			process_receive_message(socket_cliente, logger, colas->GET_POKEMON, colas->GET_POKEMON_IDS, my_semaphores, count, false, COLA_GET);

			break;

		case OPERATION_LOCALIZED:
			my_semaphores = semaphores_localized;
			count = &total_localized_messages;
			process_receive_message(socket_cliente, logger, colas->LOCALIZED_POKEMON, colas->LOCALIZED_POKEMON_IDS, my_semaphores, count, true, COLA_LOCALIZED);

			break;

		case OPERATION_CATCH:
			my_semaphores = semaphores_catch;
			count = &total_catch_messages;
			process_receive_message(socket_cliente, logger, colas->CATCH_POKEMON, colas->CATCH_POKEMON_IDS, my_semaphores, count, false, COLA_CATCH);

			break;

		case OPERATION_CAUGHT:
			my_semaphores = semaphores_caught;
			count = &total_caught_messages;
			process_receive_message(socket_cliente, logger, colas->CAUGHT_POKEMON, colas->CAUGHT_POKEMON_IDS, my_semaphores, count, true, COLA_CAUGHT);

			break;

		default:
			log_warning(logger, "Error de numero de cod_op, finaliza el thread de conexion", cod_op);
			close(socket_cliente);
			pthread_exit(NULL);
		}
}

t_pending* broker_receive_mensaje(uint32_t socket_cliente, uint32_t* size, bool response, t_log* logger){

	t_pending* t_mensaje = malloc(sizeof(t_pending));
	t_mensaje->subs_confirmados = list_create();
	t_mensaje->subs_enviados = list_create();

	log_info(logger, "Esperando recibir tamanio del stream\n");

	if(recv_with_retry(socket_cliente, size, sizeof(uint32_t), MSG_WAITALL) < sizeof(uint32_t))
		log_error(logger, "Error al recibir el tamanio del stream");
	else
		log_info(logger, "Se solicito recibir un tamanio de stream de: %d\n", *size);

	//recibir id de new. (El cual va a ignorar, porque setea el suyo propio luego)
	if(recv_with_retry(socket_cliente, &(t_mensaje->ID_mensaje), sizeof(t_mensaje->ID_mensaje), MSG_WAITALL) < sizeof(t_mensaje->ID_mensaje))
		log_error(logger, "Error al recibir el id del mensaje");
	else
		log_info(logger, "id del mensaje recibido: %d (se va a sobreescribir)", t_mensaje->ID_mensaje);

	uint32_t size_co = 0;
	//recibir id correlativo, si aplica
	if(response){
		if(recv_with_retry(socket_cliente, &(t_mensaje->ID_correlativo), sizeof(t_mensaje->ID_correlativo), MSG_WAITALL) < sizeof(t_mensaje->ID_correlativo))
			log_error(logger, "Error al recibir el id correlativo del mensaje");
		else
			log_info(logger, "id correlativo del mensaje recibido: %d (no se toca)", t_mensaje->ID_correlativo);
		size_co = sizeof(uint32_t);
	}
	else	//si el mensaje no tiene correlativo, lo pongo en 0
		t_mensaje->ID_correlativo = 0;


	uint32_t size_ID = sizeof(uint32_t);
	uint32_t size_datos = *size - size_ID - size_co;
	t_mensaje->datos_mensaje = malloc(size_datos);

	//recibir t0do el resto de datos del mensaje
	int32_t bytes_received = recv_with_retry(socket_cliente, t_mensaje->datos_mensaje, size_datos, MSG_WAITALL);
	if(bytes_received < size_datos)
		log_error(logger, "Error al recibir los datos del mensaje");
	else
		log_info(logger, "Datos del mensaje recibidos. (%d bytes de un total de %d)", bytes_received, size_datos);

	t_mensaje->bytes = size_datos;

	log_debug(logger, "Se guardo un mensaje de %d bytes (+4 bytes ID)", t_mensaje->bytes);

	if(*size != size_ID + size_co + size_datos)
		log_error(logger, "Tamanio erroneo");

	return t_mensaje;
}

void process_receive_message(int32_t socket_cliente, t_log* logger, t_list* queue, t_list* queueIds, t_semaforos* semaforos, uint32_t* total_queue_messages, bool response, queue_code queue_code){
	uint32_t size;
	t_pending* t_mensaje;

	t_mensaje = broker_receive_mensaje(socket_cliente, &size, response, logger);

	//Generar ID del mensaje
	pthread_mutex_lock(&mutex_ID_global);
		t_mensaje->ID_mensaje = ID_GLOBAL;
		ID_GLOBAL++;
	pthread_mutex_unlock(&mutex_ID_global);


	//Enviar ID del mensaje
	send_ID(t_mensaje->ID_mensaje, socket_cliente, logger);

	//Recibir confirmacion de haber recibido la ID
	receive_ACK(socket_cliente, logger);
	close(socket_cliente);

//------------------------------------------------------------------------------------------------------
	//En el caso de trabajar con memoria, agregar el mensaje a memoria

	if(strcmp(memory_algorithm, "PARTICIONES") == 0)
		store_message_partition(t_mensaje->ID_mensaje, t_mensaje->bytes, t_mensaje->datos_mensaje, queue_code);
	else if(strcmp(memory_algorithm, "BS") == 0)
		store_message_buddy(t_mensaje->ID_mensaje, t_mensaje->bytes, t_mensaje->datos_mensaje, queue_code);

//------------------------------------------------------------------------------------------------------

	//La cola la tomamos como estructura administrativa, hay que actualizarla luego de que el mensaje ya este en la memoria
	//Se puede usar un mutex separado para cache y cola, igual el hilo de envio no se va a enterar de mensajes nuevos hasta que aparezca en la cola
	//Agregar mensaje a cola correspondiente
	agregar_Acola(queue, queueIds, t_mensaje, semaforos->mutex_cola, logger, semaforos, total_queue_messages);

	log_info(obligatorio, "Se agrega un mensaje a la cola %s", queue_to_string(queue_code));
	pthread_exit(NULL);
}

void store_message_partition(uint32_t message_id, uint32_t size_message, void* message_data, queue_code queue_code){

	t_partition* new_partition = malloc(sizeof(t_partition));
	new_partition->ID_message = message_id;
	new_partition->available = false;
	new_partition->size = size_message;
	new_partition->queue_code = queue_code;
	new_partition->lru = 0; //para que no quede sin inicializar, pero en update lru se setea
	t_list* deleted_messages = list_create();

	pthread_mutex_lock(&(mutex_cache));
	//buscar la posicion para la particion nueva (eliminar mensajes si corresponde)
	uint32_t free_partition_index;
	if(size_message < min_partition_size)
		free_partition_index = find_available_dynamic_partition(min_partition_size, &deleted_messages);
	else
		free_partition_index = find_available_dynamic_partition(size_message, &deleted_messages);
	if(free_partition_index != -1){

		//actualizar la posicion de la particion nueva, y agregar a la lista
		t_partition* free_partition = (t_partition*)list_get(partitions, free_partition_index);
		new_partition->initial_position = free_partition->initial_position;

		if(size_message < min_partition_size){
			new_partition->final_position = new_partition->initial_position + min_partition_size;
			free_partition->size = free_partition->size - min_partition_size;
		}else{
			new_partition->final_position = new_partition->initial_position + new_partition->size;
			free_partition->size = free_partition->size - new_partition->size;
		}

		free_partition->initial_position = new_partition->final_position;

		//TODO actualizar lru's antes de agregar a la lista
		list_add_in_index(partitions, free_partition_index, new_partition);

		//guardar el message_data en la particion
		memmove(new_partition->initial_position, message_data, size_message);

		update_LRU(new_partition);
		log_info(obligatorio, "Se guarda un mensaje en la posicion %d a %d", new_partition->initial_position - mem, new_partition->final_position - mem);
	}
	else{
		log_error(logger, "No hay particion disponible");
		free(new_partition);
	}

	pthread_mutex_unlock(&(mutex_cache));

	//eliminar de la cola los mensajes que se eliminaron de la memoria
	//TODO ver si conviene meter este mutex adentro del mutex anterior para que no haya inconsistencias
	delete_messages_from_queue(deleted_messages);

	free(message_data); //nadie va a volver a usar los datos en cola en modo con memoria
}

void delete_messages_from_queue(t_list* deleted_messages){
	uint32_t id_mensaje;
	queue_code queue_id;
	t_pending* t_mensaje;
	pthread_mutex_t mutex_cola;
	t_list* queue;
	t_list* queueIds;

		while(!list_is_empty(deleted_messages)){
			id_mensaje = (uint32_t)list_remove(deleted_messages, 0);
			queue_id = (queue_code)list_remove(deleted_messages, 0);
			mutex_cola = get_mutex_and_queues_by_id(queue_id, &queue, &queueIds);
			pthread_mutex_lock(&(mutex_cola));
				t_mensaje = remove_element_given_ID_short(id_mensaje, queue);
				list_destroy(t_mensaje->subs_enviados);
				list_destroy(t_mensaje->subs_confirmados);
				//if(t_mensaje->datos_mensaje != NULL)	//TODO checkear que no cause memory LIK
					//free(t_mensaje->datos_mensaje);
				free(t_mensaje);
				remove_ID_short(id_mensaje, queueIds);
			pthread_mutex_unlock(&(mutex_cola));
		}

	list_destroy(deleted_messages);
}

pthread_mutex_t get_mutex_and_queues_by_id(queue_code queue_id, t_list** queue, t_list** queueIDS){
	switch(queue_id){
	case COLA_NEW:
		*queue = queues->NEW_POKEMON;
		*queueIDS = queues->NEW_POKEMON_IDS;
		return semaphores_new->mutex_cola;
		break;
	case COLA_APPEARED:
		*queue = queues->APPEARED_POKEMON;
		*queueIDS = queues->APPEARED_POKEMON_IDS;
		return semaphores_appeared->mutex_cola;
		break;
	case COLA_GET:
		*queue = queues->GET_POKEMON;
		*queueIDS = queues->GET_POKEMON_IDS;
		return semaphores_get->mutex_cola;
		break;
	case COLA_LOCALIZED:
		*queue = queues->LOCALIZED_POKEMON;
		*queueIDS = queues->LOCALIZED_POKEMON_IDS;
		return semaphores_localized->mutex_cola;
		break;
	case COLA_CATCH:
		*queue = queues->CATCH_POKEMON;
		*queueIDS = queues->CATCH_POKEMON_IDS;
		return semaphores_catch->mutex_cola;
		break;
	case COLA_CAUGHT:
		*queue = queues->CAUGHT_POKEMON;
		*queueIDS = queues->CAUGHT_POKEMON_IDS;
		return semaphores_caught->mutex_cola;
		break;
	default:
		log_error(logger, "Error impossible en get_mutex_and_queues_by_id()");
		return semaphores_new->mutex_cola; //???
		break;
	}
}

void store_message_buddy(uint32_t message_id, uint32_t size_message, void* message_data, queue_code queue_code){

	//Primero crear la t_partition nueva, no se necesita mutex hasta tocar la lista de particiones
	//t0do el resto necesita el mismo mutex

pthread_mutex_lock(&(mutex_cache));

	//buscar la posicion para la particion nueva (eliminar mensajes si corresponde)
	//actualizar la posicion de la particion nueva y del resto si corresponde
	//guardar el message_data en la particion



pthread_mutex_unlock(&(mutex_cache));
	free(message_data); //nadie va a volver a usar los datos en cola en modo con memoria

	//TODO indicar por log obligatorio cuando se realiza asociacion de bloques
}
