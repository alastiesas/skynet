#include "listas.h"


t_pending* find_element_given_ID(void* ID_encontrar, t_list* cola, pthread_mutex_t mutex_cola, uint32_t* bytes, void** datos_mensaje, t_log* logsub){
	t_pending* elemento;
	uint32_t size;

	bool _soy_ID_buscado(void* p){
		return ((t_pending*) p)->ID_mensaje == (uint32_t) ID_encontrar;
	}

	pthread_mutex_lock(&mutex_cola);
		elemento = list_find(cola, _soy_ID_buscado);
		if(elemento != NULL){
			*bytes = elemento->bytes;
			*datos_mensaje = malloc(*bytes);
			size = *bytes;
			memcpy(*datos_mensaje, elemento->datos_mensaje, *bytes);
		}
	pthread_mutex_unlock(&mutex_cola);


	if(elemento != NULL){
		log_trace(logsub, "Se encontro el t_pending de ID: %d", elemento->ID_mensaje);
		log_trace(logsub, "Adentro de la struct hay %d bytes", elemento->bytes);
	}
	else
		log_debug(logsub, "No esta el message_data en la cola, quizas este en la cache\n");

	return elemento;
}

t_suscriber* find_suscriber_given_ID(void* ID_encontrar, t_list* subs, pthread_mutex_t mutex_subs){
	t_suscriber* suscriber;

	bool _soy_ID_buscado(void* p){
		return ((t_suscriber*) p)->ID_suscriber == (uint32_t) ID_encontrar;
	}

	pthread_mutex_lock(&mutex_subs);
		suscriber = list_find(subs, _soy_ID_buscado);
	pthread_mutex_unlock(&mutex_subs);

	if(suscriber == NULL)
		log_info(logger, "No se encontro el suscriptor %d en la lista", (int) ID_encontrar);
	else
		log_info(logger, "Si se encontro al suscriptor %d en la lista", (int) ID_encontrar);

	return suscriber;
}

void dump_cache(void){

	char* dump = string_new();
	string_append(&dump, "\n-----------------------------------------------------------------------------------------------------------------------------\n");
	string_append(&dump, "Dump: ");
	char* time = temporal_get_string_time();
	string_append(&dump, time);
	free(time);

	uint32_t list_amount;
	uint32_t i;
	//mutex_particiones TODO
	list_amount = list_size(partitions);
	for(i=1; i<(list_amount + 1); i++){
		t_partition* partition = list_get(partitions, (i-1));

		string_append(&dump, "\nParticion ");
		char* num = string_itoa(i);
		string_append(&dump, num);
		free(num);
		string_append(&dump, ": ");
		char* initial = string_from_format("%p", partition->initial_position);
		char* final = string_from_format("%p", partition->final_position);
		string_append(&dump, initial);
		string_append(&dump, " - ");
		string_append(&dump, final);
		free(initial);
		free(final);

		if(partition->available){
			string_append(&dump, ".\t[L]");
			string_append(&dump, "\tSize: ");
			char* size = string_itoa(partition->size);
			string_append(&dump, size);
			free(size);
			string_append(&dump, "b");
		}
		else{
			string_append(&dump, ".\t[X]");
			string_append(&dump, "\tSize: ");
			char* size = string_itoa(partition->size);
			string_append(&dump, size);
			free(size);
			string_append(&dump, "b");
			//TODO agregar LRU
			//TODO agregar COLA
			//TODO agregar ID
		}
	}
	//mutex_particiones TODO

	string_append(&dump, "\n-----------------------------------------------------------------------------------------------------------------------------\n");

	FILE* file_dump = txt_open_for_append("dump.txt");
	txt_write_in_file(file_dump, dump);
	txt_close_file(file_dump);

	free(dump);
}

//------------------------------------- Funciones de prueba

void imprimir_lista(t_list* lista, char* nombre){
	int tamanio = list_size(lista);
	int i;
	void* contenido;

	for(i=0; i<tamanio; i++){
		contenido = list_get(lista, i);
		printf("%s[%d] vale: %d\n", nombre, i, (int) contenido);
	}

}

void encontrar_numero(t_list *lista, void* numero_buscado, t_list* elementos_coincidentes) {
	bool _soy_el_numero_buscado(void *p) {
		return p == numero_buscado;
	}
	if(list_any_satisfy(lista, _soy_el_numero_buscado)){
		list_add(elementos_coincidentes, numero_buscado);
	}
}

t_list* interseccion_lista(t_list* unaLista, t_list* otraLista, t_list* resultado){
	list_clean(resultado);
	t_list* lista = list_duplicate(unaLista);
	void* elemento;

	while(list_size(lista) != 0){
		elemento = list_remove(lista, 0);
		encontrar_numero(otraLista, elemento, resultado);
	}

	if(list_size(lista) == 0)
		list_destroy(lista);
	else
		printf("ERROR no se vacio la lista\n");

	return resultado;
}


void no_contenido(t_list* lista_enviados, void* elemento_lista_global, t_list* resultado) {
	bool _soy_el_numero_buscado(void *p) {
		return p == elemento_lista_global;
	}
	if(!list_any_satisfy(lista_enviados, _soy_el_numero_buscado)){
		list_add(resultado, elemento_lista_global);
	}
}

t_list* no_enviados_lista(t_list* lista_global, t_list* lista_enviados, t_list** resultado){
	list_clean(*resultado);
	t_list* lista = list_duplicate(lista_global);
	void* elemento;

	while(list_size(lista) != 0){
		elemento = list_remove(lista, 0);
		no_contenido(lista_enviados, elemento, *resultado);
	}

	if(list_size(lista) == 0)
		list_destroy(lista);
	else
		printf("ERROR no se vacio la lista\n");

	return *resultado;
}


