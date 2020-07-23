#include "broker.h"

void create_dynamic_partition(uint32_t size) {

	t_partition* partition = malloc(sizeof(t_partition));
	t_partition* available_partition;
	//available_partition = find_available_dynamic_partition(size);
	// define uint32_t delimiting_position = available_partition->initial_position + (int) size;

	partition->available = false;
	partition->size = size;
	partition->initial_position = available_partition->initial_position;
	// define partition->final_position = delimiting_position;
	available_partition->size = available_partition->size - size;
	// define available_partition->initial_position = delimiting_position;

	list_add(partitions, partition);
	free(partition);
}

void create_fixed_partition(uint32_t size) {

}

void create_partition(uint32_t size) {

	if (strcmp(memory_algorithm, "BS") == 0) {

		create_fixed_partition(size);
	} else if (strcmp(memory_algorithm, "PARTICIONES") == 0) {

		create_dynamic_partition(size);
	}
}

void delete_dynamic_partition(t_list** deleted_messages) {
	uint32_t message_id;
	uint32_t victim_partition_number = get_partition_number_to_delete(&message_id);
	int32_t previous_partition_number = victim_partition_number - 1;
	uint32_t next_partition_number = victim_partition_number + 1;
	t_partition* victim_partition = list_get(partitions, victim_partition_number);
	list_add(*deleted_messages, (void*)message_id);
	list_add(*deleted_messages, (void*)(victim_partition->queue_code));
	uint32_t log_initial = victim_partition->initial_position - mem;
	uint32_t log_final = victim_partition->final_position - mem;

	victim_partition->available = true;
	if(min_partition_size > victim_partition->size)
		victim_partition->size = min_partition_size;

	if (next_partition_number < list_size(partitions)) {

		t_partition* next_partition = list_get(partitions, next_partition_number);
		if (next_partition->available) {

			merge_partitions(victim_partition_number, next_partition_number);
		}
	}

	if (previous_partition_number > -1) {

		t_partition* previous_partition = list_get(partitions, previous_partition_number);
		if (previous_partition->available) {

			merge_partitions(previous_partition_number, victim_partition_number);
		}
	}

	log_info(obligatorio, "Se elimina la particion en posicion %d - %d", log_initial, log_final);
	//eliminar la particion implica convertirla en disponible (no se hacen frees)
}

void delete_buddy_partition(t_list** deleted_messages) {
	uint32_t message_id;

	uint32_t victim_partition_number = get_partition_number_to_delete(&message_id);


	t_partition* victim_partition = list_get(partitions, victim_partition_number);
	list_add(*deleted_messages, (void*)message_id);
	list_add(*deleted_messages, (void*)(victim_partition->queue_code));
	uint32_t log_initial = victim_partition->initial_position - mem;
	uint32_t log_final = victim_partition->final_position - mem;

	victim_partition->available = true;
	log_info(obligatorio, "Se elimina la particion en posicion %d - %d", log_initial, log_final);
	//eliminar la particion implica convertirla en disponible (no se hacen frees)

	int32_t times_compacted_right;
	int32_t times_compacted_left;
	bool result = false;

	do {
		times_compacted_right = 0;
		times_compacted_left = 0;

		if (victim_partition_number < list_size(partitions) - 1){
			times_compacted_right = compact_buddy_to_right(victim_partition_number);
		}


		if (victim_partition_number > 0){
			times_compacted_left = compact_buddy_to_left(victim_partition_number);
			victim_partition_number -= times_compacted_left;
		}

		if (times_compacted_right > 0 || times_compacted_left > 0) {
			result = true;
		}else {
			result = false;
		}

	} while (result);



//	if(min_partition_size > victim_partition->size)
//		victim_partition->size = min_partition_size;


}

