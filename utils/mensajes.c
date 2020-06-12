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

	t_package* paquete = serialize_saludo(mensaje);


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



int32_t send_catch(t_message_catch* catch, uint32_t socket, t_log* logger){

	t_package* paquete = serialize_catch(catch);


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

int32_t send_ACK_failure(uint32_t socket, t_log* logger){

	int32_t ACK = 2;
	int32_t result;
	log_debug(logger, "Intentando enviar confirmacion de error");
	if((result = send(socket, &ACK, sizeof(int32_t), 0)) == -1)
		log_error(logger, "Error al enviar confirmacion de error");
	else
		log_info(logger, "Se envio la confirmacion de error (%d bytes)", result);

	return result;

}

int32_t receive_ACK(uint32_t socket, t_log* logger){
	int32_t ACK;

	int32_t resultado = recv(socket, &ACK, sizeof(int32_t), MSG_WAITALL);
	if(resultado == -1){
		log_error(logger, "Error al recibir la confirmacion del mensaje\n");
		return -1; //failure
	}
	if(ACK == 1){
		log_info(logger, "Se recibio la confirmacion del mensaje enviado\n");
	}
	else if(ACK == 2){
		log_error(logger, "El proceso responde error en la solicitud\n");
		return -1;
	}
	else{
		log_warning(logger, "La confirmacion recibida no puede ser %d", ACK);
		return -1; //failure
	}

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


	return ID_proceso;

}



t_message_new* receive_new(uint32_t socket_cliente, uint32_t* size, t_log* logger){

	t_message_new* new = malloc(sizeof(t_message_new));
	new->location = malloc(sizeof(t_location));
	new->location->position = malloc(sizeof(t_position));

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
	if(recv(socket_cliente, &(new->size_pokemon_name), sizeof(new->size_pokemon_name), MSG_WAITALL) == -1)
		log_error(logger, "Error al recibir el size_nombre de new");
	else
		log_info(logger, "size_nombre de new recibido: %d", new->size_pokemon_name);

	new->pokemon_name = malloc(new->size_pokemon_name);

	//recibir nombre de new
	if(recv(socket_cliente, new->pokemon_name, new->size_pokemon_name, MSG_WAITALL) == -1)
		log_error(logger, "Error al recibir el nombre de new");
	else
		log_info(logger, "nombre de new recibido: %s", new->pokemon_name);

	//recibir posX de new
	if(recv(socket_cliente, &(new->location->position->x), sizeof(new->location->position->x), MSG_WAITALL) == -1)
		log_error(logger, "Error al recibir la posX de new");
	else
		log_info(logger, "posX de new recibida: %d", new->location->position->x);

	//recibir posY de new
	if(recv(socket_cliente, &(new->location->position->y), sizeof(new->location->position->y), MSG_WAITALL) == -1)
		log_error(logger, "Error al recibir la posY de new");
	else
		log_info(logger, "posY de new recibida: %d\n", new->location->position->y);

	//recibir cantidad de new
	if(recv(socket_cliente, &(new->location->amount), sizeof(new->location->amount), MSG_WAITALL) == -1)
		log_error(logger, "Error al recibir la cantidad de new");
	else
		log_info(logger, "cantidad de new recibida: %d\n", new->location->amount);


	if(*size != sizeof(new->id) + sizeof(new->size_pokemon_name) + strlen(new->pokemon_name)+1 + sizeof(new->location->position->x) + sizeof(new->location->position->y) + sizeof(new->location->amount))
		log_error(logger, "Tamanio erroneo");


	return new;

}

t_message_appeared* receive_appeared(uint32_t socket_cliente, uint32_t* size, t_log* logger){

	t_message_appeared* appeared = malloc(sizeof(t_message_appeared));
	appeared->position = malloc(sizeof(t_position));

	log_info(logger, "Esperando recibir tamanio del stream\n");

	if(recv(socket_cliente, size, sizeof(uint32_t), MSG_WAITALL) == -1)
		log_error(logger, "Error al recibir el tamanio del stream");
	else
		log_info(logger, "Se solicito recibir un tamanio de stream de: %d\n", *size);

	//recibir id de appeared
	if(recv(socket_cliente, &(appeared->id), sizeof(appeared->id), MSG_WAITALL) == -1)
		log_error(logger, "Error al recibir el id de appeared");
	else
		log_info(logger, "id de appeared recibido: %d", appeared->id);

	//recibir correlative_id de appeared
	if(recv(socket_cliente, &(appeared->correlative_id), sizeof(appeared->correlative_id), MSG_WAITALL) == -1)
		log_error(logger, "Error al recibir el id de appeared");
	else
		log_info(logger, "id de appeared recibido: %d", appeared->correlative_id);

	//recibir size_pokemon_name de appeared
	if(recv(socket_cliente, &(appeared->size_pokemon_name), sizeof(appeared->size_pokemon_name), MSG_WAITALL) == -1)
		log_error(logger, "Error al recibir el size_nombre de appeared");
	else
		log_info(logger, "size_nombre de appeared recibido: %d", appeared->size_pokemon_name);

	//allocar memoria para el nobmre
	appeared->pokemon_name = malloc(appeared->size_pokemon_name);

	//recibir nombre de appeared
	if(recv(socket_cliente, appeared->pokemon_name, appeared->size_pokemon_name, MSG_WAITALL) == -1)
		log_error(logger, "Error al recibir el nombre de appeared");
	else
		log_info(logger, "nombre de appeared recibido: %s", appeared->pokemon_name);

	//recibir posX de appeared
	if(recv(socket_cliente, &(appeared->position->x), sizeof(appeared->position->x), MSG_WAITALL) == -1)
		log_error(logger, "Error al recibir la posX de appeared");
	else
		log_info(logger, "posX de appeared recibida: %d", appeared->position->x);

	//recibir posY de appeared
	if(recv(socket_cliente, &(appeared->position->y), sizeof(appeared->position->y), MSG_WAITALL) == -1)
		log_error(logger, "Error al recibir la posY de appeared");
	else
		log_info(logger, "posY de appeared recibida: %d\n", appeared->position->y);



	if(*size != sizeof(appeared->id) + sizeof(appeared->correlative_id) + sizeof(appeared->size_pokemon_name) + strlen(appeared->pokemon_name)+1 + sizeof(appeared->position->x) + sizeof(appeared->position->y))
		log_error(logger, "Tamanio erroneo");


	return appeared;

}

t_message_get* receive_get(uint32_t socket_cliente, uint32_t* size, t_log* logger){

	t_message_get* get = malloc(sizeof(t_message_get));

	log_info(logger, "Esperando recibir tamanio del stream\n");

	if(recv(socket_cliente, size, sizeof(uint32_t), MSG_WAITALL) == -1)
		log_error(logger, "Error al recibir el tamanio del stream");
	else
		log_info(logger, "Se solicito recibir un tamanio de stream de: %d\n", *size);

	//recibir id de get
	if(recv(socket_cliente, &(get->id), sizeof(get->id), MSG_WAITALL) == -1)
		log_error(logger, "Error al recibir el id de get");
	else
		log_info(logger, "id de get recibido: %d", get->id);

	//recibir size_pokemon_name de get
	if(recv(socket_cliente, &(get->size_pokemon_name), sizeof(get->size_pokemon_name), MSG_WAITALL) == -1)
		log_error(logger, "Error al recibir el size_nombre de get");
	else
		log_info(logger, "size_nombre de get recibido: %d", get->size_pokemon_name);

	//allocar memoria para el nobmre
	get->pokemon_name = malloc(get->size_pokemon_name);



	if(*size != sizeof(get->id) + sizeof(get->size_pokemon_name) + strlen(get->pokemon_name)+1)
		log_error(logger, "Tamanio erroneo");

	return get;

}
/*
t_message_localized* receive_localized(uint32_t socket_cliente, uint32_t* size, t_log* logger){

	t_message_localized* localized = malloc(sizeof(t_message_localized));

	log_info(logger, "Esperando recibir tamanio del stream\n");

	if(recv(socket_cliente, size, sizeof(uint32_t), MSG_WAITALL) == -1)
		log_error(logger, "Error al recibir el tamanio del stream");
	else
		log_info(logger, "Se solicito recibir un tamanio de stream de: %d\n", *size);

	//recibir id de localized
	if(recv(socket_cliente, &(localized->id), sizeof(localized->id), MSG_WAITALL) == -1)
		log_error(logger, "Error al recibir el id de localized");
	else
		log_info(logger, "id de localized recibido: %d", localized->id);

	//recibir correlative_id de localized
	if(recv(socket_cliente, &(localized->correlative_id), sizeof(localized->correlative_id), MSG_WAITALL) == -1)
		log_error(logger, "Error al recibir el id de localized");
	else
		log_info(logger, "id de localized recibido: %d", localized->correlative_id);

	//recibir size_pokemon_name de localized
	if(recv(socket_cliente, &(localized->size_pokemon_name), sizeof(localized->size_pokemon_name), MSG_WAITALL) == -1)
		log_error(logger, "Error al recibir el size_nombre de localized");
	else
		log_info(logger, "size_nombre de localized recibido: %d", localized->size_pokemon_name);

	//allocar memoria para el localized
	localized->pokemon_name = malloc(localized->size_pokemon_name);

	//recibir nombre de appeared
	if(recv(socket_cliente, localized->pokemon_name, localized->size_pokemon_name, MSG_WAITALL) == -1)
		log_error(logger, "Error al recibir el nombre de localized");
	else
		log_info(logger, "nombre de localized recibido: %s", localized->pokemon_name);

	//recibir position_amount de localized
		if(recv(socket_cliente, &(localized->position_amount), sizeof(localized->position_amount), MSG_WAITALL) == -1)
			log_error(logger, "Error al recibir el size_nombre de localized");
		else
			log_info(logger, "size_nombre de localized recibido: %d", localized->position_amount);

	//allocar memoria para las posiciones
	//localized->positions = malloc(sizeof(t_position)*localized->position_amount);

	for(int i = 0; i < localized->position_amount; i++) {
		//recibir posX de appeared positions[i]
		localized->positions[i] = malloc(sizeof(t_position));

		if(recv(socket_cliente, &(localized->positions[i]->x), sizeof(localized->positions[i]->x), MSG_WAITALL) == -1)
			log_error(logger, "Error al recibir la posX de localized");
		else
			log_info(logger, "posX de new recibida: %d", localized->positions[i]->x);

		//recibir posY de localized
		if(recv(socket_cliente, &(localized->positions[i]->y), sizeof(localized->positions[i]->y), MSG_WAITALL) == -1)
			log_error(logger, "Error al recibir la posY de localized");
		else
			log_info(logger, "posY de localized recibida: %d\n", localized->positions[i]->y);
	}



	if(*size != sizeof(localized->id) + sizeof(localized->correlative_id) + sizeof(localized->size_pokemon_name) + strlen(localized->pokemon_name)+1 + sizeof(localized->position_amount) + (sizeof(localized->positions[0]->x) + sizeof(localized->positions[0]->y))*localized->position_amount)
		log_error(logger, "Tamanio erroneo");


	return localized;

}*/

t_message_catch* receive_catch(uint32_t socket_cliente, uint32_t* size, t_log* logger){

	t_message_catch* catch = malloc(sizeof(t_message_catch));
	catch->position = malloc(sizeof(t_position));

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
	if(recv(socket_cliente, &(catch->size_pokemon_name), sizeof(catch->size_pokemon_name), MSG_WAITALL) == -1)
		log_error(logger, "Error al recibir el size_nombre de catch");
	else
		log_info(logger, "size_nombre de catch recibido: %d", catch->size_pokemon_name);

	catch->pokemon_name = malloc(catch->size_pokemon_name);

	//recibir nombre de catch
	if(recv(socket_cliente, catch->pokemon_name, catch->size_pokemon_name, MSG_WAITALL) == -1)
		log_error(logger, "Error al recibir el nombre de catch");
	else
		log_info(logger, "nombre de catch recibido: %s", catch->pokemon_name);

	//recibir posX de catch
	if(recv(socket_cliente, &(catch->position->x), sizeof(catch->position->x), MSG_WAITALL) == -1)
		log_error(logger, "Error al recibir la posX de catch");
	else
		log_info(logger, "posX de catch recibida: %d", catch->position->x);

	//recibir posY de catch
	if(recv(socket_cliente, &(catch->position->y), sizeof(catch->position->y), MSG_WAITALL) == -1)
		log_error(logger, "Error al recibir ela posY de catch");
	else
		log_info(logger, "posY de catch recibida: %d\n", catch->position->y);


	if(*size != sizeof(catch->id) + sizeof(catch->size_pokemon_name) + strlen(catch->pokemon_name)+1 + sizeof(catch->position->x) + sizeof(catch->position->y))
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

	t_message_catch* catch = malloc(sizeof(t_message_catch));
	catch->id = 2;
	catch->pokemon_name = "pepito";
	catch->size_pokemon_name = strlen(catch->pokemon_name)+1;
	catch->position->x = 4;
	catch->position->y = 7;

	uint32_t vez = 1;
	while(1){
		if(vez%2 == 0)	//alterna entre uno y otro
			enviar_mensaje(mensaje, socket, logger);
		else
			send_catch(catch, socket, logger);

		//recibir mensaje
		log_info(logger, "Intentando recibir el paquete por vez numero %d\n", vez);

		operation_code codigo;
		codigo = receive_cod_op(socket, logger);


		char* buffer;
		t_message_catch* catch2 = malloc(sizeof(t_message_catch));
		switch(codigo){
		case PRUEBA:

			log_info(logger, "Se recibe un paquete de tipo mensaje");


			buffer = recibir_mensaje(socket, &size, logger);

			//loguear mensaje recibido
			log_info(logger, "Mensaje rerespuesta del %s recibido: %s\n", el, buffer);
			free(buffer);
			break;

		case OPERATION_CATCH:

			log_info(logger, "Se recibe un paquete de tipo CATCH");


			catch2 = receive_catch(socket, &size, logger);

			//loguear mensaje recibido
			log_info(logger, "Mensaje rerespuesta del %s recibido:\n id: %d\n size_nombre: %d\n nombre: %s\n posX: %d\n posY: %d \n", el, catch2->id, catch2->size_pokemon_name, catch2->pokemon_name, catch2->position->x, catch2->position->y);

			free(catch2->pokemon_name);	//faltan frees
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

operation_code receive_cod_op(uint32_t socket, t_log* logger){
	operation_code codigo;

	int32_t resultado;
	if((resultado = recv(socket, &codigo, sizeof(operation_code), MSG_WAITALL)) == -1)
		log_error(logger, "Error al recibir la cod_op\n");
	else
		log_info(logger, "Se recibio la cod op: %d\n", codigo);

	return codigo;
}





