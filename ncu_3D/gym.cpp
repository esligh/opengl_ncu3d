#include "stdafx.h"
#include "gym.h"
#include "font.h"
#include "tools.h"
#include "drawmatobj.h"
#include "rc.h"

using namespace  util;

#define  PI   3.141516926f
#define  M    200  

#define  DRAW_GROUND_WITH_LINE

const GLint cgym::BEZIER_CURVE_N=100;
const GLint cgym::nNumPoints=4;
const GLfloat cgym::defaut_grid_step_up=0.125f;
const GLfloat cgym::default_level = -0.5f;

//bezier control points
const GLfloat cgym::default_up_ctrlPoints[4][3]= {{ -3.0f, 1.0f, 0.0f},
{ -0.275f, 2.25f, 0.0f},
{  1.15f, 0.0f, 0.0f},	
{  2.90f, 0.90f, 0.0f }};
const GLfloat cgym::default_innerup_ctrlPoints[4][3]={{ -2.65f, 0.8f, 0.0f},	
{ -0.275f, 1.85f, 0.0f},
{  1.15f, -0.2f, 0.0f},	
{  2.6f, 0.6f, 0.0f }};	
const GLfloat cgym::default_down_ctrlPoints[4][3]={{ -3.0f, -0.5f, 0.0f},	
{ 0.35f, 0.65f, 0.0f},	
{  1.05f, -1.6f, 0.0f},	
{  3.0f, -0.5f, 0.0f }};	
const GLfloat cgym::default_innerdown_ctrlPoints[4][3]={{ -2.3f, -0.25f, 0.0f},
{ 0.5f, 0.75f, 0.0f},	
{  1.15f, -1.2f, 0.0f},	
{  2.2f, -0.5f, 0.0f }};	

const GLint cgym::upspot_tbl[24]={
	18,22,22,26,
	46,50,50,54,
	76,80,80,84,
	106,110,110,114,
	136,140,140,144,
	164,168,168,172
};
const GLint cgym::downspot_tbl[24]={
	0,3,
	20,24,24,28,
	46,50,50,54,
	76,81,81,86,
	108,114,114,120,
	150,157,157,164,
	194,199
};

cgym::cgym()
:ud_grid_step(defaut_grid_step_up),
horizon_level(default_level),
dip_angle(60.0f),
innerdip_angle(45.0f),
top_vheight(0.1f),
innertop_vheight(0.2f),
innertop_hwidth(0.32f),
texture(NULL)
{
	memcpy(up_ctrlPoints,default_up_ctrlPoints,sizeof(up_ctrlPoints));
	memcpy(down_ctrlPoints,default_down_ctrlPoints,sizeof(down_ctrlPoints));
	memcpy(innerup_ctrlPoints,default_innerup_ctrlPoints,sizeof(innerup_ctrlPoints));
	memcpy(innerdown_ctrlPoints,default_innerdown_ctrlPoints,sizeof(innerdown_ctrlPoints));	
	shadowcolor[0]=shadowcolor[1]=shadowcolor[2]=0.1f;
	//correct control points
	GLfloat n1 = up_ctrlPoints[0][1]/tan((PI/180.0f)*dip_angle);
	GLfloat xPos1 =up_ctrlPoints[0][0]+n1;
	GLfloat downleft[3]={xPos1,(horizon_level-0.1f),0};

	GLfloat n2 = up_ctrlPoints[3][1]/tan((PI/180.0f)*dip_angle);
	GLfloat xPos2 =up_ctrlPoints[3][0]-n2;
	GLfloat downright[3]={xPos2,-0.6f,0.0f};
	memcpy(down_ctrlPoints[0],downleft,sizeof(downleft));
	memcpy(down_ctrlPoints[3],downright,sizeof(downright));
	InitBezierParam(); //init bezier curve params
}
cgym::~cgym()
{}
void cgym::BindTexture(ctexture* t)
{
	texture = t;
}
void cgym::DrawGym(int nShadow)
{
	shadow = nShadow;//1 shadow 0 no shadow
	//DrawUpGrid();
	//DrawRightDownGrid(2);
	//DrawLeftDownGrid(2);
	//DrawBottomGrid();
	DrawUpFace();
	DrawRightUpFace();
	DrawRightDownFace();
	DrawLeftDownFace();
	DrawLeftUpFace();

	//DrawRightInnerUpGrid(2);
	//DrawLeftInnerUpGrid(2);

	//DrawRightInnerDownGrid(2);
	//DrawLeftInnerDownGrid(2);
	DrawFrontFace();
	DrawBackLine();
	DrawBackFace();
	DrawStripLine();

	DrawFrontScence();
	DrawRMirror();
	DrawLMirror();
	DrawRBMace();
	DrawLBMace();
	DrawFrontDoor();
	DrawRightFace();
	DrawLeftFace();
	DrawRightDoor();
	DrawLeftDoor();
	DrawSurround();//last work
}


/**
	Init the enviroment before drawing gym ,and parpare any stuffs that maybe you need.
*/
void cgym::InitgymEnv()
{	
//	font config

//	font.setFace("Á¥Êé");
//	font.setfontweight(FW_ULTRALIGHT);/*FW_THIN ,FW_LIGHT,FW_NORMAL,FW_MEDIUM,FW_BOLD */
	font.selectFont();
	font.prepareCNString("Ìå  Óý  ¹Ý");
//others

}

void cgym::DrawName(GLfloat pos[3])
{
	glPushMatrix();
		glTranslatef(pos[0],pos[1],pos[2]);
		glRotated(-90, 0.0,1.0,0.0);
		glScaled(0.2f, 0.2f, 0.1f);
		if(shadow)
			glColor3fv(shadowcolor);
		else
			glColor3f(0.42f,0.35,0.035);	//luxury gold color 211,168,9
		font.drawCNString();
		glFrontFace(GL_CCW); //anti clockwise
	glPopMatrix();
}
/**
	this function generates amount of vertexs on the bezier curve
	 by using  the base of bezier function.after this,we can use
	 these vertexs draw the fold line, and also render in the later work.
*/
void cgym::InitBezierParam()
{
	//up
	up_spot[0][0]=up_ctrlPoints[0][0];
	up_spot[0][1]=up_ctrlPoints[0][1];
	up_spot[0][2]=up_ctrlPoints[0][2];
	for(int i=1;i<M;i++){
		GLfloat t =(GLfloat)(i+1)/M;
		GLfloat x = bezier_formula_x(up_ctrlPoints,t);
		GLfloat y = bezier_formula_y(up_ctrlPoints,t);
		GLfloat z = 0; 
		up_spot[i][0]=x;
		up_spot[i][1]=y;
		up_spot[i][2]=z;
	}

	//down 
	down_spot[0][0]=down_ctrlPoints[0][0];
	down_spot[0][1]=down_ctrlPoints[0][1];
	down_spot[0][2]=down_ctrlPoints[0][2];
	for(int i=1;i<M;i++){
		GLfloat t =(GLfloat)(i+1)/M;
		GLfloat x = bezier_formula_x(down_ctrlPoints,t);
		GLfloat y = bezier_formula_y(down_ctrlPoints,t);
		GLfloat z = 0; 
		down_spot[i][0]=x;
		down_spot[i][1]=y;
		down_spot[i][2]=z;
	}

	//inner down
	innerdown_spot[0][0]=innerdown_ctrlPoints[0][0];
	innerdown_spot[0][1]=innerdown_ctrlPoints[0][1];
	innerdown_spot[0][2]=innerdown_ctrlPoints[0][2];
	for(int i=1;i<M;i++){
		GLfloat t =(GLfloat)(i+1)/M;
		GLfloat x = bezier_formula_x(innerdown_ctrlPoints,t);
		GLfloat y = bezier_formula_y(innerdown_ctrlPoints,t);
		GLfloat z = 0; 
		innerdown_spot[i][0]=x;
		innerdown_spot[i][1]=y;
		innerdown_spot[i][2]=z;
	}

	//inner up
	innerup_spot[0][0]=innerup_ctrlPoints[0][0];
	innerup_spot[0][1]=innerup_ctrlPoints[0][1];
	innerup_spot[0][2]=innerup_ctrlPoints[0][2];
	for(int i=1;i<M;i++){
		GLfloat t =(GLfloat)(i+1)/M;
		GLfloat x = bezier_formula_x(innerup_ctrlPoints,t);
		GLfloat y = bezier_formula_y(innerup_ctrlPoints,t);
		GLfloat z = 0; 
		innerup_spot[i][0]=x;
		innerup_spot[i][1]=y;
		innerup_spot[i][2]=z;
	}
}

void cgym::DrawUpGrid()
{
	GLfloat temp[4][3];
	memcpy(temp,up_ctrlPoints,sizeof(up_ctrlPoints));
	for(int n=0;n<GRID_SIZE;n++){
		DrawBezierCurve(temp);
		for(int i =0 ;i<4;i++){
			temp[i][2]-=ud_grid_step;
		}
	}
}

void cgym::DrawRightDownGrid(int N)
{
	GLfloat temp[4][3];
	memcpy(temp,down_ctrlPoints,sizeof(down_ctrlPoints));
	for(int n=0;n<N;n++){
		DrawBezierCurve(temp); 
		for(int i =0 ;i<4;i++){
			temp[i][2]-=ud_grid_step;
		}	
	}
}

//draw curve face
void cgym::DrawUpFace()
{
	glDisable(GL_TEXTURE_2D);
	if(shadow)
		glColor3fv(shadowcolor);
	else
		glColor3f(0.68f,0.68f,0.68f);	
	GLfloat d = ud_grid_step*(GRID_SIZE-1);
	for(int i=0;i<M-1;i++){
		glBegin(GL_QUADS);	
		glVertex3f(up_spot[i][0],up_spot[i][1],up_spot[i][2]-d);
		glVertex3fv(up_spot[i]);	
		glVertex3fv(up_spot[i+1]);
		glVertex3f(up_spot[i+1][0],up_spot[i+1][1],up_spot[i+1][2]-d);	
		glEnd();
	}
}
void cgym::DrawRightUpFace()
{
	if(shadow)
		glColor3fv(shadowcolor);
	else
		glColor3f(1.0f,1.0f,1.0f);	
	glDepthFunc(GL_LEQUAL);							
	glEnable(GL_DEPTH_TEST);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glBindTexture( GL_TEXTURE_2D, texture->get_texture(WALL3_TEXTURE));
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND); 
	GLfloat et = 1.0f/(M*0.4f);
	GLfloat s=0.0f,t=0.0f;
	for(int i=0;i<M-1;i++){
		glBegin(GL_QUADS);	
			glNormal3f(0.0f,0.0f,1.0f);
			glTexCoord2f(s,0.0f);glVertex3fv(up_spot[i]);
			glTexCoord2f(s,0.25f);glVertex3fv(innerup_spot[i]);
			glTexCoord2f(s+et,0.25f);glVertex3fv(innerup_spot[i+1]);
			glTexCoord2f(s+et,0.0f);glVertex3fv(up_spot[i+1]);	
		glEnd();
		s+=et;
	}
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);

	if(shadow)
		glColor3fv(shadowcolor);
	else
		glColor3f(0.51f,0.51f,0.50f);	
	for(int i=0;i<M-1;i++){
		glBegin(GL_QUADS);	
			glNormal3f(0.0f,-1.0f,0.0f);
			glVertex3fv(innerup_spot[i]);
			glVertex3f(innerup_spot[i][0],innerup_spot[i][1],innerup_spot[i][2]-ud_grid_step);
			glVertex3f(innerup_spot[i+1][0],innerup_spot[i+1][1],innerup_spot[i+1][2]-ud_grid_step);
			glVertex3fv(innerup_spot[i+1]);			
		glEnd();
	}
}

void cgym::DrawRightDownFace()
{	
	if(shadow)
		glColor3fv(shadowcolor);
	else
		glColor3f(1.0f,1.0f,1.0f);	

	GLfloat n = up_ctrlPoints[0][1]/tan((PI/180.0f)*dip_angle);
	GLfloat xPos =up_ctrlPoints[0][0]+n;
	GLfloat down[3]={xPos,(horizon_level),0};
	glDepthFunc(GL_LEQUAL);							
	glEnable(GL_DEPTH_TEST);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glBindTexture( GL_TEXTURE_2D, texture->get_texture(WALL3_TEXTURE));
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND); 
	GLfloat et = 1.0f/(M*0.4f);
	GLfloat s=0.0f,t=0.0f;
	glBegin(GL_TRIANGLE_STRIP);
		glNormal3f(0.0f,0.0f,1.0f);
		glVertex3fv(innerdown_spot[0]);
		glVertex3fv(down);
		glVertex3fv(down_ctrlPoints[0]);		
	glEnd();
	for(int i=0;i<M-1;i++){
		glBegin(GL_QUADS);
		glNormal3f(0.0f,0.0f,1.0f);
		glTexCoord2f(s,0.0f);glVertex3fv(innerdown_spot[i]);
		glTexCoord2f(s,0.25f);glVertex3fv(down_spot[i]);
		glTexCoord2f(s+et,0.25f);glVertex3fv(down_spot[i+1]);
		glTexCoord2f(s+et,0.0f);glVertex3fv(innerdown_spot[i+1]);
		glEnd();
		s+=et;
	}

	if(shadow)
		glColor3fv(shadowcolor);
	else
		glColor3f(0.51f,0.51f,0.50f);	
	for(int i=0;i<M-1;i++){
		glBegin(GL_QUADS);
		glNormal3f(0.0f,-1.0f,0.0f);
		glVertex3fv(down_spot[i]);
		glVertex3f(down_spot[i][0],down_spot[i][1],down_spot[i][2]-ud_grid_step);
		glVertex3f(down_spot[i+1][0],down_spot[i+1][1],down_spot[i+1][2]-ud_grid_step);
		glVertex3fv(down_spot[i+1]);	
		glEnd();
	}
	
	for(int i=0;i<M-1;i++){
		glBegin(GL_QUADS);
		glNormal3f(0.0f,1.0f,0.0f);
		glVertex3fv(innerdown_spot[i]);
		glVertex3fv(innerdown_spot[i+1]);
		glVertex3f(innerdown_spot[i+1][0],innerdown_spot[i+1][1],innerdown_spot[i+1][2]-ud_grid_step);
		glVertex3f(innerdown_spot[i][0],innerdown_spot[i][1],innerdown_spot[i][2]-ud_grid_step);		
		glEnd();
	}	
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
}

void cgym::DrawLeftUpFace()
{
	if(shadow)
		glColor3fv(shadowcolor);
	else
		glColor3f(1.f,1.0f,1.0f);	
	GLfloat d = ud_grid_step*(GRID_SIZE-1);
	glDepthFunc(GL_LEQUAL);							
	glEnable(GL_DEPTH_TEST);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glBindTexture( GL_TEXTURE_2D, texture->get_texture(WALL3_TEXTURE));
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND); 
	GLfloat et = 1.0f/(M*0.4f);
	GLfloat s=0.0f,t=0.0f;
	for(int i=0;i<M-1;i++){
		glBegin(GL_QUADS);	
		glNormal3f(0.0f,0.0f,-1.0f);
		glTexCoord2f(s,0.0f);glVertex3f(up_spot[i][0],up_spot[i][1],up_spot[i][2]-d);
		glTexCoord2f(s+et,0.0f);glVertex3f(up_spot[i+1][0],up_spot[i+1][1],up_spot[i+1][2]-d);
		glTexCoord2f(s+et,0.25f);glVertex3f(innerup_spot[i+1][0],innerup_spot[i+1][1],innerup_spot[i+1][2]-d);	
		glTexCoord2f(s,0.25f);glVertex3f(innerup_spot[i][0],innerup_spot[i][1],innerup_spot[i][2]-d);
		glEnd();
		s+=et;
	}
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	if(shadow)
		glColor3fv(shadowcolor);
	else
		glColor3f(0.51f,0.51f,0.50f);	
	for(int i=0;i<M-1;i++){
		glBegin(GL_QUADS);	
		glNormal3f(0.0f,-1.0f,0.0f);
		glVertex3f(innerup_spot[i][0],innerup_spot[i][1],innerup_spot[i][2]-d+ud_grid_step);
		glVertex3f(innerup_spot[i][0],innerup_spot[i][1],innerup_spot[i][2]-d);
		glVertex3f(innerup_spot[i+1][0],innerup_spot[i+1][1],innerup_spot[i+1][2]-d);			
		glVertex3f(innerup_spot[i+1][0],innerup_spot[i+1][1],innerup_spot[i+1][2]-d+ud_grid_step);		
		glEnd();
	}
	
}

void cgym::DrawLeftDownFace()
{
	if(shadow)
		glColor3fv(shadowcolor);
	else
		glColor3f(1.0f,1.0f,1.0f);	
	GLfloat d = ud_grid_step*(GRID_SIZE-1);

	GLfloat n = up_ctrlPoints[0][1]/tan((PI/180.0f)*dip_angle);
	GLfloat xPos =up_ctrlPoints[0][0]+n;
	GLfloat down[3]={xPos,(horizon_level),-d};
	glDepthFunc(GL_LEQUAL);							
	glEnable(GL_DEPTH_TEST);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glBindTexture( GL_TEXTURE_2D, texture->get_texture(WALL3_TEXTURE));
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND); 
	glBegin(GL_TRIANGLE_STRIP);
		glNormal3f(0.0f,0.0f,-1.0f);
		glVertex3f(innerdown_spot[0][0],innerdown_spot[0][1],innerdown_spot[0][2]-d);		
		glVertex3f(down_ctrlPoints[0][0],down_ctrlPoints[0][1],down_ctrlPoints[0][2]-d);		
		glVertex3fv(down);
	glEnd();
	GLfloat et = 1.0f/(M*0.4);
	GLfloat s=0.0f,t=0.0f;
	for(int i=0;i<M-1;i++){
		glBegin(GL_QUADS);
		glNormal3f(0.0f,0.0f,-1.0f);
		glTexCoord2f(s,0.0f);glVertex3f(innerdown_spot[i][0],innerdown_spot[i][1],innerdown_spot[i][2]-d);
		glTexCoord2f(s+et,0.0f);glVertex3f(innerdown_spot[i+1][0],innerdown_spot[i+1][1],innerdown_spot[i+1][2]-d);
		glTexCoord2f(s+et,0.25f);glVertex3f(down_spot[i+1][0],down_spot[i+1][1],down_spot[i+1][2]-d);
		glTexCoord2f(s,0.25f);glVertex3f(down_spot[i][0],down_spot[i][1],down_spot[i][2]-d);
		glEnd();
		s+=et;
	}
	if(shadow)
		glColor3fv(shadowcolor);
	else
		glColor3f(0.51f,0.51f,0.50f);	
	for(int i=0;i<M-1;i++){
		glBegin(GL_QUADS);
			glNormal3f(0.0f,-1.0f,0.0f);
			glVertex3f(down_spot[i][0],down_spot[i][1],down_spot[i][2]-d+ud_grid_step);
			glVertex3f(down_spot[i][0],down_spot[i][1],down_spot[i][2]-d);
			glVertex3f(down_spot[i+1][0],down_spot[i+1][1],down_spot[i+1][2]-d);	
			glVertex3f(down_spot[i+1][0],down_spot[i+1][1],down_spot[i+1][2]-d+ud_grid_step);	
		glEnd();
	}
	for(int i=0;i<M-1;i++){
		glBegin(GL_QUADS);
			glNormal3f(0.0f,1.0f,0.0f);
			glVertex3f(innerdown_spot[i][0],innerdown_spot[i][1],innerdown_spot[i][2]-d+ud_grid_step);		
			glVertex3f(innerdown_spot[i+1][0],innerdown_spot[i+1][1],innerdown_spot[i+1][2]-d+ud_grid_step);
			glVertex3f(innerdown_spot[i+1][0],innerdown_spot[i+1][1],innerdown_spot[i+1][2]-d);
			glVertex3f(innerdown_spot[i][0],innerdown_spot[i][1],innerdown_spot[i][2]-d);			
		glEnd();
	}
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND); 
}


