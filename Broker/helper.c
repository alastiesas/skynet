#include "broker.h"


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
    	esperar_clientes(socket_servidor, logger, queues, suscribers, semaphores);
}

void esperar_clientes(int32_t socket_servidor, t_log* logger, t_colas* colas, t_suscriptores* suscriptores, t_semaforos* semaforos)
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
    args->semaforos = semaforos;

	pthread_create(&thread,NULL,(void*)broker_serves_client, (void *)args);		//TODO comprobar errores de pthread_create

//	pthread_detach(thread);
//	free(args);		//liberar args una vez cerrado el hilo

}


void broker_serves_client(void* input){
	int32_t socket = ((struct thread_args*)input)->socket;
	t_log*	logger = ((struct thread_args*)input)->logger;
	t_colas* colas = ((struct thread_args*)input)->colas;
	t_suscriptores*	suscriptores = ((struct thread_args*)input)->suscriptores;
	t_semaforos* semaforos = ((struct thread_args*)input)->semaforos;

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
		process_suscripcion(cod_op, socket, logger, suscriptores, semaforos);
	else
		process_mensaje(cod_op, socket, logger, colas, semaforos);


}

void process_suscripcion(op_code cod_op, int32_t socket_cliente, t_log* logger, t_suscriptores* suscriptores, t_semaforos* semaforos) {

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

	//guardar al socket_cliente en la cola de suscritos

	switch(cola){

	case COLA_NEW:
		log_info(logger, "Por suscribir al socket '%d' a la cola de NEW", socket_cliente);
		agregar_Asubs(socket_cliente, suscriptores->NEW, semaforos->mutex_subs_new, logger);
		break;

	case COLA_APPEARED:
		log_info(logger, "Por suscribir al socket '%d' a la cola de APPEARED", socket_cliente);
		agregar_Asubs(socket_cliente, suscriptores->APPEARED, semaforos->mutex_subs_appeared, logger);
		break;

	case COLA_CATCH:
		log_info(logger, "Por suscribir al socket '%d' a la cola de CATCH", socket_cliente);
		agregar_Asubs(socket_cliente, suscriptores->CATCH, semaforos->mutex_subs_catch, logger);
		break;

	case COLA_CAUGHT:
		log_info(logger, "Por suscribir al socket '%d' a la cola de CAUGHT", socket_cliente);
		agregar_Asubs(socket_cliente, suscriptores->CAUGHT, semaforos->mutex_subs_caught, logger);
		break;

	case COLA_GET:
		log_info(logger, "Por suscribir al socket '%d' a la cola de GET", socket_cliente);
		agregar_Asubs(socket_cliente, suscriptores->GET, semaforos->mutex_subs_get, logger);
		break;

	case COLA_LOCALIZED:
		log_info(logger, "Por suscribir al socket '%d' a la cola de LOCALIZED", socket_cliente);
		agregar_Asubs(socket_cliente, suscriptores->LOCALIZED, semaforos->mutex_subs_localized, logger);
		break;

	default:
		log_error(logger, "Aun no puedo suscribir a nadie en ese tipo de cola\n");
		return;
	}

	//responder al proceso que ya esta suscrito
	send_ACK(socket_cliente, logger);

	//TODO enviar mensajes en la cache del broker

}

void process_mensaje(op_code cod_op, int32_t socket_cliente, t_log* logger, t_colas* colas, t_semaforos* semaforos) {

		switch (cod_op) {

		case APPEARED:
		//	process_APPEARED(socket_cliente, logger, colas->APPEARED, semaforos);
			break;

		case GET:
		//	process_GET(socket_cliente, logger, colas->GET, semaforos);
			break;

		case LOCALIZED:
		//	process_LOCALIZED(socket_cliente, logger, colas->LOCALIZED, semaforos);
			break;

		case CATCHS:
		//	process_CATCH(socket_cliente, logger, colas->CATCH, semaforos);
			break;

		case CAUGHT:
		//	process_CAUGHT(socket_cliente, logger, colas->CAUGHT, semaforos);
			break;

		case NEW:
			process_NEW(socket_cliente, logger, colas->NEW_POKEMON, semaforos);
			break;

		default:
			log_warning(logger, "Error de numero de cod_op, finaliza el thread de conexion", cod_op);
			pthread_exit(NULL);
		}
}

