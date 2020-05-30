#include "gameboy.h"
#include <conexion.h>

void subscribe(queue_code queue_code, int time) {

	char* ip = config_get_string_value(config, "BROKER_IP");
	char* port = config_get_string_value(config, "BROKER_PORT");
	int32_t socket = connect_to_server(ip, port, log);

	uint32_t id = config_get_string_value(config, "ID");
	t_paquete* package = serialize_suscripcion(atoi(id), queue_code);

	send_paquete(socket, package); /*pending2*/
	if (receive_ACK(socket, log) == -1) {
		exit(EXIT_FAILURE);
	}

	struct thread_args* args = malloc(sizeof(struct thread_args));
	args->socket = socket;
	args->logger = log;
	pthread_t thread;
	pthread_create(&thread, NULL, (void*) recibir_muchos_mensajes,
			args); /*pending2*/

	sleep(time);
}


void send(char* ip, char* port, t_paquete* package) {

	int32_t socket = connect_to_server(ip, port, log);
	send_paquete(socket, package);

	receive_ID(socket, log);
	send_ACK(socket, log);
}
