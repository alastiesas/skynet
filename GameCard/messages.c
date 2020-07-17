/*
 * messages.c
 *
 *  Created on: 19 jun. 2020
 *      Author: utnso
 */
#include "gamecard.h"



int32_t wait_available_file_new(char* pokemon_name){

	//esperar mutex del pokemon metadata
	pthread_mutex_lock(&mutex_pkmetadata);	//TODO unlock
	//abrir metadata del pokemon dado, si no existe, crearlo, setear metadata en ocupado (soltar el mutex y retornar -1)

	//chequear en el metadata si esta ocupado, en ese caso, soltar el mutex y reintentar en TIEMPO_DE_REINTENTO_OPERACION
	//si esta disponible, actualizarlo como en uso, soltar mutex y listo
}

int32_t wait_available_file(char* pokemon_name){

	//esperar mutex del pokemon metadata
	pthread_mutex_lock(&mutex_pkmetadata);	//TODO unlock
	//abrir metadata del pokemon dado, si no existe, soltar el mutex y retornar directamente que no existe (-1)
										//si no existe, esperar en este punto el tiempo de retardo operacion (mirar si aplica en este caso ese tiempo)
	//chequear en el metadata si esta ocupado, en ese caso, soltar el mutex y reintentar en TIEMPO_DE_REINTENTO_OPERACION
	//si esta disponible, actualizarlo como en uso, soltar mutex y listo, (retornar 0)
}

void release_pokemon_file(char* pokemon_name){
	//esperar mutex del pokemon metadata
	pthread_mutex_lock(&mutex_pkmetadata);
	//abrir metadata del pokemon dado
	//actualizar el metadata del pokemon a libre
	//soltar el mutex
	pthread_mutex_unlock(&mutex_pkmetadata);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------

t_message_appeared* process_new(t_message_new* message_new){

	t_message_appeared* message_appeared;
	int32_t exists;

	exists = wait_available_file_new(message_new->pokemon_name);
	//si no existia el archivo metadata, crear directorio y archivo pokemon manualmente
	if(exists != -1){
	//pasar los bloques del archivo a memoria
	//crear diccionario con el archivo
	//verificar si existe en el archivo la posicion recibida, si no existe, crearla
	//sumar la cantidad de pokemonos nuevos
	//convertir el diccionario a void*
	//grabar el void* en los bloques
	//esperar el tiempo de retardo  operacion
	release_pokemon_file(message_new->pokemon_name);
	}
	else{
		//TODO crear metadata y setear en open antes de soltar el mutex
		create_pokemon_directory(message_new->pokemon_name, message_new->location);
		sleep(TIEMPO_RETARDO_OPERACION);
	}

	//generar mensaje appeared y destruir el mensaje new
	return message_appeared;
}

t_message_caught* process_catch(t_message_catch* message_catch){

	t_message_caught* message_caught;
	int32_t exists;

	exists = wait_available_file(message_catch->pokemon_name);
		//si no existia el archivo metadata, saltar directamente a generar la respuesta que no se pudo atrapar
	if(exists != -1){
	//pasar los bloques del archivo a memoria
	//crear diccionario con el archivo
	//verificar si existe en el archivo la posicion recibida
	//si existe la posicion, entonces tiene al menos un pokemon. Restarle 1 y responder que se pudo atrapar
		//si no existe, responder que no se pudo atrapar
	//si la cantidad queda en 0 para esa posicion, eliminar linea
	//convertir el diccionario a void*
	//grabar el void* en los bloques
	//esperar el tiempo de retardo operacion
	release_pokemon_file(message_catch->pokemon_name);
	}

	//generar mensaje caught y destruir el mensaje catch
	return message_caught;
}

t_message_localized* process_get(t_message_get* message_get){

	t_message_localized* message_localized;
	int32_t exists;

	exists = wait_available_file(message_get->pokemon_name);
		//si no existia el archivo metadata, saltar directamente a generar la respuesta localized sin posiciones
	if(exists != -1){
	//pasar los bloques del archivo a memoria
	//crear diccionario con el archivo
	//Obtener todas las posiciones y cantidades de Pokemon pedido.
	//no se modifica el archivo, entonces con destruir el diccionario alcanza
	//esperar el tiempo de retardo operacion
	release_pokemon_file(message_get->pokemon_name);
	}

	//generar mensaje localized y destruir el mensaje get
	return message_localized;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void serve_new(void* input){
	operation_code op_code = ((serve_thread_args*)input)->op_code;
	void* message = ((serve_thread_args*)input)->message;
	free(input);

	if(op_code != OPERATION_NEW)
		log_error(logger, "Aca nunca llego");

	t_message_new* message_new = (t_message_new*) message;

	t_message_appeared* message_appeared;
	//message_appeared =  process_new(message_new); TODO

//-----------------------------------------------------//TODO remover harcodeo, se hace esto en la funcion de arriba
	//Generar mensaje APPEARED
	message_appeared = create_message_appeared_long(message_new->id, message_new->pokemon_name, message_new->location->position->x, message_new->location->position->y);
	log_info(logger, "Se genero el mensaje appeared");
	destroy_message_new(message_new);

//------------------------------------------------------

	t_package* package = serialize_appeared(message_appeared);
	destroy_message_appeared(message_appeared);


	send_to_broker(package);
}

void serve_catch(void* input){
	operation_code op_code = ((serve_thread_args*)input)->op_code;
	void* message = ((serve_thread_args*)input)->message;
	free(input);

	if(op_code != OPERATION_CATCH)
		log_error(logger, "Aca nunca llego");

	t_message_catch* message_catch = (t_message_catch*) message;

	t_message_caught* message_caught;
	//message_caught = process_catch(message_catch); TODO

//-----------------------------------------------------//TODO remover harcodeo, se hace esto en la funcion de arriba
	//Generar mensaje CAUGHT
	message_caught = create_message_caught(message_catch->id, 1);
	log_info(logger, "Se genero el mensaje caught");
	destroy_message_catch(message_catch);

//-----------------------------------------------------

	t_package* package = serialize_caught(message_caught);
	destroy_message_caught(message_caught);


	send_to_broker(package);
}

void serve_get(void* input){
	operation_code op_code = ((serve_thread_args*)input)->op_code;
	void* message = ((serve_thread_args*)input)->message;
	free(input);

	if(op_code != OPERATION_GET)
		log_error(logger, "Aca nunca llego");

	t_message_get* message_get = (t_message_get*) message;

	t_message_localized* message_localized;
	//message_localized = process_get(message_get); TODO

//-----------------------------------------------------//TODO remover harcodeo, se hace esto en la funcion de arriba
	//Generar mensaje LOCALIZED
			t_position* positions = malloc(3 * sizeof(t_position));
			positions->x = 1;
			positions->y = 1;
			(positions + 1)->x = 2;
			(positions + 1)->y = 2;
			(positions + 2)->x = 3;
			(positions + 2)->y = 3;

	message_localized = create_message_localized(message_get->id, message_get->pokemon_name, 3, positions);
	log_info(logger, "Se genero el mensaje localized");
	destroy_message_get(message_get);

//-----------------------------------------------------

	t_package* package = serialize_localized(message_localized);
	destroy_message_localized(message_localized);


	send_to_broker(package);
}


//---------------------------------------------No va------------------------------------------------------------------------------------------------------------------



t_dictionary* semaphores;
pthread_mutex_t semaforo_del_diccionario_de_semaforos_JAJAJA;
//un semaforo para cada pokemon_metadata. En el caso que rompa t0do, se puede usar "sin problema" un unico mutex global para todos los pokemon_metadata
pthread_mutex_t* get_pokemon_mutex(char* pokemon_name){
	pthread_mutex_lock(&semaforo_del_diccionario_de_semaforos_JAJAJA);
	if(dictionary_has_key(semaphores, pokemon_name)){
		pthread_mutex_t* found_semaphore = dictionary_get(semaphores, pokemon_name);
		pthread_mutex_unlock(&semaforo_del_diccionario_de_semaforos_JAJAJA);
		return found_semaphore;
	}
	else{
		pthread_mutex_t new_semaphore;
		pthread_mutex_init(&new_semaphore, NULL);
		dictionary_put(semaphores, pokemon_name, &new_semaphore);
		pthread_mutex_unlock(&semaforo_del_diccionario_de_semaforos_JAJAJA);
		return &new_semaphore;
	}
}
