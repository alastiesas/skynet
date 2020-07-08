#include <pthread.h>
#include<commons/log.h>
#include<commons/config.h>
#include<commons/collections/list.h>
#include <conexion.h>
#include <mensajes.h>
#include "gameboy.h"

#define LOG_CONSOLE true

int main(int argc, char* argv[]) {

	gameboy_behavior_log = log_create("gameboy_behavior.log", "gameboy behavior", LOG_CONSOLE, LOG_LEVEL_INFO);
	log_info(gameboy_behavior_log, " 0. starting process");

	gameboy_log = log_create("gameboy.log", "gameboy", LOG_CONSOLE, LOG_LEVEL_INFO);
	log_info(gameboy_behavior_log, " 1. log created");

	gameboy_config = config_create("gameboy.config");
	log_info(gameboy_behavior_log, " 2. config created");

	log_line = (char*) malloc(300);

	strcpy(log_line, "    id=");
	strcat(log_line, config_get_string_value(gameboy_config, "ID"));
	log_info(gameboy_behavior_log, log_line);
	strcpy(log_line, "    ip_broker=");
	strcat(log_line, config_get_string_value(gameboy_config, "IP_BROKER"));
	log_info(gameboy_behavior_log, log_line);
	strcpy(log_line, "    ip_gamecard=");
	strcat(log_line, config_get_string_value(gameboy_config, "IP_GAMECARD"));
	log_info(gameboy_behavior_log, log_line);
	strcpy(log_line, "    ip_team=");
	strcat(log_line, config_get_string_value(gameboy_config, "IP_TEAM"));
	log_info(gameboy_behavior_log, log_line);
	strcpy(log_line, "    puerto_broker=");
	strcat(log_line, config_get_string_value(gameboy_config, "PUERTO_BROKER"));
	log_info(gameboy_behavior_log, log_line);
	strcpy(log_line, "    puerto_gamecard=");
	strcat(log_line, config_get_string_value(gameboy_config, "PUERTO_GAMECARD"));
	log_info(gameboy_behavior_log, log_line);
	strcpy(log_line, "    puerto_team=");
	strcat(log_line, config_get_string_value(gameboy_config, "PUERTO_TEAM"));
	log_info(gameboy_behavior_log, log_line);


	if (strcmp(argv[1], "SUSCRIPTOR") == 0 && argc == 4) {
		//gameboy SUSCRIPTOR [COLA_DE_MENSAJES] [TIEMPO]
		queue_code queue_code;
		if (strcmp(argv[2], "APPEARED_POKEMON") == 0) {
			queue_code = COLA_APPEARED;
		} else if (strcmp(argv[2], "CATCH_POKEMON") == 0) {
			queue_code = COLA_CATCH;
		} else if (strcmp(argv[2], "CAUGHT_POKEMON") == 0) {
			queue_code = COLA_CAUGHT;
		} else if (strcmp(argv[2], "GET_POKEMON") == 0) {
			queue_code = COLA_GET;
		} else if (strcmp(argv[2], "LOCALIZED_POKEMON") == 0) {
			queue_code = COLA_LOCALIZED;
		} else if (strcmp(argv[2], "NEW_POKEMON") == 0) {
			queue_code = COLA_NEW;
		} else {
			exit_failure();
		}
		subscribe_timed(queue_code, atoi(argv[3]));

	} else {

		t_package* package;
		int process_code;
		if (strcmp(argv[1], "BROKER") == 0) {

			ip = config_get_string_value(gameboy_config, "IP_BROKER");
			port = config_get_string_value(gameboy_config, "PUERTO_BROKER");
			process_code = 1;
			if (argc == 4 && strcmp(argv[2], "GET_POKEMON") == 0) {
				//gameboy BROKER GET_POKEMON [POKEMON]
				t_message_get* get;
				get = create_message_get(argv[3]);
				package = serialize_get(get);
				destroy_message_get(get);
			} else if (argc == 5 && strcmp(argv[2], "CAUGHT_POKEMON") == 0) {
				//gameboy BROKER CAUGHT_POKEMON [ID_CORRELATIVO] [OK/FAIL]
				uint32_t result;
				if(strcmp(argv[4], "OK") == 0)
					result = 1;
				else if(strcmp(argv[4], "FAIL") == 0)
					result = 0;
				else
					exit_failure();
				t_message_caught* caught;
				caught = create_message_caught(atoi(argv[3]), result);
				package = serialize_caught(caught);
				destroy_message_caught(caught);
			} else if (argc == 6 && strcmp(argv[2], "CATCH_POKEMON") == 0) {
				//gameboy BROKER CATCH_POKEMON [POKEMON] [POSX] [POSY]
				t_message_catch* catch;
				catch = create_message_catch_long(argv[3], atoi(argv[4]), atoi(argv[5]));
				package = serialize_catch(catch);
				destroy_message_catch(catch);
			} else if (argc == 7) {

				if (strcmp(argv[2], "APPEARED_POKEMON") == 0) {
					//gameboy BROKER APPEARED_POKEMON [POKEMON] [POSX] [POSY] [ID_CORRELATIVO]
					t_message_appeared* appeared;
					appeared = create_message_appeared_long(atoi(argv[6]), argv[3], atoi(argv[4]), atoi(argv[5]));
					appeared->correlative_id = atoi(argv[6]);
					package = serialize_appeared(appeared);
					destroy_message_appeared(appeared);
				} else if (strcmp(argv[2], "NEW_POKEMON") == 0) {
					//gameboy BROKER NEW_POKEMON [POKEMON] [POSX] [POSY] [CANTIDAD]
					t_message_new* new;
					new = create_message_new_long(argv[3], atoi(argv[4]), atoi(argv[5]), atoi(argv[6]));
					package = serialize_message_new(new);
					destroy_message_new(new);
				} else {
					exit_failure();
				}
			} else {
				exit_failure();
			}

		} else if (strcmp(argv[1], "GAMECARD") == 0) {

			ip = config_get_string_value(gameboy_config, "IP_GAMECARD");
			port = config_get_string_value(gameboy_config, "PUERTO_GAMECARD");
			process_code = 2;
			if (argc == 5 && strcmp(argv[2], "GET_POKEMON") == 0) {
				//gameboy GAMECARD GET_POKEMON [POKEMON] [ID_MENSAJE]
				t_message_get* get;
				get = create_message_get(argv[3]);
				get->id = atoi(argv[4]);
				package = serialize_get(get);
				destroy_message_get(get);
			} else if (argc == 7 && strcmp(argv[2], "CATCH_POKEMON") == 0) {
				//gameboy GAMECARD CATCH_POKEMON [POKEMON] [POSX] [POSY] [ID_MENSAJE]
				t_message_catch* catch;
				catch = create_message_catch_long(argv[3], atoi(argv[4]), atoi(argv[5]));
				catch->id = atoi(argv[6]);
				package = serialize_catch(catch);
				destroy_message_catch(catch);

			} else if (argc == 8 && strcmp(argv[2], "NEW_POKEMON") == 0) {
				//gameboy GAMECARD NEW_POKEMON [POKEMON] [POSX] [POSY] [CANTIDAD] [ID_MENSAJE]
				t_message_new* new;
				new = create_message_new_long(argv[3], atoi(argv[4]), atoi(argv[5]), atoi(argv[6]));
				new->id = atoi(argv[7]);
				package = serialize_message_new(new);
				destroy_message_new(new);

			} else {
				exit_failure();
			}

		} else if (strcmp(argv[1], "TEAM") == 0 && argc == 6 && strcmp(argv[2], "APPEARED_POKEMON") == 0) {
			ip = config_get_string_value(gameboy_config, "IP_TEAM");
			port = config_get_string_value(gameboy_config, "PUERTO_TEAM");
			process_code = 3;
			//gameboy TEAM APPEARED_POKEMON [POKEMON] [POSX] [POSY]
			t_message_appeared* appeared;
			appeared = create_message_appeared_long(0, argv[3], atoi(argv[4]), atoi(argv[5]));	//TODO ver id correlativo
			package = serialize_appeared(appeared);
			destroy_message_appeared(appeared);

		} else {
			exit_failure();
		}
		send_message(process_code, ip, port, package);

	}


	free(log_line);

	log_info(gameboy_behavior_log, " 4. ending process");

	config_destroy(gameboy_config);

	log_destroy(gameboy_log);

	log_destroy(gameboy_behavior_log);

	return EXIT_SUCCESS;
}

