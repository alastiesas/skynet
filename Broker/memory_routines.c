#include "broker.h"

//void create_partition(uint32_t size, bool available) {
//
//	t_partition* partition = malloc(sizeof(t_partition));
//	t_partition* available_partition = find_available_partition(size);
//	uint32_t delimiting_position = available_partition->initial_position + size;
//
//	partition->available = available;
//	partition->final_position = delimiting_position;
//	partition->initial_position = available_partition->initial_position;
//	partition->size = size;
//	available_partition->initial_position = delimiting_position;
//	available_partition->size = available_partition->size - size;
//
//	list_add(partitions, partition);
//	free(partition);
//}

void delete_partition() {

	uint32_t index_partition = get_partition_number_to_delete();

	t_partition* partition_to_delete = list_get(partitions, index_partition);

	partition_to_delete->available = true;

}

void free_partition(){

	uint32_t index_partition = get_partition_number_to_delete();

	t_partition* partition_to_free = list_get(partitions, index_partition);

	partition_to_free->available = true;

}

uint32_t get_partition_number_to_delete() {

	t_partition* partition;
	uint32_t partition_number;

	if (strcmp(replacement_algorithm, "FIFO") == 0) {

		for (int i = 0; i < list_size(partitions); i++) {

			partition = list_get(partitions, i);
			if (partition->available == false) {

				partition_number = i;
				break;
			}
		}
	} else if (strcmp(replacement_algorithm, "LRU") == 0) {

		uint32_t max_lru = -1;

		for (int i = 0; i < list_size(partitions); i++) {

			partition = list_get(partitions, i);
			if (partition->available == false
					&& partition->lru > max_lru) {

				partition_number = i;
				max_lru = partition->lru;
			}
		}
	}

	return partition_number;
}

//t_partition* find_available_partition(uint32_t size) {
//
//	t_partition* available_partition;
//	uint32_t available_partition_number;
//
//	do {
//
//		available_partition_number = get_available_partition_number(size);
//		if (available_partition_number == NULL) {
//
//			if (count < compaction_frequency) {
//
//				delete_partition();
//				count++;
//			} else {
//
//				memory_compaction();
//				count = 0;
//			}
//		}
//	} while (available_partition_number == NULL);
//
//	return available_partition;
//}

uint32_t get_available_partition_number(uint32_t size) {

	t_partition* partition;
	uint32_t partition_number;

	if (strcmp(free_partition_algorithm, "BF") == 0) {

			uint32_t remaining_size = memory_size;
			for (int i = 0; i < list_size(partitions) || remaining_size == 0; i++) {

				partition = list_get(partitions, i);
				if (partition->available == true
						&& partition->size - size < remaining_size
						&& partition->size >= size) {

					partition_number = i;
					remaining_size = partition->size - size;
				}
			}
		} else if (strcmp(free_partition_algorithm, "FF") == 0) {

			for (int i = 0; partition_number == NULL && i < list_size(partitions);
					i++) {

				partition = list_get(partitions, i);
				if (partition->available == true && partition->size >= size) {

					partition_number = i;
				}
			}
	}

	return partition_number;
}

void memory_allocation() {

	mem = malloc(memory_size);
//	create_partition(memory_size, true);
}

void memory_compaction() {

}

void merge_partitions() {

}
