
#include "Robot.h"


/* Creates a robot object and initialises all of its default values */
Robot::Robot(model3DS *model, GLfloat myX, GLfloat myY, GLfloat myZ, GLfloat initial){
	robot = model; 
	initialRot = initial;
	x = myX, y = myY, z = myZ;
	moveX = 0.0f, moveY = 0.0f, moveZ = 0.0f;
	movingLeft = false, movingRight = false;
	driftBackRight = false, driftBackLeft = false, tiltingLeft = false, tiltingRight = false;
	tiltingBack = false, sitBackUp = false;
	spin = 0.0f, tiltLeft = 0.0f, tiltBack = 0.0f;
	movingUp = true, movingDown = false;
	jumpUp = false;
	fallDown = false;
	backZ = 0.0f;
	timing = true, speedUp = false;
	boostSpeed = 0.0f;
}

Robot::~Robot(){
	delete robot;
}

void Robot::draw(GLuint id){
	glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LINE_SMOOTH); 
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST); // Ask for best line antialiasing.
	glColor3f(1.0,1.0,1.0);
    glBindTexture(GL_TEXTURE_2D, id);
    glPushMatrix();
	// update complete position of robot 
	totalZ = moveZ + z;
	totalX = moveX + x;
	totalY = moveY + y;
	// rotate it and move it 
	glRotatef(initialRot, 0, 1, 0);
	glTranslatef(totalX, moveY + y + backZ, totalZ);

	// tilting back AND left or right
	if((tiltingLeft || tiltingRight) && tiltingBack){
		glRotatef(-tiltBack, 1, 0, 0);
		glRotatef(tiltBack, 0, 1, 0);
		glRotatef(tiltLeft, 0, 1, 0);
		glRotatef(-tiltLeft, 0, 0, 1);
	}
    // tilting back ONLY 
    else if(!tiltingLeft && !tiltingRight && tiltingBack){
		glRotatef(-tiltBack, 1, 0, 0);
		glRotatef(tiltBack, 0, 1, 0);
	}
	 // tilting left OR right ONLY
	else if(!tiltingBack && (tiltingLeft || tiltingRight)){
		glRotatef(tiltLeft, 0, 1, 0);
		glRotatef(-tiltLeft, 0, 0, 1);
	}

	// jump 
	if(jumpUp || fallDown){
		glRotatef(spin, 0, 1, 0);
		spin += 18.0f;
		if(jumpUp){
			moveY += 0.05f;
			if (moveY >= 1.0f) {
				jumpUp = false;
				fallDown = true;
			}
		}
		else if(fallDown){
			moveY -= 0.05f;
			if(moveY <= 0.0f) fallDown = false; 
		}	
	}
	
	glEnable(GL_DEPTH_TEST);
	robot->draw();
	glDisable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);
	glPopMatrix();
	glEnable(GL_LIGHTING); 
}

// Animate the robot 
void Robot::move(float speed, bool &camBoost){
	moveZ += speed;

	/* Boost if necessary, for a short period of time, 100 ms*/
	if(speedUp && !movingLeft && !movingRight){
		if(timing){
			start = timeGetTime();
			timing = false;
		}
		else {	
			if(timeGetTime() - start <= 100){ }
			else if (timeGetTime() - start >= 100){
				timing = true;
				speedUp = false;
				camBoost = false;
			}
		}
	}
	/* Slide animation, tilt back and to the left 
	   and then back up again */
	if(tiltingBack && sitBackUp){
		if(tiltBack >= 0.0f) {
			tiltBack -= 7.0f;
			backZ += .05f;
		}
		else {
			sitBackUp = false;
			tiltingBack = false;
		}
	}
	else if(tiltingBack && tiltBack <= 75.0f) {
		tiltBack += 7.0f;
		backZ -= .05f;
	}

	/* Turning left Animation, updates the angle for tilting
	    the robot slighty to the left when moving left */
	if (tiltingLeft && driftBackRight){
		if(tiltLeft >= 0.0f) tiltLeft -= 0.5f;
		else {
		     tiltingLeft = false;
		     driftBackRight = false;
		}
	}
	else if(tiltingLeft && tiltLeft <= 5.0f) tiltLeft += 0.5f;
	
	/* Turning Right animation, updates the angle for tilting
	    the robot slighty to the left when moving left */
	else if (tiltingRight && driftBackLeft){
		if(tiltLeft <= 0.0f) tiltLeft += 0.5f;
		else {
			tiltingRight = false;
			driftBackLeft = false;
		}
	}
	else if(tiltingRight && tiltLeft >= -5.0f) tiltLeft -= 0.5f;

	/* This moves the camera and robot to the left or
		right depending on what button was pressed */
	if(movingLeft && totalX <= 9.0f){
		moveX += speed; 
	}
	else if(movingRight && totalX >= -9.0f){
		moveX -= speed; 
	}

	/* Jump animation, moves it up, and then down again */
	if (!fallDown && !jumpUp){
		if(movingUp){
			if (moveY <= 0.2f) moveY += 0.004f;
			else {
				movingUp = false;
				movingDown = true;
			}
		}
		if(movingDown){
			if(moveY >= 0.0f) moveY -= 0.004f;
			else {
				movingDown = false;
				movingUp = true;
			}
		}
	}
}

