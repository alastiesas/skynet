#ifndef GAMEBOY_H_
#define GAMEBOY_H_

#include <structs.h>
#include <commons/log.h>
#include <commons/config.h>
#include "routines.h"

t_log* gameboy_log;
t_config* gameboy_config;
char* ip;
char* port;

t_package* package_appearead_pokemon();
t_package* package_catch_pokemon();
t_package* package_caught_pokemon();
t_package* package_get_pokemon();
t_package* package_new_pokemon();

void process_free(void* input);
void exit_failure();

#endif /* GAMEBOY_H_ */
