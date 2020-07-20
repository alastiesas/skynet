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
/*
	t_message_new* message_new_1 = create_message_new_long("example1", 10, 10,
			15);
	new_pokemon_routine(message_new_1);

	t_message_new* message_new_2 = create_message_new_long("example1", 10, 10,
			15);
	new_pokemon_routine(message_new_2);

	t_message_new* message_new_3 = create_message_new_long("example2", 10, 10,
				15);
		new_pokemon_routine(message_new_3);

	t_message_new* message_new_4 = create_message_new_long("example3", 15, 20,
			30);
	new_pokemon_routine(message_new_4);

	t_message_new* message_new_5 = create_message_new_long("example2", 15, 84,
			2);
	new_pokemon_routine(message_new_5);
	t_message_new* message_new_11 = create_message_new_long("example1", 10, 7,
				15);
		new_pokemon_routine(message_new_11);

		t_message_new* message_new_21 = create_message_new_long("example1", 10, 10,
				15);
		new_pokemon_routine(message_new_21);

		t_message_new* message_new_31 = create_message_new_long("example2", 8, 95,
					15);
			new_pokemon_routine(message_new_31);

		t_message_new* message_new_41 = create_message_new_long("example3", 73, 20,
				30);
		new_pokemon_routine(message_new_41);

		t_message_new* message_new_51 = create_message_new_long("example2", 68, 84,
				2);
		new_pokemon_routine(message_new_51);
*/
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


	/*
	//create_pokemon_directory("Snorlax");
	printf("aca estamos bien3\n");
	t_message_catch* msg_catch = create_message_catch_long("Snorlax", 3,2);
	process_catch(msg_catch);


	t_message_get* message_get = create_message_get("Snorlax");
	t_message_localized* message_localized = process_get(message_get);
*/




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
