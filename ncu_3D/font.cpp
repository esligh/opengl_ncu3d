#include "stdafx.h"
#include "font.h"

cfont::cfont()
:fontsize(DEFAULT_SIZE)
,charset(DEFAULT_CHARSET)
,fontwight(FW_BOLD)
,listlen(0)
{
	strcpy_s(face,32,"·ÂËÎ");
}

cfont::~cfont()
{
	//glDeleteLists(LISTBASE,listlen);
}

//create logic font  with  win32 api, and select for current dc
void cfont::selectFont() {
	HFONT hFont;
	LOGFONT lf = {0};	
	lf.lfHeight = -1;  
	lf.lfWidth = -1;   
	lf.lfEscapement = 0;   
	lf.lfOrientation = 0; 
	lf.lfWeight = fontwight;    
	lf.lfItalic = FALSE; 
	lf.lfUnderline = FALSE;   
	lf.lfStrikeOut = FALSE;   
	lf.lfCharSet = charset;
	lf.lfOutPrecision = OUT_TT_PRECIS;
	lf.lfClipPrecision = CLIP_DEFAULT_PRECIS ;
	lf.lfQuality = PROOF_QUALITY;
	lf.lfPitchAndFamily = VARIABLE_PITCH | TMPF_TRUETYPE | FF_MODERN ;  
	strcpy (lf.lfFaceName,face);
	hFont=CreateFontIndirect(&lf);
	hdc = wglGetCurrentDC();
	HFONT hOldFont =(HFONT)SelectObject(hdc, hFont);
	DeleteObject(hOldFont);
}
void cfont::setFontsize(int size)
{
	fontsize=size;
}
const char* cfont::setFace(const char* f)
{
	strcpy_s(face,128,f);
	return f;
}
void cfont::setfontweight(int w)
{
	fontwight = w;
}
void cfont::drawCNString() 
{
	glListBase(LISTBASE);
	glCallLists(listlen, GL_UNSIGNED_BYTE, &textList);
}

int cfont::prepareCNString(char *text)
{
	int i,j,len =strlen(text);
	unsigned int code;
	for(i = 0, j=0; i < len && j<256; i++,j++){
		code = text[i];
		if(IsDBCSLeadByte(text[i])){
			code = (code << 8) + 256;
			code +=  text[i + 1];
			i++;
		}
		wglUseFontOutlines(hdc, code, 1, LISTBASE + j, 0.0f, 0.5f, WGL_FONT_POLYGONS, &gmf[j]); 
		textList[j] = j;
	}
	listlen = j;
	return j;
}