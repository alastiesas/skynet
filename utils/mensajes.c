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
	void* buffer;

	log_debug(logger, "Esperando recibir tamanio del mensaje\n");

	if(recv(socket_cliente, size, sizeof(uint32_t), MSG_WAITALL) == -1)
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

int32_t enviar_mensaje(char* mensaje, uint32_t socket, t_log* logger)
{

	t_paquete* paquete = serialize_message(mensaje);


//ya se puede enviar el paquete
	int32_t result;
	log_info(logger, "Intentando enviar");
	if((result = send_paquete(socket, paquete)) == -1)
		log_error(logger, "Error al enviar");
	else
		log_info(logger, "Se enviaron %d bytes", result);

//TODO se debe recibir la confirmacion del mensaje
//	receive_ACK();

	return result;
}



int32_t send_catch(t_catch* catch, uint32_t socket, t_log* logger){

	t_paquete* paquete = serialize_catch(catch);


//ya se puede enviar el paquete
	int32_t result;
	log_info(logger, "Intentando enviar");
	if((result = send_paquete(socket, paquete)) == -1)
		log_error(logger, "Error al enviar");
	else
		log_info(logger, "Se enviaron %d bytes", result);

//TODO se debe recibir la confirmacion del mensaje
//	receive_ACK();

	return result;

}

int32_t send_ID(int32_t ID, uint32_t socket, t_log* logger){


	int32_t result;
	log_debug(logger, "Intentando enviar ID del mensaje");
	if((result = send(socket, &ID, sizeof(int32_t), 0)) == -1)
		log_error(logger, "Error al enviar ID del mensaje");
	else
		log_info(logger, "Se envio el ID %d, (%d bytes)", ID, result);

	return result;

}

int32_t receive_ID(uint32_t socket, t_log* logger){
	int32_t ID;

	int32_t resultado;
	if((resultado = recv(socket, &ID, sizeof(int32_t), MSG_WAITALL)) == -1)
		log_error(logger, "Error al recibir el ID de mensaje\n");
	else
		log_info(logger, "Se recibio el ID de mensaje: %d\n", ID);

	return ID;
}

int32_t send_ACK(uint32_t socket, t_log* logger){

	int32_t ACK = 1;
	int32_t result;
	log_debug(logger, "Intentando enviar confirmacion");
	if((result = send(socket, &ACK, sizeof(int32_t), 0)) == -1)
		log_error(logger, "Error al enviar confirmacion");
	else
		log_info(logger, "Se envio la confirmacion (%d bytes)", result);

	return result;

}

int32_t receive_ACK(uint32_t socket, t_log* logger){
	int32_t ACK;

	int32_t resultado;
	if((resultado = recv(socket, &ACK, sizeof(int32_t), MSG_WAITALL)) == -1){
		log_error(logger, "Error al recibir la confirmacion del mensaje\n");
		return -1; //failure
	}
	else
		log_info(logger, "Se recibio la confirmacion del mensaje enviado\n");

	if(ACK != 1){
		log_warning(logger, "La confirmacion debe ser 1, no %d", ACK);
		return -1; //failure
	}
	else
		return 0; //success

}

uint32_t receive_size(uint32_t socket, t_log* logger){

	uint32_t size;
	log_info(logger, "Esperando recibir tamanio del stream\n");

	if(recv(socket, &size, sizeof(uint32_t), MSG_WAITALL) == -1)
		log_error(logger, "Error al recibir el tamanio del stream");
	else
		log_info(logger, "Se solicito recibir un tamanio de stream de: %d\n", size);


		return size;

}

uint32_t receive_ID_proceso(uint32_t socket, t_log* logger){
	uint32_t ID_proceso;

	int32_t resultado;
	if((resultado = recv(socket, &ID_proceso, sizeof(uint32_t), MSG_WAITALL)) == -1){
		log_error(logger, "Error al recibir el ID del proceso\n");
		return -1; //failure
	}
	else
		log_info(logger, "Se recibio el ID del proceso: %d\n", ID_proceso);


		return 0; //success

}



t_new* receive_new(uint32_t socket_cliente, uint32_t* size, t_log* logger){

	t_new* new = malloc(sizeof(t_new));


	log_info(logger, "Esperando recibir tamanio del stream\n");

	if(recv(socket_cliente, size, sizeof(uint32_t), MSG_WAITALL) == -1)
		log_error(logger, "Error al recibir el tamanio del stream");
	else
		log_info(logger, "Se solicito recibir un tamanio de stream de: %d\n", *size);


	//recibir id de new
	if(recv(socket_cliente, &(new->id), sizeof(new->id), MSG_WAITALL) == -1)
		log_error(logger, "Error al recibir el id de new");
	else
		log_info(logger, "id de new recibido: %d", new->id);

	//recibir size_nombre de new
	if(recv(socket_cliente, &(new->size_nombre), sizeof(new->size_nombre), MSG_WAITALL) == -1)
		log_error(logger, "Error al recibir el size_nombre de new");
	else
		log_info(logger, "size_nombre de new recibido: %d", new->size_nombre);

	new->nombre = malloc(new->size_nombre);

	//recibir nombre de new
	if(recv(socket_cliente, new->nombre, new->size_nombre, MSG_WAITALL) == -1)
		log_error(logger, "Error al recibir el nombre de new");
	else
		log_info(logger, "nombre de new recibido: %s", new->nombre);

	//recibir posX de new
	if(recv(socket_cliente, &(new->posX), sizeof(new->posX), MSG_WAITALL) == -1)
		log_error(logger, "Error al recibir la posX de new");
	else
		log_info(logger, "posX de new recibida: %d", new->posX);

	//recibir posY de new
	if(recv(socket_cliente, &(new->posY), sizeof(new->posY), MSG_WAITALL) == -1)
		log_error(logger, "Error al recibir la posY de new");
	else
		log_info(logger, "posY de new recibida: %d\n", new->posY);

	//recibir cantidad de new
	if(recv(socket_cliente, &(new->cantidad), sizeof(new->cantidad), MSG_WAITALL) == -1)
		log_error(logger, "Error al recibir la cantidad de new");
	else
		log_info(logger, "cantidad de new recibida: %d\n", new->cantidad);


	if(*size != sizeof(new->id) + sizeof(new->size_nombre) + strlen(new->nombre)+1 + sizeof(new->posX) + sizeof(new->posY) + sizeof(new->cantidad))
		log_error(logger, "Tamanio erroneo");

	return new;

}

