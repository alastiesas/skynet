/*
 * messages.c
 *
 *  Created on: 19 jun. 2020
 *      Author: utnso
 */
#include "gamecard.h"
#include <errno.h>


t_message_appeared* new_pokemon_routine(t_message_new* new_pokemon) {

	char* pokemon_name = new_pokemon->pokemon_name;

	pthread_mutex_lock(&mutex_pkmetadata);
	if (!exists_pokemon(pokemon_name)) {

		create_pokemon(pokemon_name);
	}
	pthread_mutex_unlock(&mutex_pkmetadata);

	t_config* pokemon_config = open_pokemon_file(pokemon_name);
	/*
	log_trace(helper, "TIEMPO_RETARDO_OPERACION %d", TIEMPO_RETARDO_OPERACION);
	sleep(TIEMPO_RETARDO_OPERACION); //una vez soltado el mutex y en open Y
	*/ //adentro de la funcion anterior

	/*----------*/


	char* pokemon_blocks = config_get_string_value(pokemon_config, "BLOCKS");
	uint32_t size = config_get_int_value(pokemon_config, "SIZE");


	t_list* blocks_list = list_create();
	t_list* blocks_list_int = list_create();
	uint32_t blocks_count = 0;
	char** blocks_array;

	t_dictionary* pokemon_dictionary;
	if (size == 0) {
		list_destroy(blocks_list);
		//si no tenia bloques se crea un diccionario vacio
		pokemon_dictionary = dictionary_create();
	} else {
		//si tenia bloques, se cargan al diccionario
		char** blocks_array = string_get_string_as_array(pokemon_blocks);
		while (blocks_array[blocks_count] != NULL) {
			list_add(blocks_list, blocks_array[blocks_count]);
			list_add(blocks_list_int, (void*)atoi(blocks_array[blocks_count]));
			blocks_count++;
		}
		void* pokemon_void = open_file_blocks(blocks_list, size);
		pokemon_dictionary = void_to_dictionary(pokemon_void, size);
	}

	/*----------*/
	//actualizar diccionario

	char* key = get_key(new_pokemon->location->position->x, new_pokemon->location->position->y);
	char* previous_value = get_value(pokemon_dictionary, key);
	uint32_t amount = atoi(previous_value) + new_pokemon->location->amount;
	char* value = string_itoa(amount);

	dictionary_put(pokemon_dictionary, key, value);

	free(key);

	/*----------*/
	//calcular nueva cantidad de bloques

	uint32_t new_size;
	void* new_pokemon_file = dictionary_to_void(pokemon_dictionary, &new_size);
	char* new_size_to_metadata = string_itoa(new_size);
	double aux = ((double) new_size / (double) block_size);
	uint32_t new_blocks_count = (uint32_t) ceil(aux);

	/*-----------------------------------*/
	/*si necesita mas bloques, tomar bloques disponibles del bitmap */

	if (new_blocks_count > blocks_count) {
		uint32_t diff = new_blocks_count - blocks_count;

		pthread_mutex_lock(&mutex_bitmap);
		t_list* available_blocks = find_available_blocks(diff);
		for(int i = 0; i < list_size(available_blocks); i++) {
			uint32_t block_number = (uint32_t)list_get(available_blocks, i);
			bitarray_set_bit(bitmap, block_number);
			list_add(blocks_list_int, (void*)block_number);
			msync(bmap, blocks / 8, MS_SYNC);
			log_debug(helper, "%s ocupa el bloque %d", new_pokemon->pokemon_name, block_number);
		}
		pthread_mutex_unlock(&mutex_bitmap);
		list_destroy(available_blocks);
	}

	/*-----------------------------------*/
	//escribir bloques

	write_file_blocks(new_pokemon_file, blocks_list_int, new_size, new_pokemon->pokemon_name);

	log_info(helper, "Escritura de bloques de %s realizada", new_pokemon->pokemon_name);

	//generar string de bloques
	char* blocks_to_write = string_new();
	uint32_t current_block = 1;
	string_append(&blocks_to_write, "[");

	void list_to_string(uint32_t element) {
		char * element_str = string_itoa(element);
		string_append(&blocks_to_write, element_str);
		if (new_blocks_count != current_block)
			string_append(&blocks_to_write, ",");
		current_block++;
		free(element_str);
	}
	list_iterate(blocks_list_int, (void*)list_to_string);
	string_append(&blocks_to_write, "]");
	list_destroy(blocks_list_int);

	//actualizar metadata
	config_set_value(pokemon_config, "BLOCKS", blocks_to_write);
	config_set_value(pokemon_config, "SIZE", new_size_to_metadata);
	config_set_value(pokemon_config, "OPEN", "N");

	pthread_mutex_lock(&mutex_pkmetadata);
	config_save(pokemon_config);
	pthread_mutex_unlock(&mutex_pkmetadata);
	log_info(helper, "Metadata de %s actualizado y liberado", new_pokemon->pokemon_name);

	config_destroy(pokemon_config);
	dictionary_destroy_and_destroy_elements(pokemon_dictionary, (void*) free);

	free(new_size_to_metadata);
	free(blocks_to_write);
	free(new_pokemon_file);

	t_message_appeared* message_appeared = create_message_appeared_long(new_pokemon->id, new_pokemon->pokemon_name, new_pokemon->location->position->x, new_pokemon->location->position->y);

	destroy_message_new(new_pokemon);

	return message_appeared;
}

