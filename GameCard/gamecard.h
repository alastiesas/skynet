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
#include<commons/bitarray.h>
#include <conexion.h>
#include <mensajes.h>
#include <math.h>

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
void* bmap;
t_bitarray* bitmap;

char* metadata_path;
char* files_metadata_path;
char* bitmap_path;
char* blocks_directory;
char* metadata_directory;
char* files_directory;

uint32_t blocks;
uint32_t block_size;


pthread_mutex_t mutex_pkmetadata;
pthread_mutex_t mutex_bitmap;

struct gamecard_thread_args {
    int32_t socket;
    t_log* logger;
};

pthread_t gameboy_thread;
pthread_t new_thread;
pthread_t catch_thread;
pthread_t get_thread;

void iniciar_servidor_gamecard();
void wait_clients(int32_t socket_servidor, t_log* logger);
void gamecard_serves_client(void* input);

void init_fs();
void get_bitarray();
void create_bitarray();
void load_bitarray();
void save_bitarray(t_bitarray* bitarray);
void* open_file_blocks(t_list* file_blocks, uint32_t total_size);
void write_file_blocks(void* pokemon_file, t_list* my_blocks, uint32_t total_size, char* pokemon_name);
t_list* find_available_blocks(uint32_t amount);
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

void create_file_directory(char*,t_location*);
char* location_to_string(t_location* location);

void new_pokemon_routine();
bool exists_pokemon(char* pokemon_name);
char* get_pokemon_directory(char* pokemon_name);
char* get_pokemon_file(char* pokemon_name);
bool exists_file(char* file_name);
void create_pokemon(char* pokemon_name);
void create_pokemon_directory(char* pokemon_name);
void create_pokemon_file(char* pokemon_name);
void open_file();
void update_file();
void send_appeared_pokemon();

#endif /* GAMECARD_H_ */
