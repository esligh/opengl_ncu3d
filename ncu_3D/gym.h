#ifndef  GYM_VIEW_MODEL_H_
#define  GYM_VIEW_MODEL_H_
#include "stdafx.h"
#include "font.h"
#include "tools.h"
#include "texture.h"
/* gym model class
	created by esli 2013/10/16
*/
class cgym
{
public:
	cgym();
	~cgym();
	void DrawGym(int nShadow);
	void InitgymEnv();
	void InitBezierParam();
	void BindTexture(ctexture* t);
	//////////////////////////////////////////////////////////////////////////
	void DrawPoints();
	void DrawGround(void);
	void DrawPillar(GLfloat innerup1[3],GLfloat innerup2[3],GLfloat innerdown1[3],GLfloat innerdown2[3],GLint flag); // aux
	void DrawUpFace();
	void DrawRightUpFace();
	void DrawRightDownFace();
	void DrawLeftDownFace();
	void DrawLeftUpFace();

	void DrawUpGrid();
	void DrawRightDownGrid(int N);
	void DrawLeftDownGrid(int N);

	void DrawRightInnerUpGrid(int N);
	void DrawLeftInnerUpGrid(int N);

	void DrawRightInnerDownGrid(int N);
	void DrawLeftInnerDownGrid(int N);
	
	void DrawRightFace(int N=2);
	void DrawLeftFace(int N =2);

	void DrawFrontLine(int N);
	void DrawFrontFace();
	void DrawWriteFace();
	void DrawBottomGrid();

	void DrawBackLine();
	void DrawBackFace();
	void DrawStripLine();
	void DrawRMirror();
	void DrawLMirror();
	void DrawRBMace();
	void DrawLBMace();
	void DrawFrontDoor();
	void DrawDownDoor();
	void aux_drawdowndoor(GLfloat move);
	void DrawRightDoor();
	void DrawLeftDoor();
	//front scence
	void DrawFrontScence();
	void DrawFrontPlat();
	void DrawPlatPillar(GLfloat move);
	void DrawFence();
	void DrawStair();
	void DrawStairDown();
	void DrawName(GLfloat pos[3]);
	void DrawStep();
	//draw surround enviroment
	void DrawSurround();
	void DrawCircle();
	void DrawGourd();
	void DrawSurroundWall();
	void DrawTree();
	void DrawLawn();
	void aux_drawlawn(util::Point left,GLfloat l,GLfloat w,GLfloat h,GLuint texture);
	void Aux_DrawWall(util::Point center,GLfloat radius,GLfloat start_angle,GLfloat end_angle,GLfloat delta_angle,GLfloat height);
	void aux_drawtree(util::Point pt,GLfloat w,GLfloat h,int type);
	void aux_drawburshwood(util::Point pt,GLfloat w,GLfloat h,GLfloat interval ,int type);
	//aux for draw plat 	
	void DrawCube(GLfloat upleft[3],GLfloat upright[3],GLfloat downleft[3],GLfloat downright[3],GLfloat move);
	void DrawWall(GLfloat upleft[3],GLfloat upright[3],GLfloat downleft[3],GLfloat downright[3],GLfloat move,GLfloat color[3]);
	//////////////////////////////////////////////////////////////////////
	//void DrawRightFoldLine(GLfloat inner);
	void DrawBezierCurve(GLfloat temp[4][3]);
	
	void DrawPointOnInnerUp(); //debug view 
	void DrawPointOnInnerDown();//debug view

	//base of the bezier caculate x and y position
	GLfloat bezier_formula_x(GLfloat temp[4][3],GLfloat t);
	GLfloat bezier_formula_y(GLfloat temp[4][3],GLfloat t);
	void SetGridStepUp(GLfloat v){ud_grid_step = v;}
private:
	GLfloat ud_grid_step; //size of a grid step
	GLfloat up_ctrlPoints[4][3]; // roof up control points;
	GLfloat down_ctrlPoints[4][3]; //dow control points 
	GLfloat innerup_ctrlPoints[4][3]; 
	GLfloat innerdown_ctrlPoints[4][3];

	GLfloat horizon_level; //ground level y posizition ;

	//define for draw the face of gym's roof
	GLfloat dip_angle ; //wall angle
	GLfloat innerdip_angle; //
	GLfloat top_vheight; //vertical height
	GLfloat innertop_vheight; 
	GLfloat innertop_hwidth;

//  save bezier info for drawing fold line and render  
	GLfloat innerup_spot[200][3];
	GLfloat innerdown_spot[200][3];
	GLfloat up_spot[200][3];
	GLfloat down_spot[200][3];

	// about plat
	GLfloat platlength;
	GLfloat platheight;
	GLfloat platwidth;
	GLfloat plathPos;
	GLfloat stair_angel;
	//font
	cfont font;
	int shadow;
	GLfloat shadowcolor[3];
	ctexture* texture;
public:
	static const GLint GRID_SIZE = 41;
private:
	//const data
	static const GLfloat defaut_grid_step_up;
	static const GLint BEZIER_CURVE_N;  //bezier vertex numbers;
	static const GLint nNumPoints; 
	static const GLfloat default_up_ctrlPoints[4][3]; //up bezier constrol points
	static const GLfloat default_down_ctrlPoints[4][3];//down bezier control points 
	static const GLfloat default_innerup_ctrlPoints[4][3]; //up bezier constrol points
	static const GLfloat default_innerdown_ctrlPoints[4][3];//down bezier control points 
	static const GLfloat default_level ;
	// fold line table,adjust by manually
	static const GLint upspot_tbl[24];
	static const GLint downspot_tbl[24];
};

#endif /*end ifndef*/