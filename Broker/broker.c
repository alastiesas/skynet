/*
 ============================================================================
 Name        : broker.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include<commons/config.h>
#include<commons/collections/queue.h>
#include <conexion.h>
#include <mensajes.h>

#define IP "127.0.0.1"


pthread_t hilo_LISTEN;
pthread_t hilo_NEW;
pthread_t hilo_APPEARED;
pthread_t hilo_GET;
pthread_t hilo_LOCALIZED;
pthread_t hilo_CATCH;
pthread_t hilo_CAUGHT;

t_config* config;

void broker_LISTEN();
void cola_NEW();
void cola_APPEARED();
void cola_GET();
void cola_LOCALIZED();
void cola_CATCH();
void cola_CAUGHT();

t_queue* queue_NEW;
t_queue* queue_APPEARED;
t_queue* queue_GET;
t_queue* queue_LOCALIZED;
t_queue* queue_CATCH;
t_queue* queue_CAUGHT;


int main(void) {
	puts("!!!Hola bienvenido al broker!!!\n"); /* prints !!!Hello World!!! */

	//Se crea el logger	obligatorio
		t_log* obligatorio;		//ver que pide loguear el tp
		if((obligatorio = log_create("Broker.txt", "Broker", LOG_CONSOLE, LOG_LEVEL_INFO)) == NULL){
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


	pthread_create(&hilo_NEW, NULL, (void*) cola_NEW, NULL);
	pthread_create(&hilo_APPEARED, NULL, (void*) cola_APPEARED, NULL);
	pthread_create(&hilo_GET, NULL, (void*) cola_GET, NULL);
	pthread_create(&hilo_LOCALIZED, NULL, (void*) cola_LOCALIZED, NULL);
	pthread_create(&hilo_CATCH, NULL, (void*) cola_CATCH, NULL);
	pthread_create(&hilo_CAUGHT, NULL, (void*) cola_CAUGHT, NULL);

	pthread_create(&hilo_LISTEN, NULL, (void*) broker_LISTEN, NULL);


	int elementos;
	while(1){
		//wait semaforo para decir la cantidad de elementos en la cola de new
		elementos = queue_size(queue_NEW);
		log_info(obligatorio, "La cola de NEW tiene, %d elementos\n", elementos);
	}













	queue_destroy(queue_NEW);
	queue_destroy(queue_APPEARED);
	queue_destroy(queue_GET);
	queue_destroy(queue_LOCALIZED);
	queue_destroy(queue_CATCH);
	queue_destroy(queue_CAUGHT);
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

	iniciar_servidor(puerto, logger);

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