void cgym::DrawLeftDownGrid(int N)
{
	GLfloat temp[4][3];
	memcpy(temp,down_ctrlPoints,sizeof(down_ctrlPoints));
	for(int i =0 ;i<4;i++){
		temp[i][2]-=(ud_grid_step*(GRID_SIZE-1));
	}
	for(int n=0;n<N;n++){
		DrawBezierCurve(temp); 
		for(int i =0 ;i<4;i++){
			temp[i][2]+=ud_grid_step;
		}		
	}
}

void cgym::DrawRightInnerDownGrid(int N)
{
	GLfloat temp[4][3];
	memcpy(temp,innerdown_ctrlPoints,sizeof(innerdown_ctrlPoints));
	for(int n=0;n<N;n++){		
		DrawBezierCurve(temp); //keep with DrawRightFace
		for(int i =0 ;i<4;i++){
			temp[i][2]-=ud_grid_step;
		}	
	}	
}
void cgym::DrawRightInnerUpGrid(int N)
{
	GLfloat temp[4][3];
	memcpy(temp,innerup_ctrlPoints,sizeof(innerup_ctrlPoints));
	for(int n=0;n<N;n++){		
		DrawBezierCurve(temp); //keep with DrawRightFace
		for(int i =0 ;i<4;i++){
			temp[i][2]-=ud_grid_step;
		}
	}	
}

void cgym::DrawLeftInnerDownGrid(int N)
{
	GLfloat temp[4][3];
	memcpy(temp,innerdown_ctrlPoints,sizeof(innerdown_ctrlPoints));
	for(int i=0;i<4;i++){
		temp[i][2]-=(ud_grid_step*(GRID_SIZE-1));
	}
	for(int n=0;n<N;n++){		
		DrawBezierCurve(temp); //keep with DrawRightFace
		for(int i =0 ;i<4;i++){
			temp[i][2]+=ud_grid_step;
		}
	}
}
void cgym::DrawLeftInnerUpGrid(int N)
{
	GLfloat temp[4][3];
	memcpy(temp,innerup_ctrlPoints,sizeof(innerup_ctrlPoints));
	for(int i=0;i<4;i++){
		temp[i][2]-=(ud_grid_step*(GRID_SIZE-1));
	}
	for(int n=0;n<N;n++){		
		DrawBezierCurve(temp); //keep with DrawRightFace
		for(int i =0 ;i<4;i++){
			temp[i][2]+=ud_grid_step;
		}
	}
}
///////////////////////////////////////////////////////////////////////////////////////
void cgym::DrawBezierCurve(GLfloat  temp[4][3])
{
	glMap1f(GL_MAP1_VERTEX_3,	// Type of data generated
		0.0f,						// Lower u range
		BEZIER_CURVE_N,						// Upper u range
		3,							// Distance between points in the data
		nNumPoints,					// number of control points
		&temp[0][0]);			// array of control points
	glEnable(GL_MAP1_VERTEX_3);
	glBegin(GL_LINE_STRIP);
	for(int i = 0; i <= BEZIER_CURVE_N; i++)
	{
		// Evaluate the curve at this point
		glEvalCoord1f((GLfloat) i); 
	}
	glEnd();
}
void cgym::DrawRightFace(int N)
{
	if(shadow)
		glColor3fv(shadowcolor);
	else
		glColor3f(0.48f,0.48f,0.48f);

	glBegin(GL_QUADS);
		glNormal3f(1.0f,0.0f,0.0f);
		glVertex3f(innerup_ctrlPoints[0][0],innerup_ctrlPoints[0][1],innerup_ctrlPoints[0][2]);
		glVertex3f(innerdown_ctrlPoints[0][0],innerdown_ctrlPoints[0][1],innerdown_ctrlPoints[0][2]);
		glVertex3f(innerdown_ctrlPoints[0][0],innerdown_ctrlPoints[0][1],innerdown_ctrlPoints[0][2]-ud_grid_step);
		glVertex3f(innerup_ctrlPoints[0][0],innerup_ctrlPoints[0][1],innerup_ctrlPoints[0][2]-ud_grid_step);		
	glEnd();
	
	glBegin(GL_QUADS);
		glNormal3f(-1.0f,0.0f,0.0f);
		glVertex3f(innerdown_ctrlPoints[3][0],innerdown_ctrlPoints[3][1],innerdown_ctrlPoints[3][2]);
		glVertex3f(innerup_ctrlPoints[3][0],innerup_ctrlPoints[3][1],innerup_ctrlPoints[3][2]);
		glVertex3f(innerup_ctrlPoints[3][0],innerup_ctrlPoints[3][1],innerup_ctrlPoints[3][2]-ud_grid_step);		
		glVertex3f(innerdown_ctrlPoints[3][0],innerdown_ctrlPoints[3][1],innerdown_ctrlPoints[3][2]-ud_grid_step);		
	glEnd();

	GLfloat d = ud_grid_step ;	
	for(int i=0;i<24;){
		int upidx1 = upspot_tbl[i];
		int downidx1 = downspot_tbl[i];
		int upidx2 = upspot_tbl[i+1];
		int downidx2= downspot_tbl[i+1];
		DrawPillar(innerup_spot[upidx1],innerup_spot[upidx2],innerdown_spot[downidx1],innerdown_spot[downidx2],0);
		int upidx3 = upspot_tbl[i+2];
		int downidx3= downspot_tbl[i+2];
		int upidx4 = upspot_tbl[i+3];
		int downidx4= downspot_tbl[i+3];
		DrawPillar(innerup_spot[upidx3],innerup_spot[upidx4],innerdown_spot[downidx3],innerdown_spot[downidx4],0);
		i+=4;
	}
//	glColor3f(0.4f,0.6f,0.1f); //green
}
//
void cgym::DrawPillar(GLfloat innerup1[3],GLfloat innerup2[3],GLfloat innerdown1[3],GLfloat innerdown2[3],GLint flag)
{
	if(shadow)
		glColor3fv(shadowcolor);
	else
		glColor3f(1.0f,1.0f,1.0f);
	glDepthFunc(GL_LEQUAL);							
	glEnable(GL_DEPTH_TEST);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glBindTexture( GL_TEXTURE_2D, texture->get_texture(WALL3_TEXTURE));
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND); 

	GLfloat d1,d2;
	GLfloat light_d = 0.0f;
	GLfloat vNormal[3];
	GLfloat temp[3][3];
	if(flag == 0){
		 d1 = 0;
		 d2= ud_grid_step;
		 light_d = 1.0f;
	}
	else if(flag == 1){
		 d1 = ud_grid_step*(GRID_SIZE-2);
		 d2 = d1+ud_grid_step;
		 light_d = -1.0f;
	}
	//front
	glBegin(GL_QUADS);
		glNormal3f(0.0f,0.0f,light_d);
		glTexCoord2f(0.0f,0.0f);glVertex3f(innerdown1[0],innerdown1[1],innerdown1[2]-d1);
		glTexCoord2f(0.1f,0.0f);glVertex3f(innerdown2[0],innerdown2[1],innerdown2[2]-d1);
		glTexCoord2f(0.1f,1.0f);glVertex3f(innerup2[0],innerup2[1],innerup2[2]-d1);
		glTexCoord2f(0.0f,1.0f);glVertex3f(innerup1[0],innerup1[1],innerup1[2]-d1);						
	glEnd();
	//back
	glBegin(GL_QUADS);
		glNormal3f(0.0f,0.0f,-light_d);
		glTexCoord2f(0.0f,0.0f);glVertex3f(innerdown1[0],innerdown1[1],innerdown1[2]-d2);
		glTexCoord2f(0.0f,1.0f);glVertex3f(innerup1[0],innerup1[1],innerup1[2]-d2);						
		glTexCoord2f(0.1f,1.0f);glVertex3f(innerup2[0],innerup2[1],innerup2[2]-d2);		
		glTexCoord2f(0.1f,0.0f);glVertex3f(innerdown2[0],innerdown2[1],innerdown2[2]-d2);		
	glEnd();

	if(shadow)
		glColor3fv(shadowcolor);
	else
		glColor3f(0.5f,0.5f,0.5f);
	//left
	glBegin(GL_QUADS);	
		temp[0][0]=innerdown1[0];temp[0][1]=innerdown1[1];temp[0][2]=innerdown1[2]-d1;
		temp[1][0]=innerup1[0];temp[1][1]=innerup1[1];temp[1][2]=innerup1[2]-d1;
		temp[2][0]=innerup1[0];temp[2][1]=innerup1[1];temp[2][2]=innerup1[2]-d2;
		gltGetNormalVector(temp[0], temp[1], temp[2], vNormal);
		glNormal3fv(vNormal);
		glVertex3f(innerdown1[0],innerdown1[1],innerdown1[2]-d1);
		glVertex3f(innerup1[0],innerup1[1],innerup1[2]-d1);
		glVertex3f(innerup1[0],innerup1[1],innerup1[2]-d2);
		glVertex3f(innerdown1[0],innerdown1[1],innerdown1[2]-d2);								
	glEnd();
	//right
	glBegin(GL_QUADS);
		temp[0][0]=innerdown2[0];temp[0][1]=innerdown2[1];temp[0][2]=innerdown2[2]-d1;
		temp[1][0]=innerup2[0];temp[1][1]=innerup2[1];temp[1][2]=innerup2[2]-d1;
		temp[2][0]=innerup2[0];temp[2][1]=innerup2[1];temp[2][2]=innerup2[2]-d2;
		gltGetNormalVector(temp[0], temp[1], temp[2], vNormal);
		glNormal3fv(vNormal);

		glVertex3f(innerdown2[0],innerdown2[1],innerdown2[2]-d1);
		glVertex3f(innerdown2[0],innerdown2[1],innerdown2[2]-d2);								
		glVertex3f(innerup2[0],innerup2[1],innerup2[2]-d2);
		glVertex3f(innerup2[0],innerup2[1],innerup2[2]-d1);		
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
}

void cgym::DrawLeftFace(int N )
{
	if(shadow)
		glColor3fv(shadowcolor);
	else
		glColor3f(0.48f,0.48f,0.48f);
	GLfloat d = ud_grid_step*(GRID_SIZE-1);
	glBegin(GL_QUADS);
		glNormal3f(1.0f,0.0f,0.0f);
		glVertex3f(innerup_ctrlPoints[0][0],innerup_ctrlPoints[0][1],innerup_ctrlPoints[0][2]-d+ud_grid_step);
		glVertex3f(innerdown_ctrlPoints[0][0],innerdown_ctrlPoints[0][1],innerdown_ctrlPoints[0][2]-d+ud_grid_step);
		glVertex3f(innerdown_ctrlPoints[0][0],innerdown_ctrlPoints[0][1],innerdown_ctrlPoints[0][2]-d);
		glVertex3f(innerup_ctrlPoints[0][0],innerup_ctrlPoints[0][1],innerup_ctrlPoints[0][2]-d);		
	glEnd();

	glBegin(GL_QUADS);
		glNormal3f(-1.0f,0.0f,0.0f);
		glVertex3f(innerdown_ctrlPoints[3][0],innerdown_ctrlPoints[3][1],innerdown_ctrlPoints[3][2]-d+ud_grid_step);
		glVertex3f(innerup_ctrlPoints[3][0],innerup_ctrlPoints[3][1],innerup_ctrlPoints[3][2]-d+ud_grid_step);
		glVertex3f(innerup_ctrlPoints[3][0],innerup_ctrlPoints[3][1],innerup_ctrlPoints[3][2]-d);		
		glVertex3f(innerdown_ctrlPoints[3][0],innerdown_ctrlPoints[3][1],innerdown_ctrlPoints[3][2]-d);		
	glEnd();

	for(int i=0;i<24;){
			int upidx1 = upspot_tbl[i];
			int downidx1 = downspot_tbl[i];
			int upidx2 = upspot_tbl[i+1];
			int downidx2= downspot_tbl[i+1];
			DrawPillar(innerup_spot[upidx1],innerup_spot[upidx2],innerdown_spot[downidx1],innerdown_spot[downidx2],1);
			int upidx3 = upspot_tbl[i+2];
			int downidx3= downspot_tbl[i+2];
			int upidx4 = upspot_tbl[i+3];
			int downidx4= downspot_tbl[i+3];
			DrawPillar(innerup_spot[upidx3],innerup_spot[upidx4],innerdown_spot[downidx3],innerdown_spot[downidx4],1);
			i+=4;
	}
}

void cgym::DrawFrontLine(int N)
{
	GLfloat up[3];
	memcpy(up,&up_ctrlPoints[0],sizeof(up));
	GLfloat n1 = top_vheight/tan((PI/180.0f)*dip_angle);
	GLfloat xPos1 =up_ctrlPoints[0][0]+n1;
	GLfloat yPos1 = up_ctrlPoints[0][1]-top_vheight;
	GLfloat center[3]={xPos1,yPos1,0.0f};
	
	GLfloat n2 = innertop_vheight/tan((PI/180.0f)*innerdip_angle);
	GLfloat xPos2 =xPos1+n2;
	GLfloat yPos2 = up_ctrlPoints[0][1]-top_vheight-innertop_vheight;
	GLfloat down[3]={xPos2,yPos2,0.0f};
	GLfloat xPos3 = xPos2+innertop_hwidth;
	GLfloat inner[3]={xPos3,yPos2,0.0f};
	GLfloat bottom[3]={xPos3,horizon_level,0.0f};
	GLfloat vNormal[3];
	GLfloat temp[3][3];
	if(shadow)
		glColor3fv(shadowcolor);
	else
		glColor3f(1.0f,1.0f,1.0f);	
	glDepthFunc(GL_LEQUAL);							
	glEnable(GL_DEPTH_TEST);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glBindTexture( GL_TEXTURE_2D, texture->get_texture(WALL1_TEXTURE));
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND); 

	GLfloat et = 1.0f/(GRID_SIZE*ud_grid_step*0.5f);
	GLfloat s=0.0f;
	GLfloat move = ud_grid_step;
	for(int i=0;i<N-2;i++){
		glColor3f(0.9f,0.9f,0.9f);
		glBegin(GL_QUADS);
			temp[0][0]=up[0];temp[0][1]=up[1];temp[0][2]=up[2]-move;
			temp[1][0]=up[0];temp[1][1]=up[1];temp[1][2]=up[2]-move-ud_grid_step;
			temp[2][0]=center[0];temp[2][1]=center[1];temp[2][2]=center[2]-move-ud_grid_step;
			gltGetNormalVector(temp[0], temp[1], temp[2], vNormal);
			glNormal3fv(vNormal);
			glTexCoord2f(s,0.0f);glVertex3f(up[0],up[1],up[2]-move);
			glTexCoord2f(s+et,0.0f);glVertex3f(up[0],up[1],up[2]-move-ud_grid_step);
			glTexCoord2f(s+et,0.25f);glVertex3f(center[0],center[1],center[2]-move-ud_grid_step);
			glTexCoord2f(s,0.25f);glVertex3f(center[0],center[1],center[2]-move);	
		glEnd();
		glColor3f(0.75f,0.75f,0.75f);
		glBegin(GL_QUADS);
			temp[0][0]=center[0];temp[0][1]=center[1];temp[0][2]=center[2]-move;
			temp[1][0]=center[0];temp[1][1]=center[1];temp[1][2]=center[2]-move-ud_grid_step;
			temp[2][0]=down[0];temp[2][1]=down[1];temp[2][2]=down[2]-move-ud_grid_step;
			gltGetNormalVector(temp[0], temp[1], temp[2], vNormal);
			glNormal3fv(vNormal);

			glTexCoord2f(s,0.0f);glVertex3f(center[0],center[1],center[2]-move);
			glTexCoord2f(s+et,0.0f);glVertex3f(center[0],center[1],center[2]-move-ud_grid_step);
			glTexCoord2f(s+et,0.25f);glVertex3f(down[0],down[1],down[2]-move-ud_grid_step);		
			glTexCoord2f(s,0.25f);glVertex3f(down[0],down[1],down[2]-move);			
		glEnd();
		move+=ud_grid_step;
		s+=et;
	}

	if(shadow)
		glColor3fv(shadowcolor);
	else
		glColor3f(0.4f,0.4f,0.4f);
	glBegin(GL_QUADS);
		glNormal3f(0.0f,-1.0f,0.0f);
		glVertex3f(down[0],down[1],down[2]-ud_grid_step);	
		glVertex3f(down[0],down[1],down[2]-(ud_grid_step*(N-2)));
		glVertex3f(inner[0],inner[1],inner[2]-(ud_grid_step*(N-2)));	
		glVertex3f(inner[0],inner[1],inner[2]-ud_grid_step);	
	glEnd();

	if(shadow)
		glColor3fv(shadowcolor);
	else
		glColor3f(0.0f,0.52f,0.51f);	
	
	move = ud_grid_step;
	GLfloat d = 0.0f;
	s=0.0f;
	et = 1.0f/(GRID_SIZE*ud_grid_step*0.5f);
	for(int i=0;i<10;i++){
		for(int i=0;i<N-3;i++){
			glBegin(GL_QUADS);
			glNormal3f(-1.0f,0.0f,0.0f);
			glTexCoord2f(s,0.0f);glVertex3f(inner[0],inner[1]-d,inner[2]-move);
			glTexCoord2f(s+et,0.0f);glVertex3f(inner[0],inner[1]-d,inner[2]-move-ud_grid_step);
			glTexCoord2f(s+et,1.0f);glVertex3f(bottom[0],bottom[1]-d-ud_grid_step,bottom[2]-move-ud_grid_step);	
			glTexCoord2f(s,1.0f);glVertex3f(bottom[0],bottom[1]-d-ud_grid_step,bottom[2]-move);						
			glEnd();
			move+=ud_grid_step;
			s+=et;
		}
		s=0.0f;
		d+=ud_grid_step;
		move=ud_grid_step;
	}

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);

	if(shadow)
		glColor3fv(shadowcolor);
	else	
		glColor3f(0.68f,0.68f,0.68f);	
	glBegin(GL_TRIANGLE_STRIP);
	    glNormal3f(0.0f,0.0f,-1.0f);
		glVertex3f(inner[0],inner[1],inner[2]-ud_grid_step);
		glVertex3f(bottom[0],bottom[1],bottom[2]-ud_grid_step);
		glVertex3f(up[0],up[1],up[2]-ud_grid_step);
	glEnd();
	glBegin(GL_TRIANGLE_STRIP);
		glNormal3f(0.0f,0.0f,1.0f);
		glVertex3f(inner[0],inner[1],inner[2]-(ud_grid_step*(N-2)));
		glVertex3f(up[0],up[1],up[2]-(ud_grid_step*(N-2)));
		glVertex3f(bottom[0],bottom[1],bottom[2]-(ud_grid_step*(N-2)));		
	glEnd();
}
void cgym::DrawFrontFace()
{
	DrawFrontLine(GRID_SIZE);
	
	GLfloat up[3];
	memcpy(up,&up_ctrlPoints[0],sizeof(up));
	GLfloat n = up_ctrlPoints[0][1]/tan((PI/180.0f)*dip_angle);
	GLfloat xPos =up_ctrlPoints[0][0]+n;
	GLfloat down[3]={xPos,(horizon_level),0};
	if(shadow)
		glColor3fv(shadowcolor);
	else
		glColor3f(0.7f,0.7f,0.7f);
	GLfloat temp[3][3];
	GLfloat vNormal[3];

	temp[0][0]=up[0];temp[0][1]=up[1];temp[0][2]=up[2];
	temp[1][0]=up[0];temp[1][1]=up[1];temp[1][2]=up[2]-ud_grid_step;
	temp[2][0]=down[0];temp[2][1]=down[1];temp[2][2]=down[2]-ud_grid_step;
	gltGetNormalVector(temp[0], temp[1], temp[2], vNormal);

	glEnable(GL_TEXTURE_2D);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glBindTexture( GL_TEXTURE_2D, texture->get_texture(WALL1_TEXTURE));
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);

	glBegin(GL_QUADS);
		glNormal3fv(vNormal);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(up[0],up[1],up[2]);	
		glTexCoord2f(0.5f, 0.0f);glVertex3f(up[0],up[1],up[2]-ud_grid_step);
		glTexCoord2f(0.5f, 1.0f);glVertex3f(down[0],down[1],down[2]-ud_grid_step);
		glTexCoord2f(0.0f, 1.0f);glVertex3f(down[0],down[1],down[2]);
	glEnd();

	memcpy(up,&up_ctrlPoints[0],sizeof(up));
	xPos =up_ctrlPoints[0][0]+n;
	down[0] = xPos;down[1]=(horizon_level);down[2]=0.0f;

	for(int i=0;i<GRID_SIZE;i++){
		up[2]-=ud_grid_step;
		down[2]-=ud_grid_step;
	}

	glBegin(GL_QUADS);
		glNormal3fv(vNormal);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(up[0],up[1],up[2]+2*ud_grid_step);
		glTexCoord2f(0.5f, 0.0f);glVertex3f(up[0],up[1],up[2]+ud_grid_step);	
		glTexCoord2f(0.5f, 1.0f);glVertex3f(down[0],down[1],down[2]+ud_grid_step);
		glTexCoord2f(0.0f, 1.0f);glVertex3f(down[0],down[1],down[2]+2*ud_grid_step);		
	glEnd();
	glDisable(GL_TEXTURE_2D);
