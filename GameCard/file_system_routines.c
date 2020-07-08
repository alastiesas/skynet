#include "gamecard.h"
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#define BIT_NUMBERING LSB_FIRST

void init_fs(){
	char* metadata_path = (char*) malloc(strlen(PUNTO_MONTAJE_TALLGRASS) + 24);	//22?
	strcpy(metadata_path, PUNTO_MONTAJE_TALLGRASS);
	strcat(metadata_path, "/Metadata/Metadata.bin");
	char* files_metadata_path = string_new();
	string_append(&files_metadata_path, PUNTO_MONTAJE_TALLGRASS);
	string_append(&files_metadata_path, "/Files/Metadata.bin");
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
		//crear archivo metadata
		FILE* file;
		char s[100];
		printf("%s\n", getcwd(s, 100));
		chdir("..");
		printf("%s\n", getcwd(s, 100));
		chdir("..");	//TODO desde eclipse hay un .. de mas
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
		file = fopen(metadata_path, "w");
		if(file == NULL){
			log_error(logger, "dio n ull la afile");
		    printf("fopen failed, errno = %d\n", errno);
		}
		fclose(file);
		metaconfig = config_create(metadata_path);
		config_set_value(metaconfig, "BLOCK_SIZE", block_size_char);
		config_set_value(metaconfig, "BLOCKS", blocks_char);
		config_set_value(metaconfig, "MAGIC_NUMBER", "TALL_GRASS");
		config_save(metaconfig);

		//crear bitmap
		t_bitarray* bitmap = create_bitarray();
		save_bitarray(bitmap);

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
		blocks = config_get_int_value(metaconfig, "BLOCKS");
		block_size = config_get_int_value(metaconfig, "BLOCK_SIZE");
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

t_bitarray* get_bitarray(){
	t_bitarray* bitarray = malloc(sizeof(t_bitarray));
	bitarray->mode = BIT_NUMBERING;
	bitarray->size = blocks;

	//TODO ver donde van los mutex
	FILE *p;
	p = fopen("aca va el path del bitarray", "r");
	fread(bitarray->bitarray, blocks/8, 1, p);
	fclose(p);

	return bitarray;
}

t_bitarray* create_bitarray(){
	t_bitarray* bitarray = malloc(sizeof(t_bitarray));
	bitarray->mode = BIT_NUMBERING;
	bitarray->size = blocks;
	bitarray->bitarray = malloc(blocks/8);
	//TODO setear los bits en 0?

	return bitarray;
}

void save_bitarray(t_bitarray* bitarray){
	//TODO si ya estaba modificando el bitarray entonces el mutex supuestamente ya lo tenia
	FILE *p;
	p = fopen("aca va el path del bitarray", "w");
	fwrite(bitarray->bitarray, blocks/8, 1, p);
	fclose(p);

	free(bitarray->bitarray);
	free(bitarray);
}

void terminate_file_system() {

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
void write_file_blocks(void* pokemon_file, t_list* my_blocks, uint32_t total_size){
	//chequear si ahora el archivo ocupa mas o menos bloques
	uint32_t blocks_amount = total_size/block_size + 1; //?? esta bien?
	if(list_size(my_blocks) < blocks_amount){
		//calcular cuantos bloques mas necesita
		//encontrar esa cantidad de bloques libres
		//agregar los bloques encontrados a my_blocks
	//TODO modificar los bloques en el metadata del pokemon
	}
	else if(list_size(my_blocks) > blocks_amount){
		//calcular cuantos bloques de menos
		//borrar de la lista esta cantidad de bloques ??? puede ser cualquier bloque?
	//TODO modificar los bloques en el metadata del pokemon
	}
	//TODO modificar el size en el metadata del pokemon


	//TODO escribir los bloques, similar a la lectura
	//al abrir un archivo en modo "w", ya borra todo el contenido, no preocuparse si ahora el contenido es menos

	list_destroy(my_blocks);
}

//retorna una lista con amount bloques que esten disponibles en el bitmap
t_list* find_available_blocks(uint32_t amount){
	t_list* available_blocks = list_create();
	//TODO encontrar amount cantidad de bloques libres y meterlos a la lista
	//TODO marcar esos bloques como ocupados en el bitmap
	return available_blocks;
}