int32_t compact_buddy_to_right(int32_t partition_index){
	int32_t next_partition_index;
	next_partition_index = partition_index;
	int32_t times_compacted = 0;
			while (next_partition_index + 1 <= list_size(partitions) && is_free_partition_by_index(next_partition_index + 1) && is_buddy(next_partition_index, next_partition_index + 1 )) {

				log_info(obligatorio, "Se asocia la particion (%d-%d) con la particion (%d-%d)", ((t_partition*)list_get(partitions, next_partition_index))->initial_position - mem,
																									((t_partition*)list_get(partitions, next_partition_index))->final_position - mem,
																									((t_partition*)list_get(partitions, next_partition_index + 1))->initial_position - mem,
																									((t_partition*)list_get(partitions, next_partition_index + 1))->final_position - mem);
				merge_partitions(next_partition_index, next_partition_index + 1);
				times_compacted ++;
			}
	return times_compacted;
}

int32_t compact_buddy_to_left(int32_t partition_index){
	int32_t previous_partition_index;
	previous_partition_index = partition_index;
	int32_t times_compacted = 0;
			while (previous_partition_index - 1 >= 0 && is_free_partition_by_index(previous_partition_index - 1) && is_buddy(previous_partition_index, previous_partition_index - 1 )) {

				log_info(obligatorio, "Se asocia la particion (%d-%d) con la particion (%d-%d)", ((t_partition*)list_get(partitions, previous_partition_index - 1))->initial_position - mem,
																									((t_partition*)list_get(partitions, previous_partition_index - 1))->final_position - mem,
																									((t_partition*)list_get(partitions, previous_partition_index))->initial_position - mem,
																									((t_partition*)list_get(partitions, previous_partition_index))->final_position - mem);
				merge_partitions(previous_partition_index -1, previous_partition_index);
				previous_partition_index --;

				times_compacted ++;
			}
	return times_compacted;
}

bool is_free_partition_by_index(int32_t partition_index){
	bool is_free = false;
	t_partition* partition = list_get(partitions, partition_index);
	is_free = partition->available;
	return is_free;
}

bool is_buddy (int32_t partition_index, int32_t buddy_index){

	t_partition* partition;
	t_partition* buddy;
	bool is_buddy = false;

	partition = list_get(partitions, partition_index);
	buddy = list_get(partitions, buddy_index);

	if (get_size_partition_by_index(partition_index) == get_size_partition_by_index(buddy_index) && (int32_t)(partition->initial_position - mem) == ((int32_t)(buddy->initial_position - mem) ^ (int32_t)get_size_partition_by_index(partition_index)) && (int32_t)(buddy->initial_position - mem) == ((int32_t)(partition->initial_position - mem)^ (int32_t)get_size_partition_by_index(buddy_index))) {
		is_buddy = true;
	}

	return is_buddy;
}

void delete_fixed_partition() {

}

void delete_partition() {

}

/* compaction_frequency < 0:
 * 1. busco
 * 1.1. si encuentro, me voy
 * 1.2. si no encuentro, elimino
 * 2. repito
 * compaction_frequency == 0 || compaction_frequency == 1:
 * 1. busco
 * 1.1. si encuentro, me voy
 * 1.2. si no encuentro, incremento el contador
 * 1.2.1. si el contador fue incrementado solo una vez, elimino
 * 1.2.2. si el contador fue incrementado mas de una vez, compacto y reinicio el contador
 * 2. repito
 * compaction_frequency > 1:
 * 1. busco
 * 1.1. si encuentro, me voy
 * 1.2. si no encuentro, incremento el contador
 * 1.2.1. si el contador es menor a la frecuencia de compactacion, elimino
 * 1.2.2. si el contador es igual a la frecuencia de compactacion, compacto y reinicio el contador
 * 2. repito */
int32_t find_available_dynamic_partition(uint32_t size, t_list** deleted_messages) {

	int32_t available_partition_number;
	uint32_t count = 0;

	do {

		available_partition_number = get_available_partition_number(size);
		if (available_partition_number == -1) {

			if (compaction_frequency < 0) {

				delete_dynamic_partition(deleted_messages);
			} else {

				count++;
				if (count == 1 || (compaction_frequency > 1 && count < compaction_frequency)) {

					delete_dynamic_partition(deleted_messages);
				} else {

					memory_compaction();
					count = 0;
				}
			}
		}
	} while (available_partition_number == -1);

	return available_partition_number;
}

