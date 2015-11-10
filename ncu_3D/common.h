#ifndef COMMON_H__
#define COMMON_H__
#include "stdafx.h"
extern GLfloat xRot;
extern GLfloat fogcolor[3];
void openfogeffect()
{
	glEnable(GL_FOG);                   // Turn Fog on
	glFogfv(GL_FOG_COLOR, fogcolor);   // Set fog color to match background
	glFogf(GL_FOG_START, 10.0f);         // How far away does the fog start
	glFogf(GL_FOG_END, 500.0f);          // How far away does the fog stop
	glFogi(GL_FOG_MODE, GL_LINEAR);     // Which fog equation do I use?
}

void antialias()
{
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glEnable(GL_POINT_SMOOTH);
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
	glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glEnable(GL_POLYGON_SMOOTH);
	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
}

void DrawEarth(GLuint texture)
{
	glEnable(GL_TEXTURE_2D);	
	glBindTexture(GL_TEXTURE_2D, texture);	
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);

	glPushMatrix();
	glColor3f(1.0f,1.0f,1.0f);		
	glTranslatef(-4.2f, 0.2f, -8.1f);
	//glRotatef(90, 1.0f, 0.0f, 0.0f);
	glRotatef(xRot,0.0,1.0f,0.0f);
	gltDrawSphere(0.5f,21,11);	
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
}

void DrawSky(GLuint texture)
{
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);	
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
	glPushMatrix();
	glColor3f(1.0f,1.0f,1.0f);
	glTranslatef(0.0f, -1.f, 0.0f);
	glRotatef(180, 1.0f, 0.0f, 0.0f);
	gltDrawSphere(50.0f,200,150);
	glPopMatrix();
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_TEXTURE_2D);
}

#endif