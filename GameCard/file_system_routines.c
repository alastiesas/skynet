#include "gamecard.h"
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <math.h>
#include <sys/mman.h>
#include <fcntl.h>
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
		//crear carpetas		//TODO remover harcodeo de filesystem adentro de la carpeta del tp
		char s[100];
		printf("%s\n", getcwd(s, 100));
		chdir("..");
		printf("%s\n", getcwd(s, 100));
		//chdir("..");	//TODO desde eclipse hay un .. de mas
		printf("%s\n", getcwd(s, 100));
		mkdir("tall_grass", 0777);
		chdir("tall_grass");
		mkdir("Metadata", 0777);
		mkdir("Files", 0777);
		mkdir("Blocks", 0777);
		chdir("..");
		chdir("GameCard");
		chdir("Debug");
		printf("%s\n", getcwd(s, 100));
		//crear archivo metadata
		FILE* file;
		file = fopen(metadata_path, "w");
		if(file == NULL){
			log_error(logger, "no se pudo crear archivo en /Metadata/Metadata.bin");
		    printf("fopen failed, errno = %d\n", errno);
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
		void load_bitarray();
	}
	config_destroy(metaconfig);
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
	int fd = open(bitmap_path, O_RDWR, 0664);

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
}

//abre el bitmap del archivo, y devuelve un bitarray para poder modificarlo
void get_bitarray(){



	pthread_mutex_lock(&mutex_bitmap);


	//return bitmap;
}

//guardo el bitarray en el bitmap cuando lo termino de usar
void save_bitarray(t_bitarray* bitarray){
	printf("aca rompioooo");
	msync(bmap, blocks/8, MS_SYNC);

	pthread_mutex_unlock(&mutex_bitmap);
}

//convierte una lista de numeros de bloque a un void* con el archivo pokemon
void* open_file_blocks(t_list* file_blocks, uint32_t total_size){
	void* pokemon_file = malloc(total_size);
	FILE *p;
	uint32_t block_number;
	char* number;
	char* block_path;
	uint32_t blocks_amount = list_size(file_blocks);
	uint32_t size;
	uint32_t i = 0;

	while(!list_is_empty(file_blocks)){
		block_number = (uint32_t) list_remove(file_blocks, 0);
		//obtener path del archivo
		number = string_itoa(block_number);
		block_path = string_new();
		string_append(&block_path, number);
		string_append(&block_path, ".bin");
		free(number);
		//abrir el archivo
		p=fopen(block_path,"r");
		//leer el tamano del bloque, o el tamano restante si es el ultimo bloque
		if(list_is_empty(file_blocks))
			size = total_size - (block_size * (blocks_amount - 1));
		else
			size = block_size;

		fread(pokemon_file + (block_size * i), size, 1, p);
		fclose(p);

		free(block_path);
		i++;
	}

	list_destroy(file_blocks);
	return pokemon_file;	//este es mi void* del string del archivo pokemon, sin el '\0' del string
}

//escribe el archivo_pokemon en los bloques
void write_file_blocks(void* pokemon_file, t_list* my_blocks, uint32_t total_size, char* pokemon_name){
	//pthread_mutex_t* my_semaphore = get_pokemon_mutex(pokemon_name);

	//chequear si ahora el archivo ocupa mas o menos bloques
	//double blocks_amount = ceil(total_size/block_size); //?? esta bien?
	uint32_t blocks_amount;
	//aca iria tipo de dato float o double y 6.2 y funcion en c math (float y devuelve int)
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

	//TODO modificar el size en el metadata (con su mutex) del pokemon, segun lo que ocupe el nuevo void* pokemon_file


	//TODO escribir los bloques, similar a la lectura, (open_file_blocks())

	void* pos_init = pokemon_file;
	//uint32_t pos_finish = block_size;
	//"hosaldkasldasldkasldkasldksald"
	void write_block(uint32_t number_block){
		//string a copiar va de pos_init a pos_finish
		//fopen

		char* block_path = string_new();
		block_path = PUNTO_MONTAJE_TALLGRASS;
		string_append(&block_path,PUNTO_MONTAJE_TALLGRASS);
		string_append(&block_path,"/Blocks/");
		char* number_block_str = string_itoa(number_block);
		string_append(&block_path,number_block_str);
		string_append(&block_path,".bin");
		FILE* file = fopen(block_path, "w+");
		//fwrite(void*, tamno, )

		fwrite(pos_init, 1, block_size,	file);
		fclose(file);
		printf("acallegofile\n");
		pos_init =+ block_size;
		free(number_block_str);
		free(block_path);
		//pos_finish =+ block_size;


	}
	list_iterate(my_blocks, &write_block);
	//al abrir un archivo en modo "w", ya borra t0do el contenido, no preocuparse si ahora el contenido es menos
	printf("ACALLEGOOOOOOOO?\n");
	list_destroy(my_blocks);
}

//retorna una lista con n cantidad bloques pedidos que esten disponibles en el bitmap
t_list* find_available_blocks(uint32_t amount){
	printf("aca no llega44\n");
	t_list* available_blocks = list_create();
	printf("aca no llega55\n");
	list_add(available_blocks,5);
	printf("aca no llega22\n");
	//t_bitarray* bitmap = get_bitarray();
	printf("aca no llega33\n");
	uint32_t until = 0;
	for(uint32_t i = 0; i<blocks;i++){
		printf("aca no llega43\n");
		bool result = bitarray_test_bit(bitmap, i);
		if(result){
			if(until<amount)
				list_add(available_blocks,i);
		}
		else
			printf("bloque no disponible o ocupado\n");
	}
	printf("aca no llega44\n");
	if(list_size(available_blocks)<amount){
		printf("espacio suficiente no disponible en disco\n");
		list_clean(available_blocks);
	}
	else
	{
		void bitarray_set_bit_iterate(int32_t index){
			bitarray_test_bit(bitmap, index);
		}
		printf("aca no llega");
		list_iterate(available_blocks,&bitarray_set_bit_iterate);
	}
	save_bitarray(bitmap);
	return available_blocks;
}

t_dictionary* void_to_dictionary(void* pokemon_file){

	t_dictionary* dictionary = dictionary_create();

	//pasar el void* a un diccionario como hace config_create() de config.c

	return dictionary;
}

void* dictionary_to_void(t_dictionary* pokemon_file_dictionary){

	void* pokemon_file;

	//escribir el diccionario en un void*, similar a config_save_in_file() de config.c

	//destruir el diccionario

	return pokemon_file;
}

void create_pokemon_directory(char* pokemon,t_location* location){
	//

	//CREAR DIRECTORIO
	chdir("..");
	//chdir("..");
	chdir("tall_grass");
	chdir("Files");
	//buscar mkdir pasandole todo el path
	mkdir(pokemon, 0777);
	//	//1-1=10
		//bitmap con todos los bloques
		//tendriamos que tener la cantidad de bloques que necesitamos
		//funcion que en base a la cantidad nos de una lista de bloques vacios y ordenados (lista ordenada)
		//con esa lista tenemos escribir los bloques.
		//tenemos que pasar esa lista de bloques al metadata.bin del pokemon.
	//
	//pasar location a string
	char* location_string = location_to_string(location);
	printf("aca llego0\n");
	uint32_t total_size = strlen(location_string);
	printf("aca llego 1\n");
	//double blocks_amount = ceil(total_size/block_size);
	t_list* available_blocks = find_available_blocks(1);
	printf("aca llego2\n");
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

