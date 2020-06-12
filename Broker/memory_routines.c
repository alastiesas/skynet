#include "broker.h"

//void create_partition(uint32_t size, bool available) {
//
//	t_partition* available_partition;
//
//
//	t_partition* partition = malloc(sizeof(t_partition));
//	partition->available = available;
//	partition->final_position = atoi(mem) + atoi(memory_size);
//	partition->initial_position = atoi(mem);
//	partition->size = size;
//	list_add(partitions, partition);
//}

void delete_partition() {

}

//t_partition* find_available_partition(uint32_t size) {
//
//	t_partition* available_partition;
//	uint32_t available_partition_number;
//	do {
//
//		available_partition_number = get_available_partition_number(size);
//		if (available_partition_number == NULL) {
//
//			memory_compaction();
//		} else if () {
//
//			delete_partition();
//		}
//	} while (available_partition_number == NULL);
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
	//create_partition(memory_size, true);

}

void memory_compaction() {

}

void merge_partitions() {

}