//	glColor3f(0.4f,0.6f,0.1f); //green	
}

void cgym::DrawBackFace()
{
	GLfloat up[3];
	memcpy(up,&up_ctrlPoints[3],sizeof(up));
	GLfloat n = up_ctrlPoints[3][1]/tan((PI/180.0f)*dip_angle);
	GLfloat xPos =up_ctrlPoints[3][0]-n;
	GLfloat down[3]={xPos,horizon_level,0.0f};

	GLfloat temp[3][3];
	GLfloat vNormal[3];

	temp[0][0]=up[0];temp[0][1]=up[1];temp[0][2]=up[2];
	temp[1][0]=down[0];temp[1][1]=down[1];temp[1][2]=down[2];
	temp[2][0]=down[0];temp[2][1]=down[1];temp[2][2]=down[2]-ud_grid_step;
	gltGetNormalVector(temp[0], temp[1], temp[2], vNormal);
	if(shadow)
		glColor3fv(shadowcolor);
	else
		glColor3f(1.0f,1.0f,1.0f);
	glEnable(GL_TEXTURE_2D);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glBindTexture( GL_TEXTURE_2D, texture->get_texture(WALL1_TEXTURE));
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
	glBegin(GL_QUADS);
		glNormal3fv(vNormal);
		glTexCoord2f(0.0f,0.0f);glVertex3f(up[0],up[1],up[2]);	
		glTexCoord2f(0.0f,1.0f);glVertex3f(down[0],down[1],down[2]);
		glTexCoord2f(0.25f,1.0f);glVertex3f(down[0],down[1],down[2]-ud_grid_step);
		glTexCoord2f(0.25f,0.0f);glVertex3f(up[0],up[1],up[2]-ud_grid_step);	
	glEnd();

	memcpy(up,&up_ctrlPoints[3],sizeof(up));
	xPos =up_ctrlPoints[3][0]-n;
	down[0] = xPos;down[1]=(horizon_level);down[2]=0.0f;

	for(int i=0;i<GRID_SIZE;i++){
		up[2]-=ud_grid_step;
		down[2]-=ud_grid_step;
	}
	glBegin(GL_QUADS);
		glNormal3fv(vNormal);
		glTexCoord2f(0.0f,0.0f);glVertex3f(up[0],up[1],up[2]+2*ud_grid_step);
		glTexCoord2f(0.0f,1.0f);glVertex3f(down[0],down[1],down[2]+2*ud_grid_step);		
		glTexCoord2f(0.25f,1.0f);glVertex3f(down[0],down[1],down[2]+ud_grid_step);
		glTexCoord2f(0.25f,0.0f);glVertex3f(up[0],up[1],up[2]+ud_grid_step);		
	glEnd();
	glDisable(GL_TEXTURE_2D);
}

void cgym::DrawBackLine()
{
	GLfloat dip_angle2 = 180-dip_angle;
	GLfloat innerdip_angle2 = 180-innerdip_angle;
	GLfloat innertop_hwidth2 = innertop_hwidth - 0.06f;
	GLfloat up[3];
	memcpy(up,&up_ctrlPoints[3],sizeof(up));
	GLfloat n1 = top_vheight/tan((PI/180.0f)*dip_angle2);
	GLfloat xPos1 =up_ctrlPoints[3][0]+n1;
	GLfloat yPos1 = up_ctrlPoints[3][1]-top_vheight;
	GLfloat center[3]={xPos1,yPos1,0.0f};

	GLfloat n2 = innertop_vheight/tan((PI/180.0f)*innerdip_angle2);
	GLfloat xPos2 =xPos1+n2;
	GLfloat yPos2 = up_ctrlPoints[3][1]-top_vheight-innertop_vheight;
	GLfloat down[3]={xPos2,yPos2,0.0f};
	GLfloat xPos3 = xPos2-innertop_hwidth2;
	GLfloat inner[3]={xPos3,yPos2,0.0f};
	GLfloat bottom[3]={xPos3,horizon_level,0.0f};
	GLfloat temp[3][3];
	GLfloat vNormal[3];
	glDepthFunc(GL_LEQUAL);							
	glEnable(GL_DEPTH_TEST);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glBindTexture( GL_TEXTURE_2D, texture->get_texture(WALL1_TEXTURE));
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND); 

	if(shadow)
		glColor3fv(shadowcolor);
	else
		glColor3f(0.68f,0.68f,0.68f);	
	GLfloat et = 1.0f/(GRID_SIZE*ud_grid_step*0.5f);
	GLfloat s=0.0f;
	GLfloat move = ud_grid_step;
	for(int i=0;i<GRID_SIZE-2;i++){
		glColor3f(0.9f,0.9f,0.9f);
		glBegin(GL_QUADS);	
		temp[0][0]=up[0];temp[0][1]=up[1];temp[0][2]=up[2]-move;
		temp[1][0]=center[0];temp[1][1]=center[1];temp[1][2]=center[2]-move;
		temp[2][0]=center[0];temp[2][1]=center[1];temp[2][2]=center[2]-move-ud_grid_step;
		gltGetNormalVector(temp[0], temp[1], temp[2], vNormal);
		glNormal3fv(vNormal);
		glTexCoord2f(s,0.0f);glVertex3f(up[0],up[1],up[2]-move);
		glTexCoord2f(s,0.25f);glVertex3f(center[0],center[1],center[2]-move);	
		glTexCoord2f(s+et,0.25f);glVertex3f(center[0],center[1],center[2]-move-ud_grid_step);	
		glTexCoord2f(s+et,0.0f);glVertex3f(up[0],up[1],up[2]-move-ud_grid_step);	
	glEnd();
		glColor3f(0.75f,0.75f,0.75f);
	glBegin(GL_QUADS);
		temp[0][0]=center[0];temp[0][1]=center[1];temp[0][2]=center[2]-move;
		temp[1][0]=down[0];temp[1][1]=down[1];temp[1][2]=down[2]-move;
		temp[2][0]=down[0];temp[2][1]=down[1];temp[2][2]=down[2]-move-ud_grid_step;
		gltGetNormalVector(temp[0], temp[1], temp[2], vNormal);
		glNormal3fv(vNormal);
		glTexCoord2f(s,0.0f);glVertex3f(center[0],center[1],center[2]-move);
		glTexCoord2f(s,0.25f);glVertex3f(down[0],down[1],down[2]-move);			
		glTexCoord2f(s+et,0.25f);glVertex3f(down[0],down[1],down[2]-move-ud_grid_step);
		glTexCoord2f(s+et,0.0f);glVertex3f(center[0],center[1],center[2]-move-ud_grid_step);	
	glEnd();	
	move+=ud_grid_step;
	s+=et;
	}
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);

	if(shadow)
		glColor3fv(shadowcolor);
	else
		glColor3f(0.4f,0.4f,0.4f);
	glBegin(GL_QUADS);
		glNormal3f(0.0f,-1.0f,0.0f);
		glVertex3f(down[0],down[1],down[2]-ud_grid_step);
		glVertex3f(inner[0],inner[1],inner[2]-ud_grid_step);	
		glVertex3f(inner[0],inner[1],inner[2]-(ud_grid_step*(GRID_SIZE-2)));
		glVertex3f(down[0],down[1],down[2]-(ud_grid_step*(GRID_SIZE-2)));	
	glEnd();
	
	if(shadow)
		glColor3fv(shadowcolor);
	else
		glColor3f(0.62f,0.62f,0.62f);	

	glBegin(GL_QUADS);
		glNormal3f(1.0f,0.0f,0.0f);
		glVertex3f(inner[0],inner[1],inner[2]-ud_grid_step);
		glVertex3f(bottom[0],bottom[1],bottom[2]-ud_grid_step);		
		glVertex3f(bottom[0],bottom[1],bottom[2]-(ud_grid_step*(GRID_SIZE-2)));	
		glVertex3f(inner[0],inner[1],inner[2]-(ud_grid_step*(GRID_SIZE-2)));					
	glEnd();
	if(shadow)
		glColor3fv(shadowcolor);
	else
		glColor3f(0.68f,0.68f,0.68f);	
	glBegin(GL_TRIANGLE_STRIP);
	glNormal3f(0.0f,0.0f,-1.0f);
	glVertex3f(bottom[0],bottom[1],bottom[2]-ud_grid_step);
	glVertex3f(inner[0],inner[1],inner[2]-ud_grid_step);	
	glVertex3f(up[0],up[1],up[2]-ud_grid_step);
	glEnd();
	glBegin(GL_TRIANGLE_STRIP);
	glNormal3f(0.0f,0.0f,1.0f);
	glVertex3f(up[0],up[1],up[2]-(ud_grid_step*(GRID_SIZE-2)));
	glVertex3f(inner[0],inner[1],inner[2]-(ud_grid_step*(GRID_SIZE-2)));
	glVertex3f(bottom[0],bottom[1],bottom[2]-(ud_grid_step*(GRID_SIZE-2)));		
	glEnd();
}

void cgym::DrawBottomGrid()
{
	GLfloat n1 = up_ctrlPoints[0][1]/tan((PI/180.0f)*dip_angle);
	GLfloat xPos1 =up_ctrlPoints[0][0]+n1;
	GLfloat down1[3]={xPos1,-0.6f,0.0f};

	GLfloat n2 = up_ctrlPoints[3][1]/tan((PI/180.0f)*dip_angle);
	GLfloat xPos2 =up_ctrlPoints[3][0]-n2;
	GLfloat down2[3]={xPos2,(horizon_level),0.0f};
	for(int i=0;i<GRID_SIZE;i++){
		glBegin(GL_LINE_STRIP);
		glVertex3fv(down1);
		glVertex3fv(down2);
		glEnd();
		down1[2]-=ud_grid_step;
		down2[2]-=ud_grid_step;
	}
}

void cgym::DrawStripLine() 
{
	GLfloat innerupleft[3];
	GLfloat innerupright[3];
	GLfloat innerdownleft[3];
	GLfloat innerdownright[3];
	GLfloat upleft[3];
	GLfloat upright[3];
	memcpy(innerupleft,innerup_ctrlPoints[0],sizeof(innerupleft));
	memcpy(innerupright,innerup_ctrlPoints[3],sizeof(innerupright));
	memcpy(innerdownleft,innerdown_ctrlPoints[0],sizeof(innerdownleft));
	memcpy(innerdownright,innerdown_ctrlPoints[3],sizeof(innerdownright));

	memcpy(upleft,up_ctrlPoints[0],sizeof(upleft));
	memcpy(upright,up_ctrlPoints[3],sizeof(upright));

	GLfloat n1 = up_ctrlPoints[0][1]/tan((PI/180.0f)*dip_angle);
	GLfloat xPos1 =up_ctrlPoints[0][0]+n1;
	GLfloat downleft[3]={xPos1,(horizon_level),0};

	GLfloat n2 = up_ctrlPoints[3][1]/tan((PI/180.0f)*dip_angle);
	GLfloat xPos2 =up_ctrlPoints[3][0]-n2;
	GLfloat downright[3]={xPos2,-0.6f,0.0f};

	if(shadow)
		glColor3fv(shadowcolor);
	else
		//glColor3f(0.68f,0.34f,0.0f);//orange
		glColor3f(1.0f,1.0f,1.0f);//orange
	glDepthFunc(GL_LEQUAL);							
	glEnable(GL_DEPTH_TEST);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glBindTexture( GL_TEXTURE_2D, texture->get_texture(WALL3_TEXTURE));
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND); 

	glBegin(GL_QUADS);
		glNormal3f(0.0f,0.0f,1.0f);
		glTexCoord2f(0.0f,0.0f);glVertex3f(innerupleft[0],innerupleft[1],innerupleft[2]);
		glTexCoord2f(0.10f,0.0f);glVertex3f(upleft[0],upleft[1],upleft[2]);
		glTexCoord2f(0.10f,1.0f);glVertex3f(downleft[0],downleft[1],downleft[2]);
		glTexCoord2f(0.0f,1.0f);glVertex3f(innerdownleft[0],innerdownleft[1],innerdownleft[2]);
	glEnd();

	glBegin(GL_QUADS);
		glNormal3f(0.0f,0.0f,1.0f);
		glTexCoord2f(0.0f,0.0f);glVertex3f(upright[0],upright[1],upright[2]);
		glTexCoord2f(0.08f,0.0f);glVertex3f(innerupright[0],innerupright[1],innerupright[2]);	
		glTexCoord2f(0.08f,1.0f);glVertex3f(innerdownright[0],innerdownright[1],innerdownright[2]);
		glTexCoord2f(0.0f,1.0f);glVertex3f(downright[0],downleft[1],downleft[2]);
	glEnd();
	

	if(shadow)
		glColor3fv(shadowcolor);
	else
		glColor3f(0.1f,0.4f,0.4f);
	
	//left
	glBegin(GL_QUADS);
		glVertex3f(downleft[0],downleft[1],downleft[2]);
		glVertex3f(downleft[0],downleft[1],downleft[2]-ud_grid_step);
		glVertex3f(innerdownleft[0],innerdownleft[1],innerdownleft[2]-ud_grid_step);	
		glVertex3f(innerdownleft[0],innerdownleft[1],innerdownleft[2]);	
	glEnd();
	glBegin(GL_QUADS);
		glVertex3f(upleft[0],upleft[1],upleft[2]);
		glVertex3f(innerupleft[0],innerupleft[1],innerupleft[2]);
		glVertex3f(innerupleft[0],innerupleft[1],innerupleft[2]-ud_grid_step);	
		glVertex3f(upleft[0],upleft[1],upleft[2]-ud_grid_step);
	glEnd();

	//right
	glBegin(GL_QUADS);
		glVertex3f(downright[0],downright[1],downright[2]);
		glVertex3f(innerdownright[0],innerdownright[1],innerdownright[2]);
		glVertex3f(innerdownright[0],innerdownright[1],innerdownright[2]-ud_grid_step);	
		glVertex3f(downright[0],downright[1],downright[2]-ud_grid_step);		
	glEnd();
	glBegin(GL_QUADS);
		glVertex3f(upright[0],upright[1],upright[2]);
		glVertex3f(upright[0],upright[1],upright[2]-ud_grid_step);
		glVertex3f(innerupright[0],innerupright[1],innerupright[2]-ud_grid_step);	
		glVertex3f(innerupright[0],innerupright[1],innerupright[2]);		
	glEnd();
	
	innerupleft[2]-=(GRID_SIZE-1)*ud_grid_step;
	upleft[2]-=(GRID_SIZE-1)*ud_grid_step;
	innerupright[2]-=(GRID_SIZE-1)*ud_grid_step;
	upright[2]-=(GRID_SIZE-1)*ud_grid_step;
	innerdownleft[2]-=(GRID_SIZE-1)*ud_grid_step;
	downleft[2]-=(GRID_SIZE-1)*ud_grid_step;
	innerdownright[2]-=(GRID_SIZE-1)*ud_grid_step;
	downright[2]-=(GRID_SIZE-1)*ud_grid_step;

	if(shadow)
		glColor3fv(shadowcolor);
	else
		glColor3f(1.0f,1.0f,1.0f);//orange
	glBegin(GL_QUADS);
	glNormal3f(0.0f,0.0f,-1.0f);
	glTexCoord2f(0.0f,0.0f);glVertex3f(innerupleft[0],innerupleft[1],innerupleft[2]);
	glTexCoord2f(0.0f,1.0f);glVertex3f(innerdownleft[0],innerdownleft[1],innerdownleft[2]);
	glTexCoord2f(0.1f,1.0f);glVertex3f(downleft[0],downleft[1],downleft[2]);
	glTexCoord2f(0.1f,0.0f);glVertex3f(upleft[0],upleft[1],upleft[2]);	
	glEnd();

	glBegin(GL_QUADS);
	glNormal3f(0.0f,0.0f,-1.0f);
	glTexCoord2f(0.0f,0.0f);glVertex3f(upright[0],upright[1],upright[2]);
	glTexCoord2f(0.0f,1.0f);glVertex3f(downright[0],downleft[1],downleft[2]);
	glTexCoord2f(0.1f,1.0f);glVertex3f(innerdownright[0],innerdownright[1],innerdownright[2]);
	glTexCoord2f(0.1f,0.0f);glVertex3f(innerupright[0],innerupright[1],innerupright[2]);	
	glEnd();

	if(shadow)
		glColor3fv(shadowcolor);
	else
		glColor3f(0.1f,0.4f,0.4f);
	//left
	glBegin(GL_QUADS);
	glVertex3f(downleft[0],downleft[1],downleft[2]+ud_grid_step);
	glVertex3f(downleft[0],downleft[1],downleft[2]);
	glVertex3f(innerdownleft[0],innerdownleft[1],innerdownleft[2]);	
	glVertex3f(innerdownleft[0],innerdownleft[1],innerdownleft[2]+ud_grid_step);	
	glEnd();
	glBegin(GL_QUADS);
	glVertex3f(upleft[0],upleft[1],upleft[2]+ud_grid_step);
	glVertex3f(innerupleft[0],innerupleft[1],innerupleft[2]+ud_grid_step);
	glVertex3f(innerupleft[0],innerupleft[1],innerupleft[2]);	
	glVertex3f(upleft[0],upleft[1],upleft[2]);
	glEnd();

	//right
	glBegin(GL_QUADS);
	glVertex3f(downright[0],downright[1],downright[2]+ud_grid_step);
	glVertex3f(innerdownright[0],innerdownright[1],innerdownright[2]+ud_grid_step);
	glVertex3f(innerdownright[0],innerdownright[1],innerdownright[2]);	
	glVertex3f(downright[0],downright[1],downright[2]);		
	glEnd();
	glBegin(GL_QUADS);
	glVertex3f(upright[0],upright[1],upright[2]+ud_grid_step);
	glVertex3f(upright[0],upright[1],upright[2]);
	glVertex3f(innerupright[0],innerupright[1],innerupright[2]);	
	glVertex3f(innerupright[0],innerupright[1],innerupright[2]+ud_grid_step);		
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
}

