
#include "Laser.h"

// Creates a laser with some important initial info 
Laser::Laser(model3DS *model, GLfloat x, GLfloat y, GLfloat z, GLfloat initialRot){
	laser = model;
	this->x = x;
	this->y = y;
	this->z = z;
	this->initialRot = initialRot;
	timingLaser = true;
}

Laser::~Laser(){
	delete laser;
}

// draws it
void Laser::draw(GLuint id){
	glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glColor3f(1.0,1.0,1.0);
    glBindTexture(GL_TEXTURE_2D, id);
    glPushMatrix();
	
	glRotatef(initialRot, 0, 1, 0);
	glTranslatef(x, y, z);
	
	glEnable(GL_DEPTH_TEST);
	laser->draw();
	glDisable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);
	glPopMatrix();
	glEnable(GL_LIGHTING); 
}

/* Draw a laser, randomly on top or bottom  */
void Laser::drawRandom(GLuint id, int last = 0){
	int which = -1;
	if(timingLaser){
		startLaser = timeGetTime();
		timingLaser = false;
	}
	else {	
		if(timeGetTime() - startLaser >= 2000){
			which = rand() % 2;
			timingLaser = true;
		}
	}
	// set the positions
	if(last == 0){ // for the 1st few 
		if(which == 0) y = -2.65f;
		else if (which == 1) y = -1.0f;
	}
	// last few are a bit awkward
	else if(last == 4){
		x = 168.73f;
		if(which == 0) y = -2.75;
		else if (which == 1)  y = -1.1f;
	}
	else if(last == 5){
		x = 188.73f;
		if(which == 0) y = -2.75;
		else if (which == 1)  y = -1.1f;
	}
	else if(last == 6){
		x = 208.73f;
		if(which == 0) y = -2.75;
		else if (which == 1)  y = -1.1f;
	}
	draw(id); // draw it
}

GLfloat Laser::getX(){
	return x;
}

GLfloat Laser::getY(){
	return y;
}

GLfloat Laser::getZ(){
	return z;
}