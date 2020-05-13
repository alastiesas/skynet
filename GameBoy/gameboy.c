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

pthread_t hilo_suscripcion;

t_config* config;

char* puerto;
char* ID_PROPIO;

//void server_broker();
void case_NEW(char* argv[], t_log* logger);
void modo_suscriptor(queue_code mensaje, int tiempo, t_log* logger);



int main(int argc, char* argv[]) {
	puts("!!!Hola bienvenido al Game Boy!!!\n"); /* prints !!!Hello World!!! */

//Se crea el logger	obligatorio
	t_log* obligatorio;		//ver que pide loguear el tp
	if((obligatorio = log_create("GameBoy.log", "GameBoy", LOG_CONSOLE, LOG_LEVEL_INFO)) == NULL){
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
	//./gameboy SUSCRIPTOR [COLA_DE_MENSAJES] [TIEMPO]

	ID_PROPIO = config_get_string_value(config, "ID_GAMEBOY");



	process_code proceso;
	if (strcmp(argv[1], "BROKER") == 0){
		proceso = BROKER;
		puerto = config_get_string_value(config, "PUERTO_BROKER");
	}else if(strcmp(argv[1], "SUSCRIPTOR") == 0){
		proceso = SUSCRIPTOR;
		puerto = config_get_string_value(config, "PUERTO_BROKER");
	}
	else{
		proceso = 0;
		printf("Solo envio al broker, o modo suscripcion\n\n");
		exit(EXIT_FAILURE);
	}


	queue_code mensaje;
	if (strcmp(argv[2], "NEW_POKEMON") == 0)
		mensaje = COLA_NEW;
	else{
		mensaje = 0;
		printf("Solo envio mensaje NEW o me suscribo a NEW\n\n");
		exit(EXIT_FAILURE);
	}

	switch(proceso){
	case BROKER:
		switch(mensaje){
		case COLA_NEW:
			if(argc != 7){
				printf("Error de cantidad de argumentos\n");
				exit(EXIT_FAILURE);
				}
			case_NEW(argv, obligatorio);
			//fin de proceso gameboy

			break;
		default:
			printf("Error de argumento\n");
			break;
		}
		break;

	case SUSCRIPTOR:
		if(argc != 4){
			printf("Error de cantidad de argumentos\n");
			exit(EXIT_FAILURE);
			}
		int tiempo = atoi(argv[3]);
		modo_suscriptor(mensaje, tiempo, obligatorio);

		break;
	default:
		printf("Error de primer argumento\n");
		break;
	}



	//pthread_create(&hilo1, NULL, (void*) server_broker, NULL);




	config_destroy(config);
	puts("Fin\n");

	return EXIT_SUCCESS;
}

void case_NEW(char* argv[], t_log* logger){

	printf("se llego al caso NEW\n");

	int32_t posX = atoi(argv[4]);	//convierte a int el argumento de main

	int32_t posY = atoi(argv[5]);
	int32_t cantidad = atoi(argv[6]);


	t_new* new = malloc(sizeof(t_new));
	new->id = 0;
	strcpy(new->nombre, argv[3]);				//valgrind dice "use of uninitialized value of size 4 en strcpy." Pero no aclara si es esta linea
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

	free(new->nombre);
	free(new);
	//	el send_paquete ya libera el paquete

}

void modo_suscriptor(queue_code cola, int tiempo, t_log* logger){

	//informar que me estoy suscribiendo y a que cola
	//serializar
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete = serialize_suscripcion(atoi(ID_PROPIO), cola);
	//conectarse al broker
	int32_t socket_server;
	printf("va a conectar en el puerto: %s", puerto);
	socket_server = connect_to_server(IP, puerto, logger);
	//enviar
	send_paquete(socket_server, paquete);

	//esperar confirmacion de suscripcion
	if(receive_ACK(socket_server, logger) == -1){
		log_error(logger, "No se pudo suscribir\n");
		exit(EXIT_FAILURE);
	}

	log_info(logger, "Ya estoy suscrito por %d segundos.\n", tiempo);

	//crear un nuevo hilo para quedarse en escucha


    struct thread_args* args = malloc(sizeof(struct thread_args));
    args->socket = socket_server;
    args->logger = logger;
	pthread_create(&hilo_suscripcion, NULL, (void*) recibir_muchos_mensajes, args);

	//hacer un sleep en el hilo principal esperando para cerrar el hilo. (O terminar el proceso mas facil)
		//(al cerrar el hilo puede quedar memoria sin liberar, pero tambien se cierra el proceso asi que no importa)

	sleep(tiempo);
	log_info(logger, "Termino la suscripcion de %d segundos\n", tiempo);

}














//--------Funciones de prueba

/*
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
*/