void cgym::DrawRMirror()
{
	glDepthFunc(GL_LEQUAL);							
	glEnable(GL_DEPTH_TEST);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glBindTexture( GL_TEXTURE_2D, texture->get_texture(WALL7_TEXTURE));
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND); 

	int n =0,m=0,idx=0;
	if(shadow)
		glColor3fv(shadowcolor);
	else
		glColor3f(0.0f,0.52f,0.51f);	
	n=upspot_tbl[0];
	m = downspot_tbl[0];
	GLfloat tiny = 0.01f;//fine tuning
	GLfloat et = 1.0f/n*0.2;
	GLfloat s=0.2f,t=0.0f;
	for(int k=0;k<n;k++){
		glBegin(GL_TRIANGLE_STRIP);
			glNormal3f(0.0f,0.0f,1.0f);
			glTexCoord2f(s,0.0f);glVertex3f(innerup_spot[k][0],innerup_spot[k][1],innerup_spot[k][2]-ud_grid_step+tiny);
			glTexCoord2f(s,1.0f);glVertex3f(innerdown_spot[m][0],innerdown_spot[m][1],innerdown_spot[m][2]-ud_grid_step+tiny);
			glTexCoord2f(s+et,0.0f);glVertex3f(innerup_spot[k+1][0],innerup_spot[k+1][1],innerup_spot[k+1][2]-ud_grid_step+tiny);			
		glEnd();
		s+=et;
	}
	int i = 1;
	for(int j=0;j<5;j++)
	{	
		n = downspot_tbl[i+1]-downspot_tbl[i];
		m = upspot_tbl[i];
		idx = downspot_tbl[i];
		s=0.0f;
		for(int k=0;k<n;k++){
			glBegin(GL_TRIANGLE_STRIP);
				glNormal3f(0.0f,0.0f,1.0f);
				glTexCoord2f(s,1.0f);glVertex3f(innerup_spot[m][0],innerup_spot[m][1],innerup_spot[m][2]-ud_grid_step+tiny);	
				glTexCoord2f(s,0.0f);glVertex3f(innerdown_spot[idx+k][0],innerdown_spot[idx+k][1],innerdown_spot[idx+k][2]-ud_grid_step+tiny);
				glTexCoord2f(s+et,0.0f);glVertex3f(innerdown_spot[idx+k+1][0],innerdown_spot[idx+k+1][1],innerdown_spot[idx+k+1][2]-ud_grid_step+tiny);			
			glEnd();
			s+=et;
		}
		n=upspot_tbl[i+3] - upspot_tbl[i+2];
		m = downspot_tbl[i+2];
		idx = upspot_tbl[i+2];
		s=0.0f;
		for(int k=0;k<n;k++){
			glBegin(GL_TRIANGLE_STRIP);
			glNormal3f(0.0f,0.0f,1.0f);
			glTexCoord2f(s,0.0f);glVertex3f(innerup_spot[idx+k][0],innerup_spot[idx+k][1],innerup_spot[idx+k][2]-ud_grid_step+tiny);
			glTexCoord2f(s,1.0f);glVertex3f(innerdown_spot[m][0],innerdown_spot[m][1],innerdown_spot[m][2]-ud_grid_step+tiny);			
			glTexCoord2f(s+et,0.0f);glVertex3f(innerup_spot[idx+k+1][0],innerup_spot[idx+k+1][1],innerup_spot[idx+k+1][2]-ud_grid_step+tiny);
			glEnd();
			s+=et;
		}
		i+=4;
	}
	n=downspot_tbl[22] - downspot_tbl[21];
	m = upspot_tbl[21];
	idx = downspot_tbl[21];
	s=0.0f;
	for(int k=0;k<n;k++){
		glBegin(GL_TRIANGLE_STRIP);
		glNormal3f(0.0f,0.0f,1.0f);
		glTexCoord2f(s,1.0f);glVertex3f(innerup_spot[m][0],innerup_spot[m][1],innerup_spot[m][2]-ud_grid_step+tiny);		
		glTexCoord2f(s,0.0f);glVertex3f(innerdown_spot[idx+k][0],innerdown_spot[idx+k][1],innerdown_spot[idx+k][2]-ud_grid_step+tiny);
		glTexCoord2f(s+et,0.0f);glVertex3f(innerdown_spot[idx+k+1][0],innerdown_spot[idx+k+1][1],innerdown_spot[idx+k+1][2]-ud_grid_step+tiny);
		glEnd();
		s+=et;
	}	
	n=M - upspot_tbl[23];
	m = downspot_tbl[23];
	idx = upspot_tbl[23];
	for(int k =0;k<n-1;k++){
		glBegin(GL_TRIANGLE_STRIP);
			glNormal3f(0.0f,0.0f,1.0f);
			glTexCoord2f(s,0.0f);glVertex3f(innerup_spot[idx+k][0],innerup_spot[idx+k][1],innerup_spot[idx+k][2]-ud_grid_step+tiny);		
			glTexCoord2f(s,1.0f);glVertex3f(innerdown_spot[m][0],innerdown_spot[m][1],innerdown_spot[m][2]-ud_grid_step+tiny);
			glTexCoord2f(s+et,0.0f);glVertex3f(innerup_spot[idx+k+1][0],innerup_spot[idx+k+1][1],innerup_spot[idx+k+1][2]-ud_grid_step+tiny);
		glEnd();
		s+=et;
	}
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
}

void cgym::DrawLMirror()
{
	glDepthFunc(GL_LEQUAL);							
	glEnable(GL_DEPTH_TEST);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glBindTexture( GL_TEXTURE_2D, texture->get_texture(WALL7_TEXTURE));
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);

	int n =0,m=0,idx=0;
	GLfloat move = (GRID_SIZE-2)*ud_grid_step;
	if(shadow)
		glColor3fv(shadowcolor);
	else
		glColor3f(0.0f,0.52f,0.51f);	
	n=upspot_tbl[0];
	m = downspot_tbl[0];
	GLfloat tiny = 0.01f;//fine tuning
	GLfloat et = 1.0f/n*0.2;
	GLfloat s=0.2f,t=0.0f;
	for(int k=0;k<n;k++){
		glBegin(GL_TRIANGLE_STRIP);
		glNormal3f(0.0f,0.0f,-1.0f);
		glTexCoord2f(s,0.0f);glVertex3f(innerup_spot[k][0],innerup_spot[k][1],innerup_spot[k][2]-move-tiny);
		glTexCoord2f(s+et,0.0f);glVertex3f(innerup_spot[k+1][0],innerup_spot[k+1][1],innerup_spot[k+1][2]-move-tiny);
		glTexCoord2f(s,1.0f);glVertex3f(innerdown_spot[m][0],innerdown_spot[m][1],innerdown_spot[m][2]-move-tiny);					
		glEnd();
		s+=et;
	}
	int i = 1;
	for(int j=0;j<5;j++)
	{	
		n = downspot_tbl[i+1]-downspot_tbl[i];
		et = 1.0f/n*0.2;s=0.2f;
		m = upspot_tbl[i];
		idx = downspot_tbl[i];
		for(int k=0;k<n;k++){
			glBegin(GL_TRIANGLE_STRIP);
				glNormal3f(0.0f,0.0f,-1.0f);
				glTexCoord2f(s,1.0f);glVertex3f(innerup_spot[m][0],innerup_spot[m][1],innerup_spot[m][2]-move-tiny);	
				glTexCoord2f(s+et,0.0f);glVertex3f(innerdown_spot[idx+k+1][0],innerdown_spot[idx+k+1][1],innerdown_spot[idx+k+1][2]-move-tiny);
				glTexCoord2f(s,0.0f);glVertex3f(innerdown_spot[idx+k][0],innerdown_spot[idx+k][1],innerdown_spot[idx+k][2]-move-tiny);			
			glEnd();
			s+=et;
		}
		n=upspot_tbl[i+3] - upspot_tbl[i+2];
		et = 1.0f/n*0.2;s=0.2f;
		m = downspot_tbl[i+2];
		idx = upspot_tbl[i+2];
		for(int k=0;k<n;k++){
			glBegin(GL_TRIANGLE_STRIP);
			glNormal3f(0.0f,0.0f,-1.0f);
			glTexCoord2f(s,0.0f);glVertex3f(innerup_spot[idx+k][0],innerup_spot[idx+k][1],innerup_spot[idx+k][2]-move-tiny);
			glTexCoord2f(s+et,0.0f);glVertex3f(innerup_spot[idx+k+1][0],innerup_spot[idx+k+1][1],innerup_spot[idx+k+1][2]-move-tiny);
			glTexCoord2f(s,1.0f);glVertex3f(innerdown_spot[m][0],innerdown_spot[m][1],innerdown_spot[m][2]-move-tiny);			
			glEnd();
			s+=et;
		}
		i+=4;
	}

	n=downspot_tbl[22] - downspot_tbl[21];
	et = 1.0f/n*0.2;s=0.2f;
	m = upspot_tbl[21];
	idx = downspot_tbl[21];
	for(int k=0;k<n;k++){
		glBegin(GL_TRIANGLE_STRIP);		
		glNormal3f(0.0f,0.0f,-1.0f);
		glTexCoord2f(s,1.0f);glVertex3f(innerup_spot[m][0],innerup_spot[m][1],innerup_spot[m][2]-move-tiny);		
		glTexCoord2f(s+et,0.0f);glVertex3f(innerdown_spot[idx+k+1][0],innerdown_spot[idx+k+1][1],innerdown_spot[idx+k+1][2]-move-tiny);
		glTexCoord2f(s,0.0f);glVertex3f(innerdown_spot[idx+k][0],innerdown_spot[idx+k][1],innerdown_spot[idx+k][2]-move-tiny);		
		glEnd();
		s+=et;
	}	
	n=M - upspot_tbl[23];
	et = 1.0f/n*0.2;
	m = downspot_tbl[23];
	idx = upspot_tbl[23];
	for(int k =0;k<n-1;k++){
		glBegin(GL_TRIANGLE_STRIP);		
		glNormal3f(0.0f,0.0f,-1.0f);
		glTexCoord2f(s,0.0f);glVertex3f(innerup_spot[idx+k][0],innerup_spot[idx+k][1],innerup_spot[idx+k][2]-move-tiny);
		glTexCoord2f(s+et,0.0f);glVertex3f(innerup_spot[idx+k+1][0],innerup_spot[idx+k+1][1],innerup_spot[idx+k+1][2]-move-tiny);
		glTexCoord2f(s,1.0f);glVertex3f(innerdown_spot[m][0],innerdown_spot[m][1],innerdown_spot[m][2]-move-tiny);
		glEnd();
		s+=et;
	}
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
}
void cgym::DrawRBMace()
{
	glDepthFunc(GL_LEQUAL);							
	glEnable(GL_DEPTH_TEST);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glBindTexture( GL_TEXTURE_2D, texture->get_texture(WALL7_TEXTURE));
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND); 
	GLfloat et = 1.0f/100.0f;
	GLfloat s=0.0f,t=0.0f;
	GLfloat temp[3];
	memcpy(temp,down_ctrlPoints[0],sizeof(temp));
	for(int i=0;i<100;i++){
		glBegin(GL_QUADS);
			glNormal3f(0.0f,0.0f,1.0f);
			glTexCoord2f(s,1.0f);glVertex3f(down_spot[i][0],down_spot[i][1],down_spot[i][2]-ud_grid_step);
			glTexCoord2f(s,0.0f);glVertex3f(temp[0],temp[1],temp[2]-ud_grid_step);
			glTexCoord2f(s+et,0.0f);glVertex3f(temp[0]+0.02f,temp[1],temp[2]-ud_grid_step);
			glTexCoord2f(s+et,1.0f);glVertex3f(down_spot[i+1][0],down_spot[i+1][1],down_spot[i+1][2]-ud_grid_step);
		glEnd();
		temp[0]+=0.02f;
		s+=et;
	}
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
}

void cgym::DrawLBMace()
{

	glDepthFunc(GL_LEQUAL);							
	glEnable(GL_DEPTH_TEST);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glBindTexture( GL_TEXTURE_2D, texture->get_texture(WALL7_TEXTURE));
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND); 
	GLfloat et = 1.0f/100.0f;
	GLfloat s=0.0f,t=0.0f;
	GLfloat temp[3];
	memcpy(temp,down_ctrlPoints[0],sizeof(temp));
	GLfloat move =(GRID_SIZE-2)*ud_grid_step;
	for(int i=0;i<100;i++){
		glBegin(GL_QUADS);
		glNormal3f(0.0f,0.0f,-1.0f);
		glTexCoord2f(s,0.0f);glVertex3f(down_spot[i][0],down_spot[i][1],down_spot[i][2]-move);
		glTexCoord2f(s+et,0.0f);glVertex3f(down_spot[i+1][0],down_spot[i+1][1],down_spot[i+1][2]-move);
		glTexCoord2f(s+et,1.0f);glVertex3f(temp[0]+0.02f,temp[1],temp[2]-move);
		glTexCoord2f(s,1.0f);glVertex3f(temp[0],temp[1],temp[2]-move);		
		glEnd();
		temp[0]+=0.02f;
		s+=et;
	}
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
}
/////////////////////////////////////////////////////////////////////////////////
//debug print
void cgym::DrawPoints(void)
{
	int i;
//	glColor3f(1.0f,0.0f,0.0f);
	glPointSize(5.0f);
	glBegin(GL_POINTS);
	for(i = 0; i < nNumPoints; i++)
		glVertex2fv(down_ctrlPoints[i]);
	glEnd();
//	glColor3f(0.4f,0.6f,0.1f); //green
}

void cgym::DrawPointOnInnerUp()
{
	glPointSize(5.0f);
	glBegin(GL_POINTS);
	for(int i=0;i<M;i++){
		glVertex3fv(innerup_spot[i]);
	}
	glEnd();
}

void cgym::DrawPointOnInnerDown()
{
	glPointSize(5.0f);
	glBegin(GL_POINTS);
	for(int i=0;i<M;i++){
		glVertex3fv(innerdown_spot[i]);
	}
	glEnd();
}


// Draw a gridded ground
void cgym::DrawGround(void)
{
	GLfloat fExtent = 60.0f;
	GLfloat fStep = 1.0f;
	GLint iStrip ,iRun;
	GLfloat s = 0.0f;
	GLfloat t = 0.0f;
	GLfloat texStep = 1.0f / (fExtent * .035f);

#ifdef DRAW_GROUND_WITH_LINE
//	glColor3f(0.60f, .40f, .10f);

	for(iStrip = -fExtent;iStrip<=fExtent;iStrip+=fStep)
	{
		t=0.0f;
		glBegin(GL_TRIANGLE_STRIP);	
		for(iRun = fExtent;iRun>=-fExtent;iRun-=fStep){
			glTexCoord2f(s,t);
			glNormal3f(0.0f,1.0f,0.0f);
			glVertex3f(iStrip,horizon_level,iRun);

			glTexCoord2f(s+texStep,t);
			glNormal3f(0.0f,1.0f,0.0f);
			glVertex3f(iStrip+fStep,horizon_level,iRun);
			t+=texStep;
		}
		glEnd();
		s+=texStep;
	}
#else
	glBegin(GL_LINES);
	for(iLine = -fExtent; iLine <= fExtent; iLine += fStep)
	{		
		glVertex3f(iLine, horizon_level, fExtent);    // Draw Z lines
		glVertex3f(iLine, horizon_level, -fExtent);
		glVertex3f(fExtent, horizon_level, iLine);
		glVertex3f(-fExtent, horizon_level, iLine);
	}
	glEnd();
	
#endif	
}

//bezier expression
GLfloat cgym::bezier_formula_x(GLfloat temp[4][3],GLfloat t)
{
	/* triple expression:
	B(t)=P0(1-t)^3 + 3P1t(1-t)^2+3P2t^2(1-t)+P3t^3  0<=t<=1
	*/
	GLfloat vy = temp[0][0]*pow((1-t),3)+
						3*temp[1][0]*t*pow((1-t),2)+
						3*temp[2][0]*pow(t,2)*(1-t)+
						temp[3][0]*pow(t,3);
	return vy;
}

GLfloat cgym::bezier_formula_y(GLfloat temp[4][3],GLfloat t)
{
	/* triple expression:
	B(t)=P0(1-t)^3 + 3P1t(1-t)^2+3P2t^2(1-t)+P3t^3  0<=t<=1
	*/
	GLfloat vy = temp[0][1]*pow((1-t),3)+
						3*temp[1][1]*t*pow((1-t),2)+
						3*temp[2][1]*pow(t,2)*(1-t)+
						temp[3][1]*pow(t,3);
	return vy;
}

//////////////////////////////////////////////////////////////////////////

