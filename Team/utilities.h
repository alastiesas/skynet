/*
 * utilities.h
 *
 *  Created on: 10 jun. 2020
 *      Author: utnso
 */

#ifndef UTILITIES_H_
#define UTILITIES_H_

#include"includes.h"
#include"team_structs.h"



int size_array (char*);
int char_count(char* array, char parameter);
int string_list_size(char** array);
void free_string_list(char** string_list);
t_index* search_index(t_index* index ,t_objective* objective);
t_objective* find_key(t_list* list, char* key);
char* create_copy_string(char* from);
char* queue_code_string(queue_code queue);
uint32_t max(int32_t x, int32_t y);
uint32_t min(int32_t x, int32_t y);
void add_one_to_dictionary(t_dictionary* dictonary, char* pokemon);

#endif /* UTILITIES_H_ */
