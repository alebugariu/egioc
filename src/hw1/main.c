#include <stdio.h>
#include <string.h>
#include "xpmimage.h"

void
assignColorTable(XPM *img){
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

void
makeTransientImg(XPM *img){
	int ypix = 0;
	int xpix = 0;
	int colIndex = 0;
	
	for(ypix = 0; ypix < img->height; ++ypix){
		colIndex = 0;
		for(xpix = 0; xpix < img->width; ++xpix){
			putXPMpixel(img, xpix, ypix, colIndex);
			colIndex ++;
		}
	}
}

int
main(int argc, char *argv[]){
	XPM *img = newXPM(50, 50, 2, 50);
	
	assignColorTable(img);
	makeTransientImg(img);
	saveXPMtofile(img, "tema1.xpm");
	
	freeXPM(&img);
	return 0;
}
