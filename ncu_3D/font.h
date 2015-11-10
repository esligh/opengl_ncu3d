#ifndef  FONT_CLASS_H__
#define FONT_CLASS_H__
/**create by esli at 2013/11/5
	desc: cfont class for draw 3d text 
*/
class cfont 
{
public:
	cfont();
	~cfont();
	void drawCNString();
//config font attribute
	void setFontsize(int size);
	void selectFont() ;
	const char* setFace(const char* f);
	void setfontweight(int w);
	int prepareCNString(char *text);
private:
	int fontsize;
	int fontwight;
	int charset;
	char face[32];
	GLYPHMETRICSFLOAT gmf[256];
	BYTE textList[128]; //show list
	int listlen;
	HDC hdc; //current dc
	
private:
	static const int DEFAULT_SIZE = 10;
	static const int LISTBASE = 1000;
	static const int MAXLEN = 256;
};
#endif