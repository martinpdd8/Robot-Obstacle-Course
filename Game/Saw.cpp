
#include "Saw.h"

/* Creates a saw and loads some initial values */
Saw::Saw(model3DS *model, GLfloat x, GLfloat y, GLfloat z, GLfloat initial, GLfloat rotSpeed){
	saw = model;
	this->x = x;
	this->y = y;
	this->z = z;
	sawLeft = false;
	sawRight = true;
	bladeRot = 0.0f;
	initialRot = initial;
	this->rotSpeed = rotSpeed;
}

Saw::~Saw(){
	delete saw;
}

/* Draws the Saw */
void Saw::draw(GLuint id){
	glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glColor3f(1.0,1.0,1.0);
    glBindTexture(GL_TEXTURE_2D, id);
    glPushMatrix();

	glTranslatef(x, y, z);
	
	glRotatef(initialRot, 1, 0, 0);
	glRotatef(bladeRot, 0, 0, 1);

	glEnable(GL_DEPTH_TEST);
	saw->draw();
	glDisable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);
	glPopMatrix();
	glEnable(GL_LIGHTING); 
}

// moves left and right and rotates continuously
void Saw::move(){
	bladeRot += rotSpeed;
	if(sawLeft){
		if(x >= -9.5f) x -= 0.1f;
		else {
			sawRight = true;
			sawLeft = false;
		}
	}
	else if(sawRight){
		if(x <= 9.5f) x += 0.1f;
		else {
			sawLeft = true;
			sawRight = false;
		}
	}
}

float Saw::getY(){
	return y;
}

float Saw::getZ(){
	return z;
}

float Saw::getX(){
	return x;
}

void Saw::setX(GLfloat val){
	x = val;
}

void Saw::setY(GLfloat val){
	y = val;
}

void Saw::setZ(GLfloat val){
	z = val;
}



