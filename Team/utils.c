/*
 * utils.c
 *
 *  Created on: 19 abr. 2020
 *      Author: utnso
 */


#include "utils.h"

//TODO
/*
 * Recibe un paquete a serializar, y un puntero a un int en el que dejar
 * el tamaño del stream de bytes serializados que devuelve
 */
void* serializar_paquete(t_paquete* paquete, int *bytes)
{
	printf("the bytes are %d ", *bytes);
	void * magic = malloc(*bytes);
	int desplazamiento = 0;

	memcpy(magic + desplazamiento, &(paquete->codigo_operacion), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(magic + desplazamiento, &(paquete->buffer->size), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(magic + desplazamiento, paquete->buffer->stream, paquete->buffer->size);
	desplazamiento+= paquete->buffer->size;

	return magic;
}



int crear_conexion(char *ip, char* puerto)
{
	printf("aca llego bien crack");
	struct addrinfo hints;
	struct addrinfo *server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, puerto, &hints, &server_info);

	int socket_cliente = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);

	if(connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen) == -1)
		printf("error");

	freeaddrinfo(server_info);
	printf("the socket is %d \n",socket_cliente);
	return socket_cliente;
}

//TODO
void enviar_mensaje(char* mensaje, int socket_cliente)
{
	printf("the message is %s \n", mensaje);
	t_buffer* buffer = malloc(sizeof(t_buffer));
	(*buffer).size = strlen(mensaje) + 1;
	printf("el size en el buffer es %d \n", (*buffer).size);
	void* stream = malloc((*buffer).size);

	memcpy(stream, mensaje, (*buffer).size);

	(*buffer).stream = stream;

	t_paquete* paquete = malloc(sizeof(t_paquete));

	paquete->codigo_operacion = MENSAJE;
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer = buffer;
	printf("aca no llega \n");
	printf("tamaño de byte en line 75 %d \n", paquete->buffer->size);
	int bytes = paquete->buffer->size + 2*sizeof(int);
	printf("tamaño de byte en line 76 %d \n", bytes);
	void* a_enviar = serializar_paquete(paquete, &bytes);

	send(socket_cliente, a_enviar, bytes, 0);
	printf("aca no llega2? \n");
	free(a_enviar);
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}

//TODO
char* recibir_mensaje(int socket_cliente)
{
	printf("acano llego recibir mensaje \n");
	t_paquete* paquete = malloc(sizeof(paquete));
	t_buffer* buffer = malloc(sizeof(buffer));
	recv(socket_cliente, &(paquete->codigo_operacion), sizeof(int), 0);

	recv(socket_cliente, &(paquete->buffer->size), sizeof(int), 0);
	recv(socket_cliente, &(paquete->buffer->stream), paquete->buffer->size, 0);
	printf("aca tasdasdw221 \n");
	char* message = malloc((int)(paquete->buffer->size));
	int bytes;
	//buffer = paquete->buffer;
	void* stream = paquete->buffer->stream;
	// Deserializamos los campos que tenemos en el buffer
	memcpy(&bytes,&(paquete->buffer->size) , sizeof(int));

	stream += sizeof(int);
	printf("test %d \n", (paquete->buffer->size));
	printf("aca tasdasdw size %d \n", bytes);
	memcpy(message, &(paquete->buffer->stream), bytes);
	printf("the message was retorned %s \n", message);
	free(paquete);


		 // Evaluamos los demás casos según corresponda

	return message;
}

void liberar_conexion(int socket_cliente)
{
	close(socket_cliente);
}

//TODO
t_log* iniciar_logger(void)
{	char file[] = "tp0.log";
	char program_name[] = "lasti tp0";

	return log_create(file, program_name, true, LOG_LEVEL_INFO);
}

//TODO
t_config* leer_config(void)
{
	return config_create("tp0.config");

}

//TODO
void terminar_programa(int conexion, t_log* logger, t_config* config)
{
	log_destroy(logger);
	config_destroy(config);
	liberar_conexion(conexion);
	//Y por ultimo, para cerrar, hay que liberar lo que utilizamos (conexion, log y config) con las funciones de las commons y del TP mencionadas en el enunciado
}
