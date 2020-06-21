#include "broker.h"

void initialization() {

	generic_initialization();
	specific_initialization();
	initialize_queues();
	config_init();
	semaphores_init();
}

void generic_initialization() {

	logger = log_create("broker.log", "broker", LOG_CONSOLE, LOG_LEVEL_TRACE);
	if((config = config_create("broker.config")) == NULL)
		log_error(logger, "ERROR DE CONFIG");
	IP = config_get_string_value(config, "IP_BROKER");
	PORT = config_get_string_value(config, "PUERTO_BROKER");
}

void initialize_queues() {
	queues = malloc(sizeof(t_queues));

	queues->APPEARED_POKEMON = list_create();
	suscribers->APPEARED = list_create();
	queues->CATCH_POKEMON = list_create();
	suscribers->CATCH = list_create();
	queues->CAUGHT_POKEMON = list_create();
	suscribers->CAUGHT = list_create();
	queues->GET_POKEMON = list_create();
	suscribers->GET = list_create();
	queues->LOCALIZED_POKEMON = list_create();
	suscribers->LOCALIZED = list_create();
	queues->NEW_POKEMON = list_create();
	suscribers->NEW = list_create();

	queues->NEW_POKEMON_IDS = list_create();
	queues->APPEARED_POKEMON_IDS = list_create();
	queues->GET_POKEMON_IDS = list_create();
	queues->LOCALIZED_POKEMON_IDS = list_create();
	queues->CATCH_POKEMON_IDS = list_create();
	queues->CAUGHT_POKEMON_IDS = list_create();
}

void specific_initialization() {

	ID_GLOBAL = 1;
	pthread_mutex_init(&mutex_ID_global, NULL);
	pthread_mutex_init(&(mutex_cache), NULL);

	count = 0;

	//memory_init();

	suscribers = malloc(sizeof(t_suscribers));


	total_new_messages = 0;
	total_appeared_messages = 0;
	total_catch_messages = 0;
	total_caught_messages = 0;
	total_get_messages = 0;
	total_localized_messages = 0;


}

void config_init(){
	memory_algorithm = config_get_string_value(config, "ALGORITMO_MEMORIA");
	if(strcmp(memory_algorithm, "DEFAULT") == 0){
		log_info(logger, "Algoritmo memoria: %s", memory_algorithm);
	}
	else{
		if(strcmp(memory_algorithm, "PARTICIONES") != 0 && strcmp(memory_algorithm, "BS") != 0){
			log_error(logger, "error ALGORITMO_MEMORIA");
			exit(-1);
		}
		log_info(logger, "Algoritmo memoria: %s", memory_algorithm);

		memory_size = atoi(config_get_string_value(config, "TAMANO_MEMORIA"));
		log_info(logger, "Tamano memoria: %d", memory_size);
		//TODO comprobar que sea 2^n para buddy

		min_partition_size = atoi(config_get_string_value(config, "TAMANO_MINIMO_PARTICION"));
		log_info(logger, "Tamano minimo particion: %d", min_partition_size);
		compaction_frequency = atoi(config_get_string_value(config, "FRECUENCIA_COMPACTACION"));
		log_info(logger, "Frecuencia compactacion: %d", compaction_frequency);



		free_partition_algorithm = config_get_string_value(config, "ALGORITMO_PARTICION_LIBRE");
		if(strcmp(free_partition_algorithm, "FF") != 0 && strcmp(free_partition_algorithm, "BF") != 0){
			log_error(logger, "error ALGORITMO_PARTICION_LIBRE");
			exit(-1);
		}
		log_info(logger, "Algoritmo particion libre: %s", free_partition_algorithm);

		replacement_algorithm = config_get_string_value(config, "ALGORITMO_REEMPLAZO");
		if(strcmp(replacement_algorithm, "FIFO") != 0 && strcmp(replacement_algorithm, "LRU") != 0){
			log_error(logger, "error ALGORITMO_REEMPLAZO");
			exit(-1);
		}
		log_info(logger, "Algoritmo reemplazo: %s", replacement_algorithm);
	}

}

void semaphores_init(){
	semaphores_new = malloc(sizeof(t_semaforos));
	semaphores_appeared = malloc(sizeof(t_semaforos));
	semaphores_get = malloc(sizeof(t_semaforos));
	semaphores_localized = malloc(sizeof(t_semaforos));
	semaphores_catch = malloc(sizeof(t_semaforos));
	semaphores_caught = malloc(sizeof(t_semaforos));


	pthread_mutex_init(&(semaphores_new->mutex_cola), NULL);
	pthread_mutex_init(&(semaphores_appeared->mutex_cola), NULL);
	pthread_mutex_init(&(semaphores_get->mutex_cola), NULL);
	pthread_mutex_init(&(semaphores_localized->mutex_cola), NULL);
	pthread_mutex_init(&(semaphores_catch->mutex_cola), NULL);
	pthread_mutex_init(&(semaphores_caught->mutex_cola), NULL);

	pthread_mutex_init(&(semaphores_new->mutex_subs), NULL);
	pthread_mutex_init(&(semaphores_appeared->mutex_subs), NULL);
	pthread_mutex_init(&(semaphores_get->mutex_subs), NULL);
	pthread_mutex_init(&(semaphores_localized->mutex_subs), NULL);
	pthread_mutex_init(&(semaphores_catch->mutex_subs), NULL);
	pthread_mutex_init(&(semaphores_caught->mutex_subs), NULL);

	pthread_cond_init(&(semaphores_new->broadcast), NULL);
	pthread_cond_init(&(semaphores_appeared->broadcast), NULL);
	pthread_cond_init(&(semaphores_get->broadcast), NULL);
	pthread_cond_init(&(semaphores_localized->broadcast), NULL);
	pthread_cond_init(&(semaphores_catch->broadcast), NULL);
	pthread_cond_init(&(semaphores_caught->broadcast), NULL);
}

void behavior() {

	pthread_create(&listening_thread, NULL, (void*) listening, NULL);
	pthread_join(listening_thread, NULL);
}

void listening() {

	iniciar_servidor_broker(); //pending clean
}

void termination() {

	specific_termination();
}

void specific_termination() {

	list_destroy(queues->NEW_POKEMON);
	list_destroy(queues->APPEARED_POKEMON);
	list_destroy(queues->GET_POKEMON);
	list_destroy(queues->LOCALIZED_POKEMON);
	list_destroy(queues->CATCH_POKEMON);
	list_destroy(queues->CAUGHT_POKEMON);
	list_destroy(queues->NEW_POKEMON_IDS);
	list_destroy(queues->APPEARED_POKEMON_IDS);
	list_destroy(queues->GET_POKEMON_IDS);
	list_destroy(queues->LOCALIZED_POKEMON_IDS);
	list_destroy(queues->CATCH_POKEMON_IDS);
	list_destroy(queues->CAUGHT_POKEMON_IDS);
	free(queues);
	free(suscribers);
	free(semaphores_new);
	free(semaphores_appeared);
	free(semaphores_get);
	free(semaphores_localized);
	free(semaphores_catch);
	free(semaphores_caught);
}
