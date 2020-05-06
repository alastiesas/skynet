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
#include<commons/collections/list.h>
#include <conexion.h>
#include <mensajes.h>

#define LOG_CONSOLE true

#define IP "127.0.0.1"

pthread_t hilo1;
pthread_t hilo2;
pthread_t hilo3;

t_config* config;

char* puerto;

void server_broker();
void case_NEW(char* argv[], t_log* logger);



int main(int argc, char* argv[]) {
	puts("!!!Hola bienvenido al Game Boy!!!\n"); /* prints !!!Hello World!!! */

//Se crea el logger	obligatorio
	t_log* obligatorio;		//ver que pide loguear el tp
	if((obligatorio = log_create("GameBoy.txt", "GameBoy", LOG_CONSOLE, LOG_LEVEL_INFO)) == NULL){
		puts("No se pudo crear el log");
	}
	else
		log_info(obligatorio, "Log del GameBoy creado");


//Crear config
	if((config = config_create("gameBoy.config")) == NULL){
		log_error(obligatorio, "No se pudo crear la config");
	}
	else
		log_info(obligatorio, "config creada");

	//./gameboy BROKER NEW_POKEMON [POKEMON] [POSX] [POSY] [CANTIDAD]

	if(argc != 7){
			printf("Solo recibo mensaje NEW con 6 argumentos\n./gameboy BROKER NEW_POKEMON [POKEMON] [POSX] [POSY] [CANTIDAD]\n");
			exit(EXIT_SUCCESS);
		}

	int proceso;
	if (strcmp(argv[1], "BROKER") == 0){
		proceso = 1;
		puerto = "6001"; //Leer puerto de config
	}
	else{
		proceso = 0;
		printf("Solo envio al broker\n\n");
		exit(EXIT_SUCCESS);
	}

	int mensaje;
	if (strcmp(argv[2], "NEW_POKEMON") == 0)
		mensaje = 1;
	else{
		mensaje = 0;
		printf("Solo envio mensaje NEW\n\n");
		exit(EXIT_SUCCESS);
	}

	switch(proceso){
	case 1:
		switch(mensaje){
		case 1:

			case_NEW(argv, obligatorio);
			//fin de proceso gameboy

			break;
		default:
			printf("Error de argumento\n");
			break;
		}
		break;
	default:
		printf("Error de argumento\n");
		break;
	}



	//pthread_create(&hilo1, NULL, (void*) server_broker, NULL);




	config_destroy(config);
	puts("Fin\n");

	return EXIT_SUCCESS;
}

void case_NEW(char* argv[], t_log* logger){

	printf("se llego al caso NEW\n");
	printf("argv[4] como char*: %s\n", argv[4]);
	printf("argv[4] como int: %d\n", argv[4]);

	int32_t posX = atoi(argv[4]);
	printf("argv[4] como int luego de convertir: %d\n", posX);

	int32_t posY = atoi(argv[5]);
	int32_t cantidad = atoi(argv[6]);


	t_new* new = malloc(sizeof(t_new));
	new->id = 0;
	strcpy(new->nombre, argv[3]);
	uint32_t size_name = strlen(new->nombre)+1;
	new->size_nombre = size_name;
	new->posX = posX;				//convertir string a int?
	new->posY = posY;
	new->cantidad = cantidad;


	//serializar
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete = serialize_new(new);
	//conectar al broker para enviar
	int32_t socket_server;
	printf("va a conectar en el puerto: %s", puerto);
	socket_server = connect_to_server(IP, puerto, logger);
	//enviar
	send_paquete(socket_server, paquete);
	//recibir ID
	receive_ID(socket_server, logger);
	//no usa para nada el ID
	//enviar confirmacion
	send_ACK(socket_server, logger);



}


//--------Funciones de prueba

void server_broker(){
	char* yo = "GameBoy";
	char* el = "Broker";

	t_log* logger;
	logger = initialize_thread(yo, el, hilo1);


	char* puerto; //config_get_string reserva la memoria necesaria
	char* ip;
puerto="6003";//		puerto = config_get_string_value(config, "PUERTO_BROKER");
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


