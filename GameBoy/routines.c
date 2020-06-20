#include "gameboy.h"
#include <conexion.h>
#include <mensajes.h>

void subscribe_timed(queue_code queue_code, int time) {

	ip = config_get_string_value(config, "BROKER_IP");
	port = config_get_string_value(config, "BROKER_PORT");

	int32_t socket = connect_to_server(ip, port, 4, logger); //TODO el gameboy no tiene tiempo de reintento?

	char* id = config_get_string_value(config, "ID");
	t_package* suscription_package = serialize_suscripcion(atoi(id), queue_code);

	send_paquete(socket, suscription_package); /*pending2*/
	if (receive_ACK(socket, logger) == -1) {
		exit(EXIT_FAILURE);
	}

	struct thread_args* args = malloc(sizeof(struct thread_args));
	args->socket = socket;
	args->logger = logger;
	args->function = &process_free;

	//se crea un thread para recibir muchos mensajes
	pthread_t thread;
	pthread_create(&thread, NULL, (void*) listen_messages, args);

	//quedo vivo hasta que termine el tiempo de suscripcion
	sleep(time);

	log_info(logger, "El gameboy no reconecta");
	printf("TODO: enviar mensaje de fin de suscripcion para liberar memoria del broker\n");

	printf("TODO: ver como se deben mostrar los mensajes recibidos\n");

}


void send_message(char* ip, char* port, t_package* package) {

	int32_t socket = connect_to_server(ip, port, 4, logger); //TODO el gameboy no tiene tiempo de reintento?
	send_paquete(socket, package);

	receive_ID(socket, logger);
	send_ACK(socket, logger);
}

void process_free(void* input){
	operation_code op_code = ((serve_thread_args*)input)->op_code;
	void* message = ((serve_thread_args*)input)->message;

	printf("Estoy en la funcion\n");

	if(op_code == OPERATION_NEW){
		free(((t_message_new*)message)->pokemon_name);
		free(((t_message_new*)message)->location->position);
		free(((t_message_new*)message)->location);
		printf("Se libero el mensaje new\n");
	}

	if(op_code == OPERATION_CATCH){
		free(((t_message_catch*)message)->pokemon_name);
		free(((t_message_catch*)message)->position);
		printf("Se libero el mensaje catch\n");
	}
}
