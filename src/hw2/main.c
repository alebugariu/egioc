#include <stdio.h>
#include <string.h>
#include "xpmimage.h"

#define SYMMETRIC 1
#define ASYMMETRIC 0

#define BIT0IS1 1
#define BIT1IS1 2
#define BIT2IS1 4
#define BIT3IS1 8

#define WINDOWLEFT 0
#define WINDOWBOTTOM 0
#define HEIGHT 50
#define WIDTH 50

#define COMPLETELYIN 1
#define COMPLETELYOUT 0

#define PS_BEGIN_MARK "%%%BEGIN\n"
#define PS_END_MARK "%%%END\n"

typedef enum {PSFILE_OK, ERROR_ON_OPEN, ERROR_ON_PARSE} procPSFileReturnVal;
typedef enum {PSLINE_OK, UNRECOGNIZED_ELEMENT} procPSLineReturnVal;
typedef enum {UNDEFINED_ELEMENT, BLANK, LINE, POINT} GElementType;

typedef struct{
  int x;
  int y;
}Point;

typedef struct{
  union{
    Point point;
    struct{
      Point st;
      Point en;
    } line;
  } data;
  GElementType type;
}GElement;

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
CSFrameLine(XPM *canvas, Point *pStart, Point *pEnd)
{
  unsigned char codeStart = getRegionCode(canvas, pStart);
  unsigned char codeEnd = getRegionCode(canvas,pEnd);

    if((codeStart | codeEnd) == 0) return COMPLETELYIN;
    if((codeStart & codeEnd) != 0) return COMPLETELYOUT;
    trimLineToRegion(canvas, pStart, pEnd, codeStart, codeEnd);

    return CSFrameLine(canvas, pStart, pEnd);
}

void
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

void
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
        printf("Octant 1:");
        bresenhamX(canvas,pStart,pEnd,ASYMMETRIC,colorIndex);
    }
    else if(pEnd.x >= pStart.x &&
	    pEnd.y > pStart.y && 
	    (pEnd.x - pStart.x) < (pEnd.y - pStart.y))
        //octant 2
    {
        printf("Octant 2:");
        bresenhamY(canvas,pStart,pEnd,ASYMMETRIC,colorIndex);
    }
    else if(pEnd.x < pStart.x &&
	    pEnd.y > pStart.y && 
	    (pStart.x - pEnd.x) < (pEnd.y - pStart.y))
        //octant 3
    {
        printf("Octant 3:");
        bresenhamY(canvas,pStart,pEnd,SYMMETRIC,colorIndex);
    }
    else if(pEnd.x < pStart.x &&
	    pEnd.y >= pStart.y &&
	    (pStart.x - pEnd.x) >= (pEnd.y - pStart.y))
        //octant 4
    {
        printf("Octant 4:");
        bresenhamX(canvas,pEnd,pStart,SYMMETRIC,colorIndex);
    }
    else if (pEnd.x < pStart.x &&
	     pEnd.y < pStart.y &&
	     (pStart.x - pEnd.x) >= (pStart.y - pEnd.y))
        //octant 5
    {
        printf("Octant 5:");
        bresenhamX(canvas,pEnd,pStart,ASYMMETRIC,colorIndex);
    }
    else if (pEnd.x <= pStart.x &&
	     pEnd.y < pStart.y &&
	     (pStart.x - pEnd.x) < (pStart.y - pEnd.y))
        //octant 6
    {
        printf("Octant 6:");
        bresenhamY(canvas,pEnd,pStart,ASYMMETRIC,colorIndex);
    }
    else if(pEnd.x > pStart.x &&
	    pEnd.y < pStart.y &&
	    (pEnd.x - pStart.x) <= (pStart.y - pEnd.y))
        //octant 7
    {
        printf("Octant 7:");
        bresenhamY(canvas,pEnd,pStart,SYMMETRIC,colorIndex);
    }
    else //octant 8
    {
        printf("Octant 8:");
        bresenhamX(canvas,pStart,pEnd,SYMMETRIC,colorIndex);
    }
}

