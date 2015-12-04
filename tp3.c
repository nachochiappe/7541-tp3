// csv ciudades -  id	nombre	longitud   latitud	 provincia	habitantes
// csv rutas - id  id_ciudad_1	id_ciudad_2	 puntaje  distancia

#include "tp3.h"

#define ARCHIVO_CIUDADES "ciudades.csv"
#define ARCHIVO_RUTAS "rutas.csv"
#define COL_CIUDADES 6
#define COL_RUTAS 5
#define PRIMER_CIUDAD "1"
#define N_PRIMER_CIUDAD 1

struct grafo {
	size_t vertices;
	size_t aristas;
	ruta_t** matriz;
};

struct ciudad{
	char* id;
	char* nombre;
	double latitud;
	double longitud;
	int provincia;
	unsigned long int habitantes;
};

struct ruta{
	char* id;
	char* id_ciudad_1;
	char* id_ciudad_2;
	unsigned long int puntaje;
	double distancia;
};

int cmp_rutas(const void* ruta1, const void* ruta2) {
	double distancia_ruta1 = ((ruta_t*) ruta1)->distancia;
	double distancia_ruta2 = ((ruta_t*) ruta2)->distancia;
	if (distancia_ruta1 == distancia_ruta2) return 0;
	else if (distancia_ruta1 < distancia_ruta2) return 1;
	return (-1);
}
cmp_func_t cmp = &cmp_rutas;



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
		matriz[(id_ciudad_2 - 1) * cantidad_vertices + (id_ciudad_1 - 1)] = ruta; // Tambien ponemos los valores del otro lado de la 'diagonal' de la matriz, dado a que valen los caminos de ida y vuelta (si una ruta va de ciudad 1 a ciudad 2, tambien vale de ciudad 2 a ciudad 1).
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
	ciudad->provincia = atoi(provincia);
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

void imprimir_en_archivo(FILE* archivo_kml, ciudad_t* ciudad){
	fprintf(archivo_kml, "\t\t<Placemark>\n\t\t\t<name>%s</name>\n", ciudad->nombre);
	fprintf(archivo_kml, "\t\t\t<Point>\n\t\t\t\t<coordinates>%f, %f</coordinates>\n\t\t\t</Point>\n\t\t</Placemark>\n\n", ciudad->longitud, ciudad->latitud);
}

/* Función que genera un archivo KML */
void generar_archivo_kml(char* nombre_archivo, hash_t* hash_ciudades, cola_t* camino_rutas) {
	FILE* archivo_kml = fopen(nombre_archivo, "w");
	if (!archivo_kml) return;
	
	fputs("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n", archivo_kml);
	fputs("<kml xmlns=\"http://earth.google.com/kml/2.1\">\n", archivo_kml);
	fputs("\t<Document>\n", archivo_kml);
	
	// Parte del medio
	if (camino_rutas){
		pila_t* ciudad_actual = pila_crear();
		pila_apilar(ciudad_actual, PRIMER_CIUDAD);
		while (!cola_esta_vacia(camino_rutas)){
			ruta_t* ruta = cola_desencolar(camino_rutas);
			ciudad_t* ciudad_1 = hash_obtener(hash_ciudades, (char*) pila_ver_tope(ciudad_actual));
			ciudad_t* ciudad_2;
			if (strcmp(pila_ver_tope(ciudad_actual), ruta->id_ciudad_1) == 0)
				ciudad_2 = hash_obtener(hash_ciudades, ruta->id_ciudad_2);
			else
				ciudad_2 = hash_obtener(hash_ciudades, ruta->id_ciudad_1);
			

			imprimir_en_archivo(archivo_kml, ciudad_1);
			imprimir_en_archivo(archivo_kml, ciudad_2);
			fprintf(archivo_kml, "\t\t<Placemark>\n\t\t\t<LineString>\n\t\t\t\t<coordinates>%f, %f %f, %f</coordinates>\n", 
												ciudad_1->longitud, ciudad_1->latitud, ciudad_2->longitud, ciudad_2->latitud);
			fputs("\t\t\t</LineString>\n\t\t</Placemark>\n\n", archivo_kml);
			if (strcmp(pila_ver_tope(ciudad_actual), ruta->id_ciudad_1) == 0)
				pila_apilar(ciudad_actual, ruta->id_ciudad_2);
			else pila_apilar(ciudad_actual, ruta->id_ciudad_1);
		}
		pila_destruir(ciudad_actual);
	}
	
	fputs("\t</Document>\n", archivo_kml);
	fputs("</kml>\n", archivo_kml);
	
	fclose(archivo_kml);
	return;
}

