#include "stdafx.h"
#include "drawmatobj.h"

CDrawMatObj::CDrawMatObj()
{}
CDrawMatObj::~CDrawMatObj()
{}
/**
	annotation: auxiliary method
	desc: draw a tree but no texture, actually it is a cross of two squares .In subsequent work,mapping transparent texture on it.
	params: center  - the position of the tree
				 w			- width of the tree
				 h		    - height of the tree
*/
void CDrawMatObj::drawtree(util::Point center,GLfloat w,GLfloat h)
{	
	GLfloat height = h;
	GLfloat xpos=center.x,ypos=center.y,zpos=center.z;
	GLfloat width=w;
	glColor3f(1.0f,1.0f,1.0f);
	glBegin(GL_QUADS);
		glNormal3f( 1.0f, 1.0f, 1.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(xpos+width,ypos-height, zpos);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(xpos-width,ypos-height,zpos);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(xpos-width,ypos+height,zpos);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(xpos+width,ypos+height,zpos);
	glEnd();	
	glBegin(GL_QUADS);	
		glNormal3f( 1.0f, 1.0f, 1.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(xpos,ypos-height, zpos-width);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(xpos,ypos-height,zpos+width);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(xpos,ypos+height,zpos+width);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(xpos,ypos+height,zpos-width);
	glEnd();
}