void exit_failure(){
	printf("\n//gameboy BROKER NEW_POKEMON [POKEMON] [POSX] [POSY] [CANTIDAD]"
			"\n//gameboy BROKER APPEARED_POKEMON [POKEMON] [POSX] [POSY] [ID_CORRELATIVO]"
			"\n//gameboy BROKER GET_POKEMON [POKEMON]"
			"\n//gameboy BROKER CATCH_POKEMON [POKEMON] [POSX] [POSY]"
			"\n//gameboy BROKER CAUGHT_POKEMON [ID_CORRELATIVO] [OK/FAIL]"
			"\n"
			"\n//gameboy TEAM APPEARED_POKEMON [POKEMON] [POSX] [POSY]"
			"\n"
			"\n//gameboy GAMECARD NEW_POKEMON [POKEMON] [POSX] [POSY] [CANTIDAD] [ID_MENSAJE]"
			"\n//gameboy GAMECARD GET_POKEMON [POKEMON] [ID_MENSAJE]"
			"\n//gameboy GAMECARD CATCH_POKEMON [POKEMON] [POSX] [POSY] [ID_MENSAJE]"
			"\n"
			"\n//gameboy SUSCRIPTOR [COLA_DE_MENSAJES] [TIEMPO]\n");
	exit(EXIT_FAILURE);
}