t_message_caught* process_catch(t_message_catch* message_catch){

	t_message_caught* message_caught;
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
		log_warning(helper, "no existe %s/Metadata.bin, resulta caught false", message_catch->pokemon_name);
		pthread_mutex_unlock(&mutex_pkmetadata);

		//xxx si no puede apropiarse del metadata en Y, entonces tampoco tiene sentido que haga sleep(TIEMPO_RETARDO_OPERACION)
		caught_result =0;
	}
	else{

		//CANTINDAD DE REINTENTOS
		//ESTO ADENTRO DE UNA FUNCION -> retorna el open
		char* open;
		uint32_t retry = 1;
		while(retry){
			open = config_get_string_value(file, "OPEN");
			if(strcmp(open, "N") == 0){
				//editar el metada.bin -> OPEN=Y
				config_set_value(file, "OPEN", "Y");
				config_save(file);
				pthread_mutex_unlock(&mutex_pkmetadata);

				log_trace(helper, "TIEMPO_RETARDO_OPERACION %d", TIEMPO_RETARDO_OPERACION);
				sleep(TIEMPO_RETARDO_OPERACION); //una vez soltado el mutex y en open Y
				retry = 0;
			}
			else{
				pthread_mutex_unlock(&mutex_pkmetadata);
				config_destroy(file);
				log_trace(helper, "TIEMPO_DE_REINTENTO_OPERACION %d (file ocupado)", TIEMPO_DE_REINTENTO_OPERACION);
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
			log_debug(logger, "any block is %s \n",blocks_array[blocks_count]);
			list_add(blocks_list, blocks_array[blocks_count]);
			list_add(blocks_list_int, (void*)atoi(blocks_array[blocks_count]));
			blocks_count++;
		}

		log_debug(logger, "Se cargaron %d bloques en memoria\n", list_size(blocks_list_int));

		//PASAR LOS BLOQUES A MEMORIA
			//chequear primero que tenga bloques
		if(list_size(blocks_list_int)>0){
		void* pokemon_file = open_file_blocks(blocks_list, size_metadata);
		//DICCIONARIO CON POSITION(KEY)->CANT(VALUE)
		//CREAR DICCIONARIO DEL POKEMON_FILE
		t_dictionary* pokemon_dictionary;
		pokemon_dictionary = void_to_dictionary(pokemon_file, size_metadata);

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
		log_debug(logger, "the key for pokemon map is %s\n",key);
		//MIRAR EN EL DICCIONARIO SI EXISTE LA KEY
		if(dictionary_has_key(pokemon_dictionary,key)){
			//MODIFICA VALUE EN EL diccionario
			char* value_str = dictionary_get(pokemon_dictionary,key);
			uint32_t value = atoi(value_str);
			if(value>1){
				value--;
				char* new_value_str = string_itoa(value); //no se free() porque la meto en el diccionario
				dictionary_put(pokemon_dictionary,key,new_value_str);
				//misma cantidad de bloques, salvo que se reduzca un digito
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
			log_debug(logger, "nueva cantidad de bloques: %d\n",new_blocks_count);

			//si cambio la cantidad de bloques, actualizar el metadata y el bitmap, y mi lista de bloques
			//para catch puede haber menos o igual bloques
			if(new_blocks_count != blocks_count){
				//sacar algun bloque al azar de la lista y actualizar el bitmap y el metadabata.bin
				uint32_t diff = blocks_count - new_blocks_count;
				for(uint32_t c = 0; c< diff; c++){
					uint32_t block_number = (uint32_t)list_remove(blocks_list_int, 0);
					//ACTUALIZO BITMAP
					//ACA VA UN MUTEX--
					pthread_mutex_lock(&mutex_bitmap);
					bitarray_clean_bit(bitmap, block_number);
					msync(bmap, blocks/8, MS_SYNC);
					pthread_mutex_unlock(&mutex_bitmap);
					log_debug(helper, "%s libera el bloque %d", message_catch->pokemon_name, block_number);
				}
				//se actualizan los bloques en el metadata mas adelante
			}
			log_debug(logger, "tamaño de lista de bloques luego de eliminar: %d\n",list_size(blocks_list_int));
			void imprimir(uint32_t elemt){
				log_debug(logger, "elemento de la lista de bloques: %d\n", elemt);
			}
			list_iterate(blocks_list_int, (void*)imprimir);

			//RECORRER LISTA DE BLOQUES Y ESCRIBIR BLOQUES EN DISCO
			if(list_size(blocks_list_int)>0)
				write_file_blocks((void*)new_pokemon_file, blocks_list_int, new_size, message_catch->pokemon_name);


			log_info(helper, "Escritura de bloques de %s realizada", message_catch->pokemon_name);
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
			list_iterate(blocks_list_int, (void*)list_to_string);
			string_append(&blocks_to_write,"]");


			log_debug(logger, "Bloques a escribir en el metadata: %s\n", blocks_to_write);
			config_set_value(file, "BLOCKS", blocks_to_write);
			config_set_value(file, "SIZE", new_size_to_metadata);

			pthread_mutex_lock(&mutex_pkmetadata);
			config_save(file);
			pthread_mutex_unlock(&mutex_pkmetadata);

			free(new_size_to_metadata);
			free(blocks_to_write);
			//destruir diccionario y actualizar a open = N
			free(new_pokemon_file);
		}
		else
		{
			log_warning(helper, "%s/Metadata.bin no contenia la posicion %s, resulta caught false", message_catch->pokemon_name, key);
			//caso archivo sin la posicion, diccionario ya creado
			caught_result = 0;
			//SI NO LO ENCUENTRA ES QUE NO HAY POKEMON EN ESA POSICION CAUGHT FALSE.
			//NO SE ENCONTRO LA POSICION BUSCADA EN EL FILESYSTEM
			//destruir diccionario y actualizar a open = N
		}
		free(key);

		dictionary_destroy_and_destroy_elements(pokemon_dictionary, (void*) free);


		}
		else{
			list_destroy_and_destroy_elements(blocks_list, (void*) free);
			//caso archivo sin bloques, diccionario no creado
			log_warning(helper, "%s/Metadata.bin no contenia bloques, resulta caught false", message_catch->pokemon_name);
			caught_result = 0;

		}
		list_destroy(blocks_list_int);

		config_set_value(file, "OPEN","N");

		pthread_mutex_lock(&mutex_pkmetadata);
		config_save(file);
		pthread_mutex_unlock(&mutex_pkmetadata);
		log_info(helper, "Metadata de %s actualizado y liberado", message_catch->pokemon_name);

		config_destroy(file);

	}


	//generar mensaje caught y destruir el mensaje catch

	//Generar mensaje CAUGHT
	message_caught = create_message_caught(message_catch->id, caught_result);
	log_info(logger, "Se genero el mensaje caught");
	destroy_message_catch(message_catch);

	free(pokemon_metadata);
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
		log_warning(helper, "no existe %s/Metadata.bin, resulta localized vacio", message_get->pokemon_name);
		pthread_mutex_unlock(&mutex_pkmetadata);

		//xxx si no puede apropiarse del metadata en Y, entonces tampoco tiene sentido que haga sleep(TIEMPO_RETARDO_OPERACION)
		//retorna aca un localized sin posiciones
		message_localized = create_message_localized(message_get->id, message_get->pokemon_name, 0, NULL);
	}
	else{

		//esperar a obtener propiedad del archivo
		char* open;
		uint32_t retry = 1;
		while(retry){
			open = config_get_string_value(file, "OPEN");
			if(strcmp(open, "N") == 0){
				//editar el metada.bin -> OPEN=Y
				config_set_value(file, "OPEN", "Y");
				config_save(file);
				pthread_mutex_unlock(&mutex_pkmetadata);

				log_trace(helper, "TIEMPO_RETARDO_OPERACION %d", TIEMPO_RETARDO_OPERACION);
				sleep(TIEMPO_RETARDO_OPERACION); //una vez soltado el mutex y en open Y
				retry = 0;
			}
			else{
				pthread_mutex_unlock(&mutex_pkmetadata);
				config_destroy(file);
				log_trace(helper, "TIEMPO_DE_REINTENTO_OPERACION %d (file ocupado)", TIEMPO_DE_REINTENTO_OPERACION);
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

		t_list* blocks_list = list_create();
		t_list* blocks_list_int = list_create();
		uint32_t blocks_count = 0;
		while(blocks_array[blocks_count]!=NULL){
			log_debug(logger, "any block is %s \n",blocks_array[blocks_count]);
			list_add(blocks_list,blocks_array[blocks_count]);
			list_add(blocks_list_int, (void*)atoi(blocks_array[blocks_count]));
			blocks_count++;
		}

		log_debug(logger, "Se cargaron %d bloques en memoria\n", list_size(blocks_list_int));

		//verificar que el archivo contenga al menos un bloque
		if(list_size(blocks_list_int)>0){
			//tiene bloques, entonces existe al menos una posicion para generar el localized
			//abrir los bloques y crear diccionario
			void* pokemon_file = open_file_blocks(blocks_list, size_metadata);
			//DICCIONARIO CON POSITION(KEY)->CANT(VALUE)
			t_dictionary* pokemon_dictionary = void_to_dictionary(pokemon_file, size_metadata);


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

			dictionary_destroy_and_destroy_elements(pokemon_dictionary, (void*) free);

		}
		else
		{
			list_destroy_and_destroy_elements(blocks_list, (void*) free);
			//no contenia bloques el archivo entonces ni se crea el diccionario
			log_warning(helper, "%s/Metadata.bin no contenia bloques, resulta localized vacio", message_get->pokemon_name);
			message_localized = create_message_localized(message_get->id, message_get->pokemon_name, 0, NULL);
		}
		list_destroy(blocks_list_int);

		//actualizar metadata. No van a cambiar ni el size ni la lista de bloques
		config_set_value(file, "OPEN", "N");
		pthread_mutex_lock(&mutex_pkmetadata);
		config_save(file);
		pthread_mutex_unlock(&mutex_pkmetadata);
		log_info(helper, "Metadata de %s actualizado y liberado", message_get->pokemon_name);

		config_destroy(file);

	}


	log_info(logger, "Se genero el mensaje localized");
	destroy_message_get(message_get);

	free(pokemon_metadata);
	//generar mensaje localized y destruir el mensaje get
	return message_localized;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void serve_new(void* input){
	operation_code op_code = ((serve_thread_args*)input)->op_code;
	void* message = ((serve_thread_args*)input)->message;
	free(input);

	if(op_code != OPERATION_NEW)
		log_error(helper, "Aca nunca llego");

	log_info(helper, "Recibido mensaje new_pokemon");
	t_message_new* message_new = (t_message_new*) message;

	t_message_appeared* message_appeared;

	message_appeared = new_pokemon_routine(message_new);

	log_info(logger, "Se genero el mensaje appeared");


	t_package* package = serialize_appeared(message_appeared);
	destroy_message_appeared(message_appeared);


	int32_t result = send_to_broker(package);
	if(result != -1)
		log_info(helper, "Se envio un mensaje APPEARED");
	else{
		free(package->buffer->stream);
		free(package->buffer);
		free(package);
	}
}

void serve_catch(void* input){
	operation_code op_code = ((serve_thread_args*)input)->op_code;
	void* message = ((serve_thread_args*)input)->message;
	free(input);

	if(op_code != OPERATION_CATCH)
		log_error(helper, "Aca nunca llego");

	log_info(helper, "Recibido mensaje catch_pokemon");
	t_message_catch* message_catch = (t_message_catch*) message;

	t_message_caught* message_caught;
	message_caught = process_catch(message_catch);



	t_package* package = serialize_caught(message_caught);
	destroy_message_caught(message_caught);


	int32_t result = send_to_broker(package);
	if(result != -1)
		log_info(helper, "Se envio un mensaje CAUGHT");
	else{
		free(package->buffer->stream);
		free(package->buffer);
		free(package);
	}
}

void serve_get(void* input){
	operation_code op_code = ((serve_thread_args*)input)->op_code;
	void* message = ((serve_thread_args*)input)->message;
	free(input);

	if(op_code != OPERATION_GET)
		log_error(helper, "Aca nunca llego");

	log_info(helper, "Recibido mensaje get_pokemon");
	t_message_get* message_get = (t_message_get*) message;

	t_message_localized* message_localized;
	message_localized = process_get(message_get);



	t_package* package = serialize_localized(message_localized);
	destroy_message_localized(message_localized);


	int32_t result = send_to_broker(package);
	if(result != -1)
		log_info(helper, "Se envio un mensaje LOCALIZED");
	else{
		free(package->buffer->stream);
		free(package->buffer);
		free(package);
	}
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
