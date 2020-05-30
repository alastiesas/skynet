#ifndef GAMEBOY_H_
#define GAMEBOY_H_

#include <structs.h>

t_log* log;
t_config* config;

typedef enum {
	PARAMETER_APPEARED_POKEMON = "APPEARED_POKEMON",
	PARAMETER_BROKER = "BROKER",
	PARAMETER_CATCH_POKEMON = "CATCH_POKEMON",
	PARAMETER_CAUGHT_POKEMON = "CAUGHT_POKEMON",
	PARAMETER_GAMECARD = "GAMECARD",
	PARAMETER_GET_POKEMON = "GET_POKEMON",
	PARAMETER_LOCALIZED_POKEMON = "LOCALIZED_POKEMON",
	PARAMETER_NEW_POKEMON = "NEW_POKEMON",
	PARAMETER_SUBSCRIBER = "SUSCRIPTOR",
	PARAMETER_TEAM = "TEAM"
} parameters;

t_paquete* package_appearead_pokemon();
t_paquete* package_catch_pokemon();
t_paquete* package_caught_pokemon();
t_paquete* package_get_pokemon();
t_paquete* package_new_pokemon();
void subscribe(queue_code queue, int time);
void send(char* ip, char* port, t_paquete* package);

#endif /* GAMEBOY_H_ */
