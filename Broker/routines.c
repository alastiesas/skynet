#include "broker.h"

void initialization() {

	general_initialization("broker");
	specific_initialization();
}

void general_initialization(char* process_name) {

	log = log_create(strcat(process_name, ".log"), process_name, LOG_CONSOLE, LOG_LEVEL_INFO);
	config = config_create(strcat(process_name, ".config"));
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

	pthread_create(&hilo_LISTEN, NULL, (void*) listening, NULL);
	/**/
	int elementos;
	while (1) {
		sem_wait(&(semaphores->nuevo_new));
		//wait semaforo para decir la cantidad de elementos en la cola de new
		elementos = queue_size(queues->NEW);
		log_info(log, "La cola de NEW tiene, %d elementos\n", elementos);
	}
	/**/
}

void listening() {

	/**/
	t_log* logger = initialize_thread("Broker", "Todos", hilo_LISTEN);
	char* puerto;
	puerto = "6001"; //	puerto = config_get_string_value(config, "PUERTO_BROKER");
	log_info(logger, "Iniciando servidor en el puerto: %s", puerto);
	iniciar_servidor_broker(puerto, logger, queues, suscribers, semaphores);
	/**/
}

void termination() {

	void specific_termination();
}

void specific_termination() {

	queue_destroy(queues->NEW);
	free(queues);
	free(suscribers);
	free(semaphores);
}
