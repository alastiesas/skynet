#ifndef GAMEBOY_H_
#define GAMEBOY_H_

#include <structs.h>
#include <commons/log.h>
#include <commons/config.h>

t_log* logger;
t_config* config;
char* ip;
char* port;

t_package* package_appearead_pokemon();
t_package* package_catch_pokemon();
t_package* package_caught_pokemon();
t_package* package_get_pokemon();
t_package* package_new_pokemon();
void subscribe(queue_code queue, int time);
//void send(char* ip, char* port, t_paquete* package);

#endif /* GAMEBOY_H_ */
