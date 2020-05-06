/*
 ============================================================================
 Name        : broker.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "broker.h"


int main(void) {
	puts("!!!Hola bienvenido al broker!!!\n"); /* prints !!!Hello World!!! */

	//Se crea el logger	obligatorio
		t_log* obligatorio;		//ver que pide loguear el tp
		if((obligatorio = log_create("Broker.log", "Broker", LOG_CONSOLE, LOG_LEVEL_INFO)) == NULL){
			puts("No se pudo crear el log");
		}
		else
			log_info(obligatorio, "Log del Broker creado");


	//Crear config
		if((config = config_create("broker.config")) == NULL){
			log_error(obligatorio, "No se pudo crear la config");
		}
		else
			log_info(obligatorio, "config creada");


		colas = malloc(sizeof(t_colas));
		suscriptores = malloc(sizeof(t_suscriptores));
		semaforos = malloc(sizeof(t_semaforos));

		pthread_mutex_init(&(semaforos->mutex_cola_new), NULL);
		sem_init(&(semaforos->nuevo_new), 0, 0);
		colas->NEW = queue_create();

	pthread_create(&hilo_NEW, NULL, (void*) cola_NEW, NULL);
	pthread_create(&hilo_APPEARED, NULL, (void*) cola_APPEARED, NULL);
	pthread_create(&hilo_GET, NULL, (void*) cola_GET, NULL);
	pthread_create(&hilo_LOCALIZED, NULL, (void*) cola_LOCALIZED, NULL);
	pthread_create(&hilo_CATCH, NULL, (void*) cola_CATCH, NULL);
	pthread_create(&hilo_CAUGHT, NULL, (void*) cola_CAUGHT, NULL);

	pthread_create(&hilo_LISTEN, NULL, (void*) broker_LISTEN, NULL);


	int elementos;
	while(1){

		sem_wait(&(semaforos->nuevo_new));
		//wait semaforo para decir la cantidad de elementos en la cola de new

		elementos = queue_size(colas->NEW);
		log_info(obligatorio, "La cola de NEW tiene, %d elementos\n", elementos);
	}










	queue_destroy(colas->NEW);
	queue_destroy(colas->APPEARED);
	queue_destroy(colas->GET);
	queue_destroy(colas->LOCALIZED);
	queue_destroy(colas->CATCH);
	queue_destroy(colas->CAUGHT);
	free(colas);
	free(suscriptores);
	for(;;);
	puts("Fin\n");

	return EXIT_SUCCESS;
}

//--------Funciones de prueba

void broker_LISTEN(){
	char* yo = "Broker";
	char* el = "Todos";

	t_log* logger;
	logger = initialize_thread(yo, el, hilo_LISTEN);

	char* puerto;
puerto="6001";//	puerto = config_get_string_value(config, "PUERTO_BROKER");

	log_info(logger, "Iniciando servidor en el puerto: %s", puerto);

	iniciar_servidor_broker(puerto, logger, colas, suscriptores, semaforos);







}

void cola_NEW(){

}
void cola_APPEARED(){

}
void cola_GET(){

}
void cola_LOCALIZED(){

}
void cola_CATCH(){

}
void cola_CAUGHT(){

}

