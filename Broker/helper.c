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

    getaddrinfo(IP, PORT, &hints, &servinfo);

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

void esperar_clientes(int32_t socket_servidor, t_log* logger, t_queues* colas, t_suscribers* suscriptores)
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


    struct broker_thread_args* args = malloc(sizeof(struct broker_thread_args));
    args->socket = socket_cliente;
    args->logger = logger;
    args->colas = colas;
    args->suscriptores = suscriptores;

	pthread_create(&thread,NULL,(void*)broker_serves_client, (void *)args);		//TODO comprobar errores de pthread_create

//	pthread_detach(thread);
//	free(args);		//liberar args una vez cerrado el hilo

}


void broker_serves_client(void* input){
	int32_t socket = ((struct broker_thread_args*)input)->socket;
	t_log*	logger = ((struct broker_thread_args*)input)->logger;
	t_queues* colas = ((struct broker_thread_args*)input)->colas;
	t_suscribers*	suscriptores = ((struct broker_thread_args*)input)->suscriptores;

	pthread_t self = pthread_self();
	log_info(logger, "Se creo un thread %d para atender la conexion del cliente %d\n", self, socket);

	char modulo[16];
	pthread_getname_np(self, modulo, 16);

	operation_code cod_op;

	int recibido = recv_with_retry(socket, &cod_op, sizeof(int32_t), MSG_WAITALL);
	if(recibido == -1)
		log_error(logger, "Error del recv()");
	if(recibido == 0)
		log_error(logger, "Se recibieron 0 bytes, se cierra el recv()");

	log_info(logger, "se recibieron %d bytes", recibido);

	log_info(logger, "se recibio la cod op: %d\n", cod_op);



	if(cod_op == OPERATION_SUSCRIPTION)
		process_suscripcion(cod_op, socket, logger, suscriptores);
	else
		first_process(cod_op, socket, logger, colas);

}


t_package* broker_serialize(queue_code queue_code, uint32_t id_message, uint32_t id_co, void** message, uint32_t bytes){
	t_buffer* buffer = malloc(sizeof(t_buffer));
	t_package* paquete = malloc(sizeof(t_package));
	operation_code cod_op;
	bool response;

	switch(queue_code){
	case COLA_NEW:
		cod_op = OPERATION_NEW;
		response = false;
		break;
	case COLA_APPEARED:
		cod_op = OPERATION_APPEARED;
		response = true;
		break;
	case COLA_GET:
		cod_op = OPERATION_GET;
		response = false;
		break;
	case COLA_LOCALIZED:
		cod_op = OPERATION_LOCALIZED;
		response = true;
		break;
	case COLA_CATCH:
		cod_op = OPERATION_CATCH;
		response = false;
		break;
	case COLA_CAUGHT:
		cod_op = OPERATION_CAUGHT;
		response = true;
		break;
	}

	paquete->operation_code = cod_op;
	paquete->buffer = buffer;
	paquete->buffer->size = sizeof(uint32_t) + bytes;
	if(response){
		paquete->buffer->size += sizeof(uint32_t);
	}
	log_debug(logger, "El buffer para el suscriptor es de tamanio: %d", paquete->buffer->size);
	paquete->buffer->stream = malloc(paquete->buffer->size);
	//con memcpy() lleno el stream
	int offset = 0;
	memcpy(paquete->buffer->stream + offset, &id_message, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	if(response){
		memcpy(paquete->buffer->stream + offset, &id_co, sizeof(uint32_t));
		offset += sizeof(uint32_t);
	}

	memcpy(paquete->buffer->stream + offset, *message, bytes);

	return paquete;
}




