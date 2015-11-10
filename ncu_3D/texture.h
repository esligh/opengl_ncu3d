#ifndef TEXTURE_H__
#define TEXTURE_H__
#include "stdafx.h"
#define  NUM 100  //max texture object number

/**class ctexture for managing all of the textures.
	support  for randomly access.
*/
/*
typedef struct tag_texture
{
	char filename[MAX_PATH];
	 int symbol;
} texture_node;
*/
class ctexture
{
public:
	ctexture();
	~ctexture();
	int load_texture(const char* t);
	int load_transp_textures(const char** filename,int n);
	int load_bmp_textures(const char** filename,int n);
	int load_tga_textures(const char** filename,int n);
	GLuint get_texture(int symbol);
	AUX_RGBImageRec* load_bmp(const char* filename);
private:
	void create(unsigned char* image,int width,int height,int bind);
private:
	GLuint textureObjects[NUM]; //texture objects
	int num; //texture amount
private:

};
#endif