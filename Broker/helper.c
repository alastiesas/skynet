#include "broker.h"
#include "listas.h"


void iniciar_servidor_broker()
{
	pthread_t thread = pthread_self();
	int32_t socket_servidor;


    struct addrinfo hints, *servinfo, *p;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(IP_BROKER, PORT, &hints, &servinfo);

    for (p=servinfo; p != NULL; p = p->ai_next)
    {
        if ((socket_servidor = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){
        	log_error(logger, "Error de socket()");
        	continue;
        }

        if (bind(socket_servidor, p->ai_addr, p->ai_addrlen) == -1) {
            close(socket_servidor);
            log_error(logger, "Error de bind (el puerto esta ocupado), reinicie el programa");
            for(;;);
            continue;
        }
        break;
    }

	listen(socket_servidor, SOMAXCONN);
	log_info(logger, "Escuchando en el socket %d, en el thread %d", socket_servidor, thread);

    freeaddrinfo(servinfo);

    while(1)
    	esperar_clientes(socket_servidor, logger, queues, suscribers);
}

void esperar_clientes(int32_t socket_servidor, t_log* logger, t_colas* colas, t_suscriptores* suscriptores)
{
	pthread_t self = pthread_self();
	struct sockaddr_in dir_cliente;

	uint32_t tam_direccion = sizeof(struct sockaddr_in);

	log_info(logger, "Esperando conexion en el thread %d", self);

	int32_t socket_cliente;
	if((socket_cliente = accept(socket_servidor, (void*) &dir_cliente, &tam_direccion)) == -1)
		log_error(logger, "Error al aceptar cliente");
	else
		log_info(logger, "Conexion aceptada");


    struct thread_args* args = malloc(sizeof(struct thread_args));
    args->socket = socket_cliente;
    args->logger = logger;
    args->colas = colas;
    args->suscriptores = suscriptores;

	pthread_create(&thread,NULL,(void*)broker_serves_client, (void *)args);		//TODO comprobar errores de pthread_create

//	pthread_detach(thread);
//	free(args);		//liberar args una vez cerrado el hilo

}


void broker_serves_client(void* input){
	int32_t socket = ((struct thread_args*)input)->socket;
	t_log*	logger = ((struct thread_args*)input)->logger;
	t_colas* colas = ((struct thread_args*)input)->colas;
	t_suscriptores*	suscriptores = ((struct thread_args*)input)->suscriptores;

	pthread_t self = pthread_self();
	log_info(logger, "Se creo un thread %d para atender la conexion del cliente %d\n", self, socket);

	char modulo[16];
	pthread_getname_np(self, modulo, 16);

	op_code cod_op;

	int recibido = recv(socket, &cod_op, sizeof(int32_t), MSG_WAITALL);
	if(recibido == -1)
		log_error(logger, "Error del recv()");
	if(recibido == 0)
		log_error(logger, "Se recibieron 0 bytes, se cierra el recv()");

	log_info(logger, "se recibieron %d bytes", recibido);

	log_info(logger, "se recibio la cod op: %d\n", cod_op);



	if(cod_op == SUSCRIPCION)
		process_suscripcion(cod_op, socket, logger, suscriptores);
	else
		process_mensaje(cod_op, socket, logger, colas);


}

void process_suscripcion(op_code cod_op, int32_t socket_cliente, t_log* logger, t_suscriptores* suscriptores) {

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

//TODO verificar si es un proceso que ya se habia suscrito antes, y verificar que se este suscribiendo a la misma cola
	//si ya estaba conectado, no se puede volver a suscribir
	//si no estaba conectado, pasar el flag a conectado
	//verificar que se suscribe a la misma cola
	//actualizar el nuevo socket

	//crear el t_suscriber
	t_suscriber* suscriber = malloc(sizeof(t_suscriber));
	suscriber->ID_suscriber = ID_proceso;
	suscriber->sent_messages = list_create();

	//asignar la cola al t_suscriber y guardar en la cola de suscritos
	t_semaforos* my_semaphores;
	t_list* my_queue;
	t_list* my_queueIDs;
	uint32_t* count;
	switch(cola){

	case COLA_NEW:
		my_semaphores = semaphores_new;
		my_queue = queues->NEW_POKEMON;
		my_queueIDs = queues->NEW_POKEMON_IDS;
		count = &total_new_messages;
		log_info(logger, "Por suscribir al socket '%d' a la cola de NEW", socket_cliente);
		agregar_Asubs(suscriber, socket_cliente, cola, suscriptores->NEW, my_semaphores->mutex_subs, logger);
		break;

	case COLA_APPEARED:
		my_semaphores = semaphores_appeared;
		my_queue = queues->APPEARED_POKEMON;
		my_queueIDs = queues->APPEARED_POKEMON_IDS;
		count = &total_appeared_messages;
		log_info(logger, "Por suscribir al socket '%d' a la cola de APPEARED", socket_cliente);
		agregar_Asubs(suscriber, socket_cliente, cola, suscriptores->APPEARED, my_semaphores->mutex_subs, logger);
		break;

	case COLA_CATCH:
		my_semaphores = semaphores_catch;
		my_queue = queues->CATCH_POKEMON;
		my_queueIDs = queues->CATCH_POKEMON_IDS;
		count = &total_catch_messages;
		log_info(logger, "Por suscribir al socket '%d' a la cola de CATCH", socket_cliente);
		agregar_Asubs(suscriber, socket_cliente, cola, suscriptores->CATCH, my_semaphores->mutex_subs, logger);
		break;

	case COLA_CAUGHT:
		my_semaphores = semaphores_caught;
		my_queue = queues->CAUGHT_POKEMON;
		my_queueIDs = queues->CAUGHT_POKEMON_IDS;
		count = &total_caught_messages;
		log_info(logger, "Por suscribir al socket '%d' a la cola de CAUGHT", socket_cliente);
		agregar_Asubs(suscriber, socket_cliente, cola, suscriptores->CAUGHT, my_semaphores->mutex_subs, logger);
		break;

	case COLA_GET:
		my_semaphores = semaphores_get;
		my_queue = queues->GET_POKEMON;
		my_queueIDs = queues->GET_POKEMON_IDS;
		count = &total_get_messages;
		log_info(logger, "Por suscribir al socket '%d' a la cola de GET", socket_cliente);
		agregar_Asubs(suscriber, socket_cliente, cola, suscriptores->GET, my_semaphores->mutex_subs, logger);
		break;

	case COLA_LOCALIZED:
		my_semaphores = semaphores_localized;
		my_queue = queues->LOCALIZED_POKEMON;
		my_queueIDs = queues->LOCALIZED_POKEMON_IDS;
		count = &total_localized_messages;
		log_info(logger, "Por suscribir al socket '%d' a la cola de LOCALIZED", socket_cliente);
		agregar_Asubs(suscriber, socket_cliente, cola, suscriptores->LOCALIZED, my_semaphores->mutex_subs, logger);
		break;

	default:
		log_error(logger, "Aun no puedo suscribir a nadie en ese tipo de cola\n");
		return;
	}

	//responder al proceso que ya esta suscrito
	send_ACK(socket_cliente, logger);

//TODO enviar mensajes en la cache del broker



	send_received_message(suscriber, my_semaphores, my_queue, my_queueIDs, *count);	//loop infinito

}

void send_received_message(t_suscriber* suscriber, t_semaforos* semaforos, t_list* cola, t_list* colaIDs, uint32_t total_queue_messages){

	//bool condicion;

	t_list* current_global_message_ids = list_create();
	t_list* not_sent_ids = list_create();
	void* elemento;
	uint32_t current_total_count;


	while(1){

		list_clean(current_global_message_ids);

		//obtener lista global de ids
		pthread_mutex_lock(&(semaforos->mutex_cola));
			list_add_all(current_global_message_ids, colaIDs);
			current_total_count = total_queue_messages;
		pthread_mutex_unlock(&(semaforos->mutex_cola));

		no_enviados_lista(current_global_message_ids, suscriber->sent_messages, not_sent_ids);

		//condicion = (!list_is_empty(not_sent_ids));


		pthread_mutex_lock(&(semaforos->mutex_cola));
			while (total_queue_messages == current_total_count)	//si cumple la condicion, pasa de largo, y sigue ejecutando el programa. Si no cumple, se bloquea.
				pthread_cond_wait(&(semaforos->broadcast), &(semaforos->mutex_cola));
		/* do something that requires holding the mutex and condition is true */
		pthread_mutex_unlock(&(semaforos->mutex_cola));

		if(total_queue_messages < current_total_count)
			printf("ERROR imposible, nunca puede ser menor\n");

		while(!list_is_empty(not_sent_ids)){
			//tomar el primer ID de mensaje que falte enviar, y sacarlo de la lista
			elemento = list_remove(not_sent_ids, 0);

			//obtener el mensaje con ese ID

			//enviar el mensaje

			//si falla el envio, cambiar el flag a desconectado, y cerrar el hilo.

			//agregar el ID del mensaje como enviado en suscriber->sent_messages

			//Agregar el ID suscriptor en el mensaje de la cola, como que ya fue enviado a este

			//esperar confirmacion del mensaje
			//Agregar el ID suscriptor en el mensaje de la cola, como que ya fue confirmado para este
		}

	}
}


void agregar_Asubs(t_suscriber* suscriber, int32_t socket, queue_code cola, t_list* lista_subs, pthread_mutex_t mutex, t_log* logger){

	suscriber->suscribed_queue = cola;
	suscriber->connected = true;
	suscriber->socket = socket;

	pthread_mutex_lock(&mutex);
		list_add(lista_subs, suscriber);
		pthread_mutex_unlock(&mutex);
	log_info(logger, "Se agrego el socket '%d' a suscriptores\n", socket);

}

void agregar_Acola(t_list* cola, t_list* colaIds, t_pending* t_mensaje, pthread_mutex_t mutex, t_log* logger, pthread_cond_t cond, uint32_t total_queue_messages){

    pthread_mutex_lock(&(mutex));
    /* do something that might make condition true */
    	list_add(cola, t_mensaje);
    	list_add(colaIds, t_mensaje->ID_mensaje);
    	total_queue_messages++;		//solo se suma, no se resta al eliminar mensajes.
    	pthread_cond_broadcast(&(cond));
    pthread_mutex_unlock(&(mutex));


}

void process_mensaje(op_code cod_op, int32_t socket_cliente, t_log* logger, t_colas* colas) {

	t_semaforos* my_semaphores;
	uint32_t* count;

		switch (cod_op) {

		case NEW:
			my_semaphores = semaphores_new;
			count = &total_new_messages;
			process_NEW(socket_cliente, logger, colas->NEW_POKEMON, colas->NEW_POKEMON_IDS, my_semaphores, *count);
			log_debug(logger, "Se notifico el mensaje new recibido");

			break;

		case APPEARED:
			my_semaphores = semaphores_appeared;
			count = &total_appeared_messages;
			process_APPEARED(socket_cliente, logger, colas->APPEARED_POKEMON, colas->APPEARED_POKEMON_IDS, my_semaphores, *count);

			break;

		case GET:
			my_semaphores = semaphores_get;
			count = &total_get_messages;
			process_GET(socket_cliente, logger, colas->GET_POKEMON, colas->GET_POKEMON_IDS, my_semaphores, *count);

			break;

		case LOCALIZED:
			my_semaphores = semaphores_localized;
			count = &total_localized_messages;
			process_LOCALIZED(socket_cliente, logger, colas->LOCALIZED_POKEMON, colas->LOCALIZED_POKEMON_IDS, my_semaphores, *count);

			break;

		case CATCHS:
			my_semaphores = semaphores_catch;
			count = &total_catch_messages;
			process_CATCH(socket_cliente, logger, colas->CATCH_POKEMON, colas->CATCH_POKEMON_IDS, my_semaphores, *count);

			break;

		case CAUGHT:
			my_semaphores = semaphores_caught;
			count = &total_caught_messages;
			process_CAUGHT(socket_cliente, logger, colas->CAUGHT_POKEMON, colas->CAUGHT_POKEMON_IDS, my_semaphores, *count);

			break;

		default:
			log_warning(logger, "Error de numero de cod_op, finaliza el thread de conexion", cod_op);
			pthread_exit(NULL);
		}
}

void process_NEW(int32_t socket_cliente, t_log* logger, t_list* queue, t_list* queueIds, t_semaforos* semaforos, uint32_t total_queue_messages){
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
	agregar_Acola(queue, queueIds, t_mensaje, semaforos->mutex_cola, logger, semaforos->broadcast, total_queue_messages);


    int elementos = list_size(queue);
    log_info(logger, "La cola de NEW tiene, %d elementos\n", elementos);
    sem_post(&(semaforos->nuevo_mensaje));


	//no se hace el free de new->nombre ni free de new porque sigo usando el mensaje en la cola
}


void process_APPEARED(int32_t socket_cliente, t_log* logger, t_list* queue, t_list* queueIds, t_semaforos* semaforos, uint32_t total_queue_messages){
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
	agregar_Acola(queue, queueIds, t_mensaje, semaforos->mutex_cola, logger, semaforos->broadcast, total_queue_messages);


    int elementos = list_size(queue);
    log_info(logger, "La cola de APPEARED tiene, %d elementos\n", elementos);
  //  sem_post(&(semaforos->nuevo_appeared));


	//no se hace el free de new->nombre ni free de new porque sigo usando el mensaje en la cola
}

void process_GET(int32_t socket_cliente, t_log* logger, t_list* queue, t_list* queueIds, t_semaforos* semaforos, uint32_t total_queue_messages){
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
	agregar_Acola(queue, queueIds, t_mensaje, semaforos->mutex_cola, logger, semaforos->broadcast, total_queue_messages);


    int elementos = list_size(queue);
    log_info(logger, "La cola de GET tiene, %d elementos\n", elementos);
 //   sem_post(&(semaforos->nuevo_get));


	//no se hace el free de new->nombre ni free de new porque sigo usando el mensaje en la cola
}

void process_LOCALIZED(int32_t socket_cliente, t_log* logger, t_list* queue, t_list* queueIds, t_semaforos* semaforos, uint32_t total_queue_messages){
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
	agregar_Acola(queue, queueIds, t_mensaje, semaforos->mutex_cola, logger, semaforos->broadcast, total_queue_messages);


    int elementos = list_size(queue);
    log_info(logger, "La cola de LOCALIZED tiene, %d elementos\n", elementos);
   // sem_post(&(semaforos->nuevo_localized));


	//no se hace el free de new->nombre ni free de new porque sigo usando el mensaje en la cola
}

void process_CATCH(int32_t socket_cliente, t_log* logger, t_list* queue, t_list* queueIds, t_semaforos* semaforos, uint32_t total_queue_messages){
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
	agregar_Acola(queue, queueIds, t_mensaje, semaforos->mutex_cola, logger, semaforos->broadcast, total_queue_messages);


    int elementos = list_size(queue);
    log_info(logger, "La cola de CATCH tiene, %d elementos\n", elementos);
  //  sem_post(&(semaforos->nuevo_catch));


	//no se hace el free de catch->nombre ni free de catch porque sigo usando el mensaje en la cola
}

void process_CAUGHT(int32_t socket_cliente, t_log* logger, t_list* queue, t_list* queueIds, t_semaforos* semaforos, uint32_t total_queue_messages){
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
	agregar_Acola(queue, queueIds, t_mensaje, semaforos->mutex_cola, logger, semaforos->broadcast, total_queue_messages);


    int elementos = list_size(queue);
    log_info(logger, "La cola de CAUGHT tiene, %d elementos\n", elementos);
  //  sem_post(&(semaforos->nuevo_caught));


	//no se hace el free de new->nombre ni free de new porque sigo usando el mensaje en la cola
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
	int32_t bytes_received = recv(socket_cliente, &(t_mensaje->datos_mensaje), size_datos, MSG_WAITALL);
	if(bytes_received == -1)
		log_error(logger, "Error al recibir los datos del mensaje");
	else
		log_info(logger, "Datos del mensaje recibidos. (%d bytes de un total de %d)", bytes_received, size_datos);



	if(*size != size_ID + size_datos)
		log_error(logger, "Tamanio erroneo");

	return t_mensaje;
}



queue_code receive_cola(uint32_t socket, t_log* logger){


	queue_code cola;
	int32_t resultado;
	if((resultado = recv(socket, &cola, sizeof(queue_code), MSG_WAITALL)) == -1){
		log_error(logger, "Error al recibir la cola a suscribirse\n");
		return -1; //failure
	}
	else
		log_info(logger, "Se recibio la cola a suscribirse: %d\n", cola);

	//verificar que sea una cola valida

	return cola;

}
