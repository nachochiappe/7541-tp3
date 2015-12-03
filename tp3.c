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
	ruta_t** matriz;
};

struct ciudad{
    char* id;
    char* nombre;
    double longitud;
    double latitud;
    unsigned long int provincia;
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
	size_t cantidad_vertices = hash_cantidad(hash_ciudades);
	// Creo una matriz de |V| x |V| (uso calloc para que no tire errores valgrind de inicializacion)
	ruta_t** matriz = calloc(cantidad_vertices * cantidad_vertices, sizeof(ruta_t));
	if (!matriz) return NULL;
    /*for (int i = 0; i < cantidad_vertices * cantidad_vertices; i++){
        matriz[i] = 0;
    }*/
	hash_iter_t* iter = hash_iter_crear(hash_rutas);
	while (!hash_iter_al_final(iter)){
		ruta_t* ruta = hash_obtener(hash_rutas, hash_iter_ver_actual(iter));
		size_t id_ciudad_1 = strtoul(ruta->id_ciudad_1, NULL, 10);
		size_t id_ciudad_2 = strtoul(ruta->id_ciudad_2, NULL, 10);
		// La forma de acceder a (x, y) en la matriz es: (x - 1) * cant_columnas + (y - 1)
		matriz[(id_ciudad_1 - 1) * cantidad_vertices + (id_ciudad_2 - 1)] = ruta;
		// Es un grafo no dirigido, deben estar las rutas también en dirección contraria
		matriz[(id_ciudad_2 - 1) * cantidad_vertices + (id_ciudad_1 - 1)] = ruta;
		grafo->aristas++;
		hash_iter_avanzar(iter);
	}
	hash_iter_destruir(iter);
	grafo->vertices = cantidad_vertices;
	grafo->matriz = matriz;
	return grafo;
}

void grafo_destruir(grafo_t* grafo) {
	free(grafo->matriz);
	free(grafo);
}

ciudad_t* crear_ciudad(char* id, char* nombre, char* longitud, char* latitud, char* provincia, char* habitantes){
    ciudad_t* ciudad = malloc(sizeof(ciudad_t));
	if (!ciudad)
		return NULL;
	ciudad->id = strcpy(malloc(strlen(id) + 1), id);
	ciudad->nombre = strcpy(malloc(strlen(nombre) + 1), nombre);
	ciudad->longitud = atof(longitud);
	ciudad->latitud = atof(latitud);
	ciudad->provincia = (unsigned long int) provincia;
	ciudad->habitantes = (unsigned long int) habitantes;
	return ciudad;
}

void destruir_ciudad(void* dato){
    if (dato){
        ciudad_t* ciudad = dato;
        free(ciudad->id);
        free(ciudad->nombre);
    }
    free(dato);
}

ruta_t* crear_ruta(char* id, char* id_ciudad_1, char* id_ciudad_2, char* puntaje, char* distancia){
    ruta_t* ruta = malloc(sizeof(ruta_t));
	if (!ruta)
		return NULL;
	ruta->id = strcpy(malloc(strlen(id) + 1), id);
	ruta->id_ciudad_1 = strcpy(malloc(strlen(id_ciudad_1) + 1), id_ciudad_1);
	ruta->id_ciudad_2 = strcpy(malloc(strlen(id_ciudad_2) + 1), id_ciudad_2);
	ruta->puntaje = strtoul(puntaje, NULL, 10);
	ruta->distancia = atof(distancia);
	return ruta;
}

void destruir_ruta(void* dato){
    if (dato){
        ruta_t* ruta = dato;
        free(ruta->id);
        free(ruta->id_ciudad_1);
        free(ruta->id_ciudad_2);
    }
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

/* Función que genera un archivo KML */
void generar_archivo_kml(char* nombre_archivo) {
	FILE* archivo_kml = fopen(nombre_archivo, "w");
    if (!archivo_kml) return;
	
	fputs("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n", archivo_kml);
	fputs("<kml xmlns=\"http://earth.google.com/kml/2.1\">\n", archivo_kml);
	fputs("\t<Document>\n", archivo_kml);
	
	// Parte del medio
	
	fputs("\t</Document>\n", archivo_kml);
	fputs("</kml>\n", archivo_kml);
	
	fclose(archivo_kml);
	return;
}

/* Función que realiza la selección de las rutas que conecten todas las
 * ciudades maximizando la cantidad de gente favorecida y considerando
 * el largo de cada ruta */
void todas_ciudades_conectadas(grafo_t* grafo, hash_t* hash_ciudades, hash_t* hash_rutas) {
    //encontrar_rutas(hash_ciudades, hash_rutas);
	generar_archivo_kml("red.kml");
	return;
}

/* Función que realiza selección de caminos entre ciudades para abastecer
 * de energía eléctrica a todas las estaciones usando un árbol de tendido
 * mínimo, a partir del subconjunto de rutas como resultado de la función
 * todas_ciudades_conectadas() */
void arbol_tendido_minimo(grafo_t* grafo) {
	generar_archivo_kml("tendido.kml");
	return;
}

/* Función que realiza la obtención de rutas eficientes entre dos ciudades
 * y exporta el resultado a un mapa usando el formato KML */
void obtencion_rutas_eficientes(grafo_t* grafo) {
	generar_archivo_kml("ruta.kml");
	return;
}

int main(void){
    hash_t* hash_ciudades = procesar_archivo_ciudades();
    hash_iter_t* iter = hash_iter_crear(hash_ciudades);
    int i = 0;
    printf ("-Prueba ciudades:");
    while (!hash_iter_al_final(iter)){
        hash_iter_avanzar(iter);
        i++;
    }
    printf (" %i ciudades\n", i);
    hash_iter_destruir(iter);

    hash_t* hash_rutas = procesar_archivo_rutas();
    iter = hash_iter_crear(hash_rutas);
    i = 0;
    printf ("-Prueba rutas:");
    while (!hash_iter_al_final(iter)){
        hash_iter_avanzar(iter);
        i++;
    }
    printf (" %i rutas\n", i);
    hash_iter_destruir(iter);
	grafo_t* grafo = grafo_crear(hash_ciudades, hash_rutas);

    //    Para imprimir la matriz  //
    for (i = 0; i < 1444; i++){
        if (i % 38 == 0 && i != 0) printf ("  .%i\n", i/38);
        if (grafo->matriz[i]){
            if (strlen((grafo->matriz[i])->id) == 1)
                printf (" %s ", (grafo->matriz[i])->id);
            else if (strlen((grafo->matriz[i])->id) == 2)
                printf ("%s ", (grafo->matriz[i])->id);
            else
                printf ("%s", (grafo->matriz[i])->id);
        }
        else printf (" 0 ");
    }
    printf ("  .%i\n", i/38);

	todas_ciudades_conectadas(grafo, hash_ciudades, hash_rutas);
	arbol_tendido_minimo(grafo);
	obtencion_rutas_eficientes(grafo);
	hash_destruir(hash_ciudades);
    hash_destruir(hash_rutas);
	grafo_destruir(grafo);
    return 0;
}
