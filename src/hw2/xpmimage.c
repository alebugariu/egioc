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
	
	for(cnt = 0; cnt < (*img)->ncolors; ++cnt){
		free((*img)->colta[cnt].key);
		free((*img)->colta[cnt].chars);
	}
	free((*img)->colta);
	
	free((*img));
}

int
putXPMpixel(XPM *img, unsigned int x, unsigned int y, unsigned short colorindex){
	int fctStatus = 1;
	
	if(colorindex > img->ncolors ||
	   x > img->width ||
	   y > img->height)
	   fctStatus = 0;
	else{
		img->data[y][x] = colorindex;
	}
	
	return fctStatus;
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
  FILE *fXPM = fopen(filepath, "w");
  int fctStatus = 1;
	
  if(fXPM == NULL)	fctStatus = 0;
  else{
    /* write XPM header */
    fprintf(fXPM, "/* XPM */\n"
	    "static char *egc[] = {\n");
		
    /* dump image properties */
    fprintf(fXPM, "\" %d %d %d %d \",\n", img->width, img->height, img->ncolors, img->chrperpixel);
		
    /* dump color table */
    int id = 0;
    for(id = 0; id < img->ncolors; ++id){
      fprintf(fXPM, "\"%s %s #%02X%02X%02X\",\n", img->colta[id].chars,
	      img->colta[id].key,
	      img->colta[id].clr.red,
	      img->colta[id].clr.green,
	      img->colta[id].clr.blue);
    }
		
    /* dump image pixels */
    int xpix = 0;
    int ypix = 0;
    for(ypix = 0; ypix < img->height; ++ypix){
      fprintf(fXPM,"\"");
      for(xpix = 0; xpix < img->width; ++xpix){
	fprintf(fXPM, "%s", img->colta[img->data[ypix][xpix]].chars);
      }
      fprintf(fXPM,"\"%c\n", (ypix == img->height - 1 ? ' ' : ','));
    }
		
    fprintf(fXPM, "};\n");
    fclose(fXPM);
  }
	
  return fctStatus;
}

void
assignColorTable(XPM *img, const unsigned char vColors[][3], int clrCnt){
  XPMColor clrData;
  int clrIndex = 0;
  char charsFormat[10];

  if(clrCnt > img->ncolors ||
     clrCnt > pow(2, img->chrperpixel * 8) - 1) return;

  for(clrIndex = 1; clrIndex <= clrCnt; clrIndex ++){
    /* load RGB values */
    clrData.clr.blue = vColors[clrIndex - 1][0];
    clrData.clr.green = vColors[clrIndex - 1][1];
    clrData.clr.red = vColors[clrIndex - 1][2];

    /* load dynamic data */
    clrData.key = (char *)malloc(2 * sizeof(char));
    strcpy(clrData.key, "c");
    clrData.chars = (char *)malloc((1 + img->chrperpixel) * sizeof(char));
    
    /* load formated pixel chars */
    switch(img->chrperpixel){
    case 1:
      if(clrCnt <= 0x0F) sprintf(clrData.chars, "%1X", clrIndex);
      else sprintf(clrData.chars, "%c", clrIndex);
      break;
    default:
      sprintf(charsFormat, "%%0%dX", img->chrperpixel);
      sprintf(clrData.chars, charsFormat, clrIndex);
      break;
    }
		
    setXPMColor(img, clrIndex - 1, clrData);
  }
}

static unsigned char
getRegionCode(XPM *canvas, Point *pt)
{
    char code = 0;
    
    if(pt->y > canvas->height) code |= BIT3IS1;
    if(pt->y < WINDOWBOTTOM) code |= BIT2IS1;
    if(pt->x > canvas->width) code |= BIT1IS1;
    if(pt->x < WINDOWLEFT) code |= BIT0IS1;
    
    return code;
}

static void
trimLineToRegion(XPM *canvas, Point *start, Point *end, unsigned char codeStart, unsigned char codeEnd)
{
    int code, newX, newY;

    if(codeStart == 0) code = codeEnd;
    else code = codeStart;

    if(code & BIT0IS1)
    {
        newY = (*start).y + ((*end).y - (*start).y) * (WINDOWLEFT - (*start).x) / ((*end).x - (*start).x);
        newX = WINDOWLEFT;
    }
    else if(code & BIT1IS1)
    {
        newY = (*start).y + ((*end).y - (*start).y) * (canvas->width - (*start).x) / ((*end).x - (*start).x);
        newX = canvas->width;
    }
    else if(code & BIT2IS1)
    {
        newX = (*start).x + ((*end).x - (*start).x) * (WINDOWBOTTOM - (*start).y) / ((*end).y - (*start).y);
        newY = WINDOWBOTTOM;

    }
    else if(code & BIT3IS1)
    {
        newX = (*start).x + ((*end).x - (*start).x) * (canvas->height - (*start).y) / ((*end).y - (*start).y);
        newY = canvas->height;

    }
    if (code == codeStart)
    {
        (*start).x = newX;
        (*start).y = newY;
    }
    else
    {
        (*end).x = newX;
        (*end).y = newY;
    }
}

