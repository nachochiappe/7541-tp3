#include "lista.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>


// Defino el struct usado para los nodos de la lista.
typedef struct nodo {
    void* dato;
    struct nodo* siguiente;
}nodo_t;

// Defino el struct de Lista Enlazada.
struct lista {
    nodo_t* primero;
    nodo_t* ultimo;
    size_t elementos;
};

// Defino el struct del Iterador de Lista Enlazada.
struct lista_iter {
    nodo_t* anterior;
    nodo_t* actual;
};


// Primitivas de Lista Enlazada

/* Crea una nueva lista enlazada, vacia. En caso
 * de fallar, devuelve NULL.*/
lista_t* lista_crear(void){
    lista_t* lista = malloc(sizeof(lista_t));
    if (!lista)
        return NULL;
    lista->elementos = 0;
    lista->primero = NULL;
    lista->ultimo = NULL;
    return lista;
}

/* Verifica si la lista esta vacia o no. En caso
 * de estarlo (o de ser nula) devolvera 'true', si
 * no, 'false'.*/
bool lista_esta_vacia(const lista_t* lista){
    if (!lista)
        return true;
    return lista->elementos == 0;
}

/* Inserta un nuevo nodo al comienzo de la lista. Si falla
 * al pedir memoria para este, devolvera 'false'. Si no,
 * añade el nuevo nodo como primero de la lista.*/
bool lista_insertar_primero(lista_t* lista, void* dato){
    if (!lista)
        return false;
    nodo_t* nuevo_nodo = malloc(sizeof(nodo_t));
    if (!nuevo_nodo)
        return false;
    nuevo_nodo->dato = dato;
    if (lista_esta_vacia(lista)){
        lista->ultimo = nuevo_nodo;
        lista->ultimo->siguiente = NULL;
    }
    else
        nuevo_nodo->siguiente = lista->primero;
    lista->primero = nuevo_nodo;
    lista->elementos += 1;
    return true;
}

/* Inserta un nuevo nodo al final de la lista. Si falla
 * al pedir memoria para este, devolvera 'false'. Si no,
 * añade el nuevo nodo como ultimo de la lista.*/
bool lista_insertar_ultimo(lista_t* lista, void* dato){
    if (!lista)
        return false;
    nodo_t* nuevo_nodo = malloc(sizeof(nodo_t));
    if (!nuevo_nodo)
        return false;
    nuevo_nodo->dato = dato;
    if (lista_esta_vacia(lista))
        lista->primero = nuevo_nodo;
    else
        lista->ultimo->siguiente = nuevo_nodo;
    lista->ultimo = nuevo_nodo;
    lista->ultimo->siguiente = NULL;
    lista->elementos += 1;
    return true;
}

/* Borra el primer nodo de la lista. En caso de estar
 * vacia, devuelve NULL. Si no, borra el primer nodo
 * y devuelve su dato.*/
void* lista_borrar_primero(lista_t* lista){
    if (lista_esta_vacia(lista))
        return NULL;
    nodo_t* primero = lista->primero;
    void* valor = primero->dato;
    if (lista->elementos == 1){
        lista->primero = NULL;
        lista->ultimo = NULL;
    }
    else
        lista->primero = lista->primero->siguiente;
    free(primero);
    lista->elementos -= 1;
    return valor;
}

/* Devuelve el valor del primer nodo de la lista. En
 * caso de estar vacia, devuelve NULL.*/
void* lista_ver_primero(const lista_t* lista){
    if (lista_esta_vacia(lista))
        return NULL;
    return lista->primero->dato;
}

/* Devuelve el largo de la lista (la cantidad de
 * elementos en ella). Si la posicion no es valida, 
 * devuelve 0.*/
size_t lista_largo(const lista_t* lista){
    if (!lista)
        return 0;
    return lista->elementos;
}

/* Destruye la lista pasada por parametro. En caso de 
 * estar vacia la lista, se libera la lista. Si no, se
 * recorren y liberan los nodos, aplicando la funcion
 * 'destruir_dato' donde sea indicado.*/
void lista_destruir(lista_t* lista, void destruir_dato(void*)){
    if (!lista_esta_vacia(lista)){
        while (!lista_esta_vacia(lista)){
            void* dato = lista_borrar_primero(lista);
            if (destruir_dato != NULL)
                destruir_dato(dato);
        }
    }
    free(lista);
}

// Primitivas del Iterador

/* Crea un nuevo iterador de una lista pasada por 
 * parametro. En caso de pasar una posicion nula,
 * devuelve NULL, tambien si falla al pedir memoria
 * para el iterador. En otro caso, devuelve el iterador.*/
lista_iter_t* lista_iter_crear(const lista_t* lista){
    if (!lista)
        return NULL;
    lista_iter_t* iter = malloc(sizeof(lista_iter_t));
    if (!iter)
        return NULL;
    iter->actual = lista->primero;
    iter->anterior = NULL;
    return iter;
}