void cgym::DrawFrontScence()
{
	DrawFrontPlat();
	DrawWriteFace();
	GLfloat d = 0.0f;
	d=2*ud_grid_step;
	DrawPlatPillar(d);
	d+=5*ud_grid_step;
	DrawPlatPillar(d);
	d= (platlength-2.5*ud_grid_step);
	DrawPlatPillar(d);
	d-=5.5*ud_grid_step;
	DrawPlatPillar(d);
	DrawFence();
	DrawStair();
	DrawStairDown();
	DrawStep();
}
void cgym::DrawFrontPlat()
{
	GLfloat vertex[3]={0};
	memcpy(vertex,down_ctrlPoints[0],sizeof(vertex));
	GLfloat xPos = down_ctrlPoints[0][0];
	plathPos = 0.45f;
	platheight = 0.05f;
	platwidth  = 0.33f;
	platlength = ud_grid_step* 32;
	
	//plat
	//up
	if(shadow)
		glColor3fv(shadowcolor);
	else
		glColor3f(0.5f,0.4f,0.45f);
	glBegin(GL_QUADS);
		glNormal3f(0.0f,1.0f,0.0f);
		glVertex3f(vertex[0],vertex[1]+plathPos,vertex[2]-4*ud_grid_step);
		glVertex3f(vertex[0]-platwidth,vertex[1]+plathPos,vertex[2]-4*ud_grid_step);
		glVertex3f(vertex[0]-platwidth,vertex[1]+plathPos,vertex[2]-platlength-4*ud_grid_step);
		glVertex3f(vertex[0],vertex[1]+plathPos,vertex[2]-platlength-4*ud_grid_step);	
	glEnd();
	//down
	GLfloat d = 0.0f;
	d-=platheight;
	if(shadow)
		glColor3fv(shadowcolor);
	else
		glColor3f(0.4f,0.5f,0.6f);
	glBegin(GL_QUADS);
		glNormal3f(0.0f,-1.0f,0.0f);
		glVertex3f(vertex[0],vertex[1]+plathPos+d,vertex[2]-4*ud_grid_step);
		glVertex3f(vertex[0]-platwidth,vertex[1]+plathPos+d,vertex[2]-4*ud_grid_step);
		glVertex3f(vertex[0]-platwidth,vertex[1]+plathPos+d,vertex[2]-platlength-4*ud_grid_step);
		glVertex3f(vertex[0],vertex[1]+plathPos+d,vertex[2]-platlength-4*ud_grid_step);	
	glEnd();

	//front
	if(shadow)
		glColor3fv(shadowcolor);
	else
		glColor3f(0.5f,0.5f,0.5f);
	glBegin(GL_QUADS);
		glNormal3f(0.0f,0.0f,1.0f);
		glVertex3f(vertex[0],vertex[1]+plathPos,vertex[2]-4*ud_grid_step);
		glVertex3f(vertex[0]-platwidth,vertex[1]+plathPos,vertex[2]-4*ud_grid_step);
		glVertex3f(vertex[0]-platwidth,vertex[1]+plathPos+d,vertex[2]-4*ud_grid_step);
		glVertex3f(vertex[0],vertex[1]+plathPos+d,vertex[2]-4*ud_grid_step);		
	glEnd();
	//back
	if(shadow)
		glColor3fv(shadowcolor);
	else
		glColor3f(0.5f,0.5f,0.5f);
	glBegin(GL_QUADS);
		glNormal3f(0.0f,0.0f,-1.0f);
		glVertex3f(vertex[0]-platwidth,vertex[1]+plathPos,vertex[2]-platlength-4*ud_grid_step);
		glVertex3f(vertex[0],vertex[1]+plathPos,vertex[2]-platlength-4*ud_grid_step);	
		glVertex3f(vertex[0],vertex[1]+plathPos+d,vertex[2]-platlength-4*ud_grid_step);	
		glVertex3f(vertex[0]-platwidth,vertex[1]+plathPos+d,vertex[2]-platlength-4*ud_grid_step);		
	glEnd();
	//left
	if(shadow)
		glColor3fv(shadowcolor);
	else
		glColor3f(0.54f,0.54f,0.54f);
	glBegin(GL_QUADS);
		glNormal3f(-1.0f,0.0f,0.0f);
		glVertex3f(vertex[0]-platwidth,vertex[1]+plathPos+d,vertex[2]-4*ud_grid_step);
		glVertex3f(vertex[0]-platwidth,vertex[1]+plathPos,vertex[2]-4*ud_grid_step);
		glVertex3f(vertex[0]-platwidth,vertex[1]+plathPos,vertex[2]-platlength-4*ud_grid_step);
		glVertex3f(vertex[0]-platwidth,vertex[1]+plathPos+d,vertex[2]-platlength-4*ud_grid_step);
	glEnd();
	//right
	if(shadow)
		glColor3fv(shadowcolor);
	else
		glColor3f(0.54f,0.54f,0.54f);
	glBegin(GL_QUADS);
		glNormal3f(-1.0f,0.0f,0.0f);
		glVertex3f(vertex[0],vertex[1]+plathPos,vertex[2]-4*ud_grid_step);
		glVertex3f(vertex[0],vertex[1]+plathPos+d,vertex[2]-4*ud_grid_step);
		glVertex3f(vertex[0],vertex[1]+plathPos+d,vertex[2]-platlength-4*ud_grid_step);
		glVertex3f(vertex[0],vertex[1]+plathPos,vertex[2]-platlength-4*ud_grid_step);		
	glEnd();
	///drow plat over

	GLfloat h = 0.12f;
	d = 0.0f;
	GLfloat sp = 0.02f;
	GLfloat upleft[3],upright[3],downleft[3],downright[3];
	for(int i = 0;i <2;i++){
			upleft[0] = vertex[0]-platwidth;upleft[1]=vertex[1]+plathPos+h;upleft[2]=vertex[2]-4*ud_grid_step+0.01f-d;
			upright[0]=vertex[0];upright[1]=vertex[1]+plathPos+h;upright[2]=vertex[2]-4*ud_grid_step+0.01f-d;
			downleft[0]=vertex[0]-platwidth;downleft[1]=vertex[1]+plathPos;downleft[2]=vertex[2]-4*ud_grid_step+0.01f-d;
			downright[0]=vertex[0];downright[1]=vertex[1]+plathPos;downright[2]=vertex[2]-4*ud_grid_step+0.01f-d;
			DrawCube(upleft,upright,downleft,downright,sp);
		d+=platlength;
	}
	
	//////////////////////////////////////////////////////////////////////////
	d= 0.0f;
	for(int i=0;i<2;i++){
		sp = 0.01f;
		upleft[0]=vertex[0]-platwidth-sp;upleft[1]=vertex[1]+plathPos+h;upleft[2]=vertex[2]-4*ud_grid_step-d;
		downleft[0]=vertex[0]-platwidth-sp;downleft[1]=vertex[1]+plathPos;downleft[2]=vertex[2]-4*ud_grid_step-d;
		sp-=0.02f;
		upright[0]=vertex[0]-platwidth-sp;upright[1]=vertex[1]+plathPos+h;upright[2]=vertex[2]-4*ud_grid_step-d;
		downright[0]=vertex[0]-platwidth-sp;downright[1]=vertex[1]+plathPos;downright[2]=vertex[2]-4*ud_grid_step-d;
		DrawCube(upleft,upright,downleft,downright,10*ud_grid_step);
		d+=22*ud_grid_step;
	}
}
//draw a cube
void cgym::DrawCube(GLfloat upleft[3],GLfloat upright[3],GLfloat downleft[3],GLfloat downright[3],GLfloat move)
{
	//front
	if(shadow)
		glColor3fv(shadowcolor);
	else
		glColor3f(0.54f,0.54f,0.54f);
	glBegin(GL_QUADS);
		glNormal3f(0.0f,0.0f,1.0f);
		glVertex3fv(upleft);
		glVertex3fv(downleft);
		glVertex3fv(downright);
		glVertex3fv(upright);
	glEnd();
	//back
	glBegin(GL_QUADS);
		glNormal3f(0.0f,0.0f,-1.0f);
		glVertex3f(upleft[0],upleft[1],upleft[2]-move);
		glVertex3f(upright[0],upright[1],upright[2]-move);
		glVertex3f(downright[0],downright[1],downright[2]-move);
		glVertex3f(downleft[0],downleft[1],downleft[2]-move);		
	glEnd();
	//left
	if(shadow)
		glColor3fv(shadowcolor);
	else
		glColor3f(0.60f,0.60f,0.60f);
	glBegin(GL_QUADS);
		glNormal3f(-1.0f,0.0f,0.0f);
		glVertex3fv(downleft);
		glVertex3fv(upleft);	
		glVertex3f(upleft[0],upleft[1],upleft[2]-move);
		glVertex3f(downleft[0],downleft[1],downleft[2]-move);		
	glEnd();
	//right
	if(shadow)
		glColor3fv(shadowcolor);
	else
		glColor3f(0.68f,0.68f,0.68f);
	glBegin(GL_QUADS);
		glNormal3f(1.0f,0.0f,0.0f);
		glVertex3fv(upright);
		glVertex3fv(downright);		
		glVertex3f(downright[0],downright[1],downright[2]-move);
		glVertex3f(upright[0],upright[1],upright[2]-move);		
	glEnd();
	//up
	if(shadow)
		glColor3fv(shadowcolor);
	else
		glColor3f(0.50f,0.4f,0.4f);
	glBegin(GL_QUADS);
		glNormal3f(0.0f,1.0f,0.0f);
		glVertex3fv(upleft);	
		glVertex3fv(upright);
		glVertex3f(upright[0],upright[1],upright[2]-move);		
		glVertex3f(upleft[0],upleft[1],upleft[2]-move);		
	glEnd();
	//down
	glBegin(GL_QUADS);
		glNormal3f(0.0f,-1.0f,0.0f);
		glVertex3fv(downright);		
		glVertex3fv(downleft);
		glVertex3f(downleft[0],downleft[1],downleft[2]-move);	
		glVertex3f(downright[0],downright[1],downright[2]-move);		
	glEnd();
}


//plat front four pillars
void cgym::DrawPlatPillar(GLfloat move)
{
	GLfloat vertex[3]={0};
	memcpy(vertex,down_ctrlPoints[0],sizeof(vertex));
	vertex[1]=horizon_level;
	GLfloat sp = 0.02f;
	GLfloat w = 0.04f;
	GLfloat h = 0.12f;
	if(shadow)
		glColor3fv(shadowcolor);
	else
		glColor3f(0.5f,0.5f,0.5f);
	//down
	glBegin(GL_QUADS);		
		glNormal3f(0.0f,-1.0f,0.0f);
		glVertex3f(vertex[0]-platwidth+2*sp,vertex[1],vertex[2]-4*ud_grid_step-move);
		glVertex3f(vertex[0]-platwidth-2*sp,vertex[1],vertex[2]-4*ud_grid_step-move);
		glVertex3f(vertex[0]-platwidth-2*sp,vertex[1],vertex[2]-4*ud_grid_step-w-move);
		glVertex3f(vertex[0]-platwidth+2*sp,vertex[1],vertex[2]-4*ud_grid_step-w-move);
	glEnd();
	//up
	if(shadow)
		glColor3fv(shadowcolor);
	else
		glColor3f(0.5f,0.5f,0.5f);
	glBegin(GL_QUADS);
		glNormal3f(0.0f,1.0f,0.0f);
		glVertex3f(vertex[0]-platwidth+2*sp,vertex[1]+plathPos+h+0.015f,vertex[2]-4*ud_grid_step-sp-move);
		glVertex3f(vertex[0]-platwidth-0.5*sp,vertex[1]+plathPos+h+0.015f,vertex[2]-4*ud_grid_step-sp-move);
		glVertex3f(vertex[0]-platwidth-0.5*sp,vertex[1]+plathPos+h+0.015f,vertex[2]-4*ud_grid_step-w-sp-move);
		glVertex3f(vertex[0]-platwidth+2*sp,vertex[1]+plathPos+h+0.015f,vertex[2]-4*ud_grid_step-w-sp-move);
	glEnd();
	//left
	if(shadow)
		glColor3fv(shadowcolor);
	else
		glColor3f(0.68f,0.68f,0.68f);
	glBegin(GL_QUADS);
		glNormal3f(-1.0f,0.0f,0.0f);
		glVertex3f(vertex[0]-platwidth-2*sp,vertex[1],vertex[2]-4*ud_grid_step-move);
		glVertex3f(vertex[0]-platwidth-0.5*sp,vertex[1]+plathPos+h+0.015f,vertex[2]-4*ud_grid_step-sp-move);		
		glVertex3f(vertex[0]-platwidth-0.5*sp,vertex[1]+plathPos+h+0.015f,vertex[2]-4*ud_grid_step-w-sp-move);
		glVertex3f(vertex[0]-platwidth-2*sp,vertex[1],vertex[2]-4*ud_grid_step-w-move);		
	glEnd();
	//right
	if(shadow)
		glColor3fv(shadowcolor);
	else
		glColor3f(0.4f,0.5f,0.5f);
	glBegin(GL_QUADS);
		glNormal3f(1.0f,0.0f,0.0f);
		glVertex3f(vertex[0]-platwidth+2*sp,vertex[1],vertex[2]-4*ud_grid_step-move);
		glVertex3f(vertex[0]-platwidth+2*sp,vertex[1],vertex[2]-4*ud_grid_step-w-move);
		glVertex3f(vertex[0]-platwidth+2*sp,vertex[1]+plathPos+h+0.015f,vertex[2]-4*ud_grid_step-w-sp-move);
		glVertex3f(vertex[0]-platwidth+2*sp,vertex[1]+plathPos+h+0.015f,vertex[2]-4*ud_grid_step-sp-move);
	glEnd();
	//back
	if(shadow)
		glColor3fv(shadowcolor);
	else
		glColor3f(0.58f,0.58f,0.58f);	
	glBegin(GL_QUADS);
		glNormal3f(0.0f,0.0f,-1.0f);
		glVertex3f(vertex[0]-platwidth+2*sp,vertex[1],vertex[2]-4*ud_grid_step-w-move);
		glVertex3f(vertex[0]-platwidth-2*sp,vertex[1],vertex[2]-4*ud_grid_step-w-move);
		glVertex3f(vertex[0]-platwidth-0.5*sp,vertex[1]+plathPos+h+0.015f,vertex[2]-4*ud_grid_step-w-sp-move);
		glVertex3f(vertex[0]-platwidth+2*sp,vertex[1]+plathPos+h+0.015f,vertex[2]-4*ud_grid_step-w-sp-move);		
	glEnd();
	//front
	if(shadow)
		glColor3fv(shadowcolor);
	else
		glColor3f(0.58f,0.58f,0.58f);
	glBegin(GL_QUADS);
		glNormal3f(0.0f,0.0f,1.0f);
		glVertex3f(vertex[0]-platwidth-2*sp,vertex[1],vertex[2]-4*ud_grid_step-move);
		glVertex3f(vertex[0]-platwidth+2*sp,vertex[1],vertex[2]-4*ud_grid_step-move);
		glVertex3f(vertex[0]-platwidth+2*sp,vertex[1]+plathPos+h+0.015f,vertex[2]-4*ud_grid_step-sp-move);
		glVertex3f(vertex[0]-platwidth-0.5*sp,vertex[1]+plathPos+h+0.015f,vertex[2]-4*ud_grid_step-sp-move);				
	glEnd();

}
void cgym::DrawFence()
{
	GLfloat vertex[3]={0};
	memcpy(vertex,down_ctrlPoints[0],sizeof(vertex));
	stair_angel = 25;
	GLfloat sp = 0.01f;
	GLfloat h = 0.12f;
	GLfloat d = 0.0f;
	GLfloat xd = 0.350f;
	GLfloat yd = tan((PI/180.0f)*stair_angel)*xd;
	GLfloat fold_angle = 30;
	GLfloat zl = 0.06f;
	GLfloat xl = zl/tan((PI/180.0f)*fold_angle);//0.1039
	GLfloat xe = (plathPos+h-yd-0.1f)/tan((PI/180.0f)*stair_angel);//0.8724 0.658

	GLfloat  dw = 0.0f;
	if(shadow)
		glColor3fv(shadowcolor);
	else
		glColor3f(0.6f,0.3f,0.1f);

	//right
	d=0.0f;
	GLfloat xm = 0.29f;
	GLfloat ym = tan((PI/180.0f)*stair_angel)*xm;
	GLfloat zshift = 0;
	GLfloat upleft[3],upright[3],downleft[3],downright[3],color[3];
	upleft[0]=vertex[0]-platwidth+sp-xd;upleft[1]=vertex[1]+plathPos+h-yd;upleft[2]=vertex[2]-4*ud_grid_step-10*ud_grid_step-zshift;
	upright[0]=vertex[0]-platwidth+sp;upright[1]=vertex[1]+plathPos+h;upright[2]=vertex[2]-4*ud_grid_step-10*ud_grid_step-zshift;
	downleft[0]=vertex[0]-platwidth+sp-xd;downleft[1]=vertex[1]+plathPos-yd;downleft[2]=vertex[2]-4*ud_grid_step-10*ud_grid_step-zshift;
	downright[0]=vertex[0]-platwidth+sp;downright[1]=vertex[1]+plathPos;downright[2]=vertex[2]-4*ud_grid_step-10*ud_grid_step-zshift;
	color[0]=0.6f,color[1]=0.6f;color[2]=0.6f;
	DrawWall(upleft,upright,downleft,downright,0.02f,color);

	memcpy(upright,upleft,sizeof(upleft));
	memcpy(downright,downleft,sizeof(downleft));
	upleft[0]=vertex[0]-platwidth+sp-xd-xl;upleft[1]=vertex[1]+plathPos+h-yd;upleft[2]=vertex[2]-4*ud_grid_step-10*ud_grid_step+zl-zshift;
	downleft[0]=vertex[0]-platwidth+sp-xd-xl;downleft[1]=vertex[1]+plathPos-yd;downleft[2]=vertex[2]-4*ud_grid_step-10*ud_grid_step+zl-zshift;
	color[0]=0.56f,color[1]=0.56f;color[2]=0.56f;
	DrawWall(upleft,upright,downleft,downright,0.02f,color);

	memcpy(upright,upleft,sizeof(upleft));
	memcpy(downright,downleft,sizeof(downleft));
	upleft[0]=vertex[0]-platwidth+sp-xd-xl-xe+xm;upleft[1]=horizon_level+ym;upleft[2]=vertex[2]-4*ud_grid_step-10*ud_grid_step+zl-zshift;
	downleft[0]=vertex[0]-platwidth+sp-xd-xl-xe+xm;downleft[1]=horizon_level-h+ym;downleft[2]=vertex[2]-4*ud_grid_step-10*ud_grid_step+zl-zshift;
	color[0]=0.6f,color[1]=0.6f;color[2]=0.6f;
	DrawWall(upleft,upright,downleft,downright,0.02f,color);

	if(shadow)
		glColor3fv(shadowcolor);
	else
		glColor3f(0.5f,0.5f,0);

	glBegin(GL_QUADS);
	glVertex3fv(downleft);
	glVertex3fv(upleft);
	glVertex3f(upleft[0],upleft[1],upleft[2]-0.02f);
	glVertex3f(downleft[0],downleft[1],downleft[2]-0.02f);		
	glEnd();

	//left
	zshift = 12*ud_grid_step-0.02f;

	upleft[0]=vertex[0]-platwidth+sp-xd;upleft[1]=vertex[1]+plathPos+h-yd;upleft[2]=vertex[2]-4*ud_grid_step-10*ud_grid_step-zshift;
	upright[0]=vertex[0]-platwidth+sp;upright[1]=vertex[1]+plathPos+h;upright[2]=vertex[2]-4*ud_grid_step-10*ud_grid_step-zshift;
	downleft[0]=vertex[0]-platwidth+sp-xd;downleft[1]=vertex[1]+plathPos-yd;downleft[2]=vertex[2]-4*ud_grid_step-10*ud_grid_step-zshift;
	downright[0]=vertex[0]-platwidth+sp;downright[1]=vertex[1]+plathPos;downright[2]=vertex[2]-4*ud_grid_step-10*ud_grid_step-zshift;
	color[0]=0.6f,color[1]=0.6f;color[2]=0.6f;
	DrawWall(upleft,upright,downleft,downright,0.02f,color);

	
	memcpy(upright,upleft,sizeof(upleft));
	memcpy(downright,downleft,sizeof(downleft));
	upleft[0]=vertex[0]-platwidth+sp-xd-xl;upleft[1]=vertex[1]+plathPos+h-yd;upleft[2]=vertex[2]-4*ud_grid_step-10*ud_grid_step-zl-zshift;
	downleft[0]=vertex[0]-platwidth+sp-xd-xl;downleft[1]=vertex[1]+plathPos-yd;downleft[2]=vertex[2]-4*ud_grid_step-10*ud_grid_step-zl-zshift;
	color[0]=0.56f,color[1]=0.56f;color[2]=0.56f;
	DrawWall(upleft,upright,downleft,downright,0.02f,color);
	
	memcpy(upright,upleft,sizeof(upleft));
	memcpy(downright,downleft,sizeof(downleft));
	upleft[0]=vertex[0]-platwidth+sp-xd-xl-xe+xm;upleft[1]=horizon_level+ym;upleft[2]=vertex[2]-4*ud_grid_step-10*ud_grid_step-zl-zshift;
	downleft[0]=vertex[0]-platwidth+sp-xd-xl-xe+xm;downleft[1]=horizon_level-h+ym;downleft[2]=vertex[2]-4*ud_grid_step-10*ud_grid_step-zl-zshift;
	color[0]=0.6f,color[1]=0.6f;color[2]=0.6f;
	DrawWall(upleft,upright,downleft,downright,0.02f,color);

	if(shadow)
		glColor3fv(shadowcolor);
	else
		glColor3f(0.5f,0.5f,0);
	glBegin(GL_QUADS);
	glVertex3fv(downleft);
	glVertex3fv(upleft);
	glVertex3f(upleft[0],upleft[1],upleft[2]-0.02f);
	glVertex3f(downleft[0],downleft[1],downleft[2]-0.02f);		
	glEnd();
}
void cgym::DrawStairDown()
{
	GLfloat vertex[3]={0};
	memcpy(vertex,down_ctrlPoints[0],sizeof(vertex));
	stair_angel = 25;
	GLfloat sp = 0.01f;
	GLfloat h = 0.12f;
	GLfloat d = 0.0f;
	GLfloat xd = 0.350f;
	GLfloat yd = tan((PI/180.0f)*stair_angel)*xd;
	GLfloat fold_angle = 30;
	GLfloat zl = 0.06f;
	GLfloat xl = zl/tan((PI/180.0f)*fold_angle);//0.1039
	GLfloat xe = (plathPos+h-yd-0.1f)/tan((PI/180.0f)*stair_angel);//0.8724 0.658
	if(shadow)
		glColor3fv(shadowcolor);
	else
		glColor3f(0.5f,0.5f,0.5f);
	//back
	GLfloat zshift = 12*ud_grid_step-0.02f;
	glBegin(GL_QUADS);
		glNormal3f(1.0f,0.0f,0.0f);
		glVertex3f(vertex[0]-platwidth+sp,vertex[1]+plathPos,vertex[2]-4*ud_grid_step-10*ud_grid_step-zshift);
		glVertex3f(vertex[0]-platwidth+sp,vertex[1]+plathPos,vertex[2]-4*ud_grid_step-10*ud_grid_step-0.02f);
		glVertex3f(vertex[0]-platwidth+sp,horizon_level,vertex[2]-4*ud_grid_step-10*ud_grid_step-0.02f);
		glVertex3f(vertex[0]-platwidth+sp,horizon_level,vertex[2]-4*ud_grid_step-10*ud_grid_step-zshift);
	glEnd();

	//right
	glBegin(GL_QUADS);
		glNormal3f(0.0f,0.0f,1.0f);
		glVertex3f(vertex[0]-platwidth+sp,vertex[1]+plathPos,vertex[2]-4*ud_grid_step-10*ud_grid_step-0.02);
		glVertex3f(vertex[0]-platwidth+sp-xd,vertex[1]+plathPos-yd,vertex[2]-4*ud_grid_step-10*ud_grid_step-0.02);
		glVertex3f(vertex[0]-platwidth+sp-xd,horizon_level,vertex[2]-4*ud_grid_step-10*ud_grid_step-0.02);
		glVertex3f(vertex[0]-platwidth+sp,horizon_level,vertex[2]-4*ud_grid_step-10*ud_grid_step-0.02);
	glEnd();
	glBegin(GL_QUADS);
		glNormal3f(0.0f,0.0f,1.0f);
		glVertex3f(vertex[0]-platwidth+sp-xd,vertex[1]+plathPos-yd,vertex[2]-4*ud_grid_step-10*ud_grid_step-0.02);
		glVertex3f(vertex[0]-platwidth+sp-xd-xl,vertex[1]+plathPos-yd,vertex[2]-4*ud_grid_step-10*ud_grid_step+zl-0.02);
		glVertex3f(vertex[0]-platwidth+sp-xd-xl,horizon_level,vertex[2]-4*ud_grid_step-10*ud_grid_step+zl-0.02);
		glVertex3f(vertex[0]-platwidth+sp-xd,horizon_level,vertex[2]-4*ud_grid_step-10*ud_grid_step-0.02);	
	glEnd();
	GLfloat xm = 0.29f;
	GLfloat ym = tan((PI/180.0f)*stair_angel)*xm;
	glBegin(GL_TRIANGLE_STRIP);
		glNormal3f(0.0f,0.0f,1.0f);
		glVertex3f(vertex[0]-platwidth+sp-xd-xl,vertex[1]+plathPos-yd,vertex[2]-4*ud_grid_step-10*ud_grid_step+zl-0.02);
		glVertex3f(vertex[0]-platwidth+sp-xd-xl-xe+xm-0.003,horizon_level-h+ym,vertex[2]-4*ud_grid_step-10*ud_grid_step+zl-0.02);
		glVertex3f(vertex[0]-platwidth+sp-xd-xl,horizon_level,vertex[2]-4*ud_grid_step-10*ud_grid_step+zl-0.02);	
	glEnd();
	//left
	glBegin(GL_QUADS);
		glNormal3f(0.0f,0.0f,1.0f);
		glVertex3f(vertex[0]-platwidth+sp-xd,vertex[1]+plathPos-yd,vertex[2]-4*ud_grid_step-10*ud_grid_step-zshift);
		glVertex3f(vertex[0]-platwidth+sp,vertex[1]+plathPos,vertex[2]-4*ud_grid_step-10*ud_grid_step-zshift);
		glVertex3f(vertex[0]-platwidth+sp,horizon_level,vertex[2]-4*ud_grid_step-10*ud_grid_step-zshift);
		glVertex3f(vertex[0]-platwidth+sp-xd,horizon_level,vertex[2]-4*ud_grid_step-10*ud_grid_step-zshift);
	glEnd();

	glBegin(GL_QUADS);
		glNormal3f(0.0f,0.0f,1.0f);
		glVertex3f(vertex[0]-platwidth+sp-xd-xl,vertex[1]+plathPos-yd,vertex[2]-4*ud_grid_step-10*ud_grid_step-zl-zshift);
		glVertex3f(vertex[0]-platwidth+sp-xd,vertex[1]+plathPos-yd,vertex[2]-4*ud_grid_step-10*ud_grid_step-zshift);
		glVertex3f(vertex[0]-platwidth+sp-xd,horizon_level,vertex[2]-4*ud_grid_step-10*ud_grid_step-zshift);	
		glVertex3f(vertex[0]-platwidth+sp-xd-xl,horizon_level,vertex[2]-4*ud_grid_step-10*ud_grid_step-zl-zshift);
	glEnd();

	glBegin(GL_TRIANGLE_STRIP);
	glNormal3f(0.0f,0.0f,1.0f);
		glVertex3f(vertex[0]-platwidth+sp-xd-xl,vertex[1]+plathPos-yd,vertex[2]-4*ud_grid_step-10*ud_grid_step-zl-zshift);
		glVertex3f(vertex[0]-platwidth+sp-xd-xl,horizon_level,vertex[2]-4*ud_grid_step-10*ud_grid_step-zl-zshift);	
		glVertex3f(vertex[0]-platwidth+sp-xd-xl-xe+xm-0.003,horizon_level-h+ym,vertex[2]-4*ud_grid_step-10*ud_grid_step-zl-zshift);	
	glEnd();
}

