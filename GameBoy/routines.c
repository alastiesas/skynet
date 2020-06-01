#include "gameboy.h"
#include <conexion.h>

void subscribe_timed(queue_code queue_code, int time) {

	ip = config_get_string_value(config, "BROKER_IP");
	port = config_get_string_value(config, "BROKER_PORT");
	int32_t socket = connect_to_server(ip, port, logger);

	uint32_t id = config_get_string_value(config, "ID");
	t_package* suscription_package = serialize_suscripcion(atoi(id), queue_code);

	send_paquete(socket, suscription_package); /*pending2*/
	if (receive_ACK(socket, logger) == -1) {
		exit(EXIT_FAILURE);
	}

	struct thread_args* args = malloc(sizeof(struct thread_args));
	args->socket = socket;
	args->logger = logger;
	pthread_t thread;
	pthread_create(&thread, NULL, (void*) recibir_muchos_mensajes, args); /*pending2*/

	sleep(time);

	printf("TODO: enviar mensaje de fin de suscripcion para liberar memoria del broker\n");

	printf("TODO: ver como se deben mostrar los mensajes recibidos\n");
}


void send_message(char* ip, char* port, t_package* package) {

	int32_t socket = connect_to_server(ip, port, logger);
	send_paquete(socket, package);

	receive_ID(socket, logger);
	send_ACK(socket, logger);
}