void encontrar_rutas(grafo_t* grafo, hash_t* hash_ciudades_visitadas, cola_t* rutas_a_tomar, int n_fila){
	if (hash_cantidad(hash_ciudades_visitadas) == 37) return; // Recorrio todas las ciudades (la ultima no la incluye en el hash, por eso son 37)
	heap_t* heap_mejor_ruta = heap_crear(cmp);
	for (int i = (n_fila - 1) * 38; i < n_fila * 38; i++){
		ruta_t* ruta = grafo->matriz[i];
		if (!ruta) continue;
		if (atoi(ruta->id_ciudad_1) == n_fila){
			if (!hash_pertenece(hash_ciudades_visitadas, ruta->id_ciudad_2))
				heap_encolar(heap_mejor_ruta, ruta);
		}
		else{
			if (!hash_pertenece(hash_ciudades_visitadas, ruta->id_ciudad_1))
				heap_encolar(heap_mejor_ruta, ruta);
		}
	}
	ruta_t* mejor_ruta = heap_desencolar(heap_mejor_ruta);
	char ciudad_visitada[4];
	sprintf (ciudad_visitada, "%i", n_fila);
	hash_guardar(hash_ciudades_visitadas, ciudad_visitada, NULL);
	cola_encolar(rutas_a_tomar, mejor_ruta);
	if (n_fila != atoi(mejor_ruta->id_ciudad_1))
		encontrar_rutas(grafo, hash_ciudades_visitadas, rutas_a_tomar, atoi(mejor_ruta->id_ciudad_1));
	else
		encontrar_rutas(grafo, hash_ciudades_visitadas, rutas_a_tomar, atoi(mejor_ruta->id_ciudad_2));
	heap_destruir(heap_mejor_ruta, NULL);
}

/* Función que realiza la selección de las rutas que conecten todas las
 * ciudades maximizando la cantidad de gente favorecida y considerando
 * el largo de cada ruta */
void todas_ciudades_conectadas(grafo_t* grafo, hash_t* hash_ciudades, hash_t* hash_rutas) {
	hash_t* hash_ciudades_visitadas = hash_crear(NULL); // Hash que guarda las ciudades visitadas
	cola_t* rutas_a_tomar = cola_crear();
	encontrar_rutas(grafo, hash_ciudades_visitadas, rutas_a_tomar, N_PRIMER_CIUDAD);
	hash_destruir(hash_ciudades_visitadas);
	generar_archivo_kml("red.kml", hash_ciudades, rutas_a_tomar);
	cola_destruir(rutas_a_tomar, NULL);
	return;
}

/* Función que realiza selección de caminos entre ciudades para abastecer
 * de energía eléctrica a todas las estaciones usando un árbol de tendido
 * mínimo, a partir del subconjunto de rutas como resultado de la función
 * todas_ciudades_conectadas() */
void arbol_tendido_minimo(grafo_t* grafo, hash_t* hash_ciudades) {
	generar_archivo_kml("tendido.kml", hash_ciudades, NULL);
	return;
}

/* Función que realiza la obtención de rutas eficientes entre dos ciudades
 * y exporta el resultado a un mapa usando el formato KML */
void obtencion_rutas_eficientes(grafo_t* grafo, hash_t* hash_ciudades) {
	generar_archivo_kml("ruta.kml", hash_ciudades, NULL);
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

	/*	Para imprimir la matriz  //
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
	printf ("  .%i\n", i/38);*/

	todas_ciudades_conectadas(grafo, hash_ciudades, hash_rutas);
	arbol_tendido_minimo(grafo, hash_ciudades);
	obtencion_rutas_eficientes(grafo, hash_ciudades);
	hash_destruir(hash_ciudades);
	hash_destruir(hash_rutas);
	grafo_destruir(grafo);
	return 0;
}
