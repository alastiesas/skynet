/*
 * includes.h
 *
 *  Created on: 10 jun. 2020
 *      Author: utnso
 */

#ifndef INCLUDES_H_
#define INCLUDES_H_

//GNU C Library - GCC
#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<semaphore.h>
#include<pthread.h>
#include<readline/readline.h>
#include<stdbool.h>

//COMMONS
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<commons/collections/list.h>

//UTILS
#include<structs.h>
#include<utils.h>
#include<serialize.h>
#include<conexion.h>
#include<mensajes.h>

#endif /* INCLUDES_H_ */
