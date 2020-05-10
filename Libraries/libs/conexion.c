#include "conexion.h"
#include "mensajes.h"
#define _GNU_SOURCE		//para pthread_setname_np
#include<pthread.h>

void iniciar_servidor(char* puerto, t_log* logger)
{
	pthread_t thread = pthread_self();
	int32_t socket_servidor;


    struct addrinfo hints, *servinfo, *p;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(IP, puerto, &hints, &servinfo);

    for (p=servinfo; p != NULL; p = p->ai_next)
    {
        if ((socket_servidor = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){
        	log_error(logger, "Error de socket()");
        	continue;
        }

        if (bind(socket_servidor, p->ai_addr, p->ai_addrlen) == -1) {
            close(socket_servidor);
            log_error(logger, "Error de bind (el puerto esta ocupado), reinicie el programa");
            for(;;);
            continue;
        }
        break;
    }

	listen(socket_servidor, SOMAXCONN);
	log_info(logger, "Escuchando en el socket %d, en el thread %d", socket_servidor, thread);

    freeaddrinfo(servinfo);

    while(1)
    	esperar_cliente(socket_servidor, logger);
}

void esperar_cliente(int32_t socket_servidor, t_log* logger)
{
	pthread_t self = pthread_self();
	struct sockaddr_in dir_cliente;

	uint32_t tam_direccion = sizeof(struct sockaddr_in);

	log_info(logger, "Esperando conexion en el thread %d", self);

	int32_t socket_cliente;
	if((socket_cliente = accept(socket_servidor, (void*) &dir_cliente, &tam_direccion)) == -1)
		log_error(logger, "Error al aceptar cliente");
	else
		log_info(logger, "Conexion aceptada");


    struct thread_args* args = malloc(sizeof(struct thread_args));
    args->socket = socket_cliente;
    args->logger = logger;
	pthread_create(&thread,NULL,(void*)recibir_muchos_mensajes, (void *)args);		//TODO comprobar errores de pthread_create

//	pthread_detach(thread);
//	free(args);		//liberar args una vez cerrado el hilo

}

void recibir_muchos_mensajes(void* input)
{
	int32_t socket = ((struct thread_args*)input)->socket;
	t_log*	logger = ((struct thread_args*)input)->logger;

	pthread_t self = pthread_self();
	log_info(logger, "Se creo un thread %d para atender la conexion del cliente %d\n", self, socket);

	char modulo[16];
	pthread_getname_np(self, modulo, 16);

	int vez = 1;
	while(1){
	log_info(logger, "esperando cod_op del %s, por vez numero %d\n", modulo, vez);
	op_code cod_op;

	int recibido = recv(socket, &cod_op, sizeof(int32_t), MSG_WAITALL);
	if(recibido == -1)
		log_error(logger, "Error del recv()");
	if(recibido == 0)
		log_error(logger, "Se recibieron 0 bytes, se cierra el recv()");

	log_info(logger, "se recibieron %d bytes", recibido);

	log_info(logger, "se recibio la cod op: %d\n", cod_op);
	process_request(cod_op, socket, logger);
	vez++;
	}

}

void process_request(op_code cod_op, int32_t cliente_fd, t_log* logger) {
	uint32_t size;
	void* msg;
	t_catch* catch = malloc(sizeof(t_catch));
		switch (cod_op) {
		case MENSAJE:

			msg = recibir_mensaje(cliente_fd, &size, logger);
			enviar_mensaje(msg, cliente_fd, logger);
			free(msg);
			break;

		case CATCH:

			catch = receive_catch(cliente_fd, &size, logger);
			send_catch(catch, cliente_fd, logger);
			free(catch);
			log_warning(logger, "Falta hacer el free() de catch->nombre");
			break;

		default:
			log_warning(logger, "se recibio la cod_op %d asi que finaliza el thread de conexion", cod_op);
			pthread_exit(NULL);
		}
}


uint32_t connect_to_server(char * ip, char * puerto, t_log* logger)
{
	int32_t socket_cliente;
	char modulo[16];
	int tid = pthread_self();
	pthread_getname_np(tid, modulo, 16);
	int conexion = -2;

	while (conexion < 0){

		if (conexion == -1){
			log_info(logger, "Reintentando en %d segundos\n", TIEMPO_REINTENTO);
				sleep(TIEMPO_REINTENTO);
		}

		struct addrinfo hints;
		struct addrinfo *server_info;

		memset(&hints, 0, sizeof(hints));
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_flags = AI_PASSIVE;

		getaddrinfo(ip, puerto, &hints, &server_info);

		socket_cliente = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);
		if(socket_cliente  == -1)
			log_error(logger, "Error de socket()");

		conexion = connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen);
		if(conexion == -1)
			log_warning(logger, "Error de conexion con el %s\n", modulo);

		freeaddrinfo(server_info);
	}

	log_info(logger, "Conexion creada\n");

	return socket_cliente;
}


