#include "xpmimage.h"

XPM * 
newXPM(unsigned int width, unsigned int height, unsigned short cpp, unsigned int ncolors){
	XPM *toRet = NULL;
	
	toRet = (XPM *)malloc(sizeof(XPM));
	toRet->width = width;
	toRet->height = height;
	toRet->chrperpixel = cpp;
	toRet->ncolors = ncolors;
	toRet->colta = (XPMColor *)calloc(ncolors, sizeof(XPMColor));
	toRet->data = (unsigned short **)malloc(height * sizeof(unsigned short *));
	
	int cnt;
	for(cnt = 0; cnt < height; ++cnt){
		toRet->data[cnt] = (unsigned short *)malloc(width * sizeof(unsigned short));
	}
	
	return toRet;
}

void
freeXPM(XPM **img){
	int cnt;
	for(cnt = 0; cnt < (*img)->height; ++cnt){
		free((*img)->data[cnt]);
	}
	free((*img)->data);
	free((*img)->colta);
	free((*img));
}

void
putXPMpixel(XPM *img, unsigned int x, unsigned int y, unsigned int colorindex){
	
}

int
setXPMColor(XPM *img, unsigned int index, XPMColor pixdata){
	int fctStatus = 1;
	
	if(index > img->ncolors)	fctStatus = 0;
	else{
		if(img->colta[index].chars != NULL)	free(img->colta[index].chars);
		if(img->colta[index].key != NULL)	free(img->colta[index].key);
		
		img->colta[index] = pixdata;
	}
	
	return fctStatus;
}

int
saveXPMtofile(XPM *img, char *filepath){
	FILE *fXMP = fopen(filepath, "w");
	int fctStatus = 1;
	
	if(fXMP == NULL)	fctStatus = 0;
	else{
		fprintf(fXMP, "static char *egc[] = {\n");
		
		/* dump image properties */
		fprintf(fXMP, "\" %d %d %d %d \",\n", img->width, img->height, img->ncolors, img->chrperpixel);
		
		/* dump color table */
		int id = 0;
		for(id = 0; id < img->ncolors; ++id){
			fprintf(fXMP, "\"%s %s #%02X%02X%02X\",\n", img->colta[id].chars,
													img->colta[id].key,
													img->colta[id].clr.red,
													img->colta[id].clr.green,
													img->colta[id].clr.blue);
		}
		
		fprintf(fXMP, "};\n");
		fclose(fXMP);
	}
	
	return fctStatus;
}
