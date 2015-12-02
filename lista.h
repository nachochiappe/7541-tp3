#ifndef LISTA_H
#define LISTA_H

#include <stdbool.h>
#include <stddef.h>

typedef struct lista lista_t;
typedef struct lista_iter lista_iter_t;

// Primitivas de Lista Enlazada

/* Crea una nueva lista enlazada, vacia. En caso
 * de fallar, devuelve NULL.*/
lista_t* lista_crear(void);

/* Verifica si la lista esta vacia o no. En caso
 * de estarlo (o de ser nula) devolvera 'true', si
 * no, 'false'.*/
bool lista_esta_vacia(const lista_t* lista);

/* Inserta un nuevo nodo al comienzo de la lista. Si falla
 * al pedir memoria para este, devolvera 'false'. Si no,
 * añade el nuevo nodo como primero de la lista.*/
bool lista_insertar_primero(lista_t* lista, void* dato);

/* Inserta un nuevo nodo al final de la lista. Si falla
 * al pedir memoria para este, devolvera 'false'. Si no,
 * añade el nuevo nodo como ultimo de la lista.*/
bool lista_insertar_ultimo(lista_t* lista, void* dato);

/* Borra el primer nodo de la lista. En caso de estar
 * vacia, devuelve NULL. Si no, borra el primer nodo
 * y devuelve su dato.*/
void* lista_borrar_primero(lista_t* lista);

/* Devuelve el valor del primer nodo de la lista. En
 * caso de estar vacia, devuelve NULL.*/
void* lista_ver_primero(const lista_t* lista);

/* Devuelve el largo de la lista (la cantidad de
 * elementos en ella). Si la posicion no es valida, 
 * devuelve 0.*/
size_t lista_largo(const lista_t* lista);

/* Destruye la lista pasada por parametro. En caso de 
 * estar vacia la lista, se libera la lista. Si no, se
 * recorren y liberan los nodos, aplicando la funcion
 * 'destruir_dato' donde sea indicado.*/
void lista_destruir(lista_t* lista, void destruir_dato(void*));


// Primitivas del Iterador

/* Crea un nuevo iterador de una lista pasada por 
 * parametro. En caso de pasar una posicion nula,
 * devuelve NULL, tambien si falla al pedir memoria
 * para el iterador. En otro caso, devuelve el iterador.*/
lista_iter_t* lista_iter_crear(const lista_t* lista);

/* Avanza el iterador al siguiente nodo, respecto del
 * actual. En caso de no quedar mas nodos por recorrer,
 * o pasar un iterador invalido, devuelve 'false'. 
 * Si no, 'true'.*/
bool lista_iter_avanzar(lista_iter_t* iter);

/* Devuelve el dato del nodo actual respecto del iterador.
 * En caso de no quedar nodos por recorrer, o pasar un
 * iterador invalido, devuelve NULL.*/
void* lista_iter_ver_actual(const lista_iter_t* iter);

/* Verifica si el iterador esta posicionado al final de
 * la lista. En caso de pasar una posicion invalida,
 * devuelve true.*/
bool lista_iter_al_final(const lista_iter_t* iter);

/* Destruye el iterador. En caso de pasar una posicion
 * invalida, termina la funcion.*/
void lista_iter_destruir(lista_iter_t* iter);


// Primitivas del Iterador (para modificar la lista)

/* Inserta un nodo en la posicion actual del iterador. Esto es,
 * cambiando el actual del iterador por el nuevo nodo ingresado.
 * En caso de fallar, devuelve 'false', si no, 'true'.*/
bool lista_insertar(lista_t* lista, lista_iter_t* iter, void* dato);

/* Borra el nodo actual del iterador. En caso de estar vacia la
 * lista, o de no quedar mas elementos, devuelve NULL. Si no,
 * devuelve el valor del nodo borrado.*/
void* lista_borrar(lista_t* lista, lista_iter_t* iter);


// Primitiva del Iterador interno

/* Recorre la lista, aplicando la funcion 'visitar' a
 * todos sus nodos. En caso de estar vacia, termina
 * la funcion. Si no, continua aplicando hasta que
 * no queden mas nodos por recorrer.*/
void lista_iterar(lista_t* lista, bool (*visitar)(void* dato, void* extra), void* extra);



/* Primitiva del archivo de pruebas a realizar, se ejecutaran para
 * comprobar el buen funcionamiento de la lista y el iterador.*/
void pruebas_lista_alumno(void);

#endif // LISTA_H
