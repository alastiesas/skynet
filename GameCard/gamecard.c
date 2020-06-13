#include "gamecard.h"

#include <stdio.h>
#include <stdlib.h>

int main(void) {
	puts("!!!Hello World!!!"); /* prints !!!Hello World!!! */

	logger = log_create("gamecard.log", "gameCard", LOG_CONSOLE, LOG_LEVEL_TRACE);
	if((config = config_create("gamecard.config")) == NULL)
		log_error(logger, "ERROR DE CONFIG");

	TIEMPO_DE_REINTENTO_CONEXION = atoi(config_get_string_value(config, "TIEMPO_DE_REINTENTO_CONEXION"));
	TIEMPO_DE_REINTENTO_OPERACION = atoi(config_get_string_value(config, "TIEMPO_DE_REINTENTO_OPERACION"));
	TIEMPO_RETARDO_OPERACION = atoi(config_get_string_value(config, "TIEMPO_RETARDO_OPERACION"));
	PUNTO_MONTAJE_TALLGRASS = config_get_string_value(config, "PUNTO_MONTAJE_TALLGRASS");
	IP_BROKER = config_get_string_value(config, "IP_BROKER");
	PUERTO_BROKER = config_get_string_value(config, "PUERTO_BROKER");
	IP_GAMECARD = config_get_string_value(config, "IP_GAMECARD");
	PUERTO_GAMECARD = config_get_string_value(config, "PUERTO_GAMECARD");
	MY_ID = atoi(config_get_string_value(config, "MY_ID"));



	pthread_create(&gameboy_thread, NULL, (void*) gameboy_function, NULL);
	pthread_create(&new_thread, NULL, (void*) new_function, NULL);
	pthread_create(&catch_thread, NULL, (void*) catch_function, NULL);
	pthread_create(&get_thread, NULL, (void*) get_function, NULL);



	pthread_join(gameboy_thread, NULL);
	pthread_join(new_thread, NULL);
	pthread_join(catch_thread, NULL);
	pthread_join(get_thread, NULL);

	return EXIT_SUCCESS;
}

void gameboy_function(void){
	//TODO crear servidor para atender mensajes de gameboy
}

void new_function(void){

	//int32_t socket_cliente = suscribe_to_broker(COLA_NEW);

// TODO cambiar por la de arriba
	int32_t socket_cliente = connect_to_server(IP_BROKER, PUERTO_BROKER, logger);

	t_package* suscription_package = serialize_suscripcion(MY_ID, COLA_NEW);

	send_paquete(socket_cliente, suscription_package);
	if (receive_ACK(socket_cliente, logger) == -1) {
		exit(EXIT_FAILURE);
	}
//------------------------------------

	struct thread_args* args = malloc(sizeof(struct thread_args));
	args->socket = socket_cliente;
	args->logger = logger;
	args->function = &atender_new;

	listen_messages(args);

	log_warning(logger, "Aca nunca llego");
}

void catch_function(void){

	//int32_t socket_cliente = suscribe_to_broker(COLA_CATCH);

// TODO cambiar por la de arriba
		int32_t socket_cliente = connect_to_server(IP_BROKER, PUERTO_BROKER, logger);

		t_package* suscription_package = serialize_suscripcion(MY_ID, COLA_CATCH);

		send_paquete(socket_cliente, suscription_package);
		if (receive_ACK(socket_cliente, logger) == -1) {
			exit(EXIT_FAILURE);
		}
//------------------------------------

	struct thread_args* args = malloc(sizeof(struct thread_args));
	args->socket = socket_cliente;
	args->logger = logger;
	args->function = &atender_catch;

	listen_messages(args);

	log_warning(logger, "Aca nunca llego");
}

void get_function(void){

	//int32_t socket_cliente = suscribe_to_broker(COLA_GET);

// TODO cambiar por la de arriba
		int32_t socket_cliente = connect_to_server(IP_BROKER, PUERTO_BROKER, logger);

		t_package* suscription_package = serialize_suscripcion(MY_ID, COLA_GET);

		send_paquete(socket_cliente, suscription_package);
		if (receive_ACK(socket_cliente, logger) == -1) {
			exit(EXIT_FAILURE);
		}
//------------------------------------

	struct thread_args* args = malloc(sizeof(struct thread_args));
	args->socket = socket_cliente;
	args->logger = logger;
	args->function = &atender_get;

	listen_messages(args);

	log_warning(logger, "Aca nunca llego");
}

int32_t subscribe_to_broker(queue_code queue_code) {

	int32_t socket = connect_to_server(IP_BROKER, PUERTO_BROKER, logger);

	t_package* suscription_package = serialize_suscripcion(MY_ID, queue_code);

	send_paquete(socket, suscription_package);
	if (receive_ACK(socket, logger) == -1) {
		exit(EXIT_FAILURE);
	}

	return socket;
}

void send_to_broker(t_package* package){

	int32_t socket = connect_to_server(IP_BROKER, PUERTO_BROKER, logger);
	send_paquete(socket, package);
	//TODO reintentar envio si falla con la variable global TIEMPO_DE_REINTENTO_CONEXION

	receive_ID(socket, logger);
	send_ACK(socket, logger);

}

void atender_new(operation_code op_code, void* message){
	if(op_code != OPERATION_NEW)
		log_error(logger, "Aca nunca llego");

	t_message_new* message_new = (t_message_new*) message;

	//TODO guardar el mensaje new en el filesystem

	//Generar mensaje APPEARED
	t_message_appeared* message_appeared = create_message_appeared_long(message_new->id, message_new->pokemon_name, message_new->location->position->x, message_new->location->position->y);
	log_info(logger, "Se genero el mensaje appeared");
	destroy_message_new(message_new);

	t_package* package = serialize_appeared(message_appeared);
	//destroy_message_appeared(message_appeared);	//se deberia borrar, probar si no rompe

	pthread_t thread;
	pthread_create(&thread, NULL, (void*) send_to_broker, (void*)package);
	pthread_detach(thread);

}

void atender_catch(operation_code op_code, void* message){
	if(op_code != OPERATION_CATCH)
		log_error(logger, "Aca nunca llego");

	t_message_catch* message_catch = (t_message_catch*) message;

	//TODO verificar en el filesystem si el pokemon esta disponible para atrapar

	//Generar mensaje CAUGHT												//TODO remover harcodeo true
	t_message_caught* message_caught = create_message_caught(message_catch->id, true);
	log_info(logger, "Se genero el mensaje caught");
	destroy_message_catch(message_catch);

	t_package* package = serialize_caught(message_caught);
	//destroy_message_caught(message_caught);	//se deberia borrar, probar si no rompe

	pthread_t thread;
	pthread_create(&thread, NULL, (void*) send_to_broker, (void*)package);
	pthread_detach(thread);

}

void atender_get(operation_code op_code, void* message){
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
	//destroy_message_localized(message_localized);	//se deberia borrar, probar si no rompe

	pthread_t thread;
	pthread_create(&thread, NULL, (void*) send_to_broker, (void*)package);
	pthread_detach(thread);

}

