#include "broker.h"

void create_dynamic_partition(uint32_t size) {

	t_partition* partition = malloc(sizeof(t_partition));
	t_partition* available_partition = find_available_dynamic_partition(size);
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

void delete_dynamic_partition() {

	uint32_t victim_partition_number = get_partition_number_to_delete();
	uint32_t previous_partition_number = victim_partition_number - 1;
	uint32_t next_partition_number = victim_partition_number + 1;
	t_partition* victim_partition = list_get(partitions, victim_partition_number);

	victim_partition->available = true;

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
}

void delete_fixed_partition() {

}

void delete_partition() {

}

t_partition* find_available_dynamic_partition(uint32_t size) {

	uint32_t available_partition_number;

	do {

		available_partition_number = get_available_partition_number(size);
		if (available_partition_number == -1) {

			if (compaction_frequency < 2) {

				delete_dynamic_partition();
				memory_compaction();
			} else {

				if (count < compaction_frequency) {

					delete_dynamic_partition();
					count++;
				} else {

					memory_compaction();
					count = 0;
				}
			}
		}
	} while (available_partition_number == -1);

	return list_get(partitions, available_partition_number);
}

void merge_partitions(uint32_t initial_partition_number, uint32_t final_partition_number) {

	t_partition* initial_partition = list_get(partitions, initial_partition_number);
	t_partition* final_partition = list_get(partitions, final_partition_number);

	initial_partition->final_position = final_partition->final_position;
	initial_partition->size += final_partition->size;
	list_remove(partitions, final_partition_number); // change
}

uint32_t get_available_partition_number(uint32_t size) {

	t_partition* partition;
	uint32_t partition_number = -1;

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
			if (partition->available == false && partition->lru > max_lru) {

				partition_number = i;
				max_lru = partition->lru;
			}
		}
	}

	return partition_number;
}

void memory_allocation() {

	mem = malloc(memory_size);
	create_dynamic_partition(memory_size);
}

void memory_compaction() {

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
}
