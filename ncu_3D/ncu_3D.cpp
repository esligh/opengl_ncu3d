// sphereworld.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "gym.h"
#include "font.h"
#include "texture.h"
#include "common.h"
#include "rc.h"
#define  DEBUG_VIEW_

/** 
	 version: ncu3D version1.0
	 content: one of spots  gym
	 author:   by esli  2013/10/15
	 email:   spacm2013@gmail.com
*/
GLfloat fLightPos[4]   = { -20.0f, 8.0f, -1.0f, 1.0f };  // Point source
GLfloat fNoLight[] = { 0.0f, 0.0f, 0.0f, 0.0f };//light blue
GLfloat fLowLight[] = { 0.25f, 0.25f, 0.25f, 1.0f };
GLfloat fBrightLight[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat fogcolor[] = { 0.25f, 0.25f, 0.25f};

GLTMatrix mShadowMatrix;
cgym gmm; //gym model global instance;
GLTFrame    frameCamera;
GLfloat xRot = 0.0f;
const char *szTextureFiles[] = {"../texture/ground1.tga", "../texture/sky2.tga", "../texture/wall1.tga","../texture/earth.tga"};
const char* treetexture[]={
"../texture/tree/tree1.bmp","../texture/tree/tree2.bmp","../texture/tree/tree3.bmp","../texture/tree/tree4.bmp","../texture/tree/tree5.bmp",
"../texture/tree/tree6.bmp","../texture/tree/tree7.bmp","../texture/tree/tree8.bmp","../texture/tree/tree9.bmp","../texture/tree/tree10.bmp",
"../texture/tree/tree11.bmp","../texture/tree/tree12.bmp","../texture/grass1.bmp"};
const char* walltexture[]={"../texture/wall/wall1.tga","../texture/wall/wall2.tga","../texture/wall/wall3.tga","../texture/wall/wall4.tga","../texture/door.tga","../texture/window.tga","../texture/wall/wall6.tga","../texture/wall/wall7.tga","../texture/grass2.tga"};
const char* lamptexture[]={"../texture/lamp2.bmp","../texture/grass3.bmp","../texture/grass4.bmp"};
const char* floortexture[]={"../texture/floor1.tga","../texture/floor2.tga"};
ctexture tex;

void SetupRC()
{	
	tex.load_tga_textures(szTextureFiles,4);
	tex.load_transp_textures(treetexture,13);
	tex.load_tga_textures(walltexture,9);
	tex.load_transp_textures(lamptexture,3);
	tex.load_tga_textures(floortexture,2);
	//open fog effect
	openfogeffect();
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); 	// Draw everything as wire frame
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW); //anti clockwise
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	// Setup and enable light 0
	// Enable lighting
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, fNoLight);
	glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);
	glLightfv(GL_LIGHT0,GL_AMBIENT,fLowLight);
	glLightfv(GL_LIGHT0,GL_DIFFUSE,fBrightLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, fBrightLight);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	
	glLightfv(GL_LIGHT0,GL_POSITION,fLightPos);
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT,GL_AMBIENT_AND_DIFFUSE);
//	glMaterialfv(GL_FRONT,GL_SPECULAR,specref);
//	glMateriali(GL_FRONT,GL_SHININESS,128);
	gltInitFrame(&frameCamera);  // Initialize the camera

	GLTVector3 points[3] = {{ 0.0f, -0.5f, 0.0f },
	{ 10.0f, -0.5f, 0.0f },
	{ 5.0f, -0.5f, -5.0f }};
	gltMakeShadowMatrix(points, fLightPos, mShadowMatrix);
	//glClearColor(fLowLight[0],fLowLight[1],fLowLight[2], fLowLight[3] );	
}

// Called to draw scene
void RenderScene(void)
{
	static GLfloat yRot = 0.0f;         // Rotation angle for animation
	yRot += 0.5f;
	// Clear the window with current clearing color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glPushMatrix();
		gltApplyCameraTransform(&frameCamera);
		glTranslatef(0.0f, 1.4f, -2.5f); //view initial position
		glScalef(4.0f,4.0f,4.0f);  //enlarge
		glLightfv(GL_LIGHT0,GL_POSITION,fLightPos);	
		
		DrawSky(tex.get_texture(SKY_TEXTURE));		
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, tex.get_texture(GROUND_TEXTURE));
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glColor3f(0.9f,0.9f,0.9f);
		gmm.DrawGround();
		/*
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_2D);		
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glPushMatrix();		
			glMultMatrixf(mShadowMatrix);
			gmm.DrawGym(1);
		glPopMatrix();
		glDisable(GL_BLEND);
		glEnable(GL_LIGHTING);
		glEnable(GL_DEPTH_TEST);
		*/
		gmm.DrawGym(0);		
		DrawEarth(tex.get_texture(EARTH_TEXTURE));
	//debug show
/*
		glPushMatrix();
			glTranslatef(fLightPos[0],fLightPos[1], fLightPos[2]);
			glColor3ub(240,240,0);
			glutSolidSphere(0.6f,10,10);
		glPopMatrix();	
*/
	glPopMatrix();		
	glutSwapBuffers();
	glutPostRedisplay();
}

// Respond to arrow keys by moving the camera frame of reference
void SpecialKeys(int key, int x, int y)
{
	if(key == GLUT_KEY_UP)
		gltMoveFrameForward(&frameCamera, 0.2f);

	if(key == GLUT_KEY_DOWN)
		gltMoveFrameForward(&frameCamera, -0.2f);

	if(key == GLUT_KEY_LEFT)
		gltRotateFrameLocalY(&frameCamera, 0.2);

	if(key == GLUT_KEY_RIGHT)
		gltRotateFrameLocalY(&frameCamera, -0.2);
	// Refresh the Window
	glutPostRedisplay();
}


///////////////////////////////////////////////////////////
// Called by GLUT library when idle (window not being
// resized or moved)
void TimerFunction(int value)
{
	// Redraw the scene with new coordinates
	xRot+=0.5f;
	if(xRot>=360)
		xRot = 0.0f;
	glutPostRedisplay();
	glutTimerFunc(100,TimerFunction, 1);
}

void ChangeSize(int w, int h)
{
	GLfloat fAspect;
	if(h == 0)
		h = 1;
	glViewport(0, 0, w, h);
	fAspect = (GLfloat)w / (GLfloat)h;
	// Reset the coordinate system before modifying
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	// Set the clipping volume
	gluPerspective(45.0f, fAspect, 1.0f, 500.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glLightfv(GL_LIGHT0,GL_POSITION,fLightPos);
}

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(1366,768);
	glutCreateWindow("opengl ncu-gym-v1.0 by esli");
	gmm.InitgymEnv();
	gmm.BindTexture(&tex);
	glutReshapeFunc(ChangeSize);
	glutDisplayFunc(RenderScene);
	glutSpecialFunc(SpecialKeys);
	SetupRC();
	glutTimerFunc(100, TimerFunction, 1);
	glutMainLoop();
	return 0;
}