void cgym::DrawStair()
{
	GLfloat vertex[3]={0};
	memcpy(vertex,down_ctrlPoints[0],sizeof(vertex));
	GLfloat sp = 0.01f;//stair height
	GLfloat d = 0.0f;
	GLfloat h = 0.12f;
	GLfloat xd = 0.350f;
	GLfloat yd = tan((PI/180.0f)*stair_angel)*xd;

	GLfloat eyd = 0.015;
	GLfloat exd = eyd/tan((PI/180.0f)*stair_angel);
	GLfloat zshift = 12*ud_grid_step;
	GLfloat d1=0.0f,d2=0.0f;
	GLfloat fold_angle = 30;
	GLfloat zl = 0.06f;
	GLfloat xl = zl/tan((PI/180.0f)*fold_angle);	

	//for mapping texture
	glDepthFunc(GL_LEQUAL);							
	glEnable(GL_DEPTH_TEST);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glBindTexture( GL_TEXTURE_2D, texture->get_texture(WALL6_TEXTURE));
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND); 
	GLfloat et = 1.0f/zshift*0.5f;
	GLfloat s=0.0f,t=0.0f;
	while(d2<xd-exd){
		if(shadow)
			glColor3fv(shadowcolor);
		else
			glColor3f(0.56f,0.56f,0.56f);
		
		glBegin(GL_QUADS);
			glNormal3f(-1.0f,0.0f,0.0f);
			glVertex3f(vertex[0]-platwidth+sp-d2,vertex[1]+plathPos-d1,vertex[2]-4*ud_grid_step-10*ud_grid_step-d-0.02f);
			glVertex3f(vertex[0]-platwidth+sp-d2,vertex[1]+plathPos-d1,vertex[2]-4*ud_grid_step-10*ud_grid_step-zshift+0.02f);
			d1+=eyd;
			glVertex3f(vertex[0]-platwidth+sp-d2,vertex[1]+plathPos-d1,vertex[2]-4*ud_grid_step-10*ud_grid_step-zshift+0.02f);
			glVertex3f(vertex[0]-platwidth+sp-d2,vertex[1]+plathPos-d1,vertex[2]-4*ud_grid_step-10*ud_grid_step-d-0.02f);
		glEnd();

		if(shadow)
			glColor3fv(shadowcolor);
		else
			glColor3f(0.6f,0.6f,0.6f);
		glBegin(GL_QUADS);
			glNormal3f(0.0f,1.0f,0.0f);
			glVertex3f(vertex[0]-platwidth+sp-d2,vertex[1]+plathPos-d1,vertex[2]-4*ud_grid_step-10*ud_grid_step-d-0.02f);
			glVertex3f(vertex[0]-platwidth+sp-d2,vertex[1]+plathPos-d1,vertex[2]-4*ud_grid_step-10*ud_grid_step-zshift+0.02f);
			d2+=exd;
			glVertex3f(vertex[0]-platwidth+sp-d2,vertex[1]+plathPos-d1,vertex[2]-4*ud_grid_step-10*ud_grid_step-zshift+0.02f);
			glVertex3f(vertex[0]-platwidth+sp-d2,vertex[1]+plathPos-d1,vertex[2]-4*ud_grid_step-10*ud_grid_step-d-0.02f);
		glEnd();
	}
	GLfloat m = (xd-d2)*tan((PI/180.0f)*stair_angel);
	if(shadow)
		glColor3fv(shadowcolor);
	else
		glColor3f(0.56f,0.56f,0.56f);
	glBegin(GL_QUADS);
		glNormal3f(-1.0f,0.0f,0.0f);
		glVertex3f(vertex[0]-platwidth+sp-d2,vertex[1]+plathPos-d1,vertex[2]-4*ud_grid_step-10*ud_grid_step-d-0.02f);
		glVertex3f(vertex[0]-platwidth+sp-d2,vertex[1]+plathPos-d1,vertex[2]-4*ud_grid_step-10*ud_grid_step-zshift+0.02f);
		glVertex3f(vertex[0]-platwidth+sp-d2,vertex[1]+plathPos-d1-m,vertex[2]-4*ud_grid_step-10*ud_grid_step-zshift+0.02f);
		glVertex3f(vertex[0]-platwidth+sp-d2,vertex[1]+plathPos-d1-m,vertex[2]-4*ud_grid_step-10*ud_grid_step-d-0.02f);		
	glEnd();
	glColor3f(0.6f,0.6f,0.6f);
	glBegin(GL_QUADS);
		glNormal3f(0.0f,1.0f,0.0f);
		glVertex3f(vertex[0]-platwidth+sp-d2,vertex[1]+plathPos-d1-m,vertex[2]-4*ud_grid_step-10*ud_grid_step-d-0.02f);		
		glVertex3f(vertex[0]-platwidth+sp-d2,vertex[1]+plathPos-d1-m,vertex[2]-4*ud_grid_step-10*ud_grid_step-zshift+0.02f);
		glVertex3f(vertex[0]-platwidth+sp-d2-(xd-d2),vertex[1]+plathPos-d1-m,vertex[2]-4*ud_grid_step-10*ud_grid_step-zshift+0.02f);
		glVertex3f(vertex[0]-platwidth+sp-d2-(xd-d2),vertex[1]+plathPos-d1-m,vertex[2]-4*ud_grid_step-10*ud_grid_step-d-0.02f);
	glEnd();
	if(shadow)
		glColor3fv(shadowcolor);
	else
		glColor3f(0.6f,0.6f,0.6f);
	glBegin(GL_QUADS);
		glNormal3f(0.0f,1.0f,0.0f);
		glVertex3f(vertex[0]-platwidth+sp-xd,vertex[1]+plathPos-yd,vertex[2]-4*ud_grid_step-10*ud_grid_step-d-0.02f);
		glVertex3f(vertex[0]-platwidth+sp-xd,vertex[1]+plathPos-yd,vertex[2]-4*ud_grid_step-10*ud_grid_step-d-zshift+0.02f);
		glVertex3f(vertex[0]-platwidth+sp-xd-xl,vertex[1]+plathPos-yd,vertex[2]-4*ud_grid_step-10*ud_grid_step-d-zl-zshift+0.02f);
		glVertex3f(vertex[0]-platwidth+sp-xd-xl,vertex[1]+plathPos-yd,vertex[2]-4*ud_grid_step-10*ud_grid_step-d+zl-0.02f);
	glEnd();

	GLfloat xe = (plathPos+h-yd)/tan((PI/180.0f)*stair_angel);
	xe-=0.51f;
	d1=d2=0;
	while(d2<xe){
		if(shadow)
			glColor3fv(shadowcolor);
		else
			glColor3f(0.50f,0.50f,0.50f);
		glBegin(GL_QUADS);
			glNormal3f(-1.0f,0.0f,0.0f);
			glVertex3f(vertex[0]-platwidth+sp-xd-xl-d2,vertex[1]+plathPos-yd-d1,vertex[2]-4*ud_grid_step-10*ud_grid_step-d+zl-0.02f);
			glVertex3f(vertex[0]-platwidth+sp-xd-xl-d2,vertex[1]+plathPos-yd-d1,vertex[2]-4*ud_grid_step-10*ud_grid_step-d-zl-zshift+0.02f);
			d1+=eyd;
			glVertex3f(vertex[0]-platwidth+sp-xd-xl-d2,vertex[1]+plathPos-yd-d1,vertex[2]-4*ud_grid_step-10*ud_grid_step-d-zl-zshift+0.02f);
			glVertex3f(vertex[0]-platwidth+sp-xd-xl-d2,vertex[1]+plathPos-yd-d1,vertex[2]-4*ud_grid_step-10*ud_grid_step-d+zl-0.02f);
		glEnd();
		if(shadow)
			glColor3fv(shadowcolor);
		else
			glColor3f(0.6f,0.6f,0.6f);
		glBegin(GL_QUADS);
			glNormal3f(0.0f,1.0f,0.0f);
			glVertex3f(vertex[0]-platwidth+sp-xd-xl-d2,vertex[1]+plathPos-yd-d1,vertex[2]-4*ud_grid_step-10*ud_grid_step-d+zl-0.02f);
			glVertex3f(vertex[0]-platwidth+sp-xd-xl-d2,vertex[1]+plathPos-yd-d1,vertex[2]-4*ud_grid_step-10*ud_grid_step-d-zl-zshift+0.02f);
			d2+=exd;
			glVertex3f(vertex[0]-platwidth+sp-xd-xl-d2,vertex[1]+plathPos-yd-d1,vertex[2]-4*ud_grid_step-10*ud_grid_step-d-zl-zshift+0.02f);
			glVertex3f(vertex[0]-platwidth+sp-xd-xl-d2,vertex[1]+plathPos-yd-d1,vertex[2]-4*ud_grid_step-10*ud_grid_step-d+zl-0.02f);
		glEnd();
	}

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
}

void cgym::DrawWall(GLfloat upleft[3],GLfloat upright[3],GLfloat downleft[3],GLfloat downright[3],GLfloat move,GLfloat color[3])
{
	//up
	if(shadow)
		glColor3fv(shadowcolor);
	else
		glColor3f(0.54f,0.54f,0.54f);
	glBegin(GL_QUADS);
		glNormal3f(0.0f,1.0f,0.0f);
		glVertex3fv(upleft);	
		glVertex3fv(upright);
		glVertex3f(upright[0],upright[1],upright[2]-move);		
		glVertex3f(upleft[0],upleft[1],upleft[2]-move);		
	glEnd();

	//down
	glBegin(GL_QUADS);
		glNormal3f(0.0f,-1.0f,0.0f);
		glVertex3fv(downright);		
		glVertex3fv(downleft);
		glVertex3f(downleft[0],downleft[1],downleft[2]-move);	
		glVertex3f(downright[0],downright[1],downright[2]-move);		
	glEnd();

	//front
	if(shadow)
		glColor3fv(shadowcolor);
	else
		glColor3f(color[0],color[1],color[2]);
	glBegin(GL_QUADS);
		glNormal3f(0.0f,0.0f,1.0f);
		glVertex3fv(upleft);
		glVertex3fv(downleft);
		glVertex3fv(downright);
		glVertex3fv(upright);
	glEnd();

	//back
	glBegin(GL_QUADS);
		glNormal3f(0.0f,0.0f,-1.0f);
		glVertex3f(upleft[0],upleft[1],upleft[2]-move);
		glVertex3f(upright[0],upright[1],upright[2]-move);
		glVertex3f(downright[0],downright[1],downright[2]-move);
		glVertex3f(downleft[0],downleft[1],downleft[2]-move);		
	glEnd();
}

void cgym::DrawFrontDoor()
{
	GLfloat bottomright[3];
	bottomright[0]=down_ctrlPoints[0][0];
	bottomright[1]=down_ctrlPoints[0][1]+plathPos;
	bottomright[2]=down_ctrlPoints[0][2]-4*ud_grid_step-10*ud_grid_step-1.5*ud_grid_step;
	GLfloat doorheight = 0.3f;
	GLfloat doorwidth = 9*ud_grid_step;
	
	GLfloat out = 0.01f;
	GLfloat edgewidth = 0.03f;

	GLfloat upleft[3],upright[3],downleft[3],downright[3];
	upleft[0]=bottomright[0]-out;upleft[1]=bottomright[1]+doorheight+edgewidth;upleft[2]=bottomright[2];
	upright[0]=bottomright[0];upright[1]=bottomright[1]+doorheight+edgewidth;upright[2]=bottomright[2];
	downleft[0]=bottomright[0]-out;downleft[1]=bottomright[1];downleft[2]=bottomright[2];
	downright[0]=bottomright[0];downright[1]=bottomright[1];downright[2]=bottomright[2];
	DrawCube(upleft,upright,downleft,downright,edgewidth);

	downleft[0]=bottomright[0]-out;downleft[1]=bottomright[1]+doorheight;downleft[2]=bottomright[2];
	downright[0]=bottomright[0];downright[1]=bottomright[1]+doorheight;downright[2]=bottomright[2];
	DrawCube(upleft,upright,downleft,downright,9*ud_grid_step+edgewidth);

	upleft[0]=bottomright[0]-out;upleft[1]=bottomright[1]+doorheight+edgewidth;upleft[2]=bottomright[2]-9*ud_grid_step;
	upright[0]=bottomright[0];upright[1]=bottomright[1]+doorheight+edgewidth;upright[2]=bottomright[2]-9*ud_grid_step;
	downleft[0]=bottomright[0]-out;downleft[1]=bottomright[1];downleft[2]=bottomright[2]-9*ud_grid_step;
	downright[0]=bottomright[0];downright[1]=bottomright[1];downright[2]=bottomright[2]-9*ud_grid_step;
	DrawCube(upleft,upright,downleft,downright,edgewidth);

	//inner

	upleft[0]=bottomright[0]-out;upleft[1]=bottomright[1]+doorheight;upleft[2]=bottomright[2]-2*ud_grid_step;
	upright[0]=bottomright[0];upright[1]=bottomright[1]+doorheight;upright[2]=bottomright[2]-2*ud_grid_step;
	downleft[0]=bottomright[0]-out;downleft[1]=bottomright[1];downleft[2]=bottomright[2]-2*ud_grid_step;
	downright[0]=bottomright[0];downright[1]=bottomright[1];downright[2]=bottomright[2]-2*ud_grid_step;
	DrawCube(upleft,upright,downleft,downright,edgewidth);

	upleft[0]=bottomright[0]-out;upleft[1]=bottomright[1]+doorheight;upleft[2]=bottomright[2]-2*ud_grid_step-5*ud_grid_step+edgewidth;
	upright[0]=bottomright[0];upright[1]=bottomright[1]+doorheight;upright[2]=bottomright[2]-2*ud_grid_step-5*ud_grid_step+edgewidth;
	downleft[0]=bottomright[0]-out;downleft[1]=bottomright[1];downleft[2]=bottomright[2]-2*ud_grid_step-5*ud_grid_step+edgewidth;
	downright[0]=bottomright[0];downright[1]=bottomright[1];downright[2]=bottomright[2]-2*ud_grid_step-5*ud_grid_step+edgewidth;
	DrawCube(upleft,upright,downleft,downright,edgewidth);

	//draw gym title
	GLfloat pos[3]={0,0,0};
	pos[0]=bottomright[0]-out;
	pos[1]=bottomright[1]+doorheight+0.15f;
	pos[2]=bottomright[2]-9*ud_grid_step+0.05f;
	DrawName(pos);

	glDepthFunc(GL_LEQUAL);							
	glEnable(GL_DEPTH_TEST);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glBindTexture( GL_TEXTURE_2D, texture->get_texture(DOOR_TEXTURE));
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND); 
	glColor3f(1.0f,1.0f,1.0f);
	out=0.005f;
	glBegin(GL_QUADS);	
		upleft[0]=bottomright[0]-out;upleft[1]=bottomright[1]+doorheight;upleft[2]=bottomright[2]-2*ud_grid_step;
		upright[0]=bottomright[0]-out;upright[1]=bottomright[1]+doorheight;upright[2]=bottomright[2]-edgewidth;
		downleft[0]=bottomright[0]-out;downleft[1]=bottomright[1];downleft[2]=bottomright[2]-2*ud_grid_step;
		downright[0]=bottomright[0]-out;downright[1]=bottomright[1];downright[2]=bottomright[2]-edgewidth;
		glNormal3f(-1.0f,0.0f,0.0f);
		glTexCoord2f(0.01f,0.0f);glVertex3fv(downleft);
		glTexCoord2f(0.72f,0.0f);glVertex3fv(downright);
		glTexCoord2f(0.72f,1.0f);glVertex3fv(upright);
		glTexCoord2f(0.01f,1.0f);glVertex3fv(upleft);
	glEnd();
	glBegin(GL_QUADS);
		upleft[0]=bottomright[0]-out;upleft[1]=bottomright[1]+doorheight;upleft[2]=bottomright[2]-7*ud_grid_step+edgewidth;
		upright[0]=bottomright[0]-out;upright[1]=bottomright[1]+doorheight;upright[2]=bottomright[2]-2*ud_grid_step-edgewidth;
		downleft[0]=bottomright[0]-out;downleft[1]=bottomright[1];downleft[2]=bottomright[2]-7*ud_grid_step+edgewidth;
		downright[0]=bottomright[0]-out;downright[1]=bottomright[1];downright[2]=bottomright[2]-2*ud_grid_step-edgewidth;
		glNormal3f(-1.0f,0.0f,0.0f);
		glTexCoord2f(0.01f,0.0f);glVertex3fv(downleft);
		glTexCoord2f(0.72f,0.0f);glVertex3fv(downright);
		glTexCoord2f(0.72f,1.0f);glVertex3fv(upright);
		glTexCoord2f(0.01f,1.0f);glVertex3fv(upleft);
	glEnd();
	glBegin(GL_QUADS);
		upleft[0]=bottomright[0]-out;upleft[1]=bottomright[1]+doorheight;upleft[2]=bottomright[2]-9*ud_grid_step;
		upright[0]=bottomright[0]-out;upright[1]=bottomright[1]+doorheight;upright[2]=bottomright[2]-7*ud_grid_step;
		downleft[0]=bottomright[0]-out;downleft[1]=bottomright[1];downleft[2]=bottomright[2]-9*ud_grid_step;
		downright[0]=bottomright[0]-out;downright[1]=bottomright[1];downright[2]=bottomright[2]-7*ud_grid_step;
		glNormal3f(-1.0f,0.0f,0.0f);
		glTexCoord2f(0.01f,0.0f);glVertex3fv(downleft);
		glTexCoord2f(0.72f,0.0f);glVertex3fv(downright);
		glTexCoord2f(0.72f,1.0f);glVertex3fv(upright);
		glTexCoord2f(0.01f,1.0f);glVertex3fv(upleft);
		glEnd();
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	DrawDownDoor();
}

