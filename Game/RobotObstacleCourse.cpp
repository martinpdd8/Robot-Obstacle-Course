#include <windows.h>	// for timeGetTime()
#include <mmsystem.h>	// ditto
#include <iostream>		// I/O
#include <GL/glut.h>
#include <GL/freeglut.h>	// GLUT
#include "model3DS.h" // 3DS model support
#include "textureBMP.h"
#include <ctime> // for time 
#include <cstdlib> // rand
#include "Robot.h" // robot player
#include <irrKlang.h> // for audio support


using namespace irrklang;

#define		NUM_SAWS 7
#define		NUM_LASERS 7
#define		NUM_BONUS 10

GLvoid setupScene();
GLvoid updateScene();
GLvoid renderScene();
GLvoid exitScene();
GLvoid print(char* string);
GLvoid keypress(unsigned char key, int x, int y);
GLvoid setViewport(int width, int height);

int         win_height = 1080;
int         win_width = 1920;
bool		gameOver = true; // game is over on startup
int			numCollected = 0; // number of items collected 
bool        begin = false; // begin is initially false
bool		win = false; // for when the user makes it to the end

GLfloat		fraction = 0.2f; // amount by which to move the robot and camera forward

/* Models */
model3DS *robot, *landscape, *sawModel, *laserModel, *present;

/* The various entities in the game */
Saw				*saws[NUM_SAWS];
Robot			*robotPlayer;
Laser			*lasers[NUM_LASERS];
Bonus			*bonus[NUM_BONUS];

ISoundEngine*	engine; // audio manager

GLfloat			boostAmount = 0.0f;
bool			boost = false;

int				windowId;
GLuint			textureId, tex2, tex3; // texture ids 
DWORD			lastTickCount;

/* The lights */
GLfloat			white_light[] = {0.9, 0.9, 0.9, 1.0};
GLfloat			left_light_position[] = {1,0,-1, 1.0}; 
GLfloat			right_light_position[] = {-1,0,-1, 1.0};

// The vectors which indicate the direction the camera is pointing
GLfloat	lx = 0.0f, ly = -0.5f, lz = -1.0f;

// The x y and z position of the camera in the scene
GLfloat	x = 0.0f, y = 2.2f, z = -13.5f;

// Draws the background scenery/model mountains and a road with streetlights
GLvoid loadScenery(model3DS *mod, GLfloat xx, GLfloat yy, GLfloat zz, GLuint id){
	glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glColor3f(1.0,1.0,1.0);
    glBindTexture(GL_TEXTURE_2D, id);
    glPushMatrix();
	glRotatef(90, 0, 1, 0);
	glTranslatef(xx, yy, zz);
	glEnable(GL_DEPTH_TEST);
	mod->draw();
	glDisable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);
	glPopMatrix();
	glEnable(GL_LIGHTING); 
}

// Draws the background image (sky) behind our scene
// You will actually only see this if you tilt the camera
// back in-game using 'k' 
 GLvoid drawBackground(GLuint texId)  {
	glDisable(GL_LIGHTING);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glRotatef(0, 0, 0, 1);
	glOrtho(0, 1, 0, 1, 0, 1);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	// No depth buffer writes for background.
	glDepthMask(false);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texId);
	glColor3f(1.0f, 1.0f, 1.0f);
	GLfloat a = 1.0f;
	glBegin(GL_QUADS ); {
	  glTexCoord2f(0.f, 0.f);  glVertex2f(0.1, 0.1);		// 0 0
	  glTexCoord2f(0.f, a);  glVertex2f(0.1, 1.f);      // 0 1
	  glTexCoord2f(a, a);  glVertex2f(1.f, 1.f);	// 1 1
	  glTexCoord2f(a, 0.f);  glVertex2f(1.f, 0.1);		// 1 0
	} glEnd();
	glDisable(GL_TEXTURE_2D);
	glDepthMask(true);
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_LIGHTING);
}

// Writes the players score on the top of the screen
GLvoid keepScore(GLvoid) {
    glPushAttrib(GL_LIGHTING_BIT | GL_DEPTH_BUFFER_BIT | GL_TEXTURE_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);

    glMatrixMode(GL_PROJECTION);
        glPushMatrix();
            glLoadIdentity();
            glOrtho(0.0f, 1.0f, 0.0f, 1.0f, -1.0f, 1.0f);
			std::ostringstream str;
		    str << "Items Collected " << numCollected << "/10";
			glEnable(GL_LINE_SMOOTH); 
			glHint(GL_LINE_SMOOTH_HINT, GL_NICEST); // Ask for best line antialiasing.
            glColor3f(1.0f, 1.0f, 1.0f);
			glRasterPos2f(0.4f, 0.95f);
			std::string msg = str.str();
            glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char*) msg.c_str());
        glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
    glPopAttrib();

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);  
    glEnable(GL_DEPTH_TEST);
}