/*
void process_APPEARED(int32_t socket_cliente, t_log* logger, t_queue* queue, t_semaforos* semaforos){
	uint32_t size;
	t_appeared* appeared = malloc(sizeof(t_appeared));

	appeared = receive_appeared(socket_cliente, &size, logger);

	//TODO GENERAR ID Y METERLO EN EL MENSAJE (tengo una variable global con mutex, luego de usarla se incrementa en 1)
	appeared->id = 99;

	send_ID(appeared->id, socket_cliente, logger);

	receive_ACK(socket_cliente, logger);


	agregar_Acola(queue, appeared, semaforos->mutex_cola_appeared, logger);


    int elementos = queue_size(queue);
    log_info(logger, "La cola de APPEARED tiene, %d elementos\n", elementos);
    sem_post(&(semaforos->nuevo_appeared));


	//no se hace el free de new->nombre ni free de new porque sigo usando el mensaje en la cola
}

void process_GET(int32_t socket_cliente, t_log* logger, t_queue* queue, t_semaforos* semaforos){
	uint32_t size;
	t_get* get = malloc(sizeof(t_get));

	get = receive_get(socket_cliente, &size, logger);

	//TODO GENERAR ID Y METERLO EN EL MENSAJE (tengo una variable global con mutex, luego de usarla se incrementa en 1)
	get->id = 99;

	send_ID(get->id, socket_cliente, logger);

	receive_ACK(socket_cliente, logger);


	agregar_Acola(queue, get, semaforos->mutex_cola_get, logger);


    int elementos = queue_size(queue);
    log_info(logger, "La cola de GET tiene, %d elementos\n", elementos);
    sem_post(&(semaforos->nuevo_get));


	//no se hace el free de new->nombre ni free de new porque sigo usando el mensaje en la cola
}

void process_LOCALIZED(int32_t socket_cliente, t_log* logger, t_queue* queue, t_semaforos* semaforos){
	uint32_t size;
	t_localized* localized = malloc(sizeof(t_localized));

	localized = receive_localized(socket_cliente, &size, logger);

	//TODO GENERAR ID Y METERLO EN EL MENSAJE (tengo una variable global con mutex, luego de usarla se incrementa en 1)
	localized->id = 99;

	send_ID(localized->id, socket_cliente, logger);

	receive_ACK(socket_cliente, logger);


	agregar_Acola(queue, localized, semaforos->mutex_cola_localized, logger);


    int elementos = queue_size(queue);
    log_info(logger, "La cola de LOCALIZED tiene, %d elementos\n", elementos);
    sem_post(&(semaforos->nuevo_localized));


	//no se hace el free de new->nombre ni free de new porque sigo usando el mensaje en la cola
}

void process_CAUGHT(int32_t socket_cliente, t_log* logger, t_queue* queue, t_semaforos* semaforos){
	uint32_t size;
	t_caught* caught = malloc(sizeof(t_caught));

	caught = receive_caught(socket_cliente, &size, logger);

	//TODO GENERAR ID Y METERLO EN EL MENSAJE (tengo una variable global con mutex, luego de usarla se incrementa en 1)
	caught->id = 99;

	send_ID(caught->id, socket_cliente, logger);

	receive_ACK(socket_cliente, logger);


	agregar_Acola(queue, caught, semaforos->mutex_cola_caught, logger);


    int elementos = queue_size(queue);
    log_info(logger, "La cola de CAUGHT tiene, %d elementos\n", elementos);
    sem_post(&(semaforos->nuevo_caught));


	//no se hace el free de new->nombre ni free de new porque sigo usando el mensaje en la cola
}

void process_CATCH(int32_t socket_cliente, t_log* logger, t_queue* queue, t_semaforos* semaforos){
	uint32_t size;
	t_catch* catch = malloc(sizeof(t_catch));

	catch = receive_catch(socket_cliente, &size, logger);

	//TODO GENERAR ID Y METERLO EN EL MENSAJE (tengo una variable global con mutex, luego de usarla se incrementa en 1)
	catch->id = 99;

	send_ID(catch->id, socket_cliente, logger);

	receive_ACK(socket_cliente, logger);


	agregar_Acola(queue, catch, semaforos->mutex_cola_catch, logger);


    int elementos = queue_size(queue);
    log_info(logger, "La cola de CATCH tiene, %d elementos\n", elementos);
    sem_post(&(semaforos->nuevo_catch));


	//no se hace el free de new->nombre ni free de new porque sigo usando el mensaje en la cola
}
*/
void process_NEW(int32_t socket_cliente, t_log* logger, t_queue* queue, t_semaforos* semaforos){
	uint32_t size;
	t_new* new = malloc(sizeof(t_new));

	new = receive_new(socket_cliente, &size, logger);

	//TODO GENERAR ID Y METERLO EN EL MENSAJE (tengo una variable global con mutex, luego de usarla se incrementa en 1)
	new->id = 99;

	send_ID(new->id, socket_cliente, logger);

	receive_ACK(socket_cliente, logger);


	agregar_Acola(queue, new, semaforos->mutex_cola_new, logger);


    int elementos = queue_size(queue);
    log_info(logger, "La cola de NEW tiene, %d elementos\n", elementos);
    sem_post(&(semaforos->nuevo_new));


	//no se hace el free de new->nombre ni free de new porque sigo usando el mensaje en la cola
}



void agregar_Asubs(int32_t socket, t_list* lista_subs, pthread_mutex_t mutex, t_log* logger){

	pthread_mutex_lock(&mutex);
	list_add(lista_subs, &socket);
	pthread_mutex_unlock(&mutex);
	log_info(logger, "Se agrego el socket '%d' a suscriptores\n", socket);

}

void agregar_Acola(t_queue* cola, void* t_mensaje, pthread_mutex_t mutex, t_log* logger){
	t_pending* elemento_de_la_cola = malloc(sizeof(t_pending));
	elemento_de_la_cola->subs_enviados = list_create();
	elemento_de_la_cola->subs_confirmados = list_create();
	elemento_de_la_cola->t_mensaje = t_mensaje;

	pthread_mutex_lock(&mutex);
	queue_push(cola, elemento_de_la_cola);
	pthread_mutex_unlock(&mutex);

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
