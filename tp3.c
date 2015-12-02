// csv ciudades -  id	nombre	longitud   latitud	 provincia	habitantes
// csv rutas - id  id_ciudad_1	id_ciudad_2	 puntaje  distancia

#include "tp3.h"

#define ARCHIVO_CIUDADES "ciudades.csv"
#define ARCHIVO_RUTAS "rutas.csv"
#define COL_CIUDADES 6
#define COL_RUTAS 5

struct grafo {
	size_t vertices;
	size_t aristas;
	ruta_t* matriz;
};

struct ciudad{
    char* id;
    char* nombre;
    double longitud;
    double latitud;
    unsigned int provincia;
    unsigned long int habitantes;
};

struct ruta{
    char* id;
    char* id_ciudad_1;
    char* id_ciudad_2;
    unsigned long int puntaje;
    double distancia;
};

grafo_t* grafo_crear(hash_t* hash_ciudades, hash_t* hash_rutas) {
	grafo_t* grafo = malloc(sizeof(grafo_t));
	if (!grafo) return NULL;
	int cantidad_vertices = hash_cantidad(hash_ciudades);
	ruta_t* matriz = malloc(sizeof(ruta_t) * cantidad_vertices);
	if (!matriz) return NULL;
	for (int i = 0; i < cantidad_vertices; i++) {
		matriz[i] = malloc(sizeof(ruta_t) * cantidad_vertices);
		if (!matriz[i]) return NULL;
		for (int j = 0; j < cantidad_vertices; j++) {
			// Dejo la matriz en NULL
			matriz[i][j] = NULL;
		}
	}
	iter = hash_iter_crear(hash_rutas);
	while (!hash_iter_al_final(iter)){
		ruta_t* ruta = hash_iter_ver_actual(iter));
		matriz[(ruta->id_ciudad_1) - 1][(ruta->id_ciudad_2) - 1] = ruta;
		grafo->aristas++;
		hash_iter_avanzar(iter);
	}
	hash_iter_destruir(iter);
	grafo->vertices = cantidad_vertices;
	grafo->matriz = matriz;
	return grafo;
}

void grafo_destruir(grafo_t* grafo) {
	for (int i = 0; i < grafo->vertices; i++) {
		free(grafo->matriz[i]);
	}
	free(grafo->matriz);
	free(grafo);
}

ciudad_t* crear_ciudad(char* id, char* nombre, char* longitud, char* latitud, char* provincia, char* habitantes){
    ciudad_t* ciudad = malloc(sizeof(ciudad_t));
	if (!ciudad)
		return NULL;
	ciudad->id = id;
	ciudad->nombre = nombre;
	ciudad->longitud = atof(longitud);
	ciudad->latitud = atof(latitud);
	ciudad->provincia = (unsigned long int) provincia;
	ciudad->habitantes = (unsigned long int) habitantes;
	return ciudad;
}

void destruir_ciudad(void* dato){
    free(dato);
}

ruta_t* crear_ruta(char* id, char* id_ciudad_1, char* id_ciudad_2, char* puntaje, char* distancia){
    ruta_t* ruta = malloc(sizeof(ruta_t));
	if (!ruta)
		return NULL;
	ruta->id = id;
	ruta->id_ciudad_1 = id_ciudad_1;
	ruta->id_ciudad_2 = id_ciudad_2;
	ruta->distancia = atof(distancia);
	ruta->puntaje = (unsigned long int) puntaje;
	return ruta;
}

void destruir_ruta(void* dato){
    free(dato);
}

/* Funcion utilizada para leer el archivo csv ARCHIVO_CIUDADES,
 * y generar un hash en base a este. En caso de no fallar, devuelve
 * el hash con los id de cada ciudad como clave, y como dato la estructura
 * 'ciudad_t' correspondiente.
 */
