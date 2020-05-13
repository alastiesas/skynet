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
	queues->APPEARED_POKEMON = queue_create();
	suscribers->APPEARED = list_create();
	queues->CATCH_POKEMON = queue_create();
	suscribers->CATCH = list_create();
	queues->CAUGHT_POKEMON = queue_create();
	suscribers->CAUGHT = list_create();
	queues->GET_POKEMON = queue_create();
	suscribers->GET = list_create();
	queues->LOCALIZED_POKEMON = queue_create();
	suscribers->LOCALIZED = list_create();
	queues->NEW_POKEMON = queue_create();
	suscribers->NEW = list_create();
}

void specific_initialization() {

	queues = malloc(sizeof(t_colas));
	suscribers = malloc(sizeof(t_suscriptores));
	semaphores = malloc(sizeof(t_semaforos));

	initialize_queues();

	pthread_mutex_init(&(semaphores->mutex_cola_new), NULL);
	pthread_mutex_init(&(semaphores->mutex_cola_appeared), NULL);
	pthread_mutex_init(&(semaphores->mutex_cola_get), NULL);
	pthread_mutex_init(&(semaphores->mutex_cola_localized), NULL);
	pthread_mutex_init(&(semaphores->mutex_cola_catch), NULL);
	pthread_mutex_init(&(semaphores->mutex_cola_caught), NULL);
	sem_init(&(semaphores->nuevo_new), 0, 0);
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

	queue_destroy(queues->NEW_POKEMON);
	queue_destroy(queues->APPEARED_POKEMON);
	queue_destroy(queues->GET_POKEMON);
	queue_destroy(queues->LOCALIZED_POKEMON);
	queue_destroy(queues->CATCH_POKEMON);
	queue_destroy(queues->CAUGHT_POKEMON);
	free(queues);
	free(suscribers);
	free(semaphores);
}
