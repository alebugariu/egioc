#include "xpmimage.h"

XPM * 
newXPM(unsigned int width, unsigned int height, unsigned short cpp, unsigned int ncolors){
	XPM *toRet = NULL;
	
	toRet = (XPM *)malloc(sizeof(XPM));
	toRet->width = width;
	toRet->height = height;
	toRet->chrperpixel = cpp;
	toRet->ncolors = ncolors;
	toRet->colta = (XPMColor *)malloc(ncolors * sizeof(XPMColor));
	toRet->data = (unsigned short **)malloc(ncolors * sizeof(unsigned short *));
	
	int cnt;
	for(cnt = 0; cnt < height; ++cnt){
		toRet->data[cnt] = (unsigned short *)malloc(width * cpp * sizeof(unsigned char));
	}
	
	return toRet;
}

void
freeXPM(XPM **img)
{
	int cnt;
	for(cnt = 0; cnt < (*img)->height; ++cnt){
		free((*img)->data[cnt]);
	}
	free((*img)->data);
	free((*img)->colta);
	free((*img));
}

void
putXPMpixel(XPM *img, unsigned int x, unsigned int y, unsigned int colorindex)
{
}

void
setXPMColor(XPM *img, unsigned int index, Color pixcolor)
{
}

void
saveXPMtofile(XPM *img, char *filepath)
{
}
