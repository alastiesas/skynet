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
#include <conexion.h>
#include <mensajes.h>

#define IP "127.0.0.1"



pthread_t hilo1;
pthread_t hilo2;
pthread_t hilo3;

t_config* config;

void cliente_game_card();
void cliente_team();
void cliente_game_boy();


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


	printf("\n\n");
	log_info(obligatorio, "Absolutamente todos los mensajes que 'recibe' broker, son en el servidor principal");
	log_info(obligatorio, "Puede ser una solicitud de suscripcion a una cola.");
	log_info(obligatorio, "O puede ser un mensaje.");
	log_info(obligatorio, "Se crea un nuevo hilo para esa conexion, y el proceso original vuelve a la escucha de conexiones.\n");

	log_info(obligatorio, "En el caso de recibir solitud de suscripcion a una cola, el proceso que se conecta informa cual, (solo una por conexion)");
	log_info(obligatorio, "Al atender la conexion, hace un receive para ver cual cola solicita.");
	log_info(obligatorio, "El broker guarda el socket del cliente en la lista global de suscriptores correspondiente (que debe ser sincronizada con semaforos)");
	log_info(obligatorio, "Le responde al proceso que ya esta suscrito");
	log_info(obligatorio, "De tener mensajes cacheados previos, se los puede mandar en este hilo?. Luego se cierra el hilo.\n");


	log_info(obligatorio, "En el caso de recibir un mensaje, genera el ID del mensaje");
	log_info(obligatorio, "Le responde al proceso el ID del mensaje");
	log_info(obligatorio, "Espera la confirmacion de haber recibido el ID");
	log_info(obligatorio, "Guarda el mensaje en la cache, y guarda el mensaje en la cola global correspondiente(que debe ser sincronizada con semaforos)\n");


	log_info(obligatorio, "Otro hilo del broker, toma el primer mensaje de la cola, y lo manda a todos los suscriptores.");
	log_info(obligatorio, "-Deberia-, tener un hilo por cada cola, para no tener que esperar que se vacie una cola para seguir con la otra. Ademas, Los clientes tienen un hilo y socket de escucha por cada suscripcion.");
	log_info(obligatorio, "Ya conoce la cola global de mensajes, y ya conoce la lista global de sockets, no le falta informacion.");
	log_info(obligatorio, "Ahora le falta hacer el send del mensaje iterando en todos los sockets");
	log_info(obligatorio, "El envio no va a fallar nunca. El otro proceso tiene un hilo de escucha exclusivo para esa cola, y el broker solo envia un mensaje de esa cola a la vez");
	log_info(obligatorio, "En el caso de fallar, si el send da 0 o una cantidad incompleta de bytes, es que se desconecto el TEAM, no es responsabilidad del broker arreglarlo. (Si quiere, el otro proceso puede volver a suscribirse, y empezar de 0)\n");

	log_info(obligatorio, "Debe recibir la confirmacion de recepcion del mensaje. Anota en su cache que el mensaje fue enviado satisfactoriamente (Para ese mensaje, para ese socket)");
	log_info(obligatorio, "Para no demorar el envio del siguiente mensaje (a la espera de la confirmacion del actual), se podria crear un nuevo hilo para cada send.\n");

	log_warning(obligatorio, "Falta crear una cola para cada tipo de mensaje\n\n");


	log_info(obligatorio, "Aqui se crearon tres hilos que no sirven:");
	log_info(obligatorio, "Presione enter para continuar");
    int test; scanf("%d", &test);


	pthread_create(&hilo1, NULL, (void*) cliente_game_card, NULL);

	pthread_create(&hilo2, NULL, (void*) cliente_team, NULL);

	pthread_create(&hilo3, NULL, (void*) cliente_game_boy, NULL);



	for(;;);
	puts("Fin\n");

	return EXIT_SUCCESS;
}

//--------Funciones de prueba

void cliente_game_card(){
	char* yo = "Broker";
	char* el = "GameCard";

	t_log* logger;
	logger = initialize_thread(yo, el, hilo1);

	char* puerto;
puerto="6001";//	puerto = config_get_string_value(config, "PUERTO_BROKER");

	log_info(logger, "Iniciando servidor en el puerto: %s", puerto);

	iniciar_servidor(puerto, logger);

}

void cliente_team(){
	char* yo = "Broker";
	char* el = "Team";

	t_log* logger;
	logger = initialize_thread(yo, el, hilo2);

	char* puerto;
	puerto="6002";//	puerto = config_get_string_value(config, "PUERTO_BROKER");

	log_info(logger, "Iniciando servidor en el puerto: %s", puerto);

	iniciar_servidor(puerto, logger);

}

void cliente_game_boy(){
	char* yo = "Broker";
	char* el = "GameBoy";

	t_log* logger;
	logger = initialize_thread(yo, el, hilo3);

	char* puerto;
	puerto="6003";//	puerto = config_get_string_value(config, "PUERTO_BROKER");

	log_info(logger, "Iniciando servidor en el puerto: %s", puerto);

	iniciar_servidor(puerto, logger);

}

