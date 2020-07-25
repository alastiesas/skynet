#include "gamecard.h"

void config_init(){
	TIEMPO_DE_REINTENTO_CONEXION = config_get_int_value(config, "TIEMPO_DE_REINTENTO_CONEXION");
	TIEMPO_DE_REINTENTO_OPERACION = config_get_int_value(config, "TIEMPO_DE_REINTENTO_OPERACION");
	CANTIDAD_DE_REINTENTOS_CONEXION = config_get_int_value(config, "CANTIDAD_DE_REINTENTOS_CONEXION");
	TIEMPO_RETARDO_OPERACION = config_get_int_value(config, "TIEMPO_RETARDO_OPERACION");
	PUNTO_MONTAJE_TALLGRASS = config_get_string_value(config, "PUNTO_MONTAJE_TALLGRASS");
	IP_BROKER = config_get_string_value(config, "IP_BROKER");
	PUERTO_BROKER = config_get_string_value(config, "PUERTO_BROKER");
	IP_GAMECARD = config_get_string_value(config, "IP_GAMECARD");
	PUERTO_GAMECARD = config_get_string_value(config, "PUERTO_GAMECARD");
	MY_ID = config_get_int_value(config, "MY_ID");

	metadata_directory = string_new();
	string_append(&metadata_directory, PUNTO_MONTAJE_TALLGRASS);
	string_append(&metadata_directory, "/Metadata/");
	metadata_path = string_new();
	string_append(&metadata_path, metadata_directory);
	string_append(&metadata_path, "Metadata.bin");
	bitmap_path = string_new();
	string_append(&bitmap_path, metadata_directory);
	string_append(&bitmap_path, "Bitmap.bin");

	files_directory = string_new();
	string_append(&files_directory, PUNTO_MONTAJE_TALLGRASS);
	string_append(&files_directory, "/Files/");
	files_metadata_path = string_new();
	string_append(&files_metadata_path, files_directory);
	string_append(&files_metadata_path, "Metadata.bin");

	blocks_directory = string_new();
	string_append(&blocks_directory, PUNTO_MONTAJE_TALLGRASS);
	string_append(&blocks_directory,"/Blocks/");
}

int main(void) {

	helper = log_create("gamecard_info.log", "gameCard", true, LOG_LEVEL_TRACE);
	if((config = config_create("gamecard.config")) == NULL){
		printf("ERROR DE CONFIG");
		exit(-1);
	}
	char* log_debug = config_get_string_value(config, "LOG_DEBUG");
	if(strcmp(log_debug, "FALSE") == 0)
		log_debug_console = false;
	else
		log_debug_console = true;
	logger = log_create("gamecard.log", "gameCard", log_debug_console, LOG_LEVEL_TRACE);

	config_init();

	init_fs();

	pthread_mutex_init(&mutex_bitmap, NULL);
	pthread_mutex_init(&mutex_pkmetadata, NULL);


	pthread_create(&gameboy_thread, NULL, (void*) gameboy_function, NULL);
	pthread_create(&new_thread, NULL, (void*) new_function, NULL);
	pthread_create(&catch_thread, NULL, (void*) catch_function, NULL);
	pthread_create(&get_thread, NULL, (void*) get_function, NULL);

	pthread_join(gameboy_thread, NULL);
	pthread_join(new_thread, NULL);
	pthread_join(catch_thread, NULL);
	pthread_join(get_thread, NULL);

	terminate_gamecard();

	return EXIT_SUCCESS;
}

void terminate_gamecard(){
	free(bitmap_path);
	free(files_metadata_path);
	free(metadata_path);
	config_destroy(config);

	pthread_mutex_destroy(&mutex_bitmap);
	pthread_mutex_destroy(&mutex_pkmetadata);
}