//FUNCIONES A USAR:

 /* Creamos un Socket Servidor de escucha.
 * Retorna ERROR si el puerto ya estaba en uso, si el bind no funcionó
 *  o si el Listen no funcionó.
 */
int crearSocketEscucha (int puerto, t_log* logger) {

    int socketDeEscucha = crearSocketServidor(puerto, logger);

    //Escuchar conexiones
    escucharSocketsEn(socketDeEscucha, logger);

    return socketDeEscucha;
}

/**
 ***Crea socket para conexion con servidor y se conecta***
 *  pasados por parametro.
 * Retorna ERROR si no se pudo conectar con el servidor.
 **/
int crearSocketCliente(char *ipServidor, int puerto, t_log* logger) {

    int cliente;
    struct sockaddr_in direccionServidor;

    direccionServidor.sin_family = AF_INET;				// Ordenación de bytes de la máquina
    direccionServidor.sin_addr.s_addr = inet_addr(ipServidor);
    direccionServidor.sin_port = htons(puerto);			// short, Ordenación de bytes de la red
    memset(&(direccionServidor.sin_zero), '\0', 8); 	// Pone cero al resto de la estructura

    cliente = crearSocket(logger); //Creamos socket
    int valorConnect = connect(cliente, (struct sockaddr *) &direccionServidor, sizeof(direccionServidor));

    if(valorConnect == ERROR)  {
        log_error(logger, "No se pudo establecer conexión entre el socket y el servidor.");
        return ERROR;
    }
    else {
//        log_info(logger, "Se estableció correctamente la conexión con el servidor a través del socket %i.", cliente);
        return cliente;
    }
}

/**
 ***Acepta la conexion con un cliente que se está queriendo conectar***
 * Retorna el fd del cliente.
 * Retorna ERROR si no se pudo conectar con el servidor.
 **/
int aceptarCliente(int fd_servidor, t_log* logger){

	struct sockaddr_in unCliente;
	memset(&unCliente, 0, sizeof(unCliente));
	unsigned int addres_size = sizeof(unCliente);

	int fd_cliente = accept(fd_servidor, (struct sockaddr*) &unCliente, &addres_size);
	if(fd_cliente == ERROR)  {
		log_error(logger, "El servidor no pudo aceptar la conexión entrante.\n");
		puts("El servidor no pudo aceptar la conexión entrante.\n");
	} else	{
		log_error(logger, "Servidor conectado con cliente %i.\n", fd_cliente);
	}

	return fd_cliente;

}

int enviarEntero(int fdDestinatario, int enteroEnviar,  t_log* logger){
	int resEntero =  send(fdDestinatario, &enteroEnviar, sizeof(int), MSG_WAITALL);
	if(resEntero == ERROR){
		log_error(logger, "Hubo un error al enviar el Entero a %i", fdDestinatario);
		return ERROR;
	}
	return resEntero;
}

int enviarTexto(int fdDestinatario, char* textoEnviar,  t_log* logger){
	int tamanio = pesoString(textoEnviar);
	int resTamanio = send(fdDestinatario, &tamanio, sizeof(int), MSG_WAITALL);
	if(resTamanio == ERROR){
		log_error(logger, "Hubo un error al enviar Tamanio a %i", fdDestinatario);
		return ERROR;
	}
	int resTexto = send(fdDestinatario, (void*)textoEnviar, tamanio, MSG_WAITALL);
	if(resTexto == ERROR){
		log_error(logger, "Hubo un error al enviar el Texto a %i", fdDestinatario);
		return ERROR;
	}
	return resTexto;
}

