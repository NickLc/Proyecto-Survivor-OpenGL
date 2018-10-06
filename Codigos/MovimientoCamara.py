# @nombre del archivo: lab02-ejemplo4-3.py
# @Computacion Grafica, 2018-II, lab02, 7/09/2018
# @Lazaro Camasca Edson Nick

# Dibujando con OpenGL Parte1
from OpenGL.GL import *
from OpenGL.GLU import *
from OpenGL.GLUT import *
import sys

import sys, time
from math import sin,cos,sqrt,pi
#----------------------------------------------------------------------
ESCAPE = '\033'

distCamObj = 10.0
posCam = [-distCamObj, 0.0, 10.0]		# posCam[x, y ,z]
posDondeMiraCam = [0.0, 0.0, 6.0] #posDondeMiraCam[x , y, z]
angCam = 0.0			#Angulo entre el eje x y Camara-Objeto


distancia = 1.0		#nos servira para el movimiento del personaje, velocidad
cambioAngCam = 4 * pi/180		#cambiar el ang en sexagesimal en el 4

#------------------------- INIT -----------------------------------------

# La siguente es una funcion de incializacion
def init():
	# Utilizaremos el color negro como fondo
	# los numeros del 0 a 1.0 corresponden a (Red, Green,Blue, Alpha)
	glClearColor(0.0, 0.0, 0.0, 1.0)
	"""
	Definimos los rangos del sistema de coordenadas
	gluOrtho2D(x-izquierda, x-derecha, y-izquierda, y-arriba)
	lo que coloca el origen (0, 0) en el centro de la pantalla """
	gluOrtho2D(-1.0, 1.0, -1.0, 1.0)


#------------------------- DISPLAY --------------------------------------

# La siguiente funcion presenta la ESCENA
def display():
	#La siguente pantalla limpia la pantalla y pinta el fondo.
	glClearColor(1.0, 1.0, 1.0, 0.0)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
	
	#------------- Que la camara se actualiza --------------
	camara()
	
	#-----------------------------CONO----------------------------------------
	
	glColor3f(0.0, 1.0, 0.0)            # VERDE
	
	glPushMatrix()
	glTranslatef(30.0, 5.0, 0.0)
	glutSolidCone(10.0, 10.0, 60, 60);			# Definimos el cono
	glPopMatrix()
	
	glPushMatrix()
	glTranslatef(25.0, 75.0, 0.0)
	glutSolidCone(10.0, 10.0, 60, 60);			# Definimos el cono
	glPopMatrix()
	
	glPushMatrix()
	glTranslatef(45.0, 45.0, 0.0)
	glutSolidCone(10.0, 20.0, 60, 60);			# Definimos el cono
	glPopMatrix()
	
	glPushMatrix()
	glTranslatef(15.0, 50.0, 0.0)
	glutSolidCone(10.0, 10.0, 60, 60);			# Definimos el cono
	glPopMatrix()
	
	#-----------------------------PLANO----------------------------------------
	# Draw a square (quadrilateral)
	ladoCuadrado = 10

	glColor3f(0.3, 0.5, 1.0)            # Bluish shade
	glBegin(GL_QUADS)                   # Start drawing a 4 sided polygon
	glVertex3f(ladoCuadrado, ladoCuadrado, 0.0)          # Top Left
	glVertex3f(-ladoCuadrado, ladoCuadrado, 0.0)           # Top Right
	glVertex3f(-ladoCuadrado, -ladoCuadrado, 0.0)          # Bottom Right
	glVertex3f(ladoCuadrado, -ladoCuadrado, 0.0)         # Bottom Left
	glEnd()                             # We are done with the polygon
	
	#-------------------------EJES------------------------------------------------
	
	glBegin(GL_LINES)                 

	#-------------------------    x    ----------------------------------
	glColor3f(1.0, 0.0, 0.0)            # ROJO
	glVertex3f(0.0, 0.0, 0.0)
	glVertex3f(100.0, 0.0, 0.0)

	#-------------------------    Y    ----------------------------------
	glColor3f(0.0, 1.0, 0.0)            # VERDE
	glVertex3f(0.0, 0.0, 0.0)
	glVertex3f(0.0, 100.0, 0.0)
	
	#-------------------------    Z    ----------------------------------
	glColor3f(0.0, 0.0, 1.0)            # AZUL
	glVertex3f(0.0, 0.0, 0.0)
	glVertex3f(0.0, 0.0, 100.0)
	glEnd()                            
	

	# Presentamos la ESCENA en pantalla
	glFlush()	#funcionar

#----------------------------- RESHAPE -----------------------------------------

"""La funcion reshape se llama cada vez que se REDIMENSIONAMOS LA VENTANA
	Los parametros que recibe son el ancho y la altura de la ventana"""
