/*
 * utilities.c
 *
 *  Created on: 10 jun. 2020
 *      Author: utnso
 */

#include"utilities.h"

/*
 * int size_array (char* array) retorna el largo de un vector en formato string
 * separado por '|' con valor final NULL
 */
int size_array (char* array)
{
	return char_count(array,'|');
}

/*
 * int char_count(char* array, char parameter)
 * retorna la cantidad de veces que PARAMETER
 * se encuentra en el string ARRAY
 */
int char_count(char* array, char parameter)
{
	int size_test =  strlen (array);
	int count = 1;
	for(int i = 0; i< size_test ; i++)
		if(array[i] == parameter)
					count++;
	return count;
}

/*
 * int size_array_config(char** array)
 * retorna el tamaño de un vector de strings con valor final NULL
 */
int string_list_size(char** string_list)
{
	int j = 0;
		while(string_list[j] != NULL){
			j++;
		}
		return j;
}

void free_string_list(char** string_list) {
	int i = 0;
	while(string_list[i] != NULL) {
		free(string_list[i]);
		i++;
	}
	free(string_list[i]);//NULL también debe liberarse
}
