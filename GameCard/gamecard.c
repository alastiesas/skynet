#include "gamecard.h"

void config_init(){
	TIEMPO_DE_REINTENTO_CONEXION = atoi(config_get_string_value(config, "TIEMPO_DE_REINTENTO_CONEXION"));
	TIEMPO_DE_REINTENTO_OPERACION = atoi(config_get_string_value(config, "TIEMPO_DE_REINTENTO_OPERACION"));
	TIEMPO_RETARDO_OPERACION = atoi(config_get_string_value(config, "TIEMPO_RETARDO_OPERACION"));
	PUNTO_MONTAJE_TALLGRASS = config_get_string_value(config, "PUNTO_MONTAJE_TALLGRASS");
	IP_BROKER = config_get_string_value(config, "IP_BROKER");
	PUERTO_BROKER = config_get_string_value(config, "PUERTO_BROKER");
	IP_GAMECARD = config_get_string_value(config, "IP_GAMECARD");
	PUERTO_GAMECARD = config_get_string_value(config, "PUERTO_GAMECARD");
	MY_ID = atoi(config_get_string_value(config, "MY_ID"));
}

int main(void) {
	puts("!!!Hello World!!!"); /* prints !!!Hello World!!! */

	logger = log_create("gamecard.log", "gameCard", LOG_CONSOLE, LOG_LEVEL_TRACE);
	if((config = config_create("gamecard.config")) == NULL)
		log_error(logger, "ERROR DE CONFIG");

	config_init();

	init_fs();
	// initiliaze_file_system();


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

