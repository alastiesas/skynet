/*
 * gamecard.h
 *
 *  Created on: 13 jun. 2020
 *      Author: utnso
 */

#ifndef GAMECARD_H_
#define GAMECARD_H_

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
void new_function(void);
void catch_function(void);
void get_function(void);

int32_t subscribe_to_broker(queue_code queue_code);
void atender_new(operation_code op_code, void* message);
void atender_catch(operation_code op_code, void* message);
void atender_get(operation_code op_code, void* message);

void send_to_broker(t_package* package);

#endif /* GAMECARD_H_ */