hash_t* procesar_archivo_ciudades(){
    FILE* csv_ciudades = fopen(ARCHIVO_CIUDADES, "r");
    if (!csv_ciudades) return NULL;

    // Creo el hash para las ciudades.
    hash_t* hash_ciudades = hash_crear(&destruir_ciudad);

    char* linea_ciudad = leer_linea(csv_ciudades);
    free(linea_ciudad); // Descarta la primera linea.
    linea_ciudad = leer_linea(csv_ciudades);
    while (linea_ciudad){
        fila_csv_t* fila_ciudad = parsear_linea_csv(linea_ciudad, COL_CIUDADES);
        char* id = obtener_columna(fila_ciudad, 0);
        char* nombre = obtener_columna(fila_ciudad, 1);
        char* longitud = obtener_columna(fila_ciudad, 2);
        char* latitud = obtener_columna(fila_ciudad, 3);
        char* provincia = obtener_columna(fila_ciudad, 4);
        char* habitantes = obtener_columna(fila_ciudad, 5);
        
        ciudad_t* ciudad = crear_ciudad(id, nombre, longitud, latitud, provincia, habitantes);
        if (!ciudad){ 
            destruir_fila_csv(fila_ciudad, true);
            break;
        }
        if (!hash_guardar(hash_ciudades, id, ciudad)) break;
        destruir_fila_csv(fila_ciudad, true);
		free(linea_ciudad);
		linea_ciudad = leer_linea(csv_ciudades);
    }
    free(linea_ciudad);
    fclose(csv_ciudades);
    return hash_ciudades;
}

/* Funcion utilizada para leer el archivo csv ARCHIVO_RUTAS,
 * y generar un hash en base a este. En caso de no fallar, devuelve
 * el hash con los id de cada ruta como clave, y como dato la estructura
 * 'rutas_t' correspondiente.
 */
hash_t* procesar_archivo_rutas(){
    FILE* csv_rutas = fopen(ARCHIVO_RUTAS, "r");
    if (!csv_rutas) return NULL;

    // Creo el hash para las rutas.
    hash_t* hash_rutas = hash_crear(&destruir_ruta);

    char* linea_ruta = leer_linea(csv_rutas);
    free(linea_ruta); // Descarta la primera linea.
    linea_ruta = leer_linea(csv_rutas);
    while (linea_ruta){
        fila_csv_t* fila_ruta = parsear_linea_csv(linea_ruta, COL_RUTAS);
        char* id = obtener_columna(fila_ruta, 0);
        char* id_ciudad_1 = obtener_columna(fila_ruta, 1);
        char* id_ciudad_2 = obtener_columna(fila_ruta, 2);
        char* puntaje = obtener_columna(fila_ruta, 3);
        char* distancia = obtener_columna(fila_ruta, 4);
        
        ruta_t* ruta = crear_ruta(id, id_ciudad_1, id_ciudad_2, puntaje, distancia);
        if (!ruta){
            destruir_fila_csv(fila_ruta, true);
            break;
        }
        if (!hash_guardar(hash_rutas, id, ruta)) break;
        destruir_fila_csv(fila_ruta, true);
		free(linea_ruta);
		linea_ruta = leer_linea(csv_rutas);
    }
    free(linea_ruta);
    fclose(csv_rutas);
    return hash_rutas;
}


// Para probar los hashes
int main(){
    hash_t* hash_ciudades = procesar_archivo_ciudades();
    hash_iter_t* iter = hash_iter_crear(hash_ciudades);
    int i = 1;
    printf ("Prueba ciudades:\n");
    while (!hash_iter_al_final(iter)){
        printf ("%i. %s\n", i, hash_iter_ver_actual(iter));
        hash_iter_avanzar(iter);
        i++;
    }
    hash_iter_destruir(iter);
    hash_destruir(hash_ciudades);

    hash_t* hash_rutas = procesar_archivo_rutas();
    iter = hash_iter_crear(hash_rutas);
    i = 1;
    printf ("\nPrueba rutas:\n");
    while (!hash_iter_al_final(iter)){
        printf ("%i. %s\n", i, hash_iter_ver_actual(iter));
        hash_iter_avanzar(iter);
        i++;
    }
    hash_iter_destruir(iter);
	grafo_t* grafo = grafo_crear(hash_ciudades, hash_rutas);
    hash_destruir(hash_rutas);
	grafo_destruir(grafo);
    return 0;
}
