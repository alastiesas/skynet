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

t_index* search_index(t_index* index ,t_objective* objective)
{
	if(0 == strcmp(objective->pokemon, index->string)){
		index->objective = objective;
	}
	return index;
}

t_objective* find_key(t_list* list, char* key)
{
	t_index* index = malloc(sizeof(t_index));
	index->string = key;
	index->objective = NULL;
	//void*(*function)(void*, void*) = &search_index;
	index = (t_index*) list_fold(list,(void*)index,(void*)&search_index);
	t_objective* objective = index->objective;
	free(index);
	return objective;
}

char* create_copy_string(char* from) {
	char* to = calloc(strlen(from)+1, 1);
	memcpy(to, from, strlen(from)+1);
	return to;

}

char* queue_code_string(queue_code queue) {
	char* string = NULL;
	switch(queue) {
	case COLA_NEW:
		string = create_copy_string("NEW");
		break;
	case COLA_APPEARED:
		string = create_copy_string("APPEARED");
		break;
	case COLA_GET:
		string = create_copy_string("GET");
		break;
	case COLA_LOCALIZED:
		string = create_copy_string("LOCALIZED");
		break;
	case COLA_CATCH:
		string = create_copy_string("CATCH");
		break;
	case COLA_CAUGHT:
		string = create_copy_string("CAUGHT");
		break;
	default:
		string = create_copy_string("");
		break;
	}
	return string;
}

uint32_t max(int32_t x, int32_t y) {
	uint32_t max_number = x;
	if(x < y) {
		max_number = y;
	}
	return max_number;
}

uint32_t min(int32_t x, int32_t y) {
	uint32_t max_number = x;
	if(x > y) {
		max_number = y;
	}
	return max_number;
}

void add_one_to_dictionary(t_dictionary* dictonary, char* pokemon) {
	if(dictionary_has_key(dictonary, pokemon)){
//		uint32_t* count = malloc(sizeof(uint32_t));
//		count = dictionary_get(dictonary, pokemon);
		uint32_t* count = dictionary_get(dictonary, pokemon);
		(*count)++;

	}else {
		uint32_t* count = malloc(sizeof(uint32_t));
		*count = 1;
		dictionary_put(dictonary, pokemon, count);
	}
}

void sub_one_from_dicionary(t_dictionary* dictonary, char* pokemon) {
	if(dictionary_has_key(dictonary, pokemon)){//debe tener el pokemon
		uint32_t* has = dictionary_get(dictonary, pokemon);//ahora se cuantos tiene
		if(*has > 1) {//si tiene más de uno se lo resto
			(*has)--;
		} else {//si solo le queda uno, lo elimino
			dictionary_remove(dictonary, pokemon);
		}


	} else {//si no tiene ese pokemon, errorazo
		printf("ERROR SACANDO UN POKEMON A UN TRAINER QUE NO LO TIENE\n");
		exit(-1);
	}

}









