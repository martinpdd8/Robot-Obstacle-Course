#include <windows.h>	// for timeGetTime()
#include <mmsystem.h>	// ditto
#include <iostream>		// I/O
#include <GL/freeglut.h>	// GLUT
#include "model3DS.h" // 3DS model support
#include "textureBMP.h"

class Saw {
private: 
	GLfloat		 x, y, z, initialRot, bladeRot, rotSpeed;
	model3DS    *saw;
	bool		 sawLeft, sawRight;

public:
	Saw(model3DS *model, GLfloat x, GLfloat y, GLfloat z, GLfloat initialRot, GLfloat rotSpeed);
	~Saw();
	float getX();
	float getY();
	float getZ();
	bool  getSawLeft();
	bool  getSawRight();
	void  setX(GLfloat newX);
	void  setY(GLfloat newY);
	void  setZ(GLfloat newZ);
	GLfloat getRotSpeed();

	void  setSawLeft(bool newSawLeft);
	void  setSawRight(bool newSawRight);
	void  updateRotSpeed(GLfloat angle);

	void  draw(GLuint id);
	void  move();
};