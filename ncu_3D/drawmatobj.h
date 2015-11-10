#ifndef DRAW_MATERIAL_OBJECT_H__
#define DRAW_MATERIAL_OBJECT_H__
#include "tools.h"
/**
annotation:
draw material object class 
*/
class CDrawMatObj
{
public:
	CDrawMatObj();
	~CDrawMatObj();
	static void drawtree(util::Point center,GLfloat w,GLfloat h);	
private:
};
#endif