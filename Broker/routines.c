#include "broker.h"

void initialization() {

	generic_initialization();
	specific_initialization();
}

void generic_initialization() {

	log = log_create("broker.log", "broker", LOG_CONSOLE, LOG_LEVEL_INFO); //pending clean
	config = config_create("broker.config"); //pending clean
}

void specific_initialization() {

	queues = malloc(sizeof(t_colas));
	suscribers = malloc(sizeof(t_suscriptores));
	semaphores = malloc(sizeof(t_semaforos));
	queues->NEW = queue_create();
	suscribers->NEW = list_create();
	pthread_mutex_init(&(semaphores->mutex_cola_new), NULL);
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

	queue_destroy(queues->NEW);
	free(queues);
	free(suscribers);
	free(semaphores);
}
