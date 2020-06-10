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
int size_array_config(char** array)
{
	int j = 0;
		while(array[j] != NULL){
			j++;
		}
		return j;
}
