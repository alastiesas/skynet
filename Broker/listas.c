#include "listas.h"

void encontrar_mensaje(){

}

t_pending* find_element_given_ID(void* ID_encontrar, t_list* cola, pthread_mutex_t mutex_cola, uint32_t* bytes, void** datos_mensaje){
	t_pending* elemento;
	uint32_t size;

	bool _soy_ID_buscado(void* p){
		return ((t_pending*) p)->ID_mensaje == (uint32_t) ID_encontrar;
	}

	pthread_mutex_lock(&mutex_cola);
	elemento = list_find(cola, _soy_ID_buscado);
	log_debug(logger, "Se encontro el t_pending de ID: %d", elemento->ID_mensaje);
	log_debug(logger, "Adentro de la struct hay %d bytes", elemento->bytes);
	*bytes = elemento->bytes;
	*datos_mensaje = malloc(*bytes);
	log_debug(logger, "(dentro mutex) El mensaje tiene %d bytes", *bytes);
	size = *bytes;
	memcpy(datos_mensaje, elemento->datos_mensaje, *bytes);
	log_debug(logger, "(dentro mutex) El mensaje tiene %d bytes", size);
	pthread_mutex_unlock(&mutex_cola);

	log_debug(logger, "El mensaje tiene %d bytes", *bytes);

	if(elemento == NULL)
		log_error(logger, "No se encontro el mensaje que tenia que estar en la cola\n");

	return elemento;
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

t_list* no_enviados_lista(t_list* lista_global, t_list* lista_enviados, t_list* resultado){
	list_clean(resultado);
	t_list* lista = list_duplicate(lista_global);
	void* elemento;

	while(list_size(lista) != 0){
		elemento = list_remove(lista, 0);
		no_contenido(lista_enviados, elemento, resultado);
	}

	if(list_size(lista) == 0)
		list_destroy(lista);
	else
		printf("ERROR no se vacio la lista\n");

	return resultado;
}

void imprimir_lista(t_list* lista, char* nombre){
	int tamanio = list_size(lista);
	int i;
	void* contenido;

	for(i=0; i<tamanio; i++){
		contenido = list_get(lista, i);
		printf("%s[%d] vale: %d\n", nombre, i, (int) contenido);
	}

}

