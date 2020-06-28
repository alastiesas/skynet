#include "gameboy.h"
#include <conexion.h>
#include <mensajes.h>

void subscribe_timed(queue_code queue_code, int time) {

	puts(" 4. subscription instruction");

	ip = config_get_string_value(gameboy_config, "IP_BROKER");
	port = config_get_string_value(gameboy_config, "PUERTO_BROKER");

	int32_t socket = connect_to_server(ip, port, 4, gameboy_log); //TODO el gameboy no tiene tiempo de reintento?

	char* id = config_get_string_value(gameboy_config, "ID");
	t_package* suscription_package = serialize_suscripcion(atoi(id), queue_code);

	send_paquete(socket, suscription_package); /*pending2*/
	if (receive_ACK(socket, gameboy_log) == -1) {
		exit(EXIT_FAILURE);
	}

	struct thread_args* args = malloc(sizeof(struct thread_args));
	args->socket = socket;
	args->logger = gameboy_log;
	args->function = &process_free;

	//se crea un thread para recibir muchos mensajes
	pthread_t thread;
	pthread_create(&thread, NULL, (void*) listen_messages, args);

	//quedo vivo hasta que termine el tiempo de suscripcion
	sleep(time);	//TODO si se cierra la conexion, se queda en el sleep sin hacer nada, hay que reconectar?

	log_info(gameboy_log, "El gameboy no reconecta");
	printf("TODO: enviar mensaje de fin de suscripcion para liberar memoria del broker\n");

	printf("TODO: ver como se deben mostrar los mensajes recibidos\n");

}


void send_message(char* ip, char* port, t_package* package) {

	int32_t socket = connect_to_server(ip, port, 4, gameboy_log); //TODO el gameboy no tiene tiempo de reintento?
	send_paquete(socket, package);

	receive_ID(socket, gameboy_log);
	send_ACK(socket, gameboy_log);
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
