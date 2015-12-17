// csv ciudades -  id	nombre	longitud   latitud	 provincia	habitantes
// csv rutas - id  id_ciudad_1	id_ciudad_2	 puntaje  distancia

#include "tp3.h"

#define ARCHIVO_CIUDADES "ciudades.csv"
#define ARCHIVO_RUTAS "rutas.csv"
#define COL_CIUDADES 6
#define COL_RUTAS 5
#define CANTIDAD_CIUDADES 38
#define PRIMER_CIUDAD "1" // Se usan para comenzar a buscar rutas desde la ciudad con este ID
#define N_PRIMER_CIUDAD 1 //

/******************************
*         ESTRUCTURAS         *
******************************/

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

/******************************
*   FUNCIONES DE COMPARACIÓN  *
******************************/

// Función de comparación de rutas. Toma en cuenta sólo la distancia del tramo.
int comparar_tendido(const void* ruta1, const void* ruta2) {
	double distancia_ruta1 = ((ruta_t*) ruta1)->distancia;
	double distancia_ruta2 = ((ruta_t*) ruta2)->distancia;
	if (distancia_ruta1 == distancia_ruta2) return 0;
	else if (distancia_ruta1 < distancia_ruta2) return 1;
	return (-1);
}
cmp_func_t cmp_tendido = &comparar_tendido; 


// Funcion de comparacion de rutas. Toma en cuenta puntaje y distancia del tramo.
int comparar_ruta(const void* ruta1, const void* ruta2) {
	double factor_ruta1 = (double) (((ruta_t*) ruta1)->distancia / ((ruta_t*) ruta1)->puntaje);
	double factor_ruta2 = (double) (((ruta_t*) ruta2)->distancia / ((ruta_t*) ruta2)->puntaje);
	if (factor_ruta1 == factor_ruta2) return 0;
	else if (factor_ruta1 < factor_ruta2) return 1;
	return (-1);
}
cmp_func_t cmp_mejor_ruta = &comparar_ruta;

/******************************
*     FUNCIONES AUXILIARES    *
******************************/

// Función que destruye un grafo.
// Pre: el grafo existe.
// Post: se destruyó el grafo y se liberó su memoria.
void grafo_destruir(grafo_t* grafo) {
	free(grafo->matriz);
	free(grafo);
}

// Función que crea una ciudad a partir de los datos pasados por parámetro.
// Pre: los parámetros fueron obtenidos del procesamiento del archivo de ciudades y son correctos.
// Post: se devuelve la ciudad creada, NULL si falló.
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

// Función que destruye una ciudad.
// Pre: la ciudad existe.
// Post: se destruyó la ciudad y se liberó su memoria.
void destruir_ciudad(void* dato){
	if (dato){
		ciudad_t* ciudad = dato;
		free(ciudad->id);
		free(ciudad->nombre);
	}
	free(dato);
}

// Función que crea una ruta a partir de los datos pasados por parámetro.
// Pre: los parámetros fueron obtenidos del procesamiento del archivo de rutas y son correctos.
// Post: se devuelve la ruta creada, NULL si falló.
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

// Función que destruye una ruta.
// Pre: la ruta existe.
// Post: se destruyó la ruta y se liberó su memoria.
void destruir_ruta(void* dato){
	if (dato){
		ruta_t* ruta = dato;
		free(ruta->id);
		free(ruta->id_ciudad_1);
		free(ruta->id_ciudad_2);
	}
	free(dato);
}

// Función que inserta una ciudad en un archivo KML.
// Pre: el archivo existe y está abierto. La ciudad existe.
// Post: se insertó la ciudad en el archivo KML.
void imprimir_ciudad_en_archivo(FILE* archivo_kml, ciudad_t* ciudad){
	fprintf(archivo_kml, "\t\t<Placemark>\n\t\t\t<name>%s</name>\n", ciudad->nombre);
	fprintf(archivo_kml, "\t\t\t<Point>\n\t\t\t\t<coordinates>%f, %f</coordinates>\n\t\t\t</Point>\n\t\t</Placemark>\n\n", ciudad->longitud, ciudad->latitud);
}

