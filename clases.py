# coding=utf-8

class Ciudad(object):
	"""Vertice del Grafo, modelado con los datos de una ciudad"""
	def __init__(self, nro_id, nombre, longitud, latitud, provincia, habitantes):
		self.id = nro_id
		self.nombre = nombre
		self.longitud = longitud
		self.latitud = latitud
		self.provincia = provincia
		self.habitantes = habitantes
	
	def coordenadas(self):
		"""Devuelve una tupla (longitud, latitud)"""
		return (self.longitud, self.latitud)


class Ruta(object):
	"""Almacena el puntaje y distancia de una ruta entre dos ciudades."""
	def __init__(self, nro_id, id_ciudad1, id_ciudad2, distancia, puntaje):
		self.id = nro_id
		self.id_ciudad1 = id_ciudad1
		self.id_ciudad2 = id_ciudad2
		self.distancia = distancia
		self.puntaje = puntaje

	def ciudades(self):
		"""Devuelve una tupla, con las ciudades que une (ciudad1, ciudad2)"""
		return (self.id_ciudad1, self.id_ciudad2)