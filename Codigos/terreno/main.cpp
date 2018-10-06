#include <iostream>
#include <stdlib.h>
#include <math.h>

#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "imageloader.h"
#include "vec3f.h"

using namespace std;
//------------------------ T E R R E N O --------------------------------------

//Represents a terrain, by storing a set of heights and normals at 2D locations
class Terrain {
	private:
		int w; //Width
		int l; //Length
		float** hs; //Heights
		Vec3f** normals;
		bool computedNormals; //Whether normals is up-to-date
	public:
		Terrain(int w2, int l2) {
			w = w2;
			l = l2;
			
			hs = new float*[l];
			for(int i = 0; i < l; i++) {
				hs[i] = new float[w];
			}
			
			normals = new Vec3f*[l];
			for(int i = 0; i < l; i++) {
				normals[i] = new Vec3f[w];
			}
			
			computedNormals = false;
		}
		
		~Terrain() {
			for(int i = 0; i < l; i++) {
				delete[] hs[i];
			}
			delete[] hs;
			
			for(int i = 0; i < l; i++) {
				delete[] normals[i];
			}
			delete[] normals;
		}
		
		int width() {
			return w;
		}
		
		int length() {
			return l;
		}
		
		//Sets the height at (x, z) to y
		void setHeight(int x, int z, float y) {
			hs[z][x] = y;
			computedNormals = false;
		}
		
		//Returns the height at (x, z)
		float getHeight(int x, int z) {
			return hs[z][x];
		}
		
		//Computes the normals, if they haven't been computed yet
		void computeNormals() {
			if (computedNormals) {
				return;
			}
			
			//Compute the rough version of the normals
			Vec3f** normals2 = new Vec3f*[l];
			for(int i = 0; i < l; i++) {
				normals2[i] = new Vec3f[w];
			}
			
			for(int z = 0; z < l; z++) {
				for(int x = 0; x < w; x++) {
					Vec3f sum(0.0f, 0.0f, 0.0f);
					
					Vec3f out;
					if (z > 0) {
						out = Vec3f(0.0f, hs[z - 1][x] - hs[z][x], -1.0f);
					}
					Vec3f in;
					if (z < l - 1) {
						in = Vec3f(0.0f, hs[z + 1][x] - hs[z][x], 1.0f);
					}
					Vec3f left;
					if (x > 0) {
						left = Vec3f(-1.0f, hs[z][x - 1] - hs[z][x], 0.0f);
					}
					Vec3f right;
					if (x < w - 1) {
						right = Vec3f(1.0f, hs[z][x + 1] - hs[z][x], 0.0f);
					}
					
					if (x > 0 && z > 0) {
						sum += out.cross(left).normalize();
					}
					if (x > 0 && z < l - 1) {
						sum += left.cross(in).normalize();
					}
					if (x < w - 1 && z < l - 1) {
						sum += in.cross(right).normalize();
					}
					if (x < w - 1 && z > 0) {
						sum += right.cross(out).normalize();
					}
					
					normals2[z][x] = sum;
				}
			}
			
			//Smooth out the normals
			const float FALLOUT_RATIO = 0.5f;
			for(int z = 0; z < l; z++) {
				for(int x = 0; x < w; x++) {
					Vec3f sum = normals2[z][x];
					
					if (x > 0) {
						sum += normals2[z][x - 1] * FALLOUT_RATIO;
					}
					if (x < w - 1) {
						sum += normals2[z][x + 1] * FALLOUT_RATIO;
					}
					if (z > 0) {
						sum += normals2[z - 1][x] * FALLOUT_RATIO;
					}
					if (z < l - 1) {
						sum += normals2[z + 1][x] * FALLOUT_RATIO;
					}
					
					if (sum.magnitude() == 0) {
						sum = Vec3f(0.0f, 1.0f, 0.0f);
					}
					normals[z][x] = sum;
				}
			}
			
			for(int i = 0; i < l; i++) {
				delete[] normals2[i];
			}
			delete[] normals2;
			
			computedNormals = true;
		}
		
		//Returns the normal at (x, z)
		Vec3f getNormal(int x, int z) {
			if (!computedNormals) {
				computeNormals();
			}
			return normals[z][x];
		}
};

//Loads a terrain from a heightmap.  The heights of the terrain range from
//-height / 2 to height / 2.
Terrain* loadTerrain(const char* filename, float height) {
	Image* image = loadBMP(filename);
	Terrain* t = new Terrain(image->width, image->height);
	for(int y = 0; y < image->height; y++) {
		for(int x = 0; x < image->width; x++) {
			unsigned char color =
				(unsigned char)image->pixels[3 * (y * image->width + x)];
			float h = height * ((color / 255.0f) - 0.5f);
			t->setHeight(x, y, h);
		}
	}
	
	delete image;
	t->computeNormals();
	return t;
}

float _angle = 60.0f;
Terrain* _terrain;

void cleanup() {
	delete _terrain;
}

//----------------------F I N - D E L - T E R R E N O --------------------------------------


float distCamObj = 0.5;
float posCam[] = {-distCamObj, 0.0, 0.0};	// posCam[x, y ,z]
float posDondeMiraCam[] = {1.0, 0.0, 0.0};		// posDondeMiraCam[x , y, z]
float angCam = 0.0;			//Angulo entre el eje x y Camara-Objeto

float pi = 3.141593;
float distancia = 0.2;		// Nos servira para el movimiento del personaje, velocidad
float cambioAngCam = 4 * pi/180;	// Cambiar el ang en sexagesimal en el 4