t_catch* receive_catch(uint32_t socket_cliente, uint32_t* size, t_log* logger){

	t_catch* catch = malloc(sizeof(t_catch));


	log_info(logger, "Esperando recibir tamanio del stream\n");

	if(recv(socket_cliente, size, sizeof(uint32_t), MSG_WAITALL) == -1)
		log_error(logger, "Error al recibir el tamanio del stream");
	else
		log_info(logger, "Se solicito recibir un tamanio de stream de: %d\n", *size);


	//recibir id de catch
	if(recv(socket_cliente, &(catch->id), sizeof(catch->id), MSG_WAITALL) == -1)
		log_error(logger, "Error al recibir el id de catch");
	else
		log_info(logger, "id de catch recibido: %d", catch->id);

	//recibir size_nombre de catch
	if(recv(socket_cliente, &(catch->size_nombre), sizeof(catch->size_nombre), MSG_WAITALL) == -1)
		log_error(logger, "Error al recibir el size_nombre de catch");
	else
		log_info(logger, "size_nombre de catch recibido: %d", catch->size_nombre);

catch->nombre = malloc(catch->size_nombre);

	//recibir nombre de catch
	if(recv(socket_cliente, catch->nombre, catch->size_nombre, MSG_WAITALL) == -1)
		log_error(logger, "Error al recibir el nombre de catch");
	else
		log_info(logger, "nombre de catch recibido: %s", catch->nombre);

	//recibir posX de catch
	if(recv(socket_cliente, &(catch->posX), sizeof(catch->posX), MSG_WAITALL) == -1)
		log_error(logger, "Error al recibir la posX de catch");
	else
		log_info(logger, "posX de catch recibida: %d", catch->posX);

	//recibir posY de catch
	if(recv(socket_cliente, &(catch->posY), sizeof(catch->posY), MSG_WAITALL) == -1)
		log_error(logger, "Error al recibir ela posY de catch");
	else
		log_info(logger, "posY de catch recibida: %d\n", catch->posY);



	if(*size != sizeof(catch->id) + sizeof(catch->size_nombre) + strlen(catch->nombre)+1 + sizeof(catch->posX) + sizeof(catch->posY))
		log_error(logger, "Tamanio erroneo");

	return catch;

}




t_log* initialize_thread(char * mi_nombre, char * proceso_a_conectar, pthread_t mi_thread){

	char * file = string_new();
		string_append(&file, mi_nombre);
		string_append(&file, "_");
		string_append(&file, proceso_a_conectar);
		string_append(&file, ".log");
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
			//size = sizeof(mensaje);	por que no se puede hacer sizeof??????

	t_catch* catch = malloc(sizeof(t_catch));
	catch->id = 2;
	catch->nombre = "pepito";
	catch->size_nombre = strlen(catch->nombre)+1;
	catch->posX = 4;
	catch->posY = 7;

	uint32_t vez = 1;
	while(1){
		if(vez%2 == 0)	//alterna entre uno y otro
			enviar_mensaje(mensaje, socket, logger);
		else
			send_catch(catch, socket, logger);

	//recibir mensaje
		log_info(logger, "Intentando recibir el paquete por vez numero %d\n", vez);

		op_code codigo;
		codigo = receive_cod_op(socket, logger);


		char* buffer;
		t_catch* catch2 = malloc(sizeof(t_catch));
		switch(codigo){
		case SALUDO:

			log_info(logger, "Se recibe un paquete de tipo mensaje");


		buffer = recibir_mensaje(socket, &size, logger);

		//loguear mensaje recibido
			log_info(logger, "Mensaje rerespuesta del %s recibido: %s\n", el, buffer);
			free(buffer);
			break;

		case CATCHS:

			log_info(logger, "Se recibe un paquete de tipo CATCH");


		catch2 = receive_catch(socket, &size, logger);

		//loguear mensaje recibido
			log_info(logger, "Mensaje rerespuesta del %s recibido:\n id: %d\n size_nombre: %d\n nombre: %s\n posX: %d\n posY: %d \n", el, catch2->id, catch2->size_nombre, catch2->nombre, catch2->posX, catch2->posY);

			free(catch2->nombre);
			free(catch2);
			break;

		default:
			log_error(logger, "Sorry no te recibo ese mensaje, ya trabaste todo el programa");

		}

		sleep(TIEMPO_CHECK);
		vez++;
	}

	free(catch);
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





