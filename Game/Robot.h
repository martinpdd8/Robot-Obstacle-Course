#include <windows.h>	// for timeGetTime()
#include <mmsystem.h>	// ditto
#include <iostream>		// I/O
#include <GL/freeglut.h>	// GLUT
#include "model3DS.h" // 3DS model support
#include "textureBMP.h"
#include "Saw.h"
#include "Laser.h"
#include "Bonus.h"

class Robot{
private: 
	GLfloat		x, y, z, moveX, moveY, moveZ, totalZ, totalX, totalY;
	GLfloat		tiltLeft, tiltRight, spin, tiltBack, backZ;
	model3DS    *robot;
	bool		jumpUp, fallDown, movingUp, movingDown; 
	bool		tiltingLeft, driftBackRight; 
	bool		tiltingRight, driftBackLeft;
	bool		tiltingBack, sitBackUp;
	bool		movingLeft, movingRight;
	GLfloat		initialRot, boostSpeed;
	DWORD		start;
	bool        timing, speedUp;

public:
	Robot(model3DS *model, GLfloat myX, GLfloat myY, GLfloat myZ, GLfloat initial);
	~Robot();
	bool        win();
	float getX();
	float getY();
	float getZ();
	bool  getMovingLeft();
	bool  getMovingRight();
	bool  getTiltBack();
	bool  getJump();
	bool  getFall();
	
	void  setX(GLfloat newX);
	void  setY(GLfloat newY);
	void  setZ(GLfloat newZ);
	void  setMoveZ(GLfloat amount);

	void  setTiltLeft(bool tilt);
	void  setTiltRight(bool tilt);
	void  setMoveRight(bool tilt);
	void  setMoveLeft(bool tilt);
	void  setDriftRight(bool drift);
	void  setDriftLeft(bool drift);
	void  setJumpUp(bool jump);
	void  setTiltBack(bool tilt);
	void  setSitBackUp(bool tilt);
	void  boost();
	
	void  reset();
	void  draw(GLuint textureId);
	void  move(GLfloat speed, bool &camBoost);
	bool  hitSaw(Saw *saw);
	bool  hitLaser(Laser *laser);
	bool  hitPresent(Bonus *present);
};