int recibirEntero(int fdOrigen, t_log* logger){
	int enteroRecibido;
	int resEntero = recv(fdOrigen, &enteroRecibido, sizeof(int), MSG_WAITALL);
	if(resEntero == ERROR){
		log_error(logger, "Hubo un error al recibir TipoDato de %i", fdOrigen);
		return ERROR;
	}
	return enteroRecibido;
}

char* recibirTexto(int fdOrigen, t_log* logger){
	int tamanio;
	int resTamanio = recv(fdOrigen, &tamanio, sizeof(int), MSG_WAITALL);
	if(resTamanio == ERROR){
		log_error(logger, "Hubo un error al recibir Tamanio de Texto de %i", fdOrigen);
		return NULL;
	}
	char* textoRecibido = malloc(tamanio);
	int resTexto = recv(fdOrigen, textoRecibido, tamanio, MSG_WAITALL);
	if(resTexto == ERROR){
		log_error(logger, "Hubo un error al recibir Texto de %i", fdOrigen);
		return NULL;
	}
	return textoRecibido;
}

//Habilitar socket servidor de esucha
void escucharSocketsEn(int fd_socket, t_log* logger){

    int valorListen;
    valorListen = listen(fd_socket, SOMAXCONN);/*Le podríamos poner al listen
				SOMAXCONN como segundo parámetro, y significaría el máximo tamaño de la cola*/
    if(valorListen == ERROR) {
        log_error(logger, "El servidor no pudo recibir escuchar conexiones de clientes.");
    } else	{
        log_info(logger, "El servidor está escuchando conexiones a través del socket %i.", fd_socket);
    }
}


//-------------------------------------

//Crear socket servidor
int crearSocketServidor(int puerto, t_log* logger)	{
    struct sockaddr_in miDireccionServidor;
    int socketDeEscucha = crearSocket(logger);

    miDireccionServidor.sin_family = AF_INET;			//Protocolo de conexion
    miDireccionServidor.sin_addr.s_addr = INADDR_ANY;	//INADDR_ANY = 0 y significa que usa la IP actual de la maquina
    miDireccionServidor.sin_port = htons(puerto);		//Puerto en el que escucha
    memset(&(miDireccionServidor.sin_zero), '\0', 8);	//Pone 0 al campo de la estructura "miDireccionServidor"

    //Veamos si el puerto está en uso
    int puertoEnUso = 1;
    int puertoYaAsociado = setsockopt(socketDeEscucha, SOL_SOCKET, SO_REUSEADDR, (char*) &puertoEnUso, sizeof(puertoEnUso));

    if (puertoYaAsociado == ERROR) {
        log_error(logger, "El puerto asignado ya está siendo utilizado.");
    }
    //Turno del bind
    int activado = 1;
    //Para evitar que falle el bind, al querer usar un mismo puerto
    setsockopt(socketDeEscucha,SOL_SOCKET,SO_REUSEADDR,&activado,sizeof(activado));

    int valorBind = bind(socketDeEscucha,(void*) &miDireccionServidor, sizeof(miDireccionServidor));

    if ( valorBind !=0) {
        log_error(logger, "El bind no funcionó, el socket no se pudo asociar al puerto");
        return 1;
    }

    log_info(logger, "Servidor levantado en el puerto %i", puerto);

    return socketDeEscucha;
}

//Crear socket
int crearSocket(t_log* logger) {
    int fileDescriptor = socket(AF_INET, SOCK_STREAM, 0);//usa protocolo TCP/IP
    if (fileDescriptor == ERROR) {
        log_error(logger, "No se pudo crear el file descriptor.");
    }

    return fileDescriptor;
}


void freeCharArray(char** charArray){

	int i = 0;
	while(charArray[i] != NULL){
		free(charArray[i]);
		i++;
	}
	free(charArray);
}

int pesoString(char *string) {
    return string == NULL ? 0 : sizeof(char) * (strlen(string) + 1);
}