def reshape(width, height):
	global posCam, posDondeMiraCam
	

	""" glViewport Define la porcion de ventana donde OpenGL podra dibujar 
	Parametos: Dist Horiz, Dist vert del inicio (Esq.Sup.Izquierda),
	ancho y altura de la ventana """
	glViewport(0, 0, width, height);
	
	"""glMatrixMode Define la matriz de transformacion elegida
		Existen: GL_PROJECTION - proyeccion, GL_MODELVIEW - Modelado"""
	glMatrixMode(GL_PROJECTION);

	#glLoadIdentity carga como matriz la proyeccion la matriz identidad - la inicializa
	glLoadIdentity()
	"""gluPerspective opera sobre la matriz de proyeccion y define en 
	angulo del campo de vision en sentido vertical(en grados)
	la relacion entre la altura y la anchura de la figura(aspecto)
	Los planos: el mas cercano y el mas distante a la camara"""
	gluPerspective(70.0, height / width, 1.0, 328.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity()
	#------------------------ CAMARA------------------------------------

	"""gluLookAt Define la transformacion sobre la vista inicial.
	tiene 9 parametros: los 3 primetos representan la DISTANCIA en x,y, z
	de los observadores los siguientes 3, las coordenadas x,y,z del punto
	de REFERENCIA a observa  y los 3 ultimos, la direccion del upVector"""
	gluLookAt(posCam[0], posCam[1], posCam[2], posDondeMiraCam[0], posDondeMiraCam[1], posDondeMiraCam[2], 0.0, 0.0, 1.0)
	
#---------------------------- FUNCION DESPLAZAMIENTO DE LA CAMARA-----------------------------------
def desplazamientoCam(angDes):

	global posCam, posDondeMiraCam
	global distancia

	posCam[0] = posCam[0] + distancia*cos(angDes)
	posCam[1] = posCam[1] + distancia*sin(angDes)

	posDondeMiraCam[0] = posDondeMiraCam[0] + distancia*cos(angDes)
	posDondeMiraCam[1] = posDondeMiraCam[1] + distancia*sin(angDes)

	print "posCam",posCam
	print "miraCam",posDondeMiraCam

#------------------------------ FUNCION ROTACION DE LA CAMARA-------------------------------------
def rotacionCam(angRot):

	global posCam, posDondeMiraCam
	global angCam

	angCam = angCam + angRot
	#------------------movimiento de la camara-------------------
	
	posCamTO = [0.0 ,0.0]
	posCamR = [0.0 , 0.0]

	# Trasladando el sistama camara-objeto al origen
	posCamTO[0] = posCam[0] - posDondeMiraCam[0]
	posCamTO[1] = posCam[1] - posDondeMiraCam[1]

	# Rotando alredor del origen
	posCamR[0] =  posCamTO[0]*cos(angRot) - posCamTO[1]*sin(angRot)
	posCamR[1] =  posCamTO[0]*sin(angRot) + posCamTO[1]*cos(angRot)

	#Trasladando al punto al sistema inicial
	posCam[0] = posCamR[0] + posDondeMiraCam[0]
	posCam[1] = posCamR[1] + posDondeMiraCam[1]


#-------------------------------------C A M A R A ------------------------------------
def camara():
	global posCam, posDondeMiraCam
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity()
	gluLookAt(posCam[0], posCam[1], posCam[2], posDondeMiraCam[0], posDondeMiraCam[1], posDondeMiraCam[2], 0.0, 0.0, 1.0)

#-------------------------FUNCIONES PARA EL TECLADO-------------------------------------------

# The function called whenever a key is pressed. Note the use of Python tuples to pass in: (key, x, y)  
def keyPressed(*args):
	
	global posCam, posDondeMiraCam 
	global angCam
	global cambioAngCam
	
	if args[0] == 'w' or args[0] == 'W':	# Adelante
		desplazamientoCam(angCam)

	if args[0] == 's' or args[0] == 'S':	# Atras
		desplazamientoCam(angCam - pi)
	
	if args[0] == 'q' or args[0] == 'Q':	# Izquierda	
		desplazamientoCam(angCam + pi/2)

	if args[0] == 'e' or args[0] == 'E':	# Derecha
		desplazamientoCam(angCam - pi/2)
	
	if args[0] == 'a' or args[0] == 'A':	#giro horario
		rotacionCam(cambioAngCam)
		
	if args[0] == 'd' or args[0] == 'D':	# giro antihorario
		rotacionCam(-cambioAngCam)

	if args[0] == ESCAPE : 
	    sys.exit()
	pass
	
	# Esta funcion indica a la GLUT que es necesario redibujar la ventana
	glutPostRedisplay()
		
#-----------------------------  MAIN -----------------------------------------

"""Usaremos la funciom main para iniciar OPEGL y llamar rutinas de
	inicializacion como Init"""

def main():
	
	glutInit(sys.argv)
#	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB)
	glutInitWindowSize(600, 600)
	glutInitWindowPosition(50, 50)
	glutCreateWindow("Laboratorio 2")
	
	# Call the funtions
	glutDisplayFunc(display)
	init()
	glutReshapeFunc(reshape)
	# Register the function called when the keyboard is pressed.  
	glutKeyboardFunc(keyPressed)

	glutMainLoop() # Mantiene a la ventana

main()
# End of program
