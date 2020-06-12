/*
 * clientUtils.c
 *
 *  Created on: 24 abr. 2020
 *      Author: utnso
 */
#include "conexion.h"
#define _GNU_SOURCE		//para pthread_setname_np

/*
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
*/


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
	pthread_create(&thread,NULL,(void*)listen_messages, (void *)args);		//TODO comprobar errores de pthread_create

//	pthread_detach(thread);
//	free(args);		//liberar args una vez cerrado el hilo

}



void listen_messages(void* input)
{
	int32_t socket = ((struct thread_args*)input)->socket;
	t_log*	logger = ((struct thread_args*)input)->logger;
	void (*function)(operation_code, void*) = ((struct thread_args*)input)->function;


	pthread_t self = pthread_self();
	//log_info(logger, "Se creo un thread %d para atender la conexion del %d\n", self, socket);

	char modulo[16];
	pthread_getname_np(self, modulo, 16);			//preguntar que hace esto XD

	int vez = 1;
	while(1){
		log_info(logger, "\nesperando recibir cod_op, por vez numero: %d\n", vez);
		operation_code cod_op;
				//Quedarse trabado en recv() hasta recibir un mensaje, y hacer lo que corresponda cuando llegue
		int recibido = recv(socket, &cod_op, sizeof(int32_t), MSG_WAITALL);
		printf("recv = %d", recibido);
		if(recibido == -1)
			log_error(logger, "Error del recv()");
		else if(recibido == 0){
			log_error(logger, "Se recibieron 0 bytes.\n Llegue a bloquearme en el recv() pero luego se cerro la conexion.\n TODO reintentar conexion");
			exit(EXIT_FAILURE);
		}
		else {

			log_info(logger, "se recibio el cod op: %d\n", cod_op);
			void* message = process_request(cod_op, socket, logger);
			function(cod_op, message);//se llama a la función otorgada al crearse el thread
			//ejemplo, en team, esta función identificaria si un catch fue exitoso para agregar el pokemon al inventario del entrenador
		}
		vez++;
	}

}

void* process_request(operation_code cod_op, int32_t socket, t_log* logger) {
	uint32_t size;
	void* msg;
	//t_message_catch* catch = malloc(sizeof(t_message_catch));//esto se borraria
	//t_message_new* new = malloc(sizeof(t_message_new));//esto se borraria
	void* message = NULL;//acá se guardaría el mensaje (void*) independientemente de su tipo.

		switch (cod_op) {
		case PRUEBA:

			message = recibir_mensaje(socket, &size, logger);

			log_info(logger, "Se recibio el mensaje: %s", (char*) message);
			break;

		case OPERATION_NEW:
			log_info(logger, "Se recibe el mensaje:\n");
			//new = receive_new(socket, &size, logger);
			message = (void*)receive_new(socket, &size, logger);			//se guarda en mensaje(void*)

			//free(new->pokemon_name);
			//free(new->location->position);
			//free(new->location);
			break;

		case OPERATION_APPEARED:
			log_info(logger, "Se recibe el mensaje:\n");
			//catch = receive_catch(socket, &size, logger);
			message = (void*)receive_appeared(socket, &size, logger);			//se guarda en mensaje(void*)

			//free(catch->pokemon_name);
			//free(catch->position);
			break;

		case OPERATION_GET:
					log_info(logger, "Se recibe el mensaje:\n");
					//catch = receive_catch(socket, &size, logger);
					message = (void*)receive_get(socket, &size, logger);			//se guarda en mensaje(void*)

					//free(catch->pokemon_name);
					//free(catch->position);
					break;

		case OPERATION_LOCALIZED:
					log_info(logger, "Se recibe el mensaje:\n");
					//catch = receive_catch(socket, &size, logger);
					message = (void*)receive_localized(socket, &size, logger);			//se guarda en mensaje(void*)

					//free(catch->pokemon_name);
					//free(catch->position);
					break;

		case OPERATION_CATCH:
					log_info(logger, "Se recibe el mensaje:\n");
					//catch = receive_catch(socket, &size, logger);
					message = (void*)receive_catch(socket, &size, logger);			//se guarda en mensaje(void*)

					//free(catch->pokemon_name);
					//free(catch->position);
					break;

		case OPERATION_CAUGHT:
					log_info(logger, "Se recibe el mensaje:\n");
					//catch = receive_catch(socket, &size, logger);
					message = (void*)receive_caught(socket, &size, logger);			//se guarda en mensaje(void*)

					//free(catch->pokemon_name);
					//free(catch->position);
					break;
		default:
			log_warning(logger, "Aun no recibio la cod_op %d, intente otro dia, finaliza el thread de conexion", cod_op);
			pthread_exit(NULL);
		}
		send_ACK(socket, logger);
		log_info(logger, "Se envio la confirmacion de la recepcion del mensaje");
		//free(msg);//tambien se borraria? si, no se usa
		//free(catch);
		//free(new);
		return message;

}



int32_t connect_to_server(char * ip, char * puerto, t_log* logger)
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




