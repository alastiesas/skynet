#include "gameboy.h"
#include <conexion.h>
#include <mensajes.h>

void subscribe_timed(queue_code queue_code, int time) {

	log_info(gameboy_behavior_log, " 3. subscription instruction");


	ip = config_get_string_value(gameboy_config, "IP_BROKER");
	port = config_get_string_value(gameboy_config, "PUERTO_BROKER");

	int32_t socket = connect_to_server(ip, port, 4, 4, gameboy_log); //TODO el gameboy no tiene tiempo de reintento?


	strcpy(log_line, " 1. connected to process broker (ip:");
	strcat(log_line, ip);
	strcat(log_line, " port:");
	strcat(log_line, port);
	strcat(log_line, ")");
	log_info(gameboy_log, log_line);


	char* id = config_get_string_value(gameboy_config, "ID");
	t_package* suscription_package = serialize_suscripcion(atoi(id), queue_code);

	send_paquete(socket, suscription_package); /*pending2*/
	if (receive_ACK(socket, gameboy_log) == -1) {
		exit(EXIT_FAILURE);
	}


	char* queue_name;
	strcpy(log_line, " 2. subscribed to queue ");

	switch (queue_code) {
	case 1:
		queue_name = "new";
		break;
	case 2:
		queue_name = "appeared";
		break;
	case 3:
		queue_name = "get";
		break;
	case 4:
		queue_name = "localized";
		break;
	case 5:
		queue_name = "catch";
		break;
	case 6:
		queue_name = "caught";
		break;
	}

	strcat(log_line, queue_name);
	log_info(gameboy_log, log_line);


	struct thread_args* args = malloc(sizeof(struct thread_args));
	args->socket = socket;
	args->logger = gameboy_log;
	args->function = &process_free;

	printf("Se suscribe por %d segundos\n", time);
	//se crea un thread para recibir muchos mensajes
	pthread_t thread;
	pthread_create(&thread, NULL, (void*) listen_messages, args);

	//quedo vivo hasta que termine el tiempo de suscripcion
	sleep(time);	//TODO si se cierra la conexion, se queda en el sleep sin hacer nada, hay que reconectar?

	log_info(gameboy_log, "El gameboy no reconecta");

	//printf("TODO: enviar mensaje de fin de suscripcion para liberar memoria del broker\n");

}

void team_send_package(int process_code, char* ip, char* port, t_package* package) {

	log_info(gameboy_behavior_log, " 3. message instruction");


	int32_t socket = connect_to_server(ip, port, 4, 4, gameboy_log);


	char* process_name;
	strcpy(log_line, " 1. connected to process ");

	switch (process_code) {
	case 1:
		process_name = "broker";
		break;
	case 2:
		process_name = "gamecard";
		break;
	case 3:
		process_name = "team";
		break;
	}

	strcat(log_line, process_name);
	strcat(log_line, " (ip:");
	strcat(log_line, ip);
	strcat(log_line, " port:");
	strcat(log_line, port);
	strcat(log_line, ")");
	log_info(gameboy_log, log_line);


	send_paquete(socket, package);

	receive_ID(socket, gameboy_log);
	send_ACK(socket, gameboy_log);
	close(socket);
}

void process_free(void* input){

	operation_code operation_code = ((serve_thread_args*)input)->op_code;
	void* message = ((serve_thread_args*)input)->message;


	char* operation_name;
	uint32_t amount = -1;
	uint32_t correlative_id = -1;
	uint32_t id;
	char* pokemon_name;
	//define t_position* positions;
	//define uint32_t positions_amount;
	uint32_t result = -1;
	uint32_t pokemon_name_size;
	uint32_t x = -1;
	uint32_t y = -1;
	strcpy(log_line, " 3. message ");

	switch (operation_code) {
	case 1:
		operation_name = "new";
		amount = ((t_message_new*) message)->location->amount;
		id = ((t_message_new*) message)->id;
		pokemon_name = ((t_message_new*) message)->pokemon_name;
		pokemon_name_size = ((t_message_new*) message)->size_pokemon_name;
		x = ((t_message_new*) message)->location->position->x;
		y = ((t_message_new*) message)->location->position->y;

		free(((t_message_new*) message)->pokemon_name);
		free(((t_message_new*) message)->location->position);
		free(((t_message_new*) message)->location);
		break;
	case 2:
		operation_name = "appeared";
		id = ((t_message_appeared*) message)->id;
		correlative_id = ((t_message_appeared*) message)->correlative_id;
		pokemon_name = ((t_message_appeared*) message)->pokemon_name;
		pokemon_name_size = ((t_message_appeared*) message)->size_pokemon_name;
		x = ((t_message_appeared*) message)->position->x;
		y = ((t_message_appeared*) message)->position->y;

		free(((t_message_appeared*) message)->pokemon_name);
		free(((t_message_appeared*) message)->position);
		break;
	case 3:
		operation_name = "get";
		id = ((t_message_get*) message)->id;
		pokemon_name = ((t_message_get*) message)->pokemon_name;
		pokemon_name_size = ((t_message_get*) message)->size_pokemon_name;

		free(((t_message_get*) message)->pokemon_name);
		break;
	case 4:
		operation_name = "localized";
		id = ((t_message_localized*) message)->id;
		correlative_id = ((t_message_localized*) message)->correlative_id;
		pokemon_name = ((t_message_localized*) message)->pokemon_name;
		pokemon_name_size = ((t_message_localized*) message)->size_pokemon_name;
		// define positions
		// define positions_amount =  ((t_message_localized*) message)->position_amount;

		free(((t_message_localized*) message)->pokemon_name);
		free(((t_message_localized*) message)->positions);
		break;
	case 5:
		operation_name = "catch";
		id = ((t_message_catch*) message)->id;
		pokemon_name = ((t_message_catch*) message)->pokemon_name;
		pokemon_name_size = ((t_message_catch*) message)->size_pokemon_name;
		x = ((t_message_catch*) message)->position->x;
		y = ((t_message_catch*) message)->position->y;

		free(((t_message_catch*) message)->pokemon_name);
		free(((t_message_catch*) message)->position);
		break;
	case 6:
		operation_name = "caught ";
		id = ((t_message_caught*) message)->id;
		correlative_id = ((t_message_caught*) message)->correlative_id;
		result = ((t_message_caught*) message)->result;

		break;
	}

	strcat(log_line, operation_name);
	strcat(log_line, " received (id:");
	strcat(log_line, string_itoa(id));
	if (correlative_id != -1) {
		strcat(log_line, " correlative_id:");
		strcat(log_line, string_itoa(correlative_id));
	}
	/*fixme rompe para caught
	if (pokemon_name_size != -1) {
		strcat(log_line, " pokemon_name_size:");
		strcat(log_line, string_itoa(pokemon_name_size));
		strcat(log_line, " pokemon_name:");
		strcat(log_line, pokemon_name);
	}
	fixme rompe para caught*/
	// define if (positions_amount != NULL) {
	//	}
	// define if (positions != NULL) {
	//	}
	if (amount != -1) {
		strcat(log_line, " amount:");
		strcat(log_line, string_itoa(amount));
	}
	if (x != -1) {
		strcat(log_line, " x:");
		strcat(log_line, string_itoa(x));
		strcat(log_line, " y:");
		strcat(log_line, string_itoa(y));
	}
	if (result != -1) {
		strcat(log_line, " result:");
		strcat(log_line, string_itoa(result));
	}
	strcat(log_line, ")");
	log_info(gameboy_log, log_line);
}