int 
cohenSutherlandFrameLine(XPM *canvas, Point *pStart, Point *pEnd)
{
  unsigned char codeStart = getRegionCode(canvas, pStart);
  unsigned char codeEnd = getRegionCode(canvas,pEnd);

    if((codeStart | codeEnd) == 0) return COMPLETELYIN;
    if((codeStart & codeEnd) != 0) return COMPLETELYOUT;
    trimLineToRegion(canvas, pStart, pEnd, codeStart, codeEnd);

    return cohenSutherlandFrameLine(canvas, pStart, pEnd);
}

static void
bresenhamX (XPM *canvas, Point q, Point r, int symmetry, unsigned short clrIndex)
{
    int dx, dy, D, x, y;
    dx = r.x - q.x;
    if (symmetry == ASYMMETRIC)
        dy = r.y - q.y;
    else dy = q.y - r.y;
    D = 2*dy - dx;
    y = q.y;
    for (x = q.x; x <= r.x; x++)
    {
      putXPMpixel(canvas, x, canvas->height - y, clrIndex);
        if (D <= 0) D += 2 * dy;
        else
        {
            D += 2*(dy - dx);
            if(symmetry == ASYMMETRIC)
                y++;
            else y--;
        }
    }
}

static void
bresenhamY (XPM *canvas, Point q, Point r, int symmetry, unsigned short clrIndex)
{
    int dx, dy, D, x, y;
    if(symmetry == ASYMMETRIC)
        dx = r.x - q.x;
    else dx = q.x - r.x;
    dy = r.y - q.y;
    D = 2*dx - dy;
    x = q.x;
    for (y = q.y; y <= r.y; y++)
    {
        putXPMpixel(canvas, x, canvas->height - y, clrIndex);
        if (D <= 0) D += 2 * dx;
        else
        {
            D += 2*(dx - dy);
            if(symmetry == ASYMMETRIC)
                x++;
            else x--;
        }
    }
}

void
drawBresenhamLine(XPM *canvas, Point pStart, Point pEnd, unsigned short colorIndex)
{
    if(pEnd.x > pStart.x &&
       pEnd.y >= pStart.y &&
       (pEnd.x - pStart.x) >= (pEnd.y - pStart.y))
        //octant 1
    {
        bresenhamX(canvas,pStart,pEnd,ASYMMETRIC,colorIndex);
    }
    else if(pEnd.x >= pStart.x &&
	    pEnd.y > pStart.y && 
	    (pEnd.x - pStart.x) < (pEnd.y - pStart.y))
        //octant 2
    {
        bresenhamY(canvas,pStart,pEnd,ASYMMETRIC,colorIndex);
    }
    else if(pEnd.x < pStart.x &&
	    pEnd.y > pStart.y && 
	    (pStart.x - pEnd.x) < (pEnd.y - pStart.y))
        //octant 3
    {
        bresenhamY(canvas,pStart,pEnd,SYMMETRIC,colorIndex);
    }
    else if(pEnd.x < pStart.x &&
	    pEnd.y >= pStart.y &&
	    (pStart.x - pEnd.x) >= (pEnd.y - pStart.y))
        //octant 4
    {
        bresenhamX(canvas,pEnd,pStart,SYMMETRIC,colorIndex);
    }
    else if (pEnd.x < pStart.x &&
	     pEnd.y < pStart.y &&
	     (pStart.x - pEnd.x) >= (pStart.y - pEnd.y))
        //octant 5
    {
        bresenhamX(canvas,pEnd,pStart,ASYMMETRIC,colorIndex);
    }
    else if (pEnd.x <= pStart.x &&
	     pEnd.y < pStart.y &&
	     (pStart.x - pEnd.x) < (pStart.y - pEnd.y))
        //octant 6
    {
        bresenhamY(canvas,pEnd,pStart,ASYMMETRIC,colorIndex);
    }
    else if(pEnd.x > pStart.x &&
	    pEnd.y < pStart.y &&
	    (pEnd.x - pStart.x) <= (pStart.y - pEnd.y))
        //octant 7
    {
        bresenhamY(canvas,pEnd,pStart,SYMMETRIC,colorIndex);
    }
    else //octant 8
    {
        bresenhamX(canvas,pStart,pEnd,SYMMETRIC,colorIndex);
    }
}