void cgym::DrawRightDoor()
{
		glColor3f(0.68f,0.68f,0.68f);
		GLfloat base=-0.65f;
		GLfloat upleft[3],upright[3],downleft[3],downright[3];
		GLfloat edgewidth = 0.02f,height = 0.25f;
		GLfloat d = 0.02f;
		GLfloat move = 0.02f;
		upleft[0]=base;upleft[1]=horizon_level+height;upleft[2]=-ud_grid_step+d+move;
		downleft[0]=base;downleft[1]=horizon_level;downleft[2]=-ud_grid_step+d+move;
		upright[0]=base+edgewidth;upright[1]=horizon_level+height;upright[2]=-ud_grid_step+d+move;
		downright[0]=base+edgewidth;downright[1]=horizon_level;downright[2]=-ud_grid_step+d+move;
		DrawCube(upleft,upright,downleft,downright,d);

		upleft[0]=base+0.3f;upleft[1]=horizon_level+height;upleft[2]=-ud_grid_step+d+move;
		downleft[0]=base+0.3f;downleft[1]=horizon_level;downleft[2]=-ud_grid_step+d+move;
		upright[0]=base+0.3+edgewidth;upright[1]=horizon_level+height;upright[2]=-ud_grid_step+d+move;
		downright[0]=base+0.3+edgewidth;downright[1]=horizon_level;downright[2]=-ud_grid_step+d+move;
		DrawCube(upleft,upright,downleft,downright,d);

		upleft[0]=base;upleft[1]=horizon_level+height+edgewidth;upleft[2]=-ud_grid_step+d+move;
		downleft[0]=base;downleft[1]=horizon_level+height;downleft[2]=-ud_grid_step+d+move;
		upright[0]=base+0.3+edgewidth;upright[1]=horizon_level+height+edgewidth;upright[2]=-ud_grid_step+d+move;
		downright[0]=base+0.3+edgewidth;downright[1]=horizon_level+height;downright[2]=-ud_grid_step+d+move;
		DrawCube(upleft,upright,downleft,downright,d);
		
		glDepthFunc(GL_LEQUAL);							
		glEnable(GL_DEPTH_TEST);
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		glBindTexture( GL_TEXTURE_2D, texture->get_texture(DOOR_TEXTURE));
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
		glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
		glEnable(GL_TEXTURE_2D);
		glColor3f(1.0f,1.0f,1.0f);
		glEnable(GL_BLEND); 
			upleft[0]=base+edgewidth;upleft[1]=horizon_level+height;upleft[2]=-ud_grid_step+d+move;
			downleft[0]=base+edgewidth;downleft[1]=horizon_level;downleft[2]=-ud_grid_step+d+move;
			upright[0]=base+0.3f;upright[1]=horizon_level+height;upright[2]=-ud_grid_step+d+move;
			downright[0]=base+0.3f;downright[1]=horizon_level;downright[2]=-ud_grid_step+d+move;
			glBegin(GL_QUADS);
				glNormal3f(0.0f,0.0f,1.0f);
				glTexCoord2f(0.01f,0.0f);glVertex3fv(downleft);
				glTexCoord2f(0.72f,0.0f);glVertex3fv(downright);
				glTexCoord2f(0.72f,1.0f);glVertex3fv(upright);
				glTexCoord2f(0.01f,1.0f);glVertex3fv(upleft);
			glEnd();
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_BLEND);	
}
void cgym::DrawLeftDoor()
{
	glColor3f(0.68f,0.68f,0.68f);
	GLfloat base=-0.65f;
	GLfloat upleft[3],upright[3],downleft[3],downright[3];
	GLfloat edgewidth = 0.02f,height = 0.25f;
	GLfloat d = 0.02f;
	GLfloat zshift = ud_grid_step*(GRID_SIZE-2)-0.1f;
	upleft[0]=base;upleft[1]=horizon_level+height;upleft[2]=-ud_grid_step+d-zshift;
	downleft[0]=base;downleft[1]=horizon_level;downleft[2]=-ud_grid_step+d-zshift;
	upright[0]=base+edgewidth;upright[1]=horizon_level+height;upright[2]=-ud_grid_step+d-zshift;
	downright[0]=base+edgewidth;downright[1]=horizon_level;downright[2]=-ud_grid_step+d-zshift;
	DrawCube(upleft,upright,downleft,downright,d);

	upleft[0]=base+0.3f;upleft[1]=horizon_level+height;upleft[2]=-ud_grid_step+d-zshift;
	downleft[0]=base+0.3f;downleft[1]=horizon_level;downleft[2]=-ud_grid_step+d-zshift;
	upright[0]=base+0.3+edgewidth;upright[1]=horizon_level+height;upright[2]=-ud_grid_step+d-zshift;
	downright[0]=base+0.3+edgewidth;downright[1]=horizon_level;downright[2]=-ud_grid_step+d-zshift;
	DrawCube(upleft,upright,downleft,downright,d);

	upleft[0]=base;upleft[1]=horizon_level+height+edgewidth;upleft[2]=-ud_grid_step+d-zshift;
	downleft[0]=base;downleft[1]=horizon_level+height;downleft[2]=-ud_grid_step+d-zshift;
	upright[0]=base+0.3+edgewidth;upright[1]=horizon_level+height+edgewidth;upright[2]=-ud_grid_step+d-zshift;
	downright[0]=base+0.3+edgewidth;downright[1]=horizon_level+height;downright[2]=-ud_grid_step+d-zshift;
	DrawCube(upleft,upright,downleft,downright,d);

	glDepthFunc(GL_LEQUAL);							
	glEnable(GL_DEPTH_TEST);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glBindTexture( GL_TEXTURE_2D, texture->get_texture(DOOR_TEXTURE));
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
	glEnable(GL_TEXTURE_2D);
	glColor3f(1.0f,1.0f,1.0f);
	glEnable(GL_BLEND); 
	upleft[0]=base+edgewidth;upleft[1]=horizon_level+height;upleft[2]=-ud_grid_step+d-zshift;
	downleft[0]=base+edgewidth;downleft[1]=horizon_level;downleft[2]=-ud_grid_step+d-zshift;
	upright[0]=base+0.3f;upright[1]=horizon_level+height;upright[2]=-ud_grid_step+d-zshift;
	downright[0]=base+0.3f;downright[1]=horizon_level;downright[2]=-ud_grid_step+d-zshift;
	glBegin(GL_QUADS);
	glNormal3f(0.0f,0.0f,-1.0f);
	glTexCoord2f(0.7f,0.0f);glVertex3fv(downright);
	glTexCoord2f(0.01f,0.0f);glVertex3fv(downleft);
	glTexCoord2f(0.01f,1.0f);glVertex3fv(upleft);
	glTexCoord2f(0.7f,1.0f);glVertex3fv(upright);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);	
}

//draw the surround enviroment of gym
void cgym::DrawSurround()
{
	DrawCircle();
	DrawGourd();
	DrawSurroundWall();	
	DrawTree();
	DrawLawn();
}

void cgym::DrawCircle()
{
	glEnable(GL_TEXTURE_2D);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glBindTexture( GL_TEXTURE_2D, texture->get_texture(GROUND_TEXTURE));
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);

	Point pt;
	pt.x = down_ctrlPoints[0][0]-1.2f;
	pt.y =horizon_level;
	pt.z = 3.0f;
	GLfloat delta_angle = PI/180;
	GLfloat end_angle = 2*PI;
	GLfloat radius = 0.86f;
	GLfloat height = 0.06f;
	Aux_DrawWall(pt,radius,0,end_angle,delta_angle,height);
	glDisable(GL_TEXTURE_2D);
	//draw grass texture
	pt.y+=0.03f;
	render::circle_render(pt,radius,2*delta_angle,texture->get_texture(GRASS2_TEXTURE));
	glDisable(GL_TEXTURE_2D);
}

void cgym::DrawSurroundWall()
{	
	glEnable(GL_TEXTURE_2D);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glBindTexture( GL_TEXTURE_2D, texture->get_texture(WALL_TEXTURE));
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);

	Point pt;
	pt.x = 0;pt.y =horizon_level;pt.z = 4.0f;
	GLfloat height = 0.4f;
	GLfloat start_angle=0,end_angle = 0,delta_angle=0;
	start_angle = 5*PI/8;
	delta_angle = PI/180;
	end_angle = 	7*PI/8+0.023f;
	//front 
	GLfloat radius=2.0f;	
	Aux_DrawWall(pt,radius,start_angle,end_angle,delta_angle,height);
	radius = 2.3f;
	Aux_DrawWall(pt,radius,start_angle,end_angle,delta_angle,height);
	radius = 2.6f;
	Aux_DrawWall(pt,radius,start_angle,end_angle,delta_angle,height);

	//back	
	pt.x = 1.0f;
	pt.y =horizon_level;
	pt.z = 4.0f;
	start_angle = PI/8;
	delta_angle = PI/180;
	end_angle = 	3*PI/8;
	radius=2.0f;	
	Aux_DrawWall(pt,radius,start_angle,end_angle,delta_angle,height);
	radius = 2.3f;
	Aux_DrawWall(pt,radius,start_angle,end_angle,delta_angle,height);
	radius = 2.6f;
	Aux_DrawWall(pt,radius,start_angle,end_angle,delta_angle,height);
	glDisable(GL_TEXTURE_2D);
}

void cgym::Aux_DrawWall(Point center,GLfloat radius,GLfloat start_angle,GLfloat end_angle,GLfloat delta_angle,GLfloat height)
{
	GLfloat i = 0.0f;
	GLfloat r = radius;
	GLfloat x1,x2,z1,z2;
	GLfloat a1,a2,c1,c2;
	GLfloat temp[3][3];
	GLfloat vNormal[3];
	GLfloat l = delta_angle*PI*radius;
	GLfloat es1 = l;
	l = delta_angle*PI*(radius+0.06f);
	GLfloat es2 = l;
	GLfloat s1=0.0f,s2=0.0f;
	for(i = start_angle;i<=end_angle-delta_angle;i+=delta_angle){
		x1 = center.x+radius*cos(i);
		z1 = center.z+radius*sin(i);		
		x2 = center.x+radius*cos(i+delta_angle);
		z2 = center.z+radius*sin(i+delta_angle);
		if(shadow)
			glColor3fv(shadowcolor);
		else
			glColor3f(0.5f,0.5f,0.5f);
		glBegin(GL_QUADS);
			temp[0][0]=x1;temp[0][1]=center.y;temp[0][2]=z1;
			temp[1][0]=x2;temp[1][1]=center.y;temp[1][2]=z2;
			temp[2][0]=x2;temp[2][1]=center.y+height;temp[2][2]=z2;
			gltGetNormalVector(temp[0], temp[1], temp[2], vNormal);
			glNormal3fv(vNormal);
			glTexCoord2f(s1, 0.0f);glVertex3f(x1,center.y,z1);
			glTexCoord2f(s1+es1, 0.0f);glVertex3f(x2,center.y,z2);
			glTexCoord2f(s1+es1, 1.0f); glVertex3f(x2,center.y+height,z2);	
			glTexCoord2f(s1, 1.0f);glVertex3f(x1,center.y+height,z1);
		glEnd();

		a1 = center.x+(radius+0.06f)*cos(i);
		c1 = center.z+(radius+0.06f)*sin(i);		
		a2 = center.x+(radius+0.06f)*cos(i+delta_angle);
		c2 = center.z+(radius+0.06f)*sin(i+delta_angle);

		if(shadow)
			glColor3fv(shadowcolor);
		else
			glColor3f(0.5f,0.5f,0.5f);
		glBegin(GL_QUADS);
			temp[0][0]=a2;temp[0][1]=center.y;temp[0][2]=c2;
			temp[1][0]=a1;temp[1][1]=center.y;temp[1][2]=c1;
			temp[2][0]=a1;temp[2][1]=center.y+height;temp[2][2]=c1;
			gltGetNormalVector(temp[0], temp[1], temp[2], vNormal);
			glNormal3fv(vNormal);
			glTexCoord2f(s2,0.0f);glVertex3f(a2,center.y,c2);
			glTexCoord2f(s2+es1, 0.0f);glVertex3f(a1,center.y,c1);				
			glTexCoord2f(s2+es1, 1.0f);glVertex3f(a1,center.y+height,c1);
			glTexCoord2f(s2, 1.0f);glVertex3f(a2,center.y+height,c2);			
		glEnd();

		if(shadow)
			glColor3fv(shadowcolor);
		else
			glColor3f(0.4f,0.3f,0.3f);
		glBegin(GL_QUADS);		
			glNormal3f(0.0f,1.0f,0.0f);
			glVertex3f(x2,center.y+height,z2);	
			glVertex3f(a2,center.y+height,c2);
			glVertex3f(a1,center.y+height,c1);
			glVertex3f(x1,center.y+height,z1);
		glEnd();
		s1+=es1;s2+=es2;
	}
	if(shadow)
		glColor3fv(shadowcolor);
	else
		glColor3f(0.54f,0.54f,0.54f);
	glBegin(GL_QUADS);
		x1 = center.x+r*cos(start_angle);
		z1 = center.z+r*sin(start_angle);		
		x2 = center.x+(r+0.06f)*cos(start_angle);
		z2 = center.z+(r+0.06f)*sin(start_angle);

		temp[0][0]=x2;temp[0][1]=center.y;temp[0][2]=z2;
		temp[1][0]=x1;temp[1][1]=center.y;temp[1][2]=z1;
		temp[2][0]=x1;temp[2][1]=center.y+height;temp[2][2]=z1;
		gltGetNormalVector(temp[0], temp[1], temp[2], vNormal);
		glNormal3fv(vNormal);

		glVertex3f(x2,center.y,z2);
		glVertex3f(x1,center.y,z1);
		glVertex3f(x1,center.y+height,z1);		
		glVertex3f(x2,center.y+height,z2);
	glEnd();
	
	glBegin(GL_QUADS);
		x1 = center.x+r*cos(i);
		z1 = center.z+r*sin(i);		
		x2 = center.x+(r+0.06f)*cos(i);
		z2 = center.z+(r+0.06f)*sin(i);

		temp[0][0]=x2;temp[0][1]=center.y;temp[0][2]=z2;
		temp[1][0]=x2;temp[1][1]=center.y+height;temp[1][2]=z2;
		temp[2][0]=x1;temp[2][1]=center.y+height;temp[2][2]=z1;
		gltGetNormalVector(temp[0], temp[1], temp[2], vNormal);
		glNormal3fv(vNormal);

		glVertex3f(x2,center.y,z2);
		glVertex3f(x2,center.y+height,z2);
		glVertex3f(x1,center.y+height,z1);
		glVertex3f(x1,center.y,z1);
	glEnd();
}

void cgym::DrawGourd()
{
	glDisable(GL_CULL_FACE);
	glEnable(GL_TEXTURE_2D);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glBindTexture( GL_TEXTURE_2D, texture->get_texture(GROUND_TEXTURE));
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
	Point center;
	center.x = down_ctrlPoints[0][0]+0.8f;
	center.y =horizon_level;
	center.z = 3.0f;
	GLfloat start_angle,end_angle,delta_angle;
	Point pt;
	GLfloat radius=0.3f;
	GLfloat move= 0.0f;
	GLfloat height = 0.06f;

	pt=center;
	start_angle = 2*PI/8-PI/25+move;
	end_angle = 14*PI/8+PI/25-move;
	delta_angle=PI/180;	
	Aux_DrawWall(pt,radius,start_angle,end_angle,delta_angle,height);
	pt.x+=0.7;
	start_angle = 26*PI/128+move;
	end_angle  = 113*PI/128-move;
	Aux_DrawWall(pt,radius+0.2f,start_angle,end_angle,delta_angle,height);
	start_angle = 143.3*PI/128+move;
	end_angle  = 230.3*PI/128-move;
	Aux_DrawWall(pt,radius+0.2f,start_angle,end_angle,delta_angle,height);
	pt.x+=0.8;
	start_angle = 15.5*PI/128+move;
	end_angle  = 102*PI/128-move;
	Aux_DrawWall(pt,radius+0.2f,start_angle,end_angle,delta_angle,height);
	start_angle = 154*PI/128+move;
	end_angle  = 241*PI/128-move;
	Aux_DrawWall(pt,radius+0.2f,start_angle,end_angle,delta_angle,height);
	pt.x+=0.7;
	start_angle = -6*PI/8-PI/25+move+PI/256;
	end_angle  = 6*PI/8+PI/25-move;
	Aux_DrawWall(pt,radius,start_angle,end_angle,delta_angle,height);
	radius+=0.06f;
	move+=6*PI/256;
	glDisable(GL_TEXTURE_2D);
}

