/*
 * messages.c
 *
 *  Created on: 19 jun. 2020
 *      Author: utnso
 */
#include "gamecard.h"
#include <errno.h>
//FIXME
/*
 * meter mutex en todos los config_create y config_save
 * ver donde van los TIEMPO_RETARDO_OPERACION
 *
 */

int32_t wait_available_file_new(char* pokemon_name){

	//esperar mutex del pokemon metadata
	pthread_mutex_lock(&mutex_pkmetadata);	//TODO unlock
	//abrir metadata del pokemon dado, si no existe, crearlo, setear metadata en ocupado (soltar el mutex y retornar -1)

	//chequear en el metadata si esta ocupado, en ese caso, soltar el mutex y reintentar en TIEMPO_DE_REINTENTO_OPERACION
	//si esta disponible, actualizarlo como en uso, soltar mutex y listo
}

int32_t wait_available_file(char* pokemon_name){

	//esperar mutex del pokemon metadata
	pthread_mutex_lock(&mutex_pkmetadata);	//TODO unlock
	//abrir metadata del pokemon dado, si no existe, soltar el mutex y retornar directamente que no existe (-1)
	//si no existe, esperar en este punto el tiempo de retardo operacion (mirar si aplica en este caso ese tiempo)
	//chequear en el metadata si esta ocupado, en ese caso, soltar el mutex y reintentar en TIEMPO_DE_REINTENTO_OPERACION
	//si esta disponible, actualizarlo como en uso, soltar mutex y listo, (retornar 0)
}

