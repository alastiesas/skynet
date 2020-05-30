#include "broker.h"

void initialization() {

	generic_initialization();
	specific_initialization();
}

void generic_initialization() {

	ID_GLOBAL = 1;
	logger = log_create("broker.log", "broker", LOG_CONSOLE, LOG_LEVEL_INFO); //pending clean
	config = config_create("broker.config"); //pending clean
	IP_BROKER = config_get_string_value(config, "IP_BROKER");
	PORT = config_get_string_value(config, "PORT");
}

void initialize_queues() {
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

	queues = malloc(sizeof(t_colas));
	suscribers = malloc(sizeof(t_suscriptores));
	semaphores_new = malloc(sizeof(t_semaforos));
	semaphores_appeared = malloc(sizeof(t_semaforos));
	semaphores_get = malloc(sizeof(t_semaforos));
	semaphores_localized = malloc(sizeof(t_semaforos));
	semaphores_catch = malloc(sizeof(t_semaforos));
	semaphores_caught = malloc(sizeof(t_semaforos));

	initialize_queues();

	pthread_mutex_init(&mutex_ID_global, NULL);

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

	sem_init(&(semaphores_new->nuevo_mensaje), 0, 0);

	pthread_mutex_init(&(semaphores_new->received), NULL);
	pthread_cond_init(&(semaphores_new->broadcast), NULL);
}

void behavior() {

	pthread_create(&listening_thread, NULL, (void*) listening, NULL);
	while (1) {} //pending clean
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
