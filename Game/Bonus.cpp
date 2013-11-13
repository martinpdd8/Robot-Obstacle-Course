
#include "Bonus.h"


Bonus::Bonus(model3DS *model, GLfloat x, GLfloat y, GLfloat z, GLfloat initial, GLfloat rotSpeed){
	present = model;
	this->x = x;
	this->y = y;
	this->z = z;
	boxRot = 0.0f;
	initialRot = initial;
	this->rotSpeed = rotSpeed;
	shown = false;
	timingLaser = true;
	finished = false;
}

Bonus::~Bonus(){
	delete present;
}

GLfloat Bonus::randX(){
	int pos = (rand() % 9) + 1;
	int sign = (rand() % 2);
	if(sign == 0) return -pos;
	else return +pos;
}

void Bonus::finish(){
	finished = true;
}

void Bonus::reset(){
	finished = false;
	shown = true;
}

/* Draws the Bonus */
void Bonus::draw(GLuint id){
	if(!finished){
		if(timingLaser){
			startLaser = timeGetTime();
			timingLaser = false;
		}
		else {	
			// change the laser up/down every 4 to 6 seconds
			if(timeGetTime() - startLaser >= rand() % 6000 + 4000){
				x = randX();
				timingLaser = true;
			}
		}
		// initially was going to have them just appear 
		// but it makes it too difficult/annoying
		shown = true; 
		if(shown){
			glEnable(GL_LIGHTING);
			glEnable(GL_TEXTURE_2D);
			glColor3f(1.0,1.0,1.0);
			glBindTexture(GL_TEXTURE_2D, id);
			glPushMatrix();

			glTranslatef(x, y, z);
	
			glRotatef(initialRot, 1, 0, 0);
			glRotatef(boxRot, 0, 1, 0);

			glEnable(GL_DEPTH_TEST);
			present->draw();
			glDisable(GL_DEPTH_TEST);
			glDisable(GL_TEXTURE_2D);
			glPopMatrix();
			glEnable(GL_LIGHTING); 
		}
	}
}

// continuously rotate 
void Bonus::move(){
	boxRot += rotSpeed;
}

float Bonus::getY(){
	return y;
}

float Bonus::getZ(){
	return z;
}

float Bonus::getX(){
	return x;
}

bool Bonus::getShown(){
	return shown;
}

void Bonus::setX(GLfloat val){
	x = val;
}

void Bonus::setY(GLfloat val){
	y = val;
}

void Bonus::setZ(GLfloat val){
	z = val;
}