/*
 * routines.h
 *
 *  Created on: 1 jun. 2020
 *      Author: utnso
 */

#ifndef ROUTINES_H_
#define ROUTINES_H_

#include "gameboy.h"

void subscribe_timed(queue_code queue, int time);
void send_message(int process_code, char* ip, char* port, t_package* package);


#endif /* ROUTINES_H_ */
