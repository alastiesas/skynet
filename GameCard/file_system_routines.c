#include "gamecard.h"

void initiliaze_file_system() {

	char* tall_grass_mount_point = config_get_string_value(config, "PUNTO_MONTAJE_TALLGRASS");
	char* metadata_path = (char*) malloc(strlen(tall_grass_mount_point) + 24);
	strcpy(metadata_path, tall_grass_mount_point);
	strcat(metadata_path, "/Metadata/Metadata.bin");

	t_config* metadata = config_create(metadata_path);
	int blocks = config_get_string_value(metadata, "BLOCKS");

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
