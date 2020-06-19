/*
 * helper.c
 *
 *  Created on: 19 jun. 2020
 *      Author: utnso
 */
#include "gamecard.h"

void gameboy_function(void){
	//TODO crear servidor para atender mensajes de gameboy
}

void new_function(void){

	int32_t socket_cliente = suscribe_to_broker(COLA_NEW);


	struct thread_args* args = malloc(sizeof(struct thread_args));
	args->socket = socket_cliente;
	args->logger = logger;
	args->function = &serve_new;
	args->failure_function = &new_function;
	args->retry_time = TIEMPO_DE_REINTENTO_CONEXION;

	listen_messages(args);

	log_warning(logger, "Aca nunca llego");
}

void catch_function(void){

	int32_t socket_cliente = suscribe_to_broker(COLA_CATCH);

	struct thread_args* args = malloc(sizeof(struct thread_args));
	args->socket = socket_cliente;
	args->logger = logger;
	args->function = &serve_catch;
	args->failure_function = &catch_function;
	args->retry_time = TIEMPO_DE_REINTENTO_CONEXION;

	listen_messages(args);

	log_warning(logger, "Aca nunca llego");
}

void get_function(void){

	int32_t socket_cliente = suscribe_to_broker(COLA_GET);


	struct thread_args* args = malloc(sizeof(struct thread_args));
	args->socket = socket_cliente;
	args->logger = logger;
	args->function = &serve_get;
	args->failure_function = &get_function;
	args->retry_time = TIEMPO_DE_REINTENTO_CONEXION;

	listen_messages(args);

	log_warning(logger, "Aca nunca llego");
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

}