int32_t find_available_buddy_partition(uint32_t size, t_list** deleted_messages) {

	int32_t available_partition_number;

	do {

		available_partition_number = get_available_partition_number_buddy(size);
		if (available_partition_number == -2) {
			delete_buddy_partition(deleted_messages);
		}
	} while (available_partition_number == -2);

	return available_partition_number;
}

/*int32_t _find_available_dynamic_partition(uint32_t size, t_list** deleted_messages){
	int32_t available_partition_number = -1;
	uint32_t current_compation_value = compaction_frequency;

	if(compaction_frequency == -1){	//no se compacta nunca
		while(available_partition_number != -1){

			available_partition_number = get_available_partition_number(size);
			if(available_partition_number != -1)
				return available_partition_number;
			delete_dynamic_partition(deleted_messages);

		}
	}

	while(available_partition_number == -1){

		while(current_compation_value > 1){ //cantidad de veces (frecuencia - 1) (hasta n-1)
			available_partition_number = get_available_partition_number(size);
			if(available_partition_number != -1)
				return available_partition_number;
			delete_dynamic_partition(deleted_messages);
			current_compation_value--;
		}

		available_partition_number = get_available_partition_number(size);
		if(available_partition_number == -1){
			memory_compaction();
			current_compation_value = compaction_frequency;
			available_partition_number = get_available_partition_number(size);
			if(available_partition_number == -1)
				delete_dynamic_partition(deleted_messages);
		}

	}

	return available_partition_number;
}*/

void merge_partitions(uint32_t initial_partition_number, uint32_t final_partition_number) {

	t_partition* initial_partition = list_get(partitions, initial_partition_number);
	t_partition* final_partition = list_get(partitions, final_partition_number);

	initial_partition->final_position = final_partition->final_position;
	initial_partition->size += final_partition->size;
	final_partition = list_remove(partitions, final_partition_number);
	free(final_partition);
}

int32_t get_available_partition_number(uint32_t size) {

	t_partition* partition;
	int32_t partition_number = -1;

	if (strcmp(free_partition_algorithm, "BF") == 0) {

		uint32_t remaining_size = memory_size;

		for (int i = 0; remaining_size > 0 && i < list_size(partitions); i++) {

			partition = list_get(partitions, i);
			if (partition->available && partition->size - size < remaining_size && partition->size >= size) {

				partition_number = i;
				remaining_size = partition->size - size;
			}
		}
	} else if (strcmp(free_partition_algorithm, "FF") == 0) {

		for (int i = 0; partition_number == -1 && i < list_size(partitions); i++) {

			partition = list_get(partitions, i);
			if (partition->available && partition->size >= size) {

				partition_number = i;
			}
		}
	}

	return partition_number;
}

uint32_t get_available_partition_number_buddy(uint32_t size) {

	t_partition* partition;
	int32_t partition_number = -1;

	uint32_t partition_size = get_buddy_partition_size(size);

	bool partition_finded = false;
	bool available_space = false;

	if (strcmp(free_partition_algorithm, "BF") == 0) {

		for (int i = 0; !partition_finded && i < list_size(partitions); i++) {

			partition = list_get(partitions, i);
			if (partition->available == true){
				if (get_size_partition_by_index(i) >= partition_size) {
					available_space = true;
					if (get_size_partition_by_index(i) == partition_size) {
						partition_finded = true;
						partition_number = i;
					}
				}
			}

		}
		if (!partition_finded) {
			if (available_space) {
				partition_number = create_and_search_partition_buddy(partition_size, -1);
			}else {
				partition_number = -2; //Codigo para indicar que debe liberar
			}
		}

	} else if (strcmp(free_partition_algorithm, "FF") == 0) {

		for (int i = 0; (partition_number == -2 || partition_number == -1) && i < list_size(partitions);
				i++) {

			partition = list_get(partitions, i);
			if (partition->available == true && get_size_partition_by_index(i) >= size) {

				if (get_size_partition_by_index(i) == partition_size) {
					partition_number = i;
				}else {
					partition_number = create_and_search_partition_buddy(partition_size, i);
				}

			}else {
				partition_number = -2; //Codigo para indicar que debe liberar
			}
		}
	}

	return partition_number;
}