void
printGElement(XPM *canvas, GElement *el){
  switch(el->type){
  case LINE:
    printf("Original Line := start{x:%4d y:%4d}, end{x:%4d y:%4d}\n", el->data.line.st.x, el->data.line.st.y, el->data.line.en.x, el->data.line.en.y);
    if(CSFrameLine(canvas, &el->data.line.st, &el->data.line.en) == 1){
      printf("Trimed Line := start{x:%4d y:%4d}, end{x:%4d y:%4d}\n", el->data.line.st.x, el->data.line.st.y, el->data.line.en.x, el->data.line.en.y);
      drawBresenhamLine(canvas, el->data.line.st, el->data.line.en, 1);
    }
    else{
      printf("*** Line was totally outside the canvas.\n");
    }
    break;
  default:
    printf("Unhandled type!\n");
    break;
  }
}

procPSLineReturnVal
parsePSLine(const char *line, GElement *dest){
  procPSLineReturnVal returnCode = PSLINE_OK;

  if(sscanf(line, "%d %d %d %d line", &dest->data.line.st.x, &dest->data.line.st.y, &dest->data.line.en.x, &dest->data.line.en.y) == 4){
    dest->type = LINE;
  }

  return returnCode;
}

procPSFileReturnVal
renderPSFile(XPM *canvas, const char *file, void (*lineCallback)(XPM *, GElement *)){
  procPSFileReturnVal returnCode = PSFILE_OK;
  char currentLine[255];
  GElement aquiredEl;
  FILE *fhandle = fopen(file, "r");

  if(NULL == fhandle) returnCode = ERROR_ON_OPEN;
  else{
    /* file got opened ok, go past the header */
    do{
      /* search for the beginning of relevant PS data structures */
      fgets(currentLine, sizeof(currentLine), fhandle);
    }while(!feof(fhandle) && strcmp(currentLine, PS_BEGIN_MARK) != 0);

    /* process the file */
    fgets(currentLine, sizeof(currentLine), fhandle);
    while(!feof(fhandle) && strcmp(currentLine, PS_END_MARK) != 0){
      parsePSLine(currentLine, &aquiredEl);
      if(NULL != lineCallback) (*lineCallback)(canvas, &aquiredEl);
      
      fgets(currentLine, sizeof(currentLine), fhandle);
    }

    fclose(fhandle);
  }

  return returnCode;
}

void
assignColorTable(XPM *img, const unsigned char vColors[][3], int clrCnt){
  XPMColor clrData;
  int clrIndex = 0;

  img->ncolors = clrCnt;
  for(clrIndex = 1; clrIndex <= clrCnt; clrIndex ++){
    /* load RGB values */
    clrData.clr.blue = vColors[clrIndex - 1][0];
    clrData.clr.green = vColors[clrIndex - 1][1];
    clrData.clr.red = vColors[clrIndex - 1][2];

    /* load dynamic data */
    clrData.key = (char *)malloc(sizeof(char) + 1);
    strcpy(clrData.key, "c");
    clrData.chars = (char *)malloc(img->chrperpixel * sizeof(char));
    sprintf(clrData.chars, "%02X", clrIndex - 1);
		
    setXPMColor(img, clrIndex - 1, clrData);
  }
}

int
main(int argc, char *argv[]){
  const unsigned char clrTable[][3] = {{255, 255, 255}, {0, 0, 0}};
  XPM *img = newXPM(200, 200, 2, sizeof(clrTable)/(sizeof(unsigned char) * 3));

  assignColorTable(img, clrTable, sizeof(clrTable)/(sizeof(unsigned char) * 3));
  renderPSFile(img, "test.ps", &printGElement);
  saveXPMtofile(img, "tema2.xpm");

  freeXPM(&img);
  return 0;
}
