#include <windows.h>	// for timeGetTime()
#include <mmsystem.h>	// ditto
#include <iostream>		// I/O
#include <GL/freeglut.h>	// GLUT
#include "model3DS.h" // 3DS model support
#include "textureBMP.h"

class Bonus {
private: 
	GLfloat		 x, y, z, initialRot, boxRot, rotSpeed;
	model3DS    *present;
	bool		shown, finished;
	bool		timingLaser;
	DWORD		startLaser;
	GLfloat       randX();

public:
	Bonus(model3DS *model, GLfloat x, GLfloat y, GLfloat z, GLfloat initialRot, GLfloat rotSpeed);
	~Bonus();
	float getX();
	float getY();
	float getZ();
	void  setX(GLfloat newX);
	void  setY(GLfloat newY);
	void  setZ(GLfloat newZ);
	void  finish();
	bool  getShown();
	GLfloat getRotSpeed();

	void  draw(GLuint id);
	void  move();
	void  reset();
};