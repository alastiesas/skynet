#include "broker.h"


void iniciar_servidor_broker(char* puerto, t_log* logger, t_colas* colas, t_suscriptores* suscriptores, t_semaforos* semaforos)
{
	pthread_t thread = pthread_self();
	int32_t socket_servidor;


    struct addrinfo hints, *servinfo, *p;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(IP, puerto, &hints, &servinfo);

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
    	esperar_clientes(socket_servidor, logger, colas, suscriptores, semaforos);
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
	//falta recibir la cola a suscribirse
	queue_code cola;
	cola = receive_cola(socket_cliente, logger);

	//guardar al socket_cliente en la cola de suscritos

	switch(cola){
	case COLA_NEW:
		agregar_Asubs(socket_cliente, suscriptores->NEW, semaforos->mutex_subs_new, logger);
		break;
	default:
		log_error(logger, "Aun no recibo esa cola\n");
		return;
	}

	//responder al proceso que ya esta suscrito
	send_ACK(socket_cliente, logger);

	//TODO enviar mensajes en la cache del broker

}

void process_mensaje(op_code cod_op, int32_t socket_cliente, t_log* logger, t_colas* colas, t_semaforos* semaforos) {
	//uint32_t size;

		switch (cod_op) {

		case NEW:
			process_NEW(socket_cliente, logger, colas->NEW, semaforos);
			break;

		default:
			log_warning(logger, "Error de numero de cod_op, finaliza el thread de conexion", cod_op);
			pthread_exit(NULL);
		}
}


void process_NEW(int32_t socket_cliente, t_log* logger, t_queue* queue_NEW, t_semaforos* semaforos){
	uint32_t size;
	t_new* new = malloc(sizeof(t_new));

	new = receive_new(socket_cliente, &size, logger);

	//TODO GENERAR ID Y METERLO EN EL MENSAJE (tengo una variable global con mutex, luego de usarla se incrementa en 1)
	new->id = 99;

	send_ID(new->id, socket_cliente, logger);

	receive_ACK(socket_cliente, logger);


	agregar_Acola(queue_NEW, new, semaforos->mutex_cola_new, logger);


    int elementos = queue_size(queue_NEW);
    log_info(logger, "La cola de NEW tiene, %d elementos\n", elementos);
    sem_post(&(semaforos->nuevo_new));


	//no se hace el free de new->nombre ni free de new porque sigo usando el mensaje en la cola
}

void agregar_Asubs(int32_t socket, t_list* lista_subs, pthread_mutex_t mutex, t_log* logger){
	pthread_mutex_lock(&mutex);
	list_add(lista_subs, &socket);
	pthread_mutex_unlock(&mutex);
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