// Función que inserta una nueva línea (LineString) en un archivo KML.
// Pre: el archivo existe y está abierto. Las ciudades existen.
// Post: se insertó la línea en el archivo KML.
void imprimir_linea_en_archivo(FILE* archivo_kml, ciudad_t* ciudad_1, ciudad_t* ciudad_2){
	fprintf(archivo_kml, "\t\t<Placemark>\n\t\t\t<LineString>\n\t\t\t\t<coordinates>%f, %f %f, %f</coordinates>\n", ciudad_1->longitud, ciudad_1->latitud, ciudad_2->longitud, ciudad_2->latitud);
	fputs("\t\t\t</LineString>\n\t\t</Placemark>\n\n", archivo_kml);
}

/******************************
*    FUNCIONES PRINCIPALES    *
******************************/

/* Funcion utilizada para leer el archivo csv ARCHIVO_CIUDADES,
 * y generar un hash en base a este. En caso de no fallar, devuelve
 * el hash con los id de cada ciudad como clave, y como dato la estructura
 * 'ciudad_t' correspondiente.*/
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

// Función que crea un grafo representado como matriz de adyacencia a partir de un hash de ciudades y uno de rutas.
// Pre: los hashes de ciudades y rutas existen.
// Post: devuelve el grafo creado, NULL si falló.
grafo_t* grafo_crear(hash_t* hash_ciudades, hash_t* hash_rutas) {
	grafo_t* grafo = malloc(sizeof(grafo_t));
	if (!grafo) return NULL;
	size_t cantidad_vertices = hash_cantidad(hash_ciudades);
	// Creo una matriz de |V| x |V| (uso calloc para que no tire errores valgrind de inicializacion)
	ruta_t** matriz = calloc(cantidad_vertices * cantidad_vertices, sizeof(ruta_t));
	if (!matriz) return NULL;
	hash_iter_t* iter = hash_iter_crear(hash_rutas);
	while (!hash_iter_al_final(iter)){
		ruta_t* ruta = hash_obtener(hash_rutas, hash_iter_ver_actual(iter));
		size_t id_ciudad_1 = strtoul(ruta->id_ciudad_1, NULL, 10);
		size_t id_ciudad_2 = strtoul(ruta->id_ciudad_2, NULL, 10);
		// La forma de acceder a (x, y) en la matriz es: (x - 1) * cant_columnas + (y - 1)
		matriz[(id_ciudad_1 - 1) * cantidad_vertices + (id_ciudad_2 - 1)] = ruta;
		// Tambien ponemos los valores del otro lado de la 'diagonal' de la matriz, dado a que valen los caminos
		// de ida y vuelta (si una ruta va de ciudad 1 a ciudad 2, tambien vale de ciudad 2 a ciudad 1).
		matriz[(id_ciudad_2 - 1) * cantidad_vertices + (id_ciudad_1 - 1)] = ruta;
		grafo->aristas++;
		hash_iter_avanzar(iter);
	}
	hash_iter_destruir(iter);
	grafo->vertices = cantidad_vertices;
	grafo->matriz = matriz;
	return grafo;
}

/* Función que genera un archivo KML */
void generar_red_kml(char* nombre_archivo, hash_t* hash_ciudades, lista_t* camino_rutas) {
	FILE* archivo_kml = fopen(nombre_archivo, "w");
	if (!archivo_kml) return;
	
	fputs("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n", archivo_kml);
	fputs("<kml xmlns=\"http://earth.google.com/kml/2.1\">\n", archivo_kml);
	fputs("\t<Document>\n", archivo_kml);
	
	if (camino_rutas){
		pila_t* ciudad_actual = pila_crear();
		pila_apilar(ciudad_actual, PRIMER_CIUDAD);
		lista_iter_t* camino_rutas_iter = lista_iter_crear(camino_rutas);
		while (!lista_iter_al_final(camino_rutas_iter)){
			ruta_t* ruta = lista_iter_ver_actual(camino_rutas_iter);
			ciudad_t* ciudad_1 = hash_obtener(hash_ciudades, pila_ver_tope(ciudad_actual));
			ciudad_t* ciudad_2;
			if (strcmp(pila_ver_tope(ciudad_actual), ruta->id_ciudad_1) == 0)
				ciudad_2 = hash_obtener(hash_ciudades, ruta->id_ciudad_2);
			else
				ciudad_2 = hash_obtener(hash_ciudades, ruta->id_ciudad_1);
			

			imprimir_ciudad_en_archivo(archivo_kml, ciudad_1);
			imprimir_ciudad_en_archivo(archivo_kml, ciudad_2);
			imprimir_linea_en_archivo(archivo_kml, ciudad_1, ciudad_2);
			
			if (strcmp(pila_ver_tope(ciudad_actual), ruta->id_ciudad_1) == 0)
				pila_apilar(ciudad_actual, ruta->id_ciudad_2);
			else pila_apilar(ciudad_actual, ruta->id_ciudad_1);
			lista_iter_avanzar(camino_rutas_iter);
		}
		lista_iter_destruir(camino_rutas_iter);
		pila_destruir(ciudad_actual);
	}
	
	fputs("\t</Document>\n", archivo_kml);
	fputs("</kml>\n", archivo_kml);
	
	fclose(archivo_kml);
}