/* Avanza el iterador al siguiente nodo, respecto del
 * actual. En caso de no quedar mas nodos por recorrer,
 * o pasar un iterador invalido, devuelve 'false'. 
 * Si no, 'true'.*/
bool lista_iter_avanzar(lista_iter_t* iter){
    if (!iter->actual)
        return false;
    iter->anterior = iter->actual;
    iter->actual = iter->actual->siguiente;
    return true;
}

/* Devuelve el dato del nodo actual respecto del iterador.
 * En caso de no quedar nodos por recorrer, o pasar un
 * iterador invalido, devuelve NULL.*/
void* lista_iter_ver_actual(const lista_iter_t* iter){
    if (!iter || !iter->actual)
        return NULL;
    return iter->actual->dato;
}

/* Verifica si el iterador esta posicionado al final de
 * la lista. En caso de pasar un iterador invalido,
 * devuelve true.*/
bool lista_iter_al_final(const lista_iter_t* iter){
    if (!iter)
        return true;
    return iter->actual == NULL;
}

/* Destruye el iterador. En caso de pasar una posicion
 * invalida, termina la funcion.*/
void lista_iter_destruir(lista_iter_t* iter){
    if (iter != NULL)
        free(iter);
}


// Primitivas del Iterador (para modificar la lista)

/* Inserta un nodo en la posicion actual del iterador. Esto es,
 * cambiando el actual del iterador por el nuevo nodo ingresado.
 * En caso de fallar, devuelve 'false', si no, 'true'.*/
bool lista_insertar(lista_t* lista, lista_iter_t* iter, void* dato){
    if (!lista || !iter)
        return false;
    if (iter->actual == lista->primero){ // Si esta al comienzo de la lista (con una lista vacia tambien inserta asi).
        bool inserto_primero = lista_insertar_primero(lista, dato);
        if (!inserto_primero)
            return false;
        iter->actual = lista->primero;
    }
    else if (!iter->actual){ // Si esta al final (como un 'append' en Python).
        bool inserto_ultimo = lista_insertar_ultimo(lista, dato);
        if (!inserto_ultimo)
            return false;
        iter->actual = lista->ultimo;
    }
    else{
        nodo_t* nuevo_nodo = malloc(sizeof(nodo_t));
        if (!nuevo_nodo)
            return false;
        nuevo_nodo->dato = dato;
        iter->anterior->siguiente = nuevo_nodo;  
        nuevo_nodo->siguiente = iter->actual;
        iter->actual = nuevo_nodo;
        lista->elementos += 1;
    }
    return true;
}

/* Borra el nodo actual del iterador. En caso de estar vacia la
 * lista, o de no quedar mas elementos, devuelve NULL. Si no,
 * devuelve el valor del nodo borrado.*/
void* lista_borrar(lista_t* lista, lista_iter_t* iter){
    if (lista_esta_vacia(lista) || !iter || !iter->actual) // Si el actual del iterador es NULL, no quedan elementos por borrar.
        return NULL;
    void* valor;
    if (iter->actual == lista->primero){ // Borra el primer elemento (En el caso que la lista tenga un solo elemento, se borrara por este medio).
        valor = lista_borrar_primero(lista);
        iter->actual = lista->primero;
    }
    else if (iter->actual == lista->ultimo){ // Borra el ultimo elemento.
        nodo_t* nodo_a_borrar = iter->actual;
        valor = nodo_a_borrar->dato;
        iter->anterior->siguiente = NULL;
        iter->actual = NULL;
        lista->ultimo = iter->anterior;
        lista->ultimo->siguiente = NULL;
        free(nodo_a_borrar);
        lista->elementos -= 1;
    }
    else{
        nodo_t* nodo_a_borrar = iter->actual;
        valor = nodo_a_borrar->dato;
        iter->anterior->siguiente = iter->actual->siguiente;
        iter->actual = iter->actual->siguiente;
        free(nodo_a_borrar);
        lista->elementos -= 1;
    }
    return valor;
}


// Primitiva del Iterador interno

/* Recorre la lista, aplicando la funcion 'visitar' a
 * todos sus elementos. En caso de estar vacia, termina
 * la funcion. Si no, continua aplicando hasta que
 * no queden mas valores por recorrer.*/
void lista_iterar(lista_t* lista, bool (*visitar)(void* dato, void* extra), void* extra){
    if (!lista_esta_vacia(lista)){
        bool continuar_iterando = true;
        size_t largo_lista = lista->elementos;
        nodo_t* actual = lista->primero;
        while(continuar_iterando && largo_lista != 0){
            continuar_iterando = visitar(actual->dato, extra);
            actual = actual->siguiente;
            largo_lista--;
        }
    }
}
