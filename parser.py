# coding=utf-8

from estructuras import *

def parsear_ciudad(linea):
	"""
	Lee una linea del archivo de ciudades, separa los campos, y devuelve un objeto Ciudad armado apropiadamente.
	Si hay un error al aplicar split, y hay menos campos de los esperados, devuelve None.
	"""
	campos = linea.split("\t")
	if (len(campos) < 6):
		return None
	try:
		nueva_ciudad = Ciudad(int(campos[0]), campos[1], float(campos[2]), float(campos[3]), int(campos[4]), int(campos[5]))
	except ValueError:
		return None
	return nueva_ciudad


def parsear_ruta(linea):
	"""
	Lee una linea del archivo de rutas, separa los campos, y devuelve un objeto Ruta armado apropiadamente.
	Si hay un error al aplicar split, y hay menos campos de los esperados, devuelve None.
	Si algun valor no tiene el formato apropiado (documentado en la clase) devuelve None.
	Si la ciudad de destino es igual a la de llegada, devuelve None.
	"""
	campos = linea.split("\t")
	if (len(campos) < 5):
		return None
	try:
		if (int(campos[1]) == int(campos[2])):
			return None
		nueva_ruta = Ruta(int(campos[0]), int(campos[1]), int(campos[2]), int(campos[3]), float(campos[4]))
	except ValueError:
		return None
	return nueva_ruta