// Writes the game-over screen information on the screen
GLvoid drawEnd() {
    glPushAttrib(GL_LIGHTING_BIT | GL_DEPTH_BUFFER_BIT | GL_TEXTURE_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);

    glMatrixMode(GL_PROJECTION);
        glPushMatrix();
            glLoadIdentity();
            glOrtho(0.0f, 1.0f, 0.0f, 1.0f, -1.0f, 1.0f);
			glColor3ub(38, 255, 201);
			char * w;  //  Hit Space to Play Again!
			if(win)  w = "   You beat the game!";
			else w = "      Game Over!";
			glRasterPos2f(.45f, .7f);
            glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char*) w);
			std::ostringstream str;
			str << " You collected " << numCollected << " items";
			glRasterPos2f(.45f, .6f);
			glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char*) str.str().c_str());
			if (numCollected <= 2) {
			    glRasterPos2f(.45f, .5f);                                          
			    glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char*) "     That's terrible!");
			}
			else {
				glRasterPos2f(.45f, .5f);                                          
			    glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char*) "     Well done!");
			}
			glRasterPos2f(.45f, .3f);
			glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char*) "Hit Space to Play Again!");
			
        glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
    glPopAttrib();

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);  
    glEnable(GL_DEPTH_TEST);
}

// Writes begin screen text / information
GLvoid drawStart() {
    glPushAttrib(GL_LIGHTING_BIT | GL_DEPTH_BUFFER_BIT | GL_TEXTURE_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);

    glMatrixMode(GL_PROJECTION);
        glPushMatrix();
            glLoadIdentity();
            glOrtho(0.0f, 1.0f, 0.0f, 1.0f, -1.0f, 1.0f);
			glColor3ub(38, 255, 201);

			glRasterPos2f(.3f, .7f);
            glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char* ) "See if you can survive to the end and collect as many items as you can along the way!!");
			
			glRasterPos2f(.45f, .6f);
            glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char*) "Press space to play!");
			
			glRasterPos2f(.45f, .35f);
            glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char*) "   W - Boost");
			
			glRasterPos2f(.45f, .30f);
            glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char*) "   A - Left");

			glRasterPos2f(.45f, .25f);
            glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char*) "   D - Right");

			glRasterPos2f(.45f, .20f);
            glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char*) "   C - Slide");

			glRasterPos2f(.45f, .15f);
            glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)   "SPACE - Jump");

			glRasterPos2f(.45f, .10f);
            glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)   " ESC - Exit");
			
        glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
    glPopAttrib();

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);  
    glEnable(GL_DEPTH_TEST);
}

// Draws the home screen 
GLvoid drawHomeScreen(){
	glPushMatrix(); //remember the current state of the modelview matrix
		glTranslatef(0.0f, 0.0f, -17.0f); // move sphere to end of cylinder like before	
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, tex2);
		glColor3f(1.0f, 1.0f, 1.0f);
		glBegin(GL_QUADS);                      // Draw A Quad
			glTexCoord2f(0, 1); glVertex3f(-12.6f, 7.1f, 0.0f);              // Top Left
			glTexCoord2f(1, 1); glVertex3f(12.6f, 7.1f, 0.0f);              // Top Right
			glTexCoord2f(1, 0); glVertex3f(12.6f, -7.1f, 0.0f);              // Bottom Right
			glTexCoord2f(0, 0); glVertex3f(-12.6f, -7.1f, 0.0f);              // Bottom Left
		glEnd(); 
		glDisable(GL_TEXTURE_2D);
	glPopMatrix();
}

// Render the scene
GLvoid renderScene(){
    // Clear framebuffer & depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Reset Modelview matrix      	
    glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();	
	
	// Sets the initial position and direction of the camera
	gluLookAt(x, y, z,  x + lx, y + ly,  z + lz,  0.0f, y, 0.0f);

	// Draw the home screen
	if(!begin){
		drawStart();
		drawHomeScreen();
		drawStart();
	}
	// Draw the game-over screen
	else if(gameOver || win){
		drawEnd();
		drawHomeScreen();
		drawEnd();
	}
	// Draw the game
	else {
		drawBackground(tex3);
		loadScenery(landscape, 117.0f, 16.0f, 0.0f, textureId);
		robotPlayer->draw(textureId);
		// draw every laser
		for (int i = 0; i < NUM_LASERS -3; i++){
			lasers[i]->drawRandom(textureId, false);
		}
		// final few lasers are out of line are draw separately
		lasers[4]->drawRandom(textureId, 4);
		lasers[5]->drawRandom(textureId, 5);
		lasers[6]->drawRandom(textureId, 6);

		// draw the saws
		for (int i = 0; i < NUM_SAWS; i++){
			saws[i]->draw(textureId);
		}
		// draw the collectable items
		for (int i = 0; i < NUM_BONUS; i++){
			bonus[i]->draw(textureId);
		}
		// draw the score on the screen
		keepScore();
	}
    glutSwapBuffers();    
}

