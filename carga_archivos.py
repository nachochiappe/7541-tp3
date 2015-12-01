# coding=utf-8

from estructuras import *
from parser import *
from collections import defaultdict

def cargar_ciudades(ruta_archivo):
	"""
	Carga todas las ciudades en el csv dado, y devuelve un diccionario con los objetos Ciudad creados apropiadamente
	Si alguna ciudad carga erroneamente, devuelve None.
	"""
	archivo = open(ruta_archivo, "r")

	#Pasar la primer linea
	archivo.readline()
	linea = archivo.readline()
	
	ciudades = defaultdict(Ciudad)
	while not (linea == ""):
		nueva_ciudad = parsear_ciudad(linea)
		if (nueva_ciudad == None):
			del ciudades
			return None
		ciudades[nueva_ciudad.id] = nueva_ciudad
		linea = archivo.readline()
	archivo.close()

	return ciudades

def cargar_rutas(ruta_archivo):
	"""
	Carga todas las rutas en el csv dado, y devuelve un diccionario con los objetos
	rutas creados apropiadamente.
	Para acceder a un elemento de este diccionario, se debe buscar en la forma
	"rutas[ciudad_A][ciudad_B]", siendo ciudad_A la ciudad de menor ID.
	Si alguna ciudad carga erroneamente, la función devuelve None.
	"""
	archivo = open(ruta_archivo, "r")

	#Pasar la primer linea
	archivo.readline()
	linea = archivo.readline()
	
	rutas = defaultdict(dict)
	while not (linea == ""):
		nueva_ruta = parsear_ruta(linea)
		if (nueva_ruta == None):
			del rutas
			return None
		if (nueva_ruta.ciudades()[0] < nueva_ruta.ciudades()[1]):
			rutas[nueva_ruta.ciudades()[0]][nueva_ruta.ciudades()[1]] = nueva_ruta
		else:
			rutas[nueva_ruta.ciudades()[1]][nueva_ruta.ciudades()[0]] = nueva_ruta
		linea = archivo.readline()
	archivo.close()

	return ruta