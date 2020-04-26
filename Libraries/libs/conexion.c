#include "conexion.h"

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
