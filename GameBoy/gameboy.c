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

pthread_t hilo1;
pthread_t hilo2;
pthread_t hilo3;

t_config* config;


void server_broker();



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
	if (strcmp(argv[1], "BROKER") == 0)
		proceso = 1;
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
			t_new* new = malloc(sizeof(t_new));		//mover a una funcion aparte asi puedo declarar la estructura
			new->id = 0;
			char* name;
			strcpy(name, argv[3]);
			strcpy(new->nombre, argv[3]);
			uint32_t size_name = strlen(name)+1;
			new->size_nombre = size_name;
			new->posX = argv[4];				//convertir string a int?
			new->posY = argv[5];
			new->cantidad = argv[6];

			//serializar
			t_paquete* paquete = malloc(sizeof(t_paquete));
			paquete = serialize_new(t_new* new);
			//conectar al broker para enviar
			connect_to_server(ip, puerto, logger);
			//enviar
			send_paquete(socket, paquete);
			//recibir ID
			receive_ID(socket, logger);
				//no usa para nada el ID
			//enviar confirmacion
			send_ACK(socket, logger);

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



	for(;;);
	config_destroy(config);
	puts("Fin\n");

	return EXIT_SUCCESS;
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


