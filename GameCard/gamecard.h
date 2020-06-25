/*
 * gamecard.h
 *
 *  Created on: 13 jun. 2020
 *      Author: utnso
 */

#ifndef GAMECARD_H_
#define GAMECARD_H_

#include <stdio.h>
#include <stdlib.h>
#include<commons/config.h>
#include <conexion.h>
#include <mensajes.h>

t_log* logger;
t_config* config;

uint32_t TIEMPO_DE_REINTENTO_CONEXION;
uint32_t TIEMPO_DE_REINTENTO_OPERACION;
uint32_t TIEMPO_RETARDO_OPERACION;
char* PUNTO_MONTAJE_TALLGRASS;
char* IP_BROKER;
char* PUERTO_BROKER;
char* IP_GAMECARD;
char* PUERTO_GAMECARD;
uint32_t MY_ID;



pthread_t gameboy_thread;
pthread_t new_thread;
pthread_t catch_thread;
pthread_t get_thread;





void gameboy_function(void);
void message_function(void (*function)(void*), queue_code queue_code);
void new_function(void);
void catch_function(void);
void get_function(void);

int32_t suscribe_to_broker(queue_code queue_code);
void serve_new(void* input);
void serve_catch(void* input);
void serve_get(void* input);

void send_to_broker(t_package* package);
void initiliaze_file_system();

#endif /* GAMECARD_H_ */
