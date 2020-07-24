#include "gamecard.h"
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>

#define BIT_NUMBERING LSB_FIRST

void init_fs(){
	//mirar si el filesystem ya existia
	t_config* metaconfig;
	if((metaconfig = config_create(metadata_path)) == NULL){
		blocks = config_get_int_value(config, "DEFAULT_BLOCKS");
		block_size = config_get_int_value(config, "DEFAULT_BLOCK_SIZE");
		log_warning(logger, "Se crea filesystem vacio");
		//se crea un filesystem
		char* blocks_char;
		char* block_size_char;
		blocks_char = config_get_string_value(config, "DEFAULT_BLOCKS");
		block_size_char = config_get_string_value(config, "DEFAULT_BLOCK_SIZE");
		//crear carpetas
		mkdir(PUNTO_MONTAJE_TALLGRASS, 0777);
		mkdir(metadata_directory, 0777);
		mkdir(files_directory, 0777);
		mkdir(blocks_directory, 0777);
		//crear archivo metadata
		FILE* file;
		file = fopen(metadata_path, "w");
		if(file == NULL){
			log_error(logger, "no se pudo crear archivo en /Metadata/Metadata.bin");
			log_warning(logger, "no se pudo crear filesystem en el path dado por config %s", PUNTO_MONTAJE_TALLGRASS);
		    printf("fopen failed, errno = %d\n", errno);
		    exit(-1);
		}
		fclose(file);
		metaconfig = config_create(metadata_path);
		config_set_value(metaconfig, "BLOCK_SIZE", block_size_char);
		config_set_value(metaconfig, "BLOCKS", blocks_char);
		config_set_value(metaconfig, "MAGIC_NUMBER", "TALL_GRASS");
		config_save(metaconfig);

		//crear bitmap
		create_bitarray();

		//crear directorio en /Files
		file = fopen(files_metadata_path, "w");
		fclose(file);
		t_config* files_config = config_create(files_metadata_path);
		config_set_value(files_config, "DIRECTORY", "Y");
		config_save(files_config);
		config_destroy(files_config);
		free(files_metadata_path);
	}
	else{
		//si ya existia el FS, se usa y listo, no se crea nada
		printf("YA EXISTE \n");
		blocks = config_get_int_value(metaconfig, "BLOCKS");
		block_size = config_get_int_value(metaconfig, "BLOCK_SIZE");
		load_bitarray();
	}
	config_destroy(metaconfig);
}