// Initiate the boost animation
void Robot::boost(){
	speedUp = true;
}

/* Detects and returns true when a robot collides with a saw */
bool Robot::hitSaw(Saw *saw){
	float z = (totalZ * -1); // workaround needed due to the way I rotated
	float x = (totalX * -1); // the scene on start up 
	if((totalZ * -1) <= saw->getZ() + 2.5f && (totalZ * -1) >= saw->getZ() - 1.f 
		&& (totalX * -1) >= saw->getX() - 1.f && (totalX * -1) <= saw->getX() + 1.f){
		return true;
	}
	return false;
}

/* Detects and returns true when a robot collides with a laser */
bool Robot::hitLaser(Laser *laser){
	GLfloat a =  totalZ + 37;
	if (a >= laser->getX() - .73f && a <= laser->getX() + .27 && (laser->getY() == -2.65f || laser->getY() == -2.75f) 
		&& !jumpUp && !fallDown) {
		return true;
	}
	else if (a >= laser->getX() - .73f && a <= laser->getX() + .27 && (laser->getY() == -1.0f || laser->getY() == -1.1f)
		&& !tiltingBack && !sitBackUp) {
		return true;
	}
	return false;
}

/* Detects and returns true when a robot passes over a present */
bool Robot::hitPresent(Bonus *present){
	if(totalX * -1 >= present->getX() - 0.8f && totalX * - 1 <= present->getX() + 0.8f 
		&& (totalZ + 2.0f) * -1 <= present->getZ() + .1f && (totalZ + 2.0f) * -1 >= present->getZ() - .1f){
			present->finish();
		return true;
	}
	return false;
}

/* Returns true when the robot reaches the end of the road */
bool Robot::win(){
	return totalZ >= 185.0f;
}

void Robot::setJumpUp(bool jump){
	if(!jumpUp && !fallDown)
	jumpUp = jump;
}

void Robot::setTiltLeft(bool tilt){
	tiltingLeft = tilt;
}

void Robot::setTiltRight(bool tilt){
	tiltingRight = tilt;
}

void Robot::setMoveLeft(bool tilt){
	movingLeft = tilt;
}

void Robot::setMoveRight(bool tilt){
	movingRight = tilt;
}

void Robot::setMoveZ(GLfloat val){
	z += val;
}

void Robot::setDriftRight(bool drift){
	driftBackRight = drift;
}

void Robot::setDriftLeft(bool drift){
	driftBackLeft = drift;
}

bool Robot::getMovingLeft(){
	return movingLeft;
}

bool Robot::getMovingRight(){
	return movingRight;
}

void Robot::setTiltBack(bool tilt){
	tiltingBack = tilt;
}

void Robot::setSitBackUp(bool tilt){
	sitBackUp = tilt;
}

bool Robot::getTiltBack(){
	return tiltingBack;
}

GLfloat Robot::getX(){
	return totalX;
}

GLfloat Robot::getY(){
	return totalY;
}

GLfloat Robot::getZ(){
	return totalZ;
}

bool Robot::getJump(){
	return jumpUp;
}

bool Robot::getFall(){
	return fallDown;
}

/* Reset the Robot to initial defaults.
   This is used when the game restarts */
void Robot::reset(){
	moveX = 0.0f, moveY = 0.0f, moveZ = 0.0f;
	movingLeft = false, movingRight = false;
	driftBackRight = false, driftBackLeft = false, tiltingLeft = false, tiltingRight = false;
	tiltingBack = false, sitBackUp = false;
	spin = 0.0f, tiltLeft = 0.0f;
	movingUp = true, movingDown = false;
	jumpUp = false;
	fallDown = false;
}