// Función que encuentra las rutas más convenientes a partir de una función de comparación pasada como parámetro.
void encontrar_rutas(grafo_t* grafo, hash_t* hash_ciudades_visitadas, lista_t* rutas_a_tomar, int n_fila, cmp_func_t cmp){
	if (hash_cantidad(hash_ciudades_visitadas) == CANTIDAD_CIUDADES - 1) return;
	heap_t* heap_mejor_ruta = heap_crear(cmp);
	for (int i = (n_fila - 1) * CANTIDAD_CIUDADES; i < n_fila * CANTIDAD_CIUDADES; i++){
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
	lista_insertar_ultimo(rutas_a_tomar, mejor_ruta);
	if (n_fila != atoi(mejor_ruta->id_ciudad_1))
		encontrar_rutas(grafo, hash_ciudades_visitadas, rutas_a_tomar, atoi(mejor_ruta->id_ciudad_1), cmp);
	else
		encontrar_rutas(grafo, hash_ciudades_visitadas, rutas_a_tomar, atoi(mejor_ruta->id_ciudad_2), cmp);
	heap_destruir(heap_mejor_ruta, NULL);
}

// Función que genera un archivo KML a partir de las rutas más eficientes pasadas como parámetro.
void generar_ruta_kml(hash_t* hash_ciudades, lista_t* rutas_a_tomar, char* primer_ciudad){
	FILE* archivo_kml = fopen("ruta.kml", "w");
	if (!archivo_kml) return;
	
	fputs("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n", archivo_kml);
	fputs("<kml xmlns=\"http://earth.google.com/kml/2.1\">\n", archivo_kml);
	fputs("\t<Document>\n", archivo_kml);

	while (!lista_esta_vacia(rutas_a_tomar)){
		ruta_t* ruta = lista_borrar_primero(rutas_a_tomar);
		ciudad_t* ciudad_1 = hash_obtener(hash_ciudades, ruta->id_ciudad_1);
		ciudad_t* ciudad_2 = hash_obtener(hash_ciudades, ruta->id_ciudad_2);
		if (strcmp(ciudad_1->id, primer_ciudad) == 0){
			imprimir_ciudad_en_archivo(archivo_kml, ciudad_1);
			imprimir_ciudad_en_archivo(archivo_kml, ciudad_2);
			strcpy(primer_ciudad, ciudad_2->id);
		}
		else {
			imprimir_ciudad_en_archivo(archivo_kml, ciudad_2);
			imprimir_ciudad_en_archivo(archivo_kml, ciudad_1);
			strcpy(primer_ciudad, ciudad_1->id);
		}
		imprimir_linea_en_archivo(archivo_kml, ciudad_1, ciudad_2);
	}

	fputs("\t</Document>\n", archivo_kml);
	fputs("</kml>\n", archivo_kml);
	
	fclose(archivo_kml);
}

/* Función que realiza la selección de las rutas que conecten todas las
 * ciudades maximizando la cantidad de gente favorecida y considerando
 * el largo de cada ruta */
void todas_ciudades_conectadas(grafo_t* grafo, hash_t* hash_ciudades, hash_t* hash_rutas) {
	hash_t* hash_ciudades_visitadas = hash_crear(NULL);
	lista_t* rutas_a_tomar = lista_crear();
	encontrar_rutas(grafo, hash_ciudades_visitadas, rutas_a_tomar, N_PRIMER_CIUDAD, cmp_mejor_ruta);
	hash_destruir(hash_ciudades_visitadas);
	generar_red_kml("red.kml", hash_ciudades, rutas_a_tomar);
	lista_destruir(rutas_a_tomar, NULL);
}

/* Función que realiza selección de caminos entre ciudades para abastecer
 * de energía eléctrica a todas las estaciones usando un árbol de tendido
 * mínimo, a partir del subconjunto de rutas como resultado de la función
 * todas_ciudades_conectadas() */
void arbol_tendido_minimo(grafo_t* grafo, hash_t* hash_ciudades) {
	hash_t* hash_ciudades_visitadas = hash_crear(NULL);
	lista_t* tendido = lista_crear();
	encontrar_rutas(grafo, hash_ciudades_visitadas, tendido, N_PRIMER_CIUDAD, cmp_tendido);
	hash_destruir(hash_ciudades_visitadas);
	generar_red_kml("tendido.kml", hash_ciudades, tendido);
	lista_destruir(tendido, NULL);
}

// Función que obtiene la ruta más eficiente entre 2 ciudades pasadas como parámetro.
void rutas_entre_dos_ciudades(grafo_t* grafo, lista_t* rutas_a_tomar, int id_salida, int id_llegada){
	hash_t* hash_ciudades_visitadas = hash_crear(NULL);
	double distancia = 0;
	while (id_salida != id_llegada){
		heap_t* heap_mejor_ruta = heap_crear(cmp_mejor_ruta);
		for (int i = (id_salida - 1) * CANTIDAD_CIUDADES; i < id_salida * CANTIDAD_CIUDADES; i++){
			ruta_t* ruta = grafo->matriz[i];
			if (!ruta) continue;
			if (atoi(ruta->id_ciudad_1) == id_llegada || atoi(ruta->id_ciudad_2) == id_llegada)
				ruta->puntaje *= 10; // La ruta es mejor si va a la ciudad que queremos llegar
			if (atoi(ruta->id_ciudad_1) == id_salida){
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
		sprintf (ciudad_visitada, "%i", id_salida);
		hash_guardar(hash_ciudades_visitadas, ciudad_visitada, NULL);
		lista_insertar_ultimo(rutas_a_tomar, mejor_ruta);
		if (id_salida == atoi(mejor_ruta->id_ciudad_1))
			id_salida = atoi(mejor_ruta->id_ciudad_2);
		else 
			id_salida = atoi(mejor_ruta->id_ciudad_1);
		heap_destruir(heap_mejor_ruta, NULL);
		distancia += mejor_ruta->distancia;
	}
	hash_destruir(hash_ciudades_visitadas);
}

/* Función que realiza la obtención de rutas eficientes entre dos ciudades
 * y exporta el resultado a un mapa usando el formato KML */
void obtencion_rutas_eficientes(grafo_t* grafo, hash_t* hash_ciudades) {
	char* id_ciudad_1 = malloc(sizeof(char) * 3);
	char* id_ciudad_2 = malloc(sizeof(char) * 3);
	do{
		printf("Ingrese el ID de la ciudad de salida: ");
		scanf("%s", id_ciudad_1);
	}while (atoi(id_ciudad_1) > CANTIDAD_CIUDADES || atoi(id_ciudad_1) < 1);
	do{
		printf("Ingrese el ID de la ciudad de llegada: ");
		scanf("%s", id_ciudad_2);
	}while (atoi(id_ciudad_2) > CANTIDAD_CIUDADES || atoi(id_ciudad_2) < 1);

	lista_t* rutas_a_tomar = lista_crear();
	rutas_entre_dos_ciudades(grafo, rutas_a_tomar, atoi(id_ciudad_1), atoi(id_ciudad_2));

	generar_ruta_kml(hash_ciudades, rutas_a_tomar, id_ciudad_1);
	free(id_ciudad_1);
	free(id_ciudad_2);
	lista_destruir(rutas_a_tomar, NULL);
}

// Función principal del programa.
// Post: se procesaron los archivos de rutas y ciudades y se generaron los KML correspondientes.
int main(void){
	hash_t* hash_ciudades = procesar_archivo_ciudades();
	hash_t* hash_rutas = procesar_archivo_rutas();
	grafo_t* grafo = grafo_crear(hash_ciudades, hash_rutas);
	todas_ciudades_conectadas(grafo, hash_ciudades, hash_rutas);
	arbol_tendido_minimo(grafo, hash_ciudades);
	obtencion_rutas_eficientes(grafo, hash_ciudades);
	hash_destruir(hash_ciudades);
	hash_destruir(hash_rutas);
	grafo_destruir(grafo);
	return 0;
}