void initRendering() {
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glShadeModel(GL_SMOOTH);
	glClearColor(85.0/255.0, 212.0/255.0, 255.0/255.0, 1.0);	///Color del cielo

	gluOrtho2D(-1.0, 1.0, -1.0, 1.0);

}

void handleResize(int w, int h) {
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, (double)w / (double)h, 1.0, 200.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//------------------------ CAMARA------------------------------------
	/*
	"""gluLookAt Define la transformacion sobre la vista inicial.
	tiene 9 parametros: los 3 primetos representan la DISTANCIA en x,y, z
	de los observadores los siguientes 3, las coordenadas x,y,z del punto
	de REFERENCIA a observa  y los 3 ultimos, la direccion del upVector"""
	*/
	gluLookAt(posCam[0], posCam[1], posCam[2], posDondeMiraCam[0], posDondeMiraCam[1], posDondeMiraCam[2], 0.0, 0.0, 1.0);
}
//-------------------------------------C A M A R A ------------------------------------
void camara(){
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(posCam[0], posCam[1], posCam[2], posDondeMiraCam[0], posDondeMiraCam[1], posDondeMiraCam[2], 0.0, 0.0, 1.0);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
}
void drawScene() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//------------- Que la camara se actualiza --------------
	camara();
	//--------------------------------------------------


	GLfloat ambientColor[] = {0.4f, 0.4f, 0.4f, 1.0f};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);
	
	GLfloat lightColor0[] = {0.6f, 0.6f, 0.6f, 1.0f};
	GLfloat lightPos0[] = {-0.5f, 0.8f, 0.1f, 0.0f};
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor0);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);
	
	float scale = 5.0f / max(_terrain->width() - 1, _terrain->length() - 1);
	glScalef(scale, scale, scale);
	glTranslatef(-(float)(_terrain->width() - 1) / 2,
				 0.0f,
				 -(float)(_terrain->length() - 1) / 2);
	
	glColor3f(0.3f, 0.9f, 0.0f);
	for(int z = 0; z < _terrain->length() - 1; z++) {
		//Makes OpenGL draw a triangle at every three consecutive vertices
		glBegin(GL_TRIANGLE_STRIP);
		for(int x = 0; x < _terrain->width(); x++) {
			Vec3f normal = _terrain->getNormal(x, z);
			glNormal3f(normal[0], normal[1], normal[2]);
			glVertex3f(x, _terrain->getHeight(x, z), z);
			normal = _terrain->getNormal(x, z + 1);
			glNormal3f(normal[0], normal[1], normal[2]);
			glVertex3f(x, _terrain->getHeight(x, z + 1), z + 1);
		}
		glEnd();
	}
	
	glutSwapBuffers();
}

void update(int value) {
	//_angle += 1.0f;
	if (_angle > 360) {
		_angle -= 360;
	}
	
	glutPostRedisplay();
	glutTimerFunc(25, update, 0);
}
void rotacionCam(float angRot){


	angCam = angCam + angRot;
	// ------------------movimiento de la camara-------------------
	
	float posCamTO[] = {0.0 ,0.0};
	float posCamR[] = {0.0 , 0.0};

	//  Trasladando el sistama camara-objeto al origen
	posCamTO[0] = posCam[0] - posDondeMiraCam[0];
	posCamTO[1] = posCam[1] - posDondeMiraCam[1];

	//  Rotando alredor del origen

	posCamR[0] =  posCamTO[0]*cos(angRot) - posCamTO[1]*sin(angRot);
	posCamR[1] =  posCamTO[0]*sin(angRot) + posCamTO[1]*cos(angRot);

	// Trasladando al punto al sistema inicial
	posCam[0] = posCamR[0] + posDondeMiraCam[0];
	posCam[1] = posCamR[1] + posDondeMiraCam[1];
}

void desplazamientoCam(float angDes){

	posCam[0] = posCam[0] + distancia*cos(angDes);
	posCam[1] = posCam[1] + distancia*sin(angDes);

	posDondeMiraCam[0] = posDondeMiraCam[0] + distancia*cos(angDes);
	posDondeMiraCam[1] = posDondeMiraCam[1] + distancia*sin(angDes);
}

void handleKeypress(unsigned char key, int x, int y) {
	switch (key) {
		case 'w':
		case 'W':	//  Adelante
			desplazamientoCam(angCam);
			break;
		case 's':
		case 'S':	//  Atras
			desplazamientoCam(angCam - pi);
			break;
		case 'q':
		case 'Q':	//  Izquierda	
			desplazamientoCam(angCam + pi/2);
			break;
		case 'e':
		case 'E':	//  Derecha
			desplazamientoCam(angCam - pi/2);
			break;
		case 'a':
		case 'A':	// giro horario
			rotacionCam(cambioAngCam);
			break;
		case 'd':
		case 'D':	//  giro antihorario
			rotacionCam(-cambioAngCam);
			break;
		case 27: //Escape key
			cleanup();
			exit(0);
	}

}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(800, 800);
	
	glutCreateWindow("Terrain ");
	initRendering();
	
	_terrain = loadTerrain("heightmap.bmp", 20);
	
	glutDisplayFunc(drawScene);
	glutKeyboardFunc(handleKeypress);
	glutReshapeFunc(handleResize);
	glutTimerFunc(25, update, 0);
	
	glutMainLoop();
	return 0;
}