//se crea un bitmap en el caso de crear el FS de 0, devuelve el bitarray para usar
//requiere llamar a save_bitarray luego
void create_bitarray(){
	/*
	t_bitarray* bitarray = malloc(sizeof(t_bitarray));
	bitarray->mode = BIT_NUMBERING;
	bitarray->size = blocks;
	bitarray->bitarray = malloc(blocks/8);
	*/
	//FILE* file = fopen(bitmap_path, "w+");
	int fd = open(bitmap_path, O_CREAT | O_RDWR, 0664);

	if (fd == -1) {
		perror("open file");
		exit(1);
	}

	ftruncate(fd, blocks/8);

	bmap = mmap(NULL, blocks/8, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	if (bmap == MAP_FAILED) {
		perror("mmap");
		close(fd);
		exit(1);
	}

	bitmap = bitarray_create_with_mode((char*) bmap, blocks/8, LSB_FIRST);

	size_t tope = bitarray_get_max_bit(bitmap);

	for(int i = 0; i < tope; i++){

		 bitarray_clean_bit(bitmap, i);
	}


	close(fd);
	msync(bmap, blocks/8, MS_SYNC);


}

void load_bitarray(){
	int fd = open(bitmap_path, O_RDWR);

	if (fd == -1) {
		perror("open file");
		exit(1);
	}

	ftruncate(fd, blocks/8);

	bmap = mmap(NULL, blocks/8, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	if (bmap == MAP_FAILED) {
		perror("mmap");
		close(fd);
		exit(1);
	}

	bitmap = bitarray_create_with_mode((char*) bmap, blocks/8, LSB_FIRST);

	/*
	bitarray_set_bit(bitmap, 511);
	msync(bmap, blocks/8, MS_SYNC);
	*/

	close(fd);

}

//abre el bitmap del archivo, y devuelve un bitarray para poder modificarlo
void get_bitarray(){



	pthread_mutex_lock(&mutex_bitmap);


	//return bitmap;
}

//guardo el bitarray en el bitmap cuando lo termino de usar
void save_bitarray(t_bitarray* bitarray){
	msync(bmap, blocks/8, MS_SYNC);

	//pthread_mutex_unlock(&mutex_bitmap);
}

//convierte una lista de numeros de bloque a un void* con el archivo pokemon
void* open_file_blocks(t_list* file_blocks, uint32_t total_size){
	void* pokemon_file = malloc(total_size);
	FILE *p;
	char* block_number;
	//char* number;

	uint32_t blocks_amount = list_size(file_blocks);
	uint32_t size;
	uint32_t i = 0;

	while(!list_is_empty(file_blocks)){
		char* block_path = string_new();
		block_number = (char*) list_remove(file_blocks, 0);
		//obtener path del archivo

		string_append(&block_path, blocks_directory);
		string_append(&block_path, block_number);
		string_append(&block_path, ".bin");
		printf("estamos en el while1\n");

		//abrir el archivo
		printf("el path es %s \n", block_path);
		p=fopen(block_path,"r");
		if(p == NULL){
		    printf("fopen failed, errno = %d\n", errno);
		}
		//leer el tamano del bloque, o el tamano restante si es el ultimo bloque
		if(list_is_empty(file_blocks))
			size = total_size - (block_size * (blocks_amount - 1));
		else
			size = block_size;

		log_info(logger, "Se van a leer %d bytes del bloque %s\n", size, block_number);
		fread(pokemon_file + (block_size * i), size, 1, p);
		fclose(p);

		i++;
		free(block_path);
		free(block_number);
		printf("estamos en el while2\n");
	}
	char* test = void_to_string(pokemon_file, total_size);
	printf("el file tiene %s\n",test);
	free(test);
	list_destroy(file_blocks);
	return pokemon_file;	//este es mi void* del string del archivo pokemon, sin el '\0' del string
}

//escribe el archivo_pokemon en los bloques
void write_file_blocks(void* pokemon_file, t_list* my_blocks, uint32_t total_size, char* pokemon_name){
	//pthread_mutex_t* my_semaphore = get_pokemon_mutex(pokemon_name);

	//chequear si ahora el archivo ocupa mas o menos bloques
	//double blocks_amount = ceil(total_size/block_size); //?? esta bien?
	printf("elementos de la lista: %d\n", list_size(my_blocks));
	//uint32_t blocks_amount = 1;
	//aca iria tipo de dato float o double y 6.2 y funcion en c math (float y devuelve int)
	/*
	if(list_size(my_blocks) < blocks_amount){
		//calcular cuantos bloques mas necesita
		//encontrar esa cantidad de bloques libres
		//agregar los bloques encontrados a my_blocks
	//TODO modificar los bloques en el metadata (con su mutex) del pokemon
	}

	else if(list_size(my_blocks) > blocks_amount){
		//calcular cuantos bloques de menos
		//borrar de la lista esta cantidad de bloques ??? puede ser cualquier bloque?
	//TODO modificar los bloques en el metadata (con su mutex) del pokemon
	}
	*/

	//TODO modificar el size en el metadata (con su mutex) del pokemon, segun lo que ocupe el nuevo void* pokemon_file


	//TODO escribir los bloques, similar a la lectura, (open_file_blocks())

	void* pos_init = pokemon_file;
	//uint32_t pos_finish = block_size;
	//"hosaldkasldasldkasldkasldksald"
	uint32_t counter = 0;

	uint32_t size_list = list_size(my_blocks);
	uint32_t last_block = (size_list - 1) * block_size;
	uint32_t size;
	void write_block(uint32_t number_block){
		//string a copiar va de pos_init a pos_finish
		//fopen
		printf("ASFASJFIKOASJGOIASMFLPAM\n");
		char* block_path = string_new();
		string_append(&block_path, blocks_directory);
		char* number_block_str = string_itoa(number_block);
		string_append(&block_path,number_block_str);
		free(number_block_str);
		string_append(&block_path,".bin");
		FILE* file = fopen(block_path, "w+");
		if(file == NULL){
		    printf("fopen failed, errno = %d\n", errno);
		    exit(-1);
		}
		//TODO si es el ultimo bloque, escribir lo restante
				//si no, escribir el tamano de un bloque completo
		if(counter == last_block)
			size = total_size - last_block;
		else
			size = block_size;

		fwrite(pos_init, size, 1, file);
		counter+= size;
		fclose(file);

		pos_init += block_size;
		free(block_path);
		//pos_finish =+ block_size;
		printf("SALIMOS DEL WRITE\n");


	}
	list_iterate(my_blocks, (void*)write_block);
	//al abrir un archivo en modo "w", ya borra t0do el contenido, no preocuparse si ahora el contenido es menos

	//list_destroy(my_blocks);
}

//retorna una lista con n cantidad bloques pedidos que esten disponibles en el bitmap
t_list* find_available_blocks(uint32_t amount){

	t_list* available_blocks = list_create();

	//list_add(available_blocks,5);
	//TODO se estaban agregando 5 elementos a la lista en vez de 1
	uint32_t until = 0;
	for(uint32_t i = 0; i<blocks;i++){
		//pthread_mutex_lock(&mutex_bitmap);
		bool result = bitarray_test_bit(bitmap, i);
		printf("valor del bit %d\n",result);
		if(result == 0){
			if(until<amount){
				list_add(available_blocks,i);
				until++;
			}
			if(until==amount)
				break;
		}
		else{
			//printf("bloque no disponible o ocupado\n");
		}
	}

	if(list_size(available_blocks)<amount){
		printf("espacio suficiente no disponible en disco\n");
		list_clean(available_blocks);
	}
	else
	{
		void bitarray_set_bit_iterate(int32_t index){
			bitarray_set_bit(bitmap, index);
		}
		printf("si llego aca\n");
		list_iterate(available_blocks, (void*)bitarray_set_bit_iterate);
	}
	save_bitarray(bitmap);


	return available_blocks;
}

t_dictionary* void_to_dictionary(void* pokemon_file, uint32_t total_size){

	t_dictionary* dictionary = dictionary_create();


	char* test = string_new();
	char* temp = void_to_string(pokemon_file, total_size);
	string_append(&test, temp);
	free(temp);
	string_append(&test, "\0");

	char** lines = string_split(test, "\n");

	void add_cofiguration(char *line) {
		if (!string_starts_with(line, "#")) {
			char** keyAndValue = string_n_split(line, 2, "=");
			dictionary_put(dictionary, keyAndValue[0], keyAndValue[1]);
			free(keyAndValue[0]);
			free(keyAndValue);
		}
	}
	string_iterate_lines(lines, add_cofiguration);
	string_iterate_lines(lines, (void*) free);

	free(test);
	free(lines);
	free(pokemon_file);
	//pasar el void* a un diccionario como hace config_create() de config.c
	return dictionary;
}

void* dictionary_to_void(t_dictionary* pokemon_file_dictionary, uint32_t* size){

	void* pokemon_file;

	char* lines = string_new();
	void add_line(char* key, void* value) {
		string_append_with_format(&lines, "%s=%s\n", key, value);
	}

	dictionary_iterator(pokemon_file_dictionary, add_line);

	*size = strlen(lines);
	//pokemon_file = string_to_void(lines, *size);


	return (void*)lines;
}

void create_file_directory(char* pokemon,t_location* location){
	//

	//CREAR DIRECTORIO
	char* pokemon_path = string_new();
	string_append(&pokemon_path, files_directory);
	string_append(&pokemon_path, pokemon);
	//buscar mkdir pasandole todo el path
	mkdir(pokemon_path, 0777);
	free(pokemon_path);
	//	//1-1=10
		//bitmap con todos los bloques
		//tendriamos que tener la cantidad de bloques que necesitamos
		//funcion que en base a la cantidad nos de una lista de bloques vacios y ordenados (lista ordenada)
		//con esa lista tenemos escribir los bloques.
		//tenemos que pasar esa lista de bloques al metadata.bin del pokemon.
	//
	//pasar location a string
	char* location_string = location_to_string(location);


	uint32_t total_size = strlen(location_string);

	//double aux = total_size/block_size;
	printf("total_size %d\n",total_size);
	printf("block_size %d\n",block_size);
	//0.093
	double aux = ((double)total_size/(double)block_size);
	uint32_t blocks_amount = (uint32_t) ceil(aux);
	printf("a ver el numero%d\n",blocks_amount);
	t_list* available_blocks = find_available_blocks(blocks_amount);
	//RECORRER LISTA DE BLOQUES Y CREAR ARCHIVOS
	write_file_blocks((void*)location_string, available_blocks, total_size, pokemon);
	//Crear cada bloque y escribirlo. Esto es por va en carpeta Blockes -> 1.bin (1-1=10 ...)
	//ASIGNAR BLOQUES
	//CREAR METADATA.BIN
}

void update_pokemon_metadata(char* pokemon){
	//entrar en el metada.bin del directorio del pokemon -> con esto tenemos caules son sus bloques.
	//con los bloques traer a memoria el contenido del archivo.
	//mapear en un diccionario el contenido.
	//si existe esa posicion, actualizarla.
	//si no existe esa posicion agregarla.
}

char* location_to_string(t_location* location){
	char* location_string = string_new();
	char* amount = string_itoa(location->amount);
	char* posx = string_itoa(location->position->x);
	char* posy = string_itoa(location->position->y);
	string_append(&location_string, posx);
	string_append(&location_string, "-");
	string_append(&location_string, posy);
	string_append(&location_string, "=");
	string_append(&location_string, amount);
	string_append(&location_string, "\n");
	free(amount);
	free(posx);
	free(posy);
	return location_string;
}


bool exists_pokemon(char* pokemon_name) {

	char* pokemon_file = get_pokemon_file(pokemon_name);

	bool exists = exists_file(pokemon_file);

	free(pokemon_file);

	return exists;
}

char* get_pokemon_directory(char* pokemon_name) {

	char* pokemon_directory = (char*) malloc(strlen(files_directory) + strlen(pokemon_name) + 1);
	strcpy(pokemon_directory, files_directory);
	strcat(pokemon_directory, pokemon_name);

	return pokemon_directory;
}

char* get_pokemon_file(char* pokemon_name) {

	char* pokemon_directory = get_pokemon_directory(pokemon_name);
	char* pokemon_file = (char*) malloc(strlen(pokemon_directory) + 14);
	strcpy(pokemon_file, pokemon_directory);
	strcat(pokemon_file, "/Metadata.bin");

	free(pokemon_directory);

	return pokemon_file;
}

bool exists_file(char* file_name) {

	bool exists = false;

	FILE* file;
	if ((file = fopen(file_name, "r"))) {
		fclose(file);
		exists = true;
	}

	return exists;
}

void create_pokemon(char* pokemon_name) {

	create_pokemon_directory(pokemon_name);
	create_pokemon_file(pokemon_name);
}

void create_pokemon_directory(char* pokemon_name) {

	char* pokemon_directory = get_pokemon_directory(pokemon_name);

	mkdir(pokemon_directory, 0777); //not working

	free(pokemon_directory);
}

void create_pokemon_file(char* pokemon_name) {

	char* pokemon_file = get_pokemon_file(pokemon_name);

	FILE* file;
	file = fopen(pokemon_file, "w");

	fprintf(file, "DIRECTORY=N\n");
	fprintf(file, "SIZE=0\n");
	fprintf(file, "BLOCKS=[]\n");
	fprintf(file, "OPEN=N\n");

	fclose(file); //implicit free(file)
	free(pokemon_file);
}

t_config* open_pokemon_file(char* pokemon_name) {

	char* pokemon_file_path = get_pokemon_file(pokemon_name);
	t_config* pokemon_config; //change object?
	char* open;

	do {
		pthread_mutex_lock(&mutex_pkmetadata);
		pokemon_config = config_create(pokemon_file_path);
		open = config_get_string_value(pokemon_config, "OPEN");
		if (strcmp(open, "Y") == 0) {
			pthread_mutex_unlock(&mutex_pkmetadata);
			config_destroy(pokemon_config);
			log_info(logger, "TIEMPO_DE_REINTENTO_OPERACION %d\n", TIEMPO_DE_REINTENTO_OPERACION);
			sleep(TIEMPO_DE_REINTENTO_OPERACION);
		}
	} while (strcmp(open, "Y") == 0);

	config_set_value(pokemon_config, "OPEN", "Y");
	config_save(pokemon_config);
	pthread_mutex_unlock(&mutex_pkmetadata);

	free(pokemon_file_path);

	return pokemon_config;
}

char* get_key(uint32_t position_x, uint32_t position_y) {

	char* x = string_itoa(position_x);
	char* y = string_itoa(position_y);
	char* key = (char*) malloc(strlen(x) + 2 + strlen(y));
	strcpy(key, x);
	strcat(key, "-");
	strcat(key, y);

	free(x);
	free(y);

	return key;
}

char* get_value(t_dictionary* dictionary, char* key) {

	char* value;

	if (dictionary_has_key(dictionary, key)) {
		value = dictionary_get(dictionary, key);
	} else {
		value = "0";
	}

	return value;
}

void update_pokemon_file() {

}

void send_appeared_pokemon() {

}

void initiliaze_file_system() {

	char* tall_grass_mount_point = config_get_string_value(config, "PUNTO_MONTAJE_TALLGRASS");
	char* metadata_path = (char*) malloc(strlen(tall_grass_mount_point) + 24);	//22?
	strcpy(metadata_path, tall_grass_mount_point);
	strcat(metadata_path, "/Metadata/Metadata.bin");

	t_config* metadata = config_create(metadata_path);
	blocks = config_get_int_value(metadata, "DEFAULT_BLOCKS");
	block_size = config_get_int_value(metadata, "DEFAULT_BLOCK_SIZE");

	char* blocks_directory = (char*) malloc(strlen(tall_grass_mount_point) + 8);
	strcpy(blocks_directory, tall_grass_mount_point);
	strcat(blocks_directory, "/Blocks/");

	FILE* file;
	char* file_name = (char*) malloc(strlen(blocks_directory) + strlen(string_itoa(blocks) + 4));
	for (int block = 1; block <= blocks; block++) {

		strcpy(file_name, blocks_directory);
		strcat(file_name, string_itoa(block));
		strcat(file_name, ".bin");
		file = fopen(file_name, "w");
		free(file);
	}
}

void terminate_file_system() {

}
