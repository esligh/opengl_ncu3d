#include "stdafx.h"
#include "texture.h"
#include <cassert>

ctexture::ctexture()
:num(0){
	memset(textureObjects,0,sizeof(textureObjects));
}
ctexture::~ctexture()
{
	glDeleteTextures(num, textureObjects);
}
/**
	annotation: this method can load bmp , tga texture just one.
*/
int ctexture::load_texture(const char* t)
{
	return 0;
}
/**
	desc:load transparent texture from bmp image.In order to crate a transparent texute, you need to do the following things.
	1.load bmp image to structure of  AUX_RGBImageRec
	2.read bmp image to buffer, and according to the color set the alpha value.
	3.enable opengl alpha test.
*/
int ctexture::load_transp_textures(const char** filename,int n)
{
	assert(filename!=NULL && n>0);
	glGenTextures(n,&textureObjects[num]);
	for(int i=0;i<n;i++){
		AUX_RGBImageRec* textureImage[1];
		if(textureImage[0]=load_bmp(*(filename+i))){		
			unsigned char* image;
			int width=0,height=0;
			width = textureImage[0]->sizeX;
			height = textureImage[0]->sizeY;
			FILE *fp;
			fp = fopen(*(filename+i),"rb");
			if(!fp) return -1;
			fseek(fp,54,SEEK_SET);
			image = (unsigned char*)malloc(width*height*3);
			int rc =fread(image,sizeof(unsigned char),width*height*3,fp);
			fclose(fp);
			create(image,width,height,i+num);
		}
		if(textureImage[0]){
			if(textureImage[0]->data){
				free(textureImage[0]->data);
			}
			free(textureImage[0]);
		}
	}
	num+=n;
	return 0;
}

void ctexture::create(unsigned char* image,int width,int height,int bind)
{
	unsigned char* data = (unsigned char*)malloc(width*height*4);
	for(int i=0;i<width;i++){
		for(int j=0;j<height;j++){
			unsigned char p1 = (GLubyte)*(image+i*width*3+j*3);
			unsigned char p2 = (GLubyte)*(image+i*width*3+j*3+1);
			unsigned char p3 = (GLubyte)*(image+i*width*3+j*3+2);
			*(data+i*width*4+j*4) = p1;
			*(data+i*width*4+j*4+1) = p2;
			*(data+i*width*4+j*4+2) = p3;
			if(p1==0&&p2==0&&p3==0){
				*(data+i*width*4+j*4+3)=0;
			}else{
				*(data+i*width*4+j*4+3)=255;
			}
		}
	}
	glPixelStorei(GL_UNPACK_ALIGNMENT,1);
	glBindTexture(GL_TEXTURE_2D, textureObjects[bind]);
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,width, height,0,GL_RGBA,GL_UNSIGNED_BYTE, data);
	//	gluBuild2DMipmaps(GL_TEXTURE_2D, 4, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
	free(data);
}
int ctexture::load_bmp_textures(const char** filename,int n)
{
	return 0;
}
/*load a group of tga textures */
int ctexture::load_tga_textures(const char** filename,int n)
{
	assert(filename !=NULL && n>0);
	glEnable(GL_TEXTURE_2D);
	glGenTextures(n, &textureObjects[num]);  //generate texture objects at specified index;
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); 
	for(int i = 0; i < n; i++){
		GLubyte *pBytes;
		GLint iWidth=0, iHeight=0, iComponents=0;
		GLenum eFormat=0;
		glBindTexture(GL_TEXTURE_2D, textureObjects[i+num]);
		glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);	
		pBytes = (GLubyte*)gltLoadTGA(*(filename+i), &iWidth, &iHeight, &iComponents, &eFormat);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGB, iWidth, iHeight, 0, eFormat, GL_UNSIGNED_BYTE, pBytes);
		free(pBytes);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
	}
	num+=n;
	return 0;
}

GLuint ctexture::get_texture(int symbol)
{
	assert(symbol >=0 && symbol<=num);	
	return textureObjects[symbol];
}

/*load bmp file*/
AUX_RGBImageRec* ctexture::load_bmp(const char* filename)
{
	FILE* file = NULL;
	if(!filename) return NULL;
	file = fopen(filename ,"r");
	if(file){
		fclose(file);
		return auxDIBImageLoad(filename);
	}
	return NULL;
}
