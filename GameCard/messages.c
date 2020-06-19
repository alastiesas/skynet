/*
 * messages.c
 *
 *  Created on: 19 jun. 2020
 *      Author: utnso
 */
#include "gamecard.h"

void serve_new(operation_code op_code, void* message){
	if(op_code != OPERATION_NEW)
		log_error(logger, "Aca nunca llego");

	t_message_new* message_new = (t_message_new*) message;

	//TODO guardar el mensaje new en el filesystem

	//Generar mensaje APPEARED
	t_message_appeared* message_appeared = create_message_appeared_long(message_new->id, message_new->pokemon_name, message_new->location->position->x, message_new->location->position->y);
	log_info(logger, "Se genero el mensaje appeared");
	destroy_message_new(message_new);

	t_package* package = serialize_appeared(message_appeared);
	destroy_message_appeared(message_appeared);

	pthread_t thread;
	pthread_create(&thread, NULL, (void*) send_to_broker, (void*)package);
	pthread_detach(thread);

}

void serve_catch(operation_code op_code, void* message){
	if(op_code != OPERATION_CATCH)
		log_error(logger, "Aca nunca llego");

	t_message_catch* message_catch = (t_message_catch*) message;

	//TODO verificar en el filesystem si el pokemon esta disponible para atrapar

	//Generar mensaje CAUGHT												//TODO remover harcodeo true
	t_message_caught* message_caught = create_message_caught(message_catch->id, true);
	log_info(logger, "Se genero el mensaje caught");
	destroy_message_catch(message_catch);

	t_package* package = serialize_caught(message_caught);
	destroy_message_caught(message_caught);

	pthread_t thread;
	pthread_create(&thread, NULL, (void*) send_to_broker, (void*)package);
	pthread_detach(thread);

}

void serve_get(operation_code op_code, void* message){
	if(op_code != OPERATION_GET)
		log_error(logger, "Aca nunca llego");

	t_message_get* message_get = (t_message_get*) message;

	//TODO buscar en el filesystem en que posiciones esta el pokemon

	//Generar mensaje LOCALIZED
			t_position* positions = malloc(3 * sizeof(t_position));
			positions->x = 1;
			positions->y = 1;
			(positions + 1)->x = 2;
			(positions + 1)->y = 2;
			(positions + 2)->x = 3;
			(positions + 2)->y = 3;
																											//TODO remover harcodeo
	t_message_localized* message_localized = create_message_localized(message_get->id, message_get->pokemon_name, 3, positions);
	log_info(logger, "Se genero el mensaje localized");
	destroy_message_get(message_get);

	t_package* package = serialize_localized(message_localized);
	destroy_message_localized(message_localized);

	pthread_t thread;
	pthread_create(&thread, NULL, (void*) send_to_broker, (void*)package);
	pthread_detach(thread);

}
