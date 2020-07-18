#include "gamecard.h"

void config_init(){
	TIEMPO_DE_REINTENTO_CONEXION = config_get_int_value(config, "TIEMPO_DE_REINTENTO_CONEXION");
	TIEMPO_DE_REINTENTO_OPERACION = config_get_int_value(config, "TIEMPO_DE_REINTENTO_OPERACION");
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



	puts("!!!Hello World!!!"); /* prints !!!Hello World!!! */
	//
	logger = log_create("gamecard.log", "gameCard", LOG_CONSOLE, LOG_LEVEL_TRACE);
	if((config = config_create("gamecard.config")) == NULL)
		log_error(logger, "ERROR DE CONFIG");

	config_init();

	init_fs();
	// initiliaze_file_system();
	t_location* location = malloc(sizeof(t_location));
	location->position = malloc(sizeof(t_position));
	location->position->x = 9;
	location->position->y = 9;
	location->amount = 2;

	//semaphores = dictionary_create();
	pthread_mutex_init(&mutex_bitmap, NULL);
	pthread_mutex_init(&mutex_pkmetadata, NULL);
	//pthread_mutex_init(&semaforo_del_diccionario_de_semaforos_JAJAJA, NULL);

	create_pokemon_directory("Snorlax",location);
	printf("aca estamos bien3\n");
	t_message_catch* msg_catch = create_message_catch_long("Snorlax", 3,2);
	process_catch(msg_catch);
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

void terminate_gamecard() {
	free(bitmap_path);
	free(files_metadata_path);
	free(metadata_path);
	config_destroy(config);

	pthread_mutex_destroy(&mutex_bitmap);
	//dictionary_destroy_and_destroy_elements(semaphores, pthread_mutex_destroy);
}
