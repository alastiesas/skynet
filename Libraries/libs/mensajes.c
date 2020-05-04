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

catch->nombre = malloc(sizeof(catch->size_nombre));

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
		case MENSAJE:

			log_info(logger, "Se recibe un paquete de tipo mensaje");


		buffer = recibir_mensaje(socket, &size, logger);

		//loguear mensaje recibido
			log_info(logger, "Mensaje rerespuesta del %s recibido: %s\n", el, buffer);
			free(buffer);
			break;

		case CATCH:

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

int enviarInt(int destinatario, int loQueEnvio){

	 void* paquete = malloc(sizeof(int));
	 void* puntero = paquete;
	 memcpy(puntero, &(loQueEnvio), sizeof(int));
	 int res = send(destinatario, paquete, sizeof(int), MSG_WAITALL);
	 free(paquete);
	 return res;
}

//-------------------------------------------

int recibirInt(int destinatario){
	int algo;
	if(recv(destinatario, &algo, sizeof(int), MSG_WAITALL) != 0)
			return algo;
		else
		{
			//close(destinatario);
			return -1;
		}

}

int enviarUint32_t(int destinatario, uint32_t loQueEnvio){
	 void* paquete = malloc(sizeof(uint32_t));
	 void* puntero = paquete;
	 memcpy(puntero, &(loQueEnvio), sizeof(uint32_t));
	 int res = send(destinatario, paquete, sizeof(uint32_t), MSG_WAITALL);
	 free(paquete);
	 return res;
}

uint32_t recibirUint32_t(int destinatario){
	uint32_t algo;
	if(recv(destinatario, &algo, sizeof(uint32_t), MSG_WAITALL) != 0)
			return algo;
		else
		{
			//close(destinatario);
			return -1;
		}

}
int enviarSizet(int destinatario,size_t n){
	 int tamanio = sizeof(size_t);
	 void* paquete = malloc(sizeof(size_t));
	 void* puntero = paquete;

	 memcpy(puntero, &(n), sizeof(size_t));
	 int res = send(destinatario, paquete, tamanio, MSG_WAITALL);
	 free(paquete);
	 return res;
}

size_t recibirSizet(int destinatario){

	size_t algo;
	if(recv(destinatario, &algo, sizeof(size_t), MSG_WAITALL) != 0)
			return algo;
		else
		{
			//close(destinatario);
			return -1;
		}
}

int enviarString(int destinatario, char* loQueEnvio){
	int tamanio;
	if(loQueEnvio == NULL){
		tamanio = 0;
	} else{
		tamanio = strlen(loQueEnvio) + 1;
	}

	int res = enviarInt(destinatario, tamanio);
    if(res > 0){
    	void* estring = malloc(tamanio);

    	memcpy(estring, &(loQueEnvio), tamanio);

    	res = send(destinatario, estring, tamanio, MSG_WAITALL);
    	free(estring);
    	return res;
    }


	return res;
}

char* recibirString(int destinatario){

		void * buffer;
		int size;
		recv(destinatario, size, sizeof(int), MSG_WAITALL);
		buffer = malloc(size);
		recv(destinatario, buffer, size, MSG_WAITALL);

		return buffer;


}