void cgym::DrawStep()
{
	GLfloat up[3];
	memcpy(up,&up_ctrlPoints[0],sizeof(up));
	GLfloat n = up_ctrlPoints[0][1]/tan((PI/180.0f)*dip_angle);
	GLfloat xPos =up_ctrlPoints[0][0]+n;
	GLfloat down[3]={xPos,(horizon_level),0};
	GLfloat step_w = 0.33f;
	GLfloat step_h = 0.05f;
	GLfloat zshift = (ud_grid_step)*(GRID_SIZE-2);
	
	glEnable(GL_TEXTURE_2D);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glBindTexture( GL_TEXTURE_2D, texture->get_texture(WALL2_TEXTURE));
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);

	glBegin(GL_QUADS);
		glNormal3f(0.0f,0.0f,1.0f);
		glVertex3f(down[0],down[1]+step_h,down[2]-ud_grid_step);
		glVertex3f(down[0]-step_w,down[1]+step_h,down[2]-ud_grid_step);
		glVertex3f(down[0]-step_w,down[1],down[2]-ud_grid_step);
		glVertex3f(down[0],down[1],down[2]-ud_grid_step);
	glEnd();
	glBegin(GL_QUADS);
		glNormal3f(0.0f,0.0f,-1.0f);
		glVertex3f(down[0]-step_w,down[1]+step_h,down[2]-zshift);
		glVertex3f(down[0],down[1]+step_h,down[2]-zshift);
		glVertex3f(down[0],down[1],down[2]-zshift);
		glVertex3f(down[0]-step_w,down[1],down[2]-zshift);		
	glEnd();

	if(shadow)
		glColor3fv(shadowcolor);
	else
		glColor3f(0.9f,0.9f,0.9f);
	GLfloat et = 1.0f/(GRID_SIZE*ud_grid_step*0.5f);
	GLfloat s=0.0f,t=0.0f;
	GLfloat move = 0.0f;
	for(move=ud_grid_step;move<zshift;move+=ud_grid_step){
		glBegin(GL_QUADS);
			glNormal3f(0.0f,1.0f,0.0f);
			glTexCoord2f(s,0.0f);glVertex3f(down[0],down[1]+step_h,down[2]-move);
			glTexCoord2f(s+et,0.0f);glVertex3f(down[0],down[1]+step_h,down[2]-move-ud_grid_step);
			glTexCoord2f(s+et,1.0f);glVertex3f(down[0]-step_w,down[1]+step_h,down[2]-move-ud_grid_step);
			glTexCoord2f(s,1.0f);glVertex3f(down[0]-step_w,down[1]+step_h,down[2]-move);
		glEnd();
		s+=et;
	}
	//draw step
	GLfloat eyd = 0.025;
	GLfloat exd = 0.03;
	glBegin(GL_QUADS);
		glNormal3f(-1.0f,0.0f,0.0f);
		glVertex3f(down[0]-step_w,down[1]+step_h,down[2]-ud_grid_step);
		glVertex3f(down[0]-step_w,down[1]+step_h,down[2]-zshift);
		glVertex3f(down[0]-step_w,down[1]+step_h-eyd,down[2]-zshift);
		glVertex3f(down[0]-step_w,down[1]+step_h-eyd,down[2]-ud_grid_step);
	glEnd();

	glBegin(GL_QUADS);
		glNormal3f(0.0f,1.0f,0.0f);
		glVertex3f(down[0]-step_w,down[1]+step_h-eyd,down[2]-ud_grid_step);
		glVertex3f(down[0]-step_w,down[1]+step_h-eyd,down[2]-zshift);	
		glVertex3f(down[0]-step_w-exd,down[1]+step_h-eyd,down[2]-zshift);
		glVertex3f(down[0]-step_w-exd,down[1]+step_h-eyd,down[2]-ud_grid_step);
	glEnd();
	glBegin(GL_QUADS);
		glNormal3f(-1.0f,0.0f,0.0f);
		glVertex3f(down[0]-step_w-exd,down[1]+step_h-eyd,down[2]-ud_grid_step);
		glVertex3f(down[0]-step_w-exd,down[1]+step_h-eyd,down[2]-zshift);
		glVertex3f(down[0]-step_w-exd,down[1]+step_h-2*eyd,down[2]-zshift);
		glVertex3f(down[0]-step_w-exd,down[1]+step_h-2*eyd,down[2]-ud_grid_step);
	glEnd();
	glDisable(GL_TEXTURE_2D);
}

void cgym::aux_drawtree(util::Point center,GLfloat w,GLfloat h,int type)
{
	glDisable(GL_CULL_FACE);
	glDepthFunc(GL_LEQUAL);							
	glEnable(GL_DEPTH_TEST);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glBindTexture( GL_TEXTURE_2D, texture->get_texture(type));
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND); 
	glEnable(GL_ALPHA_TEST); 
	glAlphaFunc(GL_GREATER ,0.5f);
	glPushMatrix();	
		CDrawMatObj::drawtree(center,w,h);
	glEnd();
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);
	glEnable(GL_LIGHTING);
}

void cgym::DrawTree()
{
	util::Point pt1;
	GLfloat height = 0.2f;
	GLfloat width=0.15f;
	pt1.x=down_ctrlPoints[0][0]-1.2f;pt1.y=horizon_level+height;pt1.z=3.0f;
	util::Point pt2=pt1;
	//for grass
	GLfloat height1 = 0.15f;
	GLfloat width1 = 0.1f;

	//circle
	aux_drawtree(pt1,width,height,TREE4_TEXTURE);
	height = 0.1f;
	pt1.y=horizon_level+height;
	pt1.x-=0.6f;
	aux_drawtree(pt1,width,height,TREE1_TEXTURE);
	pt1.x+=1.2f;
	aux_drawtree(pt1,width,height,TREE1_TEXTURE);
	pt1.x-=0.6f;
	pt1.z-=0.6f;
	aux_drawtree(pt1,width,height,TREE1_TEXTURE);
	pt1.z+=1.2f;
	aux_drawtree(pt1,width,height,TREE1_TEXTURE);
	//
	height = 0.2f;
	pt2.x = down_ctrlPoints[0][0]+0.8f;
	pt1 = pt2;
	pt1.y-=0.12f;
	aux_drawtree(pt2,width,height,TREE5_TEXTURE);
	aux_drawburshwood(pt1,width1,height1,0.15f,GRASS3_TEXTURE);
//
	pt2.x+=0.6f;
	pt1.x+=0.6f;
	aux_drawtree(pt2,width,height,TREE10_TEXTURE);	
	aux_drawburshwood(pt1,width1,height1,0.15f,GRASS3_TEXTURE);
	pt1.z+=0.2f;
	aux_drawburshwood(pt1,width1,height1,0.15f,GRASS3_TEXTURE);
	pt1.z-=0.4f;
	aux_drawburshwood(pt1,width1,height1,0.15f,GRASS3_TEXTURE);

	pt1.x+=0.4f;
	pt1.z+=0.2f;
	aux_drawburshwood(pt1,width1,height1,0.15f,GRASS3_TEXTURE);

	pt2.x+=0.9f;
	pt1.x+=0.5f;	
	aux_drawtree(pt2,width,height,TREE7_TEXTURE);
	aux_drawburshwood(pt1,width1,height1,0.15f,GRASS3_TEXTURE);
	pt1.z+=0.2f;
	aux_drawburshwood(pt1,width1,height1,0.15f,GRASS3_TEXTURE);
	pt1.z-=0.4f;
	aux_drawburshwood(pt1,width1,height1,0.15f,GRASS3_TEXTURE);

	pt2.x+=0.7f;
	pt1.x+=0.7f;
	pt1.z+=0.2f;
	aux_drawtree(pt2,width,height,TREE10_TEXTURE);
	aux_drawburshwood(pt1,width1,height1,0.15f,GRASS3_TEXTURE);

	height+=0.1;
	width+=0.1;
	pt2.x+=1.0f;
	pt2.y=horizon_level+height;
	aux_drawtree(pt2,width,height,TREE11_TEXTURE);

	/*
		pt2.x+=0.5f;
		height+=0.1;
		pt2.y=horizon_level+height-0.2f;
		width+=0.05;
		aux_drawtree(pt2,width,height,GRASS4_TEXTURE);
	*/	

	//left 
	height = 0.3f;
	width=0.18f;
	pt1.x = up_ctrlPoints[0][0]+2.1f;
	pt1.y = horizon_level+height;
	pt1.z = up_ctrlPoints[0][2]-GRID_SIZE*ud_grid_step-1.1f;
	aux_drawtree(pt1,width,height,LAMP1_TEXTURE);
	pt1.x+=0.3f;
	pt1.z-=0.1f;
	aux_drawtree(pt1,width,height,TREE3_TEXTURE);
	pt1.x+=0.8f;
	aux_drawtree(pt1,width,height,TREE7_TEXTURE);
	pt1.x+=0.8f;
	aux_drawtree(pt1,width,height,TREE3_TEXTURE);
	pt1.x+=0.8f;
	aux_drawtree(pt1,width,height,TREE7_TEXTURE);
	pt1.x+=0.8f;
	aux_drawtree(pt1,width,height,TREE3_TEXTURE);
	pt1.x+=0.8f;
	aux_drawtree(pt1,width,height,TREE7_TEXTURE);
	pt1.x+=0.8f;
	aux_drawtree(pt1,width,height,TREE3_TEXTURE);
}
void cgym::aux_drawburshwood(util::Point pt,GLfloat w,GLfloat h,GLfloat interval ,int type)
{
	util::Point tmp = pt;
	tmp.x-=interval;
	aux_drawtree(tmp,w,h,type);
	tmp.x+=2.0f*interval;
	aux_drawtree(tmp,w,h,type);
	tmp=pt;
	tmp.z-=interval;
	aux_drawtree(tmp,w,h,type);
	tmp.z+=2.0f*interval;
	aux_drawtree(tmp,w,h,type);
}
void cgym::DrawLawn()
{
	util::Point pt;
	GLfloat xlw = 4.0f;
	GLfloat xlh = 0.025f;
	GLfloat length = 5.0f;
	pt.x = up_ctrlPoints[0][0]-4.0f;
	pt.y = horizon_level+xlh;
	pt.z = up_ctrlPoints[0][2]-GRID_SIZE*ud_grid_step-1.0f;
	aux_drawlawn(pt,length,xlw,xlh,texture->get_texture(GRASS1_TEXTURE)); //left lawn
	//draw road
	pt.x+=(length+1.0f);
	length=6.0f;
	aux_drawlawn(pt,length,xlw,xlh,texture->get_texture(GRASS1_TEXTURE)); //right lawn

	pt.x=up_ctrlPoints[0][0]+1.0f;
	length = 1.0f;
	xlh=0.001f;
	pt.y=horizon_level+0.001f;
	aux_drawlawn(pt,length,xlw,xlh,texture->get_texture(FLOOR1_TEXTURE));
}
void cgym::aux_drawlawn(util::Point left,GLfloat l,GLfloat w,GLfloat h,GLuint texture)
{
	glDepthFunc(GL_LEQUAL);							
	glEnable(GL_DEPTH_TEST);
	glBindTexture( GL_TEXTURE_2D, texture);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND); 

	GLfloat xlw = w;
	GLfloat xlh = h;
	GLfloat xl = left.x;
	GLfloat yl = horizon_level+xlh;
	GLfloat zl = left.z;

	GLfloat xr = left.x+l;
	GLfloat yr = horizon_level+xlh;
	GLfloat zr = left.z;

	GLfloat fStep = 0.2f;
	GLfloat iStrip ,iRun;
	GLfloat s = 0.0f;
	GLfloat t = 0.0f;
	GLfloat texStep = 1.0f/(xlw*0.25);
	for(iStrip = 0;iStrip<=l-0.0001f;iStrip+=fStep){
		t=0.0f;
		glBegin(GL_TRIANGLE_STRIP);	
		for(iRun = 0;iRun<=xlw+0.0001f;iRun+=fStep){
			glTexCoord2f(s,t);
			glNormal3f(0.0f,1.0f,0.0f);
			glVertex3f(xl+iStrip,yl,zl-iRun);
			glTexCoord2f(s+texStep,t);
			glNormal3f(0.0f,1.0f,0.0f);
			glVertex3f(xl+iStrip+fStep,yl,zl-iRun);
			t+=texStep;
		}
		glEnd();
		s+=texStep;
	}
	glColor3f(0.8f,0.8f,0.8f);
	//front
	glBegin(GL_QUADS);
	glNormal3f(0.0f,0.0f,1.0f);
	glVertex3f(xl,horizon_level,zl);
	glVertex3f(xr,horizon_level,zr);
	glVertex3f(xr,yr,zr);
	glVertex3f(xl,yr,zr);
	glEnd();
	//back
	glBegin(GL_QUADS);
	glNormal3f(0.0f,0.0f,-1.0f);
	glVertex3f(xr,horizon_level,zr-xlw);
	glVertex3f(xl,horizon_level,zl-xlw);
	glVertex3f(xl,yr,zr-xlw);
	glVertex3f(xr,yr,zr-xlw);
	glEnd();
	//right
	glBegin(GL_QUADS);			
	glNormal3f(-1.0f,0.0f,0.0f);
	glVertex3f(xl,horizon_level,zl-xlw);
	glVertex3f(xl,horizon_level,zl);
	glVertex3f(xl,yl,zl);
	glVertex3f(xl,yl,zl-xlw);	
	glEnd();
	//left
	glBegin(GL_QUADS);
	glNormal3f(1.0f,0.0f,0.0f);
	glVertex3f(xr,horizon_level,zr);
	glVertex3f(xr,horizon_level,zr-xlw);
	glVertex3f(xr,yr,zr-xlw);
	glVertex3f(xr,yr,zr);
	glEnd();
	glDisable(GL_TEXTURE_2D);
}

void cgym::DrawWriteFace()
{
	GLfloat vertex[3]={0};
	memcpy(vertex,down_ctrlPoints[0],sizeof(vertex));
	glColor3f(0.65f,0.65f,0.65f);
	glBegin(GL_QUADS);
		glNormal3f(-1.0f,0.0f,0.0f);
		glVertex3f(vertex[0]-0.001f,horizon_level,vertex[2]-5*ud_grid_step);
		glVertex3f(vertex[0]-0.001f,vertex[1]+plathPos,vertex[2]-5*ud_grid_step);
		glVertex3f(vertex[0]-0.001f,vertex[1]+plathPos,vertex[2]-35*ud_grid_step);
		glVertex3f(vertex[0]-0.001f,horizon_level,vertex[2]-35*ud_grid_step);
	glEnd();
}

void cgym::DrawDownDoor()
{
	//door	
	GLfloat move = 0.0f;			//right
	aux_drawdowndoor(move);	
	move= 14*ud_grid_step; //left
	aux_drawdowndoor(move);

	//windows
	//right
	GLfloat upleft[3],upright[3],downleft[3],downright[3];
	GLfloat width= 0.4f,height=0.15f;
	GLfloat base=down_ctrlPoints[0][0];
	GLfloat d = 0.002f;
	GLfloat ph = -0.37f;
	glDepthFunc(GL_LEQUAL);							
	glEnable(GL_DEPTH_TEST);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glBindTexture( GL_TEXTURE_2D, texture->get_texture(WINDOW_TEXTURE));
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
	glEnable(GL_TEXTURE_2D);
	
	//right
	glEnable(GL_BLEND); 
		//right
		upleft[0]=base-d;upleft[1]=ph+height;upleft[2]=-7*ud_grid_step-width;
		downleft[0]=base-d;downleft[1]=ph;downleft[2]=-7*ud_grid_step-width;
		upright[0]=base-d;upright[1]=ph+height;upright[2]=-7*ud_grid_step;
		downright[0]=base-d;downright[1]=ph;downright[2]=-7*ud_grid_step;
		glColor3f(0.9f,0.9f,0.9f);
		glBegin(GL_QUADS);
		glNormal3f(-1.0f,0.0f,0.0f);
		glTexCoord2f(0.0f,0.09f);glVertex3fv(downleft);
		glTexCoord2f(1.0f,0.09f);glVertex3fv(downright);
		glTexCoord2f(1.0f,0.95f);glVertex3fv(upright);
		glTexCoord2f(0.0f,0.95f);glVertex3fv(upleft);
		glEnd();
		//left
		upleft[0]=base-d;upleft[1]=ph+height;upleft[2]=-30*ud_grid_step-width;
		downleft[0]=base-d;downleft[1]=ph;downleft[2]=-30*ud_grid_step-width;
		upright[0]=base-d;upright[1]=ph+height;upright[2]=-30*ud_grid_step;
		downright[0]=base-d;downright[1]=ph;downright[2]=-30*ud_grid_step;
		glBegin(GL_QUADS);
		glNormal3f(-1.0f,0.0f,0.0f);
		glTexCoord2f(0.0f,0.09f);glVertex3fv(downleft);
		glTexCoord2f(1.0f,0.09f);glVertex3fv(downright);
		glTexCoord2f(1.0f,0.95f);glVertex3fv(upright);
		glTexCoord2f(0.0f,0.95);glVertex3fv(upleft);
		glEnd();

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);	
}

void cgym::aux_drawdowndoor(GLfloat m)
{
	GLfloat base=down_ctrlPoints[0][0];
	GLfloat upleft[3],upright[3],downleft[3],downright[3];
	GLfloat edgewidth = 0.02f,height = 0.27f;
	GLfloat d = 0.01f;
	GLfloat move = 0.02f;
	GLfloat len = 0.2f;

	//right
	upleft[0]=base-d;upleft[1]=horizon_level+height;upleft[2]=-12*ud_grid_step-m;
	downleft[0]=base-d;downleft[1]=horizon_level;downleft[2]=-12*ud_grid_step-m;
	upright[0]=base;upright[1]=horizon_level+height;upright[2]=-12*ud_grid_step-m;
	downright[0]=base;downright[1]=horizon_level;downright[2]=-12*ud_grid_step-m;
	DrawCube(upleft,upright,downleft,downright,move);
	//up
	memcpy(downleft,upleft,sizeof(upleft));
	memcpy(downright,upright,sizeof(upright));
	upleft[0]=base-d;upleft[1]=horizon_level+height+edgewidth;upleft[2]=-12*ud_grid_step-m;
	upright[0]=base;upright[1]=horizon_level+height+edgewidth;upright[2]=-12*ud_grid_step-m;
	DrawCube(upleft,upright,downleft,downright,move+len);
	//left
	upleft[0]=base-d;upleft[1]=horizon_level+height+edgewidth;upleft[2]=-12*ud_grid_step-edgewidth-len-m;
	downleft[0]=base-d;downleft[1]=horizon_level;downleft[2]=-12*ud_grid_step-edgewidth-len-m;
	upright[0]=base;upright[1]=horizon_level+height+edgewidth;upright[2]=-12*ud_grid_step-edgewidth-len-m;
	downright[0]=base;downright[1]=horizon_level;downright[2]=-12*ud_grid_step-edgewidth-len-m;
	DrawCube(upleft,upright,downleft,downright,move);

	//door texture
	glDepthFunc(GL_LEQUAL);							
	glEnable(GL_DEPTH_TEST);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glBindTexture( GL_TEXTURE_2D, texture->get_texture(DOOR_TEXTURE));
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
	glEnable(GL_TEXTURE_2D);
	glColor3f(1.0f,1.0f,1.0f);
	glEnable(GL_BLEND); 
	upleft[0]=base-d;upleft[1]=horizon_level+height;upleft[2]=-12*ud_grid_step-edgewidth-len-m;
	downleft[0]=base-d;downleft[1]=horizon_level;downleft[2]=-12*ud_grid_step-edgewidth-len-m;
	upright[0]=base-d;upright[1]=horizon_level+height;upright[2]=-12*ud_grid_step-edgewidth-m;
	downright[0]=base-d;downright[1]=horizon_level;downright[2]=-12*ud_grid_step-edgewidth-m;
	glBegin(GL_QUADS);
	glNormal3f(-1.0f,0.0f,0.0f);
	glTexCoord2f(0.01f,0.0f);glVertex3fv(downleft);
	glTexCoord2f(0.72f,0.0f);glVertex3fv(downright);
	glTexCoord2f(0.72f,1.0f);glVertex3fv(upright);
	glTexCoord2f(0.01f,1.0f);glVertex3fv(upleft);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);	
}