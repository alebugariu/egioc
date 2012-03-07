#include <stdio.h>
#include <string.h>
#include "xpmimage.h"

void
loadColorTable(XPM *img){
	XPMColor clrData;
	unsigned short clrIndex = 0;
	
	/* initialize default color structure */
	clrData.clr.blue = 0;
	clrData.clr.green = 0;
	clrData.clr.red = 0;
	
	for(clrIndex = 0; clrIndex < img->ncolors; ++clrIndex){
		/* load dynamic data */
		clrData.key = (char *)malloc(sizeof(char) + 1);
		strcpy(clrData.key, "c");
		clrData.chars = (char *)malloc(img->chrperpixel * sizeof(char));
		sprintf(clrData.chars, "%02X", clrIndex);
		
		setXPMColor(img, clrIndex, clrData);
		clrData.clr.red += 255/img->ncolors;
	}
}

int
main(int argc, char *argv[]){
	XPM *img = newXPM(50, 50, 2, 50);
	
	loadColorTable(img);
	saveXPMtofile(img, "tema1.xpm");
	
	freeXPM(&img);
	return 0;
}
