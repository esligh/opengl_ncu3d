#include "stdafx.h"
#include "tools.h"
using namespace  util;
/**
according to the parameter equation of circle:
assume p{a,b,c} is the center of circle,we can get the conclusion that:
x = a+cost  0<=t<=2pi
y = b+sint
z=0 default zero,no support for 3d space now.
*/
void shape::drawarc(Point center,double radius,Point vector,double start_angle,double end_angle,double delta_angle)
{
	glBegin(GL_LINE_STRIP);
		for(double i = start_angle;i<=end_angle;i+=delta_angle){
			double x = center.x+radius*cos(i);
			double z = center.z+radius*sin(i);
			glVertex3f(x,center.y,z);
		}
	glEnd();
}

void shape::drawcircle(Point center,double radius,Point vector,double delta_angle)
{
	drawarc(center,radius,vector,0,2*util::PI,delta_angle);
}

void shape::drawpie(Point center,double radius,Point vector,double start_angle,double end_angle,double delta_angle)
{

}

void render::circle_render(Point center,GLfloat radius,GLfloat delta_angle,GLuint texture)
{
	
	glEnable(GL_TEXTURE_2D);
	glBindTexture( GL_TEXTURE_2D, texture);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
	GLfloat texcoord[2];
	GLfloat vertex[3];
	glTexCoord2f(0.5f,0.5f);glVertex3f(center.x,center.y,center.z);
	GLfloat end = 2*PI;
	glBegin(GL_TRIANGLE_FAN);
	glColor3f(1.0f,1.0f,1.0f);
	for(GLfloat i =0;i<end;i+=delta_angle){
		texcoord[0] = (cos(i) + 1.0);  
		texcoord[1] = (sin(i) + 1.0);  
		glTexCoord2fv(texcoord);
		vertex[0] = cos(i) * radius+center.x;  
		vertex[1] = center.y;
		vertex[2] = sin(i) * radius+center.z;
		glVertex3fv(vertex);
	}
	glEnd();
	glDisable(GL_TEXTURE_2D);
}
