/*
 * serverUtils.c
 *
 *  Created on: 24 abr. 2020
 *      Author: utnso
 */
#include "mensajes.h"
#define _GNU_SOURCE		//para pthread_setname_np
#include<pthread.h>




void* recibir_mensaje(uint32_t socket_cliente, uint32_t* size, t_log* logger)
{
	void * buffer;

	log_debug(logger, "Esperando recibir tamanio del mensaje\n");

	if(recv(socket_cliente, size, sizeof(int), MSG_WAITALL) == -1)
		log_error(logger, "Error al recibir el tamanio del mensaje");
	else
		log_debug(logger, "Se solicito recibir un tamanio de mensaje de: %d\n", *size);

	buffer = malloc(*size);

	if(recv(socket_cliente, buffer, *size, MSG_WAITALL) == -1)
		log_error(logger, "Error al recibir el contenido del mensaje");
	else
		log_info(logger, "Mensaje recibido: %s\n", (char*) buffer);

	return buffer;
}




void enviar_mensaje(char* mensaje, uint32_t socket, t_log* logger)
{
	//Quiero mandar el stream de datos	|cod_op|size|mensaje|
	//Para mandar cualquier cosa, primero se crea el paquete, y luego uso la funcion send_paquete()

	t_buffer* ptr_buffer = malloc(sizeof(t_buffer));
	t_paquete* paquete = malloc(sizeof(t_paquete));

	//meto la cod_op en el paquete
	paquete->codigo_operacion = MENSAJE;
	//asigno el buffer que previamente reserve memoria
	paquete->buffer = ptr_buffer;
	//asigno el size del buffer
	paquete->buffer->size = strlen(mensaje) + 1;
	//Con el size calculado, reservo memoria para el payload
	paquete->buffer->stream = malloc(paquete->buffer->size);
	//con memcpy() lleno el stream
	memcpy(paquete->buffer->stream, mensaje, paquete->buffer->size);


//ya se puede enviar el paquete
	int32_t result;
	log_info(logger, "Intentando enviar");
	if((result = send_paquete(socket, paquete)) == -1)
		log_error(logger, "Error al enviar");
	else
		log_info(logger, "Se enviaron %d bytes", result);

//TODO se debe recibir la confirmacion del mensaje
//	receive_ACK();

}




t_log* initialize_thread(char * mi_nombre, char * proceso_a_conectar, pthread_t mi_thread){

	char * file = string_new();
		string_append(&file, mi_nombre);
		string_append(&file, "_");
		string_append(&file, proceso_a_conectar);
		string_append(&file, ".txt");
	char * nombre_logger = string_new();
		string_append(&nombre_logger, "Conexion_con_");
		string_append(&nombre_logger, proceso_a_conectar);

//Se crea el logger para el thread especifico
	t_log* logger;
	if((logger = log_create(file, nombre_logger, LOG_CONSOLE, LOG_LEVEL_INFO)) == NULL){
		puts("No se pudo crear el log");
	}
	else
		log_info(logger, "Log del %s creado", mi_nombre);

//Asigno como nombre de este hilo al proceso que va a conectar
		log_debug(logger, "id del thread: '%lu'\n", mi_thread);
		char nombre[16];	//minimo es 16
		pthread_setname_np(mi_thread, proceso_a_conectar);
		pthread_getname_np(mi_thread, nombre, 16);
		log_info(logger, "Proceso a conectar: %s\n", nombre);

return logger;


}

void enviar_muchos_mensajes(char* yo, char* el, uint32_t socket, t_log* logger){

	char* mensaje = string_new();
		string_append(&mensaje, "Buen dia ");
		string_append(&mensaje, el);
		string_append(&mensaje, " soy el ");
		string_append(&mensaje, yo);

	uint32_t size = strlen(mensaje)+1;
			size = sizeof(mensaje);
	uint32_t vez = 1;
	while(1){
		enviar_mensaje(mensaje, socket, logger);

	//recibir mensaje
		log_info(logger, "Intentando recibir el mensaje por vez numero %d\n", vez);

		op_code codigo;
		codigo = receive_cod_op(socket, logger);

		char* buffer;
		switch(codigo){
		case MENSAJE:

		buffer = recibir_mensaje(socket, &size, logger);

		//loguear mensaje recibido
			log_info(logger, "Mensaje rerespuesta del %s recibido: %s\n", el, buffer);
			free(buffer);
			break;

		case CATCH:	//TODO
			log_error(logger, "Sorry no te recibo ese mensaje, ya trabaste todo el programa");
			break;
		default:
			log_error(logger, "Sorry no te recibo ese mensaje, ya trabaste todo el programa");

		}

		sleep(TIEMPO_CHECK);
		vez++;
	}
}

op_code receive_cod_op(uint32_t socket, t_log* logger){
	op_code codigo;

	int32_t resultado;
	if((resultado = recv(socket, &codigo, sizeof(op_code), MSG_WAITALL)) == -1)
		log_error(logger, "Error al recibir la cod_op\n");
	else
		log_info(logger, "Se recibio la cod op: %d\n", codigo);

	return codigo;
}