// update the scene
GLvoid updateScene() {
	while(timeGetTime() - lastTickCount < 16);
	lastTickCount = timeGetTime();
	// if the game isn't over
	if(!gameOver){
		// test to see if our robot made it to the end
		if(robotPlayer->win()){
			win = true;
		}			
		x += lx * fraction; // move our camera left or right
		// only allow boost forward, not left or right
		if(boost && !robotPlayer->getMovingLeft() && !robotPlayer->getMovingRight()) {
			boostAmount = 0.9f;
		}
		else boostAmount = 0.0f;

		// move the camera forward
		z += lz * (fraction + boostAmount);

		// move the robot forward
		robotPlayer->move(fraction + boostAmount, boost);

		// don't let the robot move off the road into the mountain
		if(robotPlayer->getMovingLeft() && x >= -9.0f){
			x -= fraction;
		}
		else if(robotPlayer->getMovingRight() && x <= 9.0f){
			x += fraction;
		}

		// move each saw and end the game if the robot collides with one
		for (int i = 0; i < NUM_SAWS; i++){
			saws[i]->move();
			if (robotPlayer->hitSaw(saws[i])){
				engine->play2D("..\\Game\\resources\\audio\\smash.wav");
				gameOver = true;
			}
		}

		// end the game if the robot hits one of the lasers
		for (int i = 0; i < NUM_LASERS; i++){
			if(robotPlayer->hitLaser(lasers[i])){
				engine->play2D("..\\Game\\resources\\audio\\smash.wav");
				gameOver = true;
			}
		}	

		// keeps of when the robot collides with a present/box 
		for (int i = 0; i < NUM_BONUS; i++){
			bonus[i]->move();
			if(robotPlayer->hitPresent(bonus[i])){
				engine->play2D("..\\Game\\resources\\audio\\blip.wav");
				numCollected++;
			}
		}
	}
	// Draw the next frame
    glutPostRedisplay();
}

// Handles keyboard events - controls for the game 
GLvoid keypress(unsigned char key, int x2, int y2) {
    GLfloat fraction = 1.5f; // amount to move stuff by
	switch (key) {
		// if space bar is pressed start the game or jump depending on game state
		case ' ' :
			if (!begin) {
				engine->play2D("..\\Game\\resources\\audio\\wecometogether.mp3");
				begin = true; 
				gameOver = false;
			}
			// reset everything
			else if(begin && (gameOver || win)){
				gameOver = false;
				win = false;
				x = 0.0f, z = -13.5f;
				for (int i = 0; i < NUM_BONUS; i++){
					bonus[i]->reset();
				}
				robotPlayer->reset();
				numCollected = 0;
				fraction = 0.2;
			}
			else robotPlayer->setJumpUp(true);
			break;
		// Close the game
		case 27 :
			exitScene();
		    break;
		// Turn left 
		case 'a' :
			robotPlayer->setMoveLeft(true);
			robotPlayer->setTiltLeft(true);
			break;
		// Turn right
		case 'd' :
			robotPlayer->setMoveRight(true);
			robotPlayer->setTiltRight(true);
			break;
		// Slide
		case 'c' :
			robotPlayer->setTiltBack(true);
			break;
		// Tilts the camera up a little bit
		case 'k' : 
			ly += 0.05f;
			y -= 0.2;
			break;
		// Changes to a birds eye view
		case 'v' :
			if(y == 2.2f && ly == -0.5){
				y = 13.0f;
				ly = -3.2f;
			}
			else {
				y = 2.2f;
				ly = -0.5f;
			}
			break;
	}
}

// Handles the keyboard up events
GLvoid keyup(unsigned char key, int x, int y){
	if(key == 'a'){
		robotPlayer->setDriftRight(true);
		robotPlayer->setMoveLeft(false);
	}
	else if (key == 'd'){
		robotPlayer->setDriftLeft(true);
		robotPlayer->setMoveRight(false);
	}
	else if(key == 'c'){
		robotPlayer->setSitBackUp(true);
	}
	// Only boost on key up, prevents constant boosting!
	else if(key == 'w'){
		if(!robotPlayer->getMovingLeft() && !robotPlayer->getMovingRight()){
			robotPlayer->boost();
			boost = true;
		}
	}
}

