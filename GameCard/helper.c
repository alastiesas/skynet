/*
 * helper.c
 *
 *  Created on: 19 jun. 2020
 *      Author: utnso
 */
#include "gamecard.h"

void gameboy_function(void){
	iniciar_servidor_gamecard();
}

void message_function(void (*function)(void*), queue_code queue_code){
	struct thread_args* args = malloc(sizeof(struct thread_args));
	args->logger = logger;
	args->function = function;

	while(1){
		int32_t socket_cliente = suscribe_to_broker(queue_code);

		args->socket = socket_cliente;

		int32_t result = listen_messages(args);
		if(result == -2){
			log_info(logger, "Se vuelve a conectar en %d segundos", TIEMPO_DE_REINTENTO_CONEXION);
			close(socket_cliente);
			sleep(TIEMPO_DE_REINTENTO_CONEXION);
		}
		else
			log_warning(logger, "Aca nunca llego");
	}
}

void new_function(void){

	message_function(&serve_new, COLA_NEW);
}

void catch_function(void){

	message_function(&serve_catch, COLA_CATCH);
}

void get_function(void){

	message_function(&serve_get, COLA_GET);
}

int32_t suscribe_to_broker(queue_code queue_code) {

	int32_t socket = connect_to_server(IP_BROKER, PUERTO_BROKER, TIEMPO_DE_REINTENTO_CONEXION, logger);

	t_package* suscription_package = serialize_suscripcion(MY_ID, queue_code);

	send_paquete(socket, suscription_package);
	if (receive_ACK(socket, logger) == -1) {
		exit(EXIT_FAILURE);
	}

	return socket;
}

void send_to_broker(t_package* package){

	int32_t socket = connect_to_server(IP_BROKER, PUERTO_BROKER, TIEMPO_DE_REINTENTO_CONEXION, logger);
	send_paquete(socket, package);
	//TODO reintentar envio si falla con la variable global TIEMPO_DE_REINTENTO_CONEXION

	receive_ID(socket, logger);
	send_ACK(socket, logger);
	close(socket);

}

//-------------------------------------------------------------------------------

void iniciar_servidor_gamecard()
{
	pthread_t thread = pthread_self();
	int32_t socket_servidor;


    struct addrinfo hints, *servinfo, *p;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(IP_GAMECARD, PUERTO_GAMECARD, &hints, &servinfo);

    for (p=servinfo; p != NULL; p = p->ai_next)
    {
        if ((socket_servidor = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){
        	log_error(logger, "Error de socket()");
        	continue;
        }
        int reuse = 1;
        if (setsockopt(socket_servidor, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(reuse)) < 0)
            perror("setsockopt(SO_REUSEADDR) failed");

		#ifdef SO_REUSEPORT
        if (setsockopt(socket_servidor, SOL_SOCKET, SO_REUSEPORT, (const char*)&reuse, sizeof(reuse)) < 0)
            perror("setsockopt(SO_REUSEPORT) failed");
    	#endif

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
    	wait_clients(socket_servidor, logger);
}

void wait_clients(int32_t socket_servidor, t_log* logger)
{
	pthread_t self = pthread_self();
	struct sockaddr_in dir_cliente;

	uint32_t tam_direccion = sizeof(struct sockaddr_in);

	log_info(logger, "Esperando conexion en el thread %d", self);

	int32_t socket_cliente;
	if((socket_cliente = accept(socket_servidor, (void*) &dir_cliente, &tam_direccion)) == -1)
		log_error(logger, "Error al aceptar cliente");
	else{
		log_info(logger, "Conexion aceptada");
	}


    struct gamecard_thread_args* args = malloc(sizeof(struct gamecard_thread_args));
    args->socket = socket_cliente;
    args->logger = logger;

	pthread_create(&thread,NULL,(void*)gamecard_serves_client, (void *)args);		//TODO comprobar errores de pthread_create
	pthread_detach(thread);

}


void gamecard_serves_client(void* input){
	int32_t socket = ((struct gamecard_thread_args*)input)->socket;
	t_log*	logger = ((struct gamecard_thread_args*)input)->logger;

	log_info(logger, "Se creo un thread para recibir mensajes del cliente %d\n", socket);


	operation_code cod_op;

	int recibido = recv_with_retry(socket, &cod_op, sizeof(int32_t), MSG_WAITALL);
	if(recibido == -1)
		log_error(logger, "Error del recv()");
	if(recibido == 0)
		log_error(logger, "Se recibieron 0 bytes, se cierra el recv()");

	log_info(logger, "se recibieron %d bytes", recibido);

	log_info(logger, "se recibio la cod op: %d\n", cod_op);

    serve_thread_args* argus = malloc(sizeof(serve_thread_args));
    argus->op_code = cod_op;
    uint32_t* size;
    void* message;

	switch(cod_op){
	case OPERATION_NEW:
		message = receive_new(socket, size, logger);
		send_ID(((t_message_new*)message)->id, socket, logger);			//TODO ver si el tp pide enviar confirmacion del mensaje antes de enviar id
		receive_ACK(socket, logger);
	    argus->message = message;
		serve_new(argus);
		break;
	case OPERATION_CATCH:
		message = receive_catch(socket, size, logger);
		send_ID(((t_message_catch*)message)->id, socket, logger);
		receive_ACK(socket, logger);
	    argus->message = message;
		serve_catch(argus);
		break;
	case OPERATION_GET:
		message = receive_get(socket, size, logger);
		send_ID( ((t_message_get*)message)->id, socket, logger);
		receive_ACK(socket, logger);
	    argus->message = message;
		serve_get(argus);
		break;
	default:
		log_info(logger, "La internet me mando un paquete random");
		break;
	}

	free(input);		//liberar args una vez cerrado el hilo
}