void release_pokemon_file(char* pokemon_name){
	//esperar mutex del pokemon metadata
	pthread_mutex_lock(&mutex_pkmetadata);
	//abrir metadata del pokemon dado
	//actualizar el metadata del pokemon a libre
	//soltar el mutex
	pthread_mutex_unlock(&mutex_pkmetadata);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------

t_message_appeared* process_new(t_message_new* message_new){

	t_message_appeared* message_appeared;
	int32_t exists;

	exists = wait_available_file_new(message_new->pokemon_name);
	//si no existia el archivo metadata, crear directorio y archivo pokemon manualmente
	if(exists != -1){
	//pasar los bloques del archivo a memoria
	//crear diccionario con el archivo
	//verificar si existe en el archivo la posicion recibida, si no existe, crearla
	//sumar la cantidad de pokemonos nuevos
	//convertir el diccionario a void*
	//grabar el void* en los bloques
	//esperar el tiempo de retardo  operacion
	release_pokemon_file(message_new->pokemon_name);
	}
	else{
		//TODO crear metadata y setear en open antes de soltar el mutex
		create_file_directory(message_new->pokemon_name, message_new->location);
		printf("TIEMPO_RETARDO_OPERACION\n");
		sleep(TIEMPO_RETARDO_OPERACION);
	}

	//generar mensaje appeared y destruir el mensaje new
	return message_appeared;
}

t_message_caught* process_catch(t_message_catch* message_catch){

	t_message_caught* message_caught;
	int32_t exists;
	uint32_t caught_result;

		//si no existia el archivo metadata, saltar directamente a generar la respuesta que no se pudo atrapar

	//esperar mutex del pokemon metadata

	//aca hay que hacer un fopen y ver que retorna
	char* pokemon_metadata = string_new();
	string_append(&pokemon_metadata, files_directory);
	string_append(&pokemon_metadata, message_catch->pokemon_name);
	string_append(&pokemon_metadata, "/Metadata.bin");
	t_config* file;
	pthread_mutex_lock(&mutex_pkmetadata);
	if((file = config_create(pokemon_metadata)) == NULL){
		log_warning(logger, "no se pudo leer %s/Metadata.bin", message_catch->pokemon_name);
		log_info(logger, "no existe entonces el caught dice que no se puede atrapar");
		pthread_mutex_unlock(&mutex_pkmetadata);

		caught_result =0;
	}
	else{

		//CANTINDAD DE REINTENTOS
		//ESTO ADENTRO DE UNA FUNCION -> retorna el open
		char* open;
		uint32_t retry = 1;
		while(retry){
			printf("aca estamos bien4\n");
			open = config_get_string_value(file, "OPEN");
			if(strcmp(open, "N") == 0){
				//editar el metada.bin -> OPEN=Y
				config_set_value(file, "OPEN", "Y");

				config_save(file);
				pthread_mutex_unlock(&mutex_pkmetadata);
				printf("TIEMPO_RETARDO_OPERACION\n");
				sleep(TIEMPO_RETARDO_OPERACION); //TODO ver donde va, y tambien en el otro caso

				retry = 0;
			}
			else{
				pthread_mutex_unlock(&mutex_pkmetadata);
				config_destroy(file);
				printf("SLEEP\n");
				sleep(TIEMPO_DE_REINTENTO_OPERACION);
				pthread_mutex_lock(&mutex_pkmetadata);
				file = config_create(pokemon_metadata);
			}
		}
		//ESTO ADENTRO DE UNA FUNCION ->
		//tomar string de blockes y convertir a LISTA DE BLOQUES
		char* blocks_string = config_get_string_value(file,"BLOCKS");
		uint32_t size_metadata = config_get_int_value(file,"SIZE");
		char** blocks_array = string_get_string_as_array(blocks_string);

		//convertir char** en t_list*

		t_list* blocks_list = list_create();
		t_list* blocks_list_int = list_create();
		uint32_t blocks_count = 0;
		while(blocks_array[blocks_count]!=NULL){
			printf("any block is %s \n",blocks_array[blocks_count]);
			list_add(blocks_list,blocks_array[blocks_count]);
			list_add(blocks_list_int,atoi(blocks_array[blocks_count]));
			blocks_count++;
		}

		printf("cargamos bien los blockes\n");

		//PASAR LOS BLOQUES A MEMORIA
		if(list_size(blocks_list_int)>0){
		void* pokemon_file = open_file_blocks(blocks_list, size_metadata);
		//DICCIONARIO CON POSITION(KEY)->CANT(VALUE)
		//CREAR DICCIONARIO DEL POKEMON_FILE
		t_dictionary* pokemon_dictionary =  void_to_dictionary(pokemon_file, size_metadata);

		printf("dictionary get en posicion 8-6 value: %s \n",dictionary_get(pokemon_dictionary, "8-6"));
		printf("dictionary get en posicion 5-5 value: %s \n",dictionary_get(pokemon_dictionary, "5-5"));
		printf("dictionary get en posicion 3-2 value: %s \n",dictionary_get(pokemon_dictionary, "3-2"));
		printf("dictionary get en posicion 1-9 value: %s \n",dictionary_get(pokemon_dictionary, "1-9"));
		/*
		DIRECTORY=N
		SIZE=250
		BLOCKS=[40,21,82,3]
		OPEN=Y
		*/
		//CREAR CHAR* DEL KEY QUE BUSCO
		char* key = string_new();
		char* x = string_itoa(message_catch->position->x);
		char* y = string_itoa(message_catch->position->y);
		string_append(&key, x);
		string_append(&key, "-");
		string_append(&key, y);
		free(x);
		free(y);
		printf("the key for pokemon map is %s\n",key);
		//MIRAR EN EL DICCIONARIO SI EXISTE LA KEY
		if(dictionary_has_key(pokemon_dictionary,key)){
			//MODIFICA VALUE EN EL diccionario
			char * value_str = dictionary_get(pokemon_dictionary,key);
			uint32_t value = atoi(value_str);
			if(value>1){
				value--;
				char* new_value_str = string_itoa(value); //no se free() porque la meto en el diccionario
				dictionary_put(pokemon_dictionary,key,new_value_str);
				//misma cantidad de bloques
			}
			else
			{
				char* new_value_str = dictionary_remove(pokemon_dictionary, key);
				free(new_value_str);
				//luego se calcula el nuevo tamano para ver la cantidad de bloques
			}
			//free(value_str);
			caught_result = 1;
			//PISA LA LISTA DE BLOQUES DEL FILE SYSTEM CON LOS DATOS DEL DICCIONARIO
			//MAPEA EN DISCO DE NUEVO CON LOS CAMBIOS Y HACE EL CAUGHT TRUE.
			uint32_t new_size;
			void* new_pokemon_file = dictionary_to_void(pokemon_dictionary, &new_size);
			char* new_size_to_metadata = string_itoa(new_size);

			//calcular nueva cantidad de bloques
			double aux = ((double)new_size/(double)block_size);
			uint32_t new_blocks_count = (uint32_t) ceil(aux);
			printf("nueva cantidad de bloques: %d\n",new_blocks_count);

			//si cambio la cantidad de bloques, actualizar el metadata y el bitmap, y mi lista de bloques
			//para catch puede haber menos o igual bloques
			if(new_blocks_count != blocks_count){
				//sacar algun bloque al azar de la lista y actualizar el bitmap y el metadabata.bin
				uint32_t diff = blocks_count - new_blocks_count;
				for(uint32_t c = 0; c< diff; c++){
					uint32_t block_number = list_remove(blocks_list_int, 0);
					//ACTUALIZO BITMAP
					//ACA VA UN MUTEX-- TODO
					bitarray_clean_bit(bitmap, block_number);
					msync(bmap, blocks/8, MS_SYNC);
				}
				//se actualizan los bloques en el metadata mas adelante
			}
			printf("tamaño de lista de bloques luego de eliminar: %d\n",list_size(blocks_list_int));
			void imprimir(uint32_t elemt){
				printf("elemento de la lista de bloques: %d\n", elemt);
			}
			list_iterate(blocks_list_int, &imprimir);

			//RECORRER LISTA DE BLOQUES Y ESCRIBIR BLOQUES EN DISCO
			if(list_size(blocks_list_int)>0)
				write_file_blocks((void*)new_pokemon_file, blocks_list_int, new_size, message_catch->pokemon_name);


			printf("Ya se escribieron los bloques \n");
			//ACTUALIZAR EL METADATA.BIN, nueva lista de blocks, el nuevo SIZE, y cambiar OPEN a N
			//BLOCKS=[40,21,82,12]
			//SIZE=250
			//generar el string de bloques
			char* blocks_to_write = string_new();
			uint32_t current_block = 1;
			string_append(&blocks_to_write,"[");

			void list_to_string(uint32_t element){
				char * element_str = string_itoa(element);
				string_append(&blocks_to_write,element_str);
				if(new_blocks_count != current_block)
					string_append(&blocks_to_write,",");
				current_block++;
				free(element_str);
			}
			list_iterate(blocks_list_int, &list_to_string);
			string_append(&blocks_to_write,"]");


			printf("Bloques a escribir en el metadata: %s\n", blocks_to_write);
			config_set_value(file, "BLOCKS", blocks_to_write);
			config_set_value(file, "SIZE", new_size_to_metadata);

			pthread_mutex_lock(&mutex_pkmetadata);
			config_save(file);
			pthread_mutex_unlock(&mutex_pkmetadata);

			free(new_size_to_metadata);
			free(blocks_to_write);




		}
		else
		{
			caught_result = 0;
			//SI NO LO ENCUENTRA ES QUE NO HAY POKEMON EN ESA POSICION CAUGHT FALSE.
			//NO SE ENCONTRO LA POSICION BUSCADA EN EL FILESYSTEM
			//TODO destruir diccionario
			//TODO actualizar el metadata que lo deje de usar


		}

		dictionary_destroy_and_destroy_elements(pokemon_dictionary, (void*) free);


		config_set_value(file, "OPEN","N");

		pthread_mutex_lock(&mutex_pkmetadata);
		config_save(file);
		pthread_mutex_unlock(&mutex_pkmetadata);

		config_destroy(file);


		}
		else{
			caught_result = 0;
		}





	}




	//generar mensaje caught y destruir el mensaje catch

	//Generar mensaje CAUGHT
	message_caught = create_message_caught(message_catch->id, caught_result);
	log_info(logger, "Se genero el mensaje caught");
	destroy_message_catch(message_catch);


	return message_caught;
}

t_message_localized* process_get(t_message_get* message_get){

	t_message_localized* message_localized;

	//crear path del metadata
	char* pokemon_metadata = string_new();
	string_append(&pokemon_metadata, files_directory);
	string_append(&pokemon_metadata, message_get->pokemon_name);
	string_append(&pokemon_metadata, "/Metadata.bin");
	t_config* file;
	pthread_mutex_lock(&mutex_pkmetadata);
	if((file = config_create(pokemon_metadata)) == NULL){
		log_warning(logger, "no se pudo leer %s/Metadata.bin", message_get->pokemon_name);
		log_info(logger, "no existe entonces el caught dice que no se puede atrapar");
		pthread_mutex_unlock(&mutex_pkmetadata);
		//sleep(TIEMPO_RETARDO_OPERACION); //TODO ver donde va, y tambien en el otro caso
		//caught_result =0;
		//retorna aca un localized sin posiciones
		message_localized = create_message_localized(message_get->id, message_get->pokemon_name, 0, NULL);
	}
	else{

		//esperar a obtener propiedad del archivo
		char* open;
		uint32_t retry = 1;
		while(retry){
			printf("aca estamos bien4\n");
			open = config_get_string_value(file, "OPEN");
			if(strcmp(open, "N") == 0){
				//editar el metada.bin -> OPEN=Y
				config_set_value(file, "OPEN", "Y");
				config_save(file);
				pthread_mutex_unlock(&mutex_pkmetadata);
				printf("TIEMPO_RETARDO_OPERACION\n");
				sleep(TIEMPO_RETARDO_OPERACION); //TODO ver donde va, y tambien en el otro caso

				retry = 0;
			}
			else{
				pthread_mutex_unlock(&mutex_pkmetadata);
				config_destroy(file);
				printf("SLEEP\n");
				sleep(TIEMPO_DE_REINTENTO_OPERACION);
				pthread_mutex_lock(&mutex_pkmetadata);
				file = config_create(pokemon_metadata);
			}
		}
		//pasar los bloques del archivo a memoria
		//crear diccionario con el archivo

		//ESTO ADENTRO DE UNA FUNCION ->
		//ARMAR LISTA DE BLOQUES
		//tomar string de blockes y convertir a LISTA DE BLOQUES
		char* blocks_string = config_get_string_value(file,"BLOCKS");
		uint32_t size_metadata = config_get_int_value(file,"SIZE");
		char** blocks_array = string_get_string_as_array(blocks_string);

		//convertir char** en t_list*
		printf("aca estamos bien6\n");
		t_list* blocks_list = list_create();
		t_list* blocks_list_int = list_create();
		uint32_t blocks_count = 0;
		while(blocks_array[blocks_count]!=NULL){
			printf("any block is %s \n",blocks_array[blocks_count]);
			list_add(blocks_list,blocks_array[blocks_count]);
			list_add(blocks_list_int,atoi(blocks_array[blocks_count]));
			blocks_count++;
		}

		printf("cargamos bien los blockes\n");

		//verificar que el archivo contenga al menos un bloque
		if(list_size(blocks_list_int)>0){
			//abrir los bloques y crear diccionario
			void* pokemon_file = open_file_blocks(blocks_list, size_metadata);
			//DICCIONARIO CON POSITION(KEY)->CANT(VALUE)
			t_dictionary* pokemon_dictionary =  void_to_dictionary(pokemon_file, size_metadata);

			printf("dictionary get en posicion 8-6 value: %s \n",dictionary_get(pokemon_dictionary, "8-6"));
			printf("dictionary get en posicion 5-5 value: %s \n",dictionary_get(pokemon_dictionary, "5-5"));
			printf("dictionary get en posicion 3-2 value: %s \n",dictionary_get(pokemon_dictionary, "3-2"));
			printf("dictionary get en posicion 1-9 value: %s \n",dictionary_get(pokemon_dictionary, "1-9"));

			//Obtener todas las posiciones y cantidades de Pokemon pedido.

			//formatear string de la posicion a dos enteros
			uint32_t size_dictionary = dictionary_size(pokemon_dictionary);
			t_position* positions = malloc(size_dictionary * sizeof(t_position));
			uint32_t pos = 0;
			void charge_positions(char* key, void* value){
				//split por - y despues pasarlos a int.
				char** position_array =   string_split(key, "-");
				(positions + pos)->x = atoi(position_array[0]);
				(positions + pos)->y = atoi(position_array[1]);
				pos++;
				free(position_array[1]);
				free(position_array[0]);
				//free(position_array);		//TODO free del char**
			}
			//La función que se pasa por paremtro recibe (char* key, void* value)
			dictionary_iterator(pokemon_dictionary, &charge_positions);
			message_localized = create_message_localized(message_get->id, message_get->pokemon_name, size_dictionary, positions);

			//no se modifica el archivo, entonces con destruir el diccionario alcanza
			//esperar el tiempo de retardo operacion

			dictionary_destroy_and_destroy_elements(pokemon_dictionary, (void*) free);
		}
		else
		{
			//no contenia bloques el archivo entonces ni se crea el diccionario

			message_localized = create_message_localized(message_get->id, message_get->pokemon_name, 0, NULL);
		}

		//actualizar metadata. No van a cambiar ni el size ni la lista de bloques
		config_set_value(file, "OPEN", "N");
		pthread_mutex_lock(&mutex_pkmetadata);
		config_save(file);
		pthread_mutex_unlock(&mutex_pkmetadata);

		config_destroy(file);

	}



	log_info(logger, "Se genero el mensaje localized");
	destroy_message_get(message_get);



	//destruir el diccionario
	//esperar el tiempo de retardo operacion


	//generar mensaje localized y destruir el mensaje get
	return message_localized;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void serve_new(void* input){
	operation_code op_code = ((serve_thread_args*)input)->op_code;
	void* message = ((serve_thread_args*)input)->message;
	free(input);

	if(op_code != OPERATION_NEW)
		log_error(logger, "Aca nunca llego");

	t_message_new* message_new = (t_message_new*) message;

	t_message_appeared* message_appeared;
	//message_appeared =  process_new(message_new); TODO

//-----------------------------------------------------//TODO remover harcodeo, se hace esto en la funcion de arriba
	//Generar mensaje APPEARED
	message_appeared = create_message_appeared_long(message_new->id, message_new->pokemon_name, message_new->location->position->x, message_new->location->position->y);
	log_info(logger, "Se genero el mensaje appeared");
	destroy_message_new(message_new);

//------------------------------------------------------

	t_package* package = serialize_appeared(message_appeared);
	destroy_message_appeared(message_appeared);


	send_to_broker(package);
}

void serve_catch(void* input){
	operation_code op_code = ((serve_thread_args*)input)->op_code;
	void* message = ((serve_thread_args*)input)->message;
	free(input);

	if(op_code != OPERATION_CATCH)
		log_error(logger, "Aca nunca llego");

	t_message_catch* message_catch = (t_message_catch*) message;

	t_message_caught* message_caught;
	message_caught = process_catch(message_catch);



	t_package* package = serialize_caught(message_caught);
	destroy_message_caught(message_caught);


	send_to_broker(package);
}

void serve_get(void* input){
	operation_code op_code = ((serve_thread_args*)input)->op_code;
	void* message = ((serve_thread_args*)input)->message;
	free(input);

	if(op_code != OPERATION_GET)
		log_error(logger, "Aca nunca llego");

	t_message_get* message_get = (t_message_get*) message;

	t_message_localized* message_localized;
	message_localized = process_get(message_get);



	t_package* package = serialize_localized(message_localized);
	destroy_message_localized(message_localized);


	send_to_broker(package);
}


//---------------------------------------------No va------------------------------------------------------------------------------------------------------------------



t_dictionary* semaphores;
pthread_mutex_t semaforo_del_diccionario_de_semaforos_JAJAJA;
//un semaforo para cada pokemon_metadata. En el caso que rompa t0do, se puede usar "sin problema" un unico mutex global para todos los pokemon_metadata
pthread_mutex_t* get_pokemon_mutex(char* pokemon_name){
	pthread_mutex_lock(&semaforo_del_diccionario_de_semaforos_JAJAJA);
	if(dictionary_has_key(semaphores, pokemon_name)){
		pthread_mutex_t* found_semaphore = dictionary_get(semaphores, pokemon_name);
		pthread_mutex_unlock(&semaforo_del_diccionario_de_semaforos_JAJAJA);
		return found_semaphore;
	}
	else{
		pthread_mutex_t new_semaphore;
		pthread_mutex_init(&new_semaphore, NULL);
		dictionary_put(semaphores, pokemon_name, &new_semaphore);
		pthread_mutex_unlock(&semaforo_del_diccionario_de_semaforos_JAJAJA);
		return &new_semaphore;
	}
}