// Selects a random x position on the road
GLfloat randX(){
	int pos = (rand() % 9) + 1;
	int sign = (rand() % 2);
	if(sign == 0) return -pos;
	else return +pos;
}

GLvoid setupScene(){

	std::cout<<"Initializing scene..."<<std::endl;
    glClearColor(0.1f, 0.1f, 0.9f, 0.0f);
	//Set up Lighting Stuff
	glLightfv(GL_LIGHT0, GL_POSITION, left_light_position);
	glLightfv(GL_LIGHT0, GL_SPECULAR, white_light);
	glLightfv(GL_LIGHT1, GL_POSITION, right_light_position);
	glLightfv(GL_LIGHT1, GL_SPECULAR, white_light);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, white_light);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, white_light);
	// Ask for smooth edges
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LINE_SMOOTH); 
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST); // Ask for best line antialiasing.
	glEnable(GL_BLEND);
	glEnable(GLUT_MULTISAMPLE);

	// Load in the images
	glGenTextures(1, &tex3);
	textureBMP skyTexture("..\\Game\\resources\\images\\blue_sky.bmp", tex3);
	glGenTextures(1, &tex2);
	textureBMP beginTexture("..\\Game\\resources\\images\\game_background.bmp", tex2);

	/* Load the models */
 	robot = new model3DS("..\\Game\\resources\\models\\robot\\robot.3DS", 0.025);  
	landscape = new model3DS("..\\Game\\resources\\models\\road-and-scenery\\lasers.3DS", 1.0);
	laserModel = new model3DS("..\\Game\\resources\\models\\laser\\laser.3DS", 1.0);
	sawModel = new model3DS("..\\Game\\resources\\models\\saw\\saw.3DS", 0.5);
	present = new model3DS("..\\Game\\resources\\models\\box\\box.3DS", 0.015);
	
	/* Create the items to collect */
	GLfloat temp = -35.0f;
	for (int i = 0; i < NUM_BONUS; i++){
		bonus[i] = new Bonus(present, randX(), -2.5f, temp, 0.0f, 2.5f);
		temp -= 12.5f + (rand() % 7);
	}
	temp = 86.73f;

	/* Create the lasers */
	for (int i = 0; i < NUM_LASERS - 3; i++){
		lasers[i] = new Laser(laserModel, temp, -1.0f, -0.05f, 90.0f);
		temp += 20.0f;
	}
	lasers[4] = new Laser(laserModel, 168.73, -1.0f, -0.05f, 90.0f);
	lasers[5] = new Laser(laserModel, 188.73, -1.0f, -0.05f, 90.0f);
	lasers[6] = new Laser(laserModel, 208.73, -1.0f, -0.05f, 90.0f);

	// Create the robot 
	robotPlayer = new Robot(robot, 0.0f, -1.0f, 18.5f, 180.0f);
	temp = -60.0f;

	/* Create the saws */
	for (int i = 0; i < NUM_SAWS; i++){
		saws[i] = new Saw(sawModel, randX(), -2.5f, temp, 90.0f, 7.0f);
		temp -= 20.0f;
	}	

	// set up sound engine
	engine = createIrrKlangDevice();
	if(!engine) std::cout << "engine failed";
}

GLvoid exitScene(){
    std::cout<<"Exiting scene..."<<std::endl;

    // Close window
    glutDestroyWindow(windowId);

	delete robot; 
	delete landscape; 
	delete sawModel; 
	delete laserModel; 
	delete present;
    // Exit program
    exit(0);
}

GLvoid setViewport(int width, int height) {
    // Work out window ratio, avoid divide-by-zero
    if(height==0)height=1;
	GLfloat ratio = GLfloat(width)/GLfloat(height);
	GLfloat ratio2 = ratio;
	// Reset projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	// Fill screen with viewport
	glViewport(0, 0, width, height);

	// Set a 45 degree perspective
	gluPerspective(45, ratio, .1, 2000);
}

int main(int argc, char *argv[]){

	srand(time(0));
        
    // Initialise OpenGL
    glutInit(&argc, argv); 

    // Set window position, size & create window
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);
    glutInitWindowPosition(0,0);
    glutInitWindowSize(win_width, win_height);

	windowId = glutCreateWindow("Robot Obstacle Course");
    
    // Set GLUT callback functions
    glutReshapeFunc(setViewport);
	glutFullScreen();
    glutDisplayFunc(renderScene);
    glutIdleFunc(updateScene);
    glutKeyboardFunc(keypress);
	glutKeyboardUpFunc(keyup);

    // Setup OpenGL state & scene resources (models, textures etc)
    setupScene();

    // Show window & start update loop
    glutMainLoop();    

    return 0;
}