int32_t get_size_partition_by_index(int32_t index_partition){
	t_partition* partition = list_get(partitions, index_partition);

	return partition->final_position - partition->initial_position;
}

int32_t create_and_search_partition_buddy(uint32_t searched_size, int32_t partition_location){

	t_partition* partition;
	uint32_t new_size;
	int32_t partition_number;

	if (partition_location == -1) {

		for (int i = 0;i < list_size(partitions); i++) {
			partition = list_get(partitions, i);

			if (get_size_partition_by_index(i) >= searched_size) {
				new_size = get_size_partition_by_index(i);
				while (new_size > searched_size && new_size >= min_partition_size) {
					partition = list_get(partitions, i);
					new_size = get_size_partition_by_index(i)/2;
					partition->size = new_size;
					partition->final_position = partition->final_position - new_size;

					t_partition* new_buddy_partition = malloc(sizeof(t_partition));
					new_buddy_partition->available = true;
					new_buddy_partition->size = new_size;
					new_buddy_partition->initial_position = partition->final_position;
					new_buddy_partition->final_position = new_buddy_partition->initial_position + new_size;

					list_add_in_index(partitions, i + 1, new_buddy_partition);

				}
				partition_number = i;
			}
		}
	}else {
		partition = list_get(partitions, partition_location);

		if (get_size_partition_by_index(partition_location) >= searched_size) {
			new_size = partition->size;
			while (new_size > searched_size && new_size >= min_partition_size) {
				partition = list_get(partitions, partition_location);
				new_size = partition->size/2;
				partition->size = new_size;
				partition->final_position = partition->final_position - new_size;

				t_partition* new_buddy_partition = malloc(sizeof(t_partition));
				new_buddy_partition->available = true;
				new_buddy_partition->size = new_size;
				new_buddy_partition->initial_position = partition->final_position;
				new_buddy_partition->final_position = new_buddy_partition->initial_position + new_size;

				list_add_in_index(partitions, partition_location + 1, new_buddy_partition);
			}
			partition_number = partition_location;
		}
	}
	return partition_number;
}

uint32_t get_buddy_partition_size(uint32_t size){

	uint32_t potencia = 1;

	uint32_t partition_size = pow(2, potencia);

	while(partition_size < size){
		potencia++;
		partition_size = pow(2, potencia);
	}

	return partition_size;

}

void update_LRU(t_partition* touched_partition){
	void _inc_lru(void* p) {
		if(!((t_partition*) p)->available)	//solo usa el lru de las ocupadas
			((t_partition*) p)->lru++;
	}

	list_iterate(partitions, (void*) _inc_lru);

	touched_partition->lru = 0;
}

uint32_t get_partition_number_to_delete(uint32_t* message_id) {

	t_partition* partition;
	uint32_t partition_number;

	if (strcmp(replacement_algorithm, "FIFO") == 0) {
		uint32_t min_id = 999999999;

		for (int i = 0; i < list_size(partitions); i++) {

			partition = list_get(partitions, i);
			if (partition->available == false && partition->ID_message < min_id) {

				partition_number = i;
				min_id = partition->ID_message;
				*message_id = partition->ID_message;
			}
		}
	} else if (strcmp(replacement_algorithm, "LRU") == 0) {
		int32_t max_lru = -1;

		for (int i = 0; i < list_size(partitions); i++) {

			partition = list_get(partitions, i);
			if (partition->available == false && partition->lru > max_lru) {

				partition_number = i;
				max_lru = partition->lru;
				*message_id = partition->ID_message;
			}
		}
	}

	if(list_size(partitions) == 1)//TODO (1 quiere decir la unica particion vacia)
		log_error(logger, "VA A EXPLOTAR TODO SI SE BORRA LA ULTIMA PARTICION");

	return partition_number;
}


void memory_init() {

	mem = malloc(memory_size);
	partitions = list_create();
	//create_dynamic_partition(memory_size);
	create_first_partition(mem, memory_size);
}

void create_first_partition(void* memory_initial_position, uint32_t memory_size){
	t_partition* free_big_partition = malloc(sizeof(t_partition));
	free_big_partition->available = true;
	free_big_partition->initial_position = memory_initial_position;
	free_big_partition->final_position = free_big_partition->initial_position + memory_size;
	free_big_partition->size = memory_size;
	free_big_partition->lru = 0; //no se usa en particion vacia

	list_add(partitions, free_big_partition);
}

void _memory_compaction() {

	uint32_t available_size = 0;
	uint32_t unavailable_size = 0;

	for (int i = 0; i < list_size(partitions); i++) {

		t_partition* partition = list_get(partitions, i);
		if (partition->available) {

			available_size += partition->size;
			list_remove(partitions, i); // change
		} else {

			// define partition->initial_position = unavailable_size;
			unavailable_size += partition->size;
			// define partition->final_position = unavailable_size;
		}
	}

	/* it's possible to refactor the following code */
	t_partition* available_partition = malloc(sizeof(t_partition));

	available_partition->available = true;
	available_partition->size = available_size;
	// define available_partition->initial_position = unavailable_size;
	// define available_partition->final_position = memory_size;

	list_add(partitions, available_partition);
	free(available_partition);
	log_info(obligatorio, "Se ha compactado la memoria");
}

int32_t get_available_partition_index(){
	t_partition* partition;
	int32_t i;

	for (i = 0; i < list_size(partitions); i++) {
		partition = list_get(partitions, i);
		if (partition->available)
			return i;
	}
	return -1;
}


void memory_compaction() {
	bool _available_partition(void* p){
		return ((t_partition*) p)->available;
	}

	t_partition* free_partition;
	t_partition* partition_ocupada;

	int32_t partition_index;
	partition_index = get_available_partition_index();	//encontrar primer particion libre
	while((partition_index != -1) && ((partition_index + 1) < list_size(partitions))){	//si existe particion libre y existe particion siguiente
		free_partition = list_get(partitions, partition_index);
		partition_ocupada = list_get(partitions, partition_index + 1); //la siguiente siempre va a estar ocupada
		void* data_position = partition_ocupada->initial_position;
		uint32_t size_data = partition_ocupada->size;
		void* free_position = free_partition->initial_position;

		//correr la libre a la derecha
		if(partition_ocupada->size < min_partition_size){
			free_partition->initial_position += min_partition_size;
			free_partition->final_position += min_partition_size;
		}else{
			free_partition->initial_position += partition_ocupada->size;
			free_partition->final_position += partition_ocupada->size;
		}
		//correr la ocupada a la izquierda		//el tamano de las libres siempre tiene el tamano real
		partition_ocupada->initial_position -= free_partition->size;
		partition_ocupada->final_position -= free_partition->size;

		//adentro de la lista mover la libre a la derecha
		free_partition = list_remove(partitions, partition_index);
		list_add_in_index(partitions, partition_index + 1, free_partition);

		//consolidar si habia otra libre a la derecha
		if(partition_index + 2 < list_size(partitions)){
			t_partition* next_partition = list_get(partitions, partition_index + 2);
			if(next_partition->available){
				merge_partitions(partition_index + 1, partition_index + 2);
			}
		}

		//mover los datos de la ocupada dentro de la memoria
		memmove(free_position, data_position, size_data);

		//repetir el while
		partition_index = get_available_partition_index();
	}

	log_info(obligatorio, "Se ha compactado la memoria");
}
