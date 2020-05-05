/*
 * clientUtils.c
 *
 *  Created on: 24 abr. 2020
 *      Author: utnso
 */
#include "conexion.h"
#include "mensajes.h"
#define _GNU_SOURCE		//para pthread_setname_np
#include<pthread.h>

void iniciar_servidor(char* puerto, t_log* logger)
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
    	esperar_cliente(socket_servidor, logger);
}

void esperar_cliente(int32_t socket_servidor, t_log* logger)
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
	pthread_create(&thread,NULL,(void*)broker_serves_client, (void *)args);		//TODO comprobar errores de pthread_create

//	pthread_detach(thread);
//	free(args);		//liberar args una vez cerrado el hilo

}

void broker_serves_client(void* input){
	int32_t socket = ((struct thread_args*)input)->socket;
	t_log*	logger = ((struct thread_args*)input)->logger;

	pthread_t self = pthread_self();
	log_info(logger, "Se creo un thread %d para atender la conexion del cliente %d\n", self, socket);

	char modulo[16];
	pthread_getname_np(self, modulo, 16);

	//chequea si es suscripcion o mensaje, y que tipo

	//recive NEW, hace process_NEW()


}

void recibir_muchos_mensajes(void* input)
{
	int32_t socket = ((struct thread_args*)input)->socket;
	t_log*	logger = ((struct thread_args*)input)->logger;

	pthread_t self = pthread_self();
	log_info(logger, "Se creo un thread %d para atender la conexion del cliente %d\n", self, socket);

	char modulo[16];
	pthread_getname_np(self, modulo, 16);

	int vez = 1;
	while(1){
	log_info(logger, "esperando cod_op del %s, por vez numero %d\n", modulo, vez);
	op_code cod_op;

	int recibido = recv(socket, &cod_op, sizeof(int32_t), MSG_WAITALL);
	if(recibido == -1)
		log_error(logger, "Error del recv()");
	if(recibido == 0)
		log_error(logger, "Se recibieron 0 bytes, se cierra el recv()");

	log_info(logger, "se recibieron %d bytes", recibido);

	log_info(logger, "se recibio la cod op: %d\n", cod_op);
	process_request(cod_op, socket, logger);
	vez++;
	}

}

void process_request(op_code cod_op, int32_t socket_cliente, t_log* logger) {
	uint32_t size;
	void* msg;
	t_catch* catch = malloc(sizeof(t_catch));
		switch (cod_op) {
		case SALUDO:

			msg = recibir_mensaje(socket_cliente, &size, logger);
			enviar_mensaje(msg, socket_cliente, logger);
			free(msg);
			break;

		case NEW:
			process_NEW(socket_cliente, logger);
			break;

		case CATCH:

			catch = receive_catch(socket_cliente, &size, logger);
			send_catch(catch, socket_cliente, logger);
			free(catch);
			log_warning(logger, "Falta hacer el free() de catch->nombre");
			break;

		default:
			log_warning(logger, "Aun no recibio la cod_op %d, intente otro dia, finaliza el thread de conexion", cod_op);
			pthread_exit(NULL);
		}
}

void process_NEW(int32_t socket_cliente, t_log* logger){
	uint32_t size;
	t_new* new = malloc(sizeof(t_new));

	new = receive_new(socket_cliente, &size, logger);

	//TODO GENERAR ID Y METERLO EN EL MENSAJE (tengo una variable global con mutex, luego de usarla se incrementa en 1)
	new->id = 99;

	send_ID(new->id, socket_cliente, logger);

	receive_ACK(socket_cliente, logger);

		//mutex
//	queue_push(queue_NEW, new);		//falta que las librerias conozcan las colas
		//end_mutex

	//no se hace el free de new->nombre ni free de new porque sigo usando el mensaje en la cola
}

uint32_t connect_to_server(char * ip, char * puerto, t_log* logger)
{
	int32_t socket_cliente;
	char modulo[16];
	int tid = pthread_self();
	pthread_getname_np(tid, modulo, 16);
	int conexion = -2;

	while (conexion < 0){

		if (conexion == -1){
			log_info(logger, "Reintentando en %d segundos\n", TIEMPO_REINTENTO);
				sleep(TIEMPO_REINTENTO);
		}

		struct addrinfo hints;
		struct addrinfo *server_info;

		memset(&hints, 0, sizeof(hints));
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_flags = AI_PASSIVE;

		getaddrinfo(ip, puerto, &hints, &server_info);

		socket_cliente = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);
		if(socket_cliente  == -1)
			log_error(logger, "Error de socket()");

		conexion = connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen);
		if(conexion == -1)
			log_warning(logger, "Error de conexion con el %s\n", modulo);

		freeaddrinfo(server_info);
	}

	log_info(logger, "Conexion creada\n");

	return socket_cliente;
}




