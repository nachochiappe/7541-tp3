#include "parser.h"
#include "lectura.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "hash.h"
#include "heap.h"
#include "pila.h"
#include "lista.h"

typedef struct grafo grafo_t;
typedef struct ciudad ciudad_t;
typedef struct ruta ruta_t;

/* Funcion utilizada para leer el archivo csv ARCHIVO_CIUDADES,
 * y generar un hash en base a este. En caso de no fallar, devuelve
 * el hash con los id de cada ciudad como clave, y como dato la estructura
 * 'ciudad_t' correspondiente.*/
hash_t* procesar_archivo_ciudades();

/* Funcion utilizada para leer el archivo csv ARCHIVO_RUTAS,
 * y generar un hash en base a este. En caso de no fallar, devuelve
 * el hash con los id de cada ruta como clave, y como dato la estructura
 * 'rutas_t' correspondiente.
 */
hash_t* procesar_archivo_rutas();

// Función que crea un grafo representado como matriz de adyacencia a partir de un hash de ciudades y uno de rutas.
// Pre: los hashes de ciudades y rutas existen.
// Post: devuelve el grafo creado, NULL si falló.
grafo_t* grafo_crear(hash_t* hash_ciudades, hash_t* hash_rutas);

/* Función que realiza la selección de las rutas que conecten todas las
 * ciudades maximizando la cantidad de gente favorecida y considerando
 * el largo de cada ruta */
void todas_ciudades_conectadas(grafo_t* grafo, hash_t* hash_ciudades, hash_t* hash_rutas);

/* Función que realiza selección de caminos entre ciudades para abastecer
 * de energía eléctrica a todas las estaciones usando un árbol de tendido
 * mínimo, a partir del subconjunto de rutas como resultado de la función
 * todas_ciudades_conectadas() */
void arbol_tendido_minimo(grafo_t* grafo, hash_t* hash_ciudades);

/* Función que realiza la obtención de rutas eficientes entre dos ciudades
 * y exporta el resultado a un mapa usando el formato KML */
void obtencion_rutas_eficientes(grafo_t* grafo, hash_t* hash_ciudades);