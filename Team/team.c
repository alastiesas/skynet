/*
 ============================================================================
 Name        : broker.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <pthread.h>
#include<commons/log.h>
#include<commons/config.h>
#include<sys/socket.h>
#include<commons/collections/list.h>
#include <conexion.h>
#include <mensajes.h>

#define IP "127.0.0.1"

#define TIEMPO_CHECK 15

pthread_t thread;
pthread_t hilo1;
pthread_t hilo2;

t_config* config;

void server_broker();
void cliente_game_boy();


int main(void) {
	puts("!!!Hola bienvenido al Team!!!\n"); /* prints !!!Hello World!!! */

	//Se crea el logger	obligatorio
		t_log* obligatorio;		//ver que pide loguear el tp
		if((obligatorio = log_create("Team.txt", "Team", LOG_CONSOLE, LOG_LEVEL_INFO)) == NULL){
			puts("No se pudo crear el log");
		}
		else
			log_info(obligatorio, "Log del Team creado");

	//Crear config
		if((config = config_create("team.config")) == NULL){
			log_error(obligatorio, "No se pudo crear la config");
		}
		else
			log_info(obligatorio, "config creada");


	printf("\n\n");
	log_info(obligatorio, "En un hilo crea un servidor, para recibir mensajes unitarios del gameboy, asi de simple");
	log_info(obligatorio, "Ese mensaje, se debe enviar al hilo principal para atenderlo normalmente\n");


	log_warning(obligatorio, "Falta inicializacion del Team:");
	log_info(obligatorio, "Leer el archivo config, y cargar en memoria los entrenadores, sus pokemon necesarios, y sus posiciones");
	log_info(obligatorio, "Definir objetivo global: sumo los pokemon de cada especie que necesitan todos los entrenadores del TEAM\n");


	log_warning(obligatorio, "Falta suscribirse a las colas de mensajes del broker:");
	log_info(obligatorio, "Un hilo por cada cola");
	log_info(obligatorio, "Un connect por cada cola: un proceso team va a ser cliente tres veces del broker (sockets abiertos permanentemente)");
	log_info(obligatorio, "Al hacer el connect, le envio la cola elegida al broker");
	log_info(obligatorio, "Luego de recibir la confirmacion de suscripcion, se queda permanentemente a la escucha de mensajes");
	log_info(obligatorio, "Al recibir un mensaje, debe responder confirmando que lo recibio, y vuelve a la escucha permanente\n");

	log_info(obligatorio, "Si no pudo conectar a determinada cola, va a usar respuestas fijas para sus mensajes. Debe reintentar conexion cada X");
	log_info(obligatorio, "Esos tres sockets van a ser para recibir mensajes. Para enviar, abre un nuevo socket en el hilo principal en los momentos que necesite enviar.\n");


	log_warning(obligatorio, "Falta enviar un mensaje GET al broker por cada especie de pokemon que necesite");
	log_info(obligatorio, "Para enviar un mensaje, hace un nuevo connect al broker, que queda abierto hasta recibir la respuesta con la confirmacion (que puede ser con el id del mensaje)\n");

	log_warning(obligatorio, "Falta crear estructuras en el TEAM");
	log_info(obligatorio, "Cola de ready, cola de bloqued, etc");
	log_info(obligatorio, "Lista de entrenadores");
	log_info(obligatorio, "Lista de pokemon");
	log_info(obligatorio, "Lista de objetivos\n");

	log_info(obligatorio, "Cada entrenador se crea en estado new");
	log_info(obligatorio, "Solo un estado exec en simultaneo\n\n");


	log_info(obligatorio, "Aqui se crearon dos hilos que no sirven:");
	log_info(obligatorio, "Presione enter para continuar");
    int test; scanf("%d", &test);


	pthread_create(&hilo1, NULL, (void*) server_broker, NULL);

	pthread_create(&hilo2, NULL, (void*) cliente_game_boy, NULL);

	for(;;);
	puts("Fin\n");

	return EXIT_SUCCESS;
}

//--------Funciones de prueba

void server_broker(){

	char* yo = "Team";
	char* el = "Broker";

	t_log* logger;
	logger = initialize_thread(yo, el, hilo1);


	char* puerto; //config_get_string reserva la memoria necesaria
	char* ip;
puerto="6002";//		puerto = config_get_string_value(config, "PUERTO_BROKER");
		ip = config_get_string_value(config, "IP_BROKER");
			log_info(logger, "Puerto del Broker: %s", puerto);
			log_info(logger, "IP del Broker: %s", ip);

	uint32_t socket;

	//crear conexion
	socket = connect_to_server(ip, puerto, logger);


	//enviar muchos mensajes
	enviar_muchos_mensajes(yo, el, socket, logger);



	log_info(logger, "Fin del la conexion con el Broker\n");
	close(socket);
	log_destroy(logger);

}

void cliente_game_boy(){
	char* yo = "Team";
	char* el = "GameBoy";

	t_log* logger;
	logger = initialize_thread(yo, el, hilo2);

	char* puerto;
puerto="6005";//	puerto = config_get_string_value(config, "PUERTO_TEAM");

	log_info(logger, "Iniciando servidor en el puerto: %s", puerto);

	iniciar_servidor(puerto, logger);

}
