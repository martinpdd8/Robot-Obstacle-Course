#include <windows.h>	// for timeGetTime()
#include <mmsystem.h>	// ditto
#include <iostream>		// I/O
#include <GL/freeglut.h>	// GLUT
#include "model3DS.h" // 3DS model support
#include "textureBMP.h"

class Laser {
private: 
	GLfloat		 x, y, z, initialRot;
	model3DS    *laser;
	bool		timingLaser;
	DWORD		startLaser;

public:
	Laser(model3DS *model, GLfloat x, GLfloat y, GLfloat z, GLfloat initialRot);
	~Laser();

	void  draw(GLuint id);
	void  drawRandom(GLuint id, int last);
	GLfloat getX();
	GLfloat getY();
	GLfloat getZ();
};