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

void
printGElement(GElement *el){
  switch(el->type){
  case LINE:
    printf("Line := start{x:%4d y:%4d}, end{x:%4d y:%4d}\n", el->data.line.st.x, el->data.line.st.y, el->data.line.en.x, el->data.line.en.y);
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
processPSFile(const char *file, void (*lineCallback)(GElement *)){
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
      if(NULL != lineCallback) (*lineCallback)(&aquiredEl);
      
      fgets(currentLine, sizeof(currentLine), fhandle);
    }

    fclose(fhandle);
  }

  return returnCode;
}

unsigned char
getRegionCode(Point p)
{
    char code = 0;
    
    if(p.y > HEIGHT) code |= BIT3IS1;
    if(p.y < WINDOWBOTTOM) code |= BIT2IS1;
    if(p.x > WIDTH) code |= BIT1IS1;
    if(p.x < WINDOWLEFT) code |= BIT0IS1;
    
    return code;
}

void
trimLine(Point *start, Point *end, char codeStart, char codeEnd)
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
        newY = (*start).y + ((*end).y - (*start).y) * (WIDTH- (*start).x) / ((*end).x - (*start).x);
        newX = WIDTH;
    }
    else if(code & BIT2IS1)
    {
        newX = (*start).x + ((*end).x - (*start).x) * (WINDOWBOTTOM - (*start).y) / ((*end).y - (*start).y);
        newY = WINDOWBOTTOM;

    }
    else if(code & BIT3IS1)
    {
        newX = (*start).x + ((*end).x - (*start).x) * (HEIGHT - (*start).y) / ((*end).y - (*start).y);
        newY = HEIGHT;

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
cohenSutherland(Point *pStart, Point *pEnd)
{
    unsigned char codeStart = getRegionCode(*pStart);
    unsigned char codeEnd = getRegionCode(*pEnd);

    if((codeStart | codeEnd) == 0) return COMPLETELYIN;
    if((codeStart & codeEnd) != 0) return COMPLETELYOUT;
    trimLine(pStart, pEnd, codeStart, codeEnd);

    return cohenSutherland(pStart, pEnd);
}

void
writePixel(int x, int y)
{
    //va apela putPixel din tema1 cu argumetele x si HEIGHT - y
}

void
bresenhamX (Point q, Point r, int symmetry)
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
        writePixel (x, y);
        if (D <= 0) D += 2 * dy;
        else
        {
            D += 2*(dy - dx);
            if(symmetry == ASYMMETRIC)
                y++;
            else y--;
        }
    }
    printf("\n");
}

void
bresenhamY (Point q, Point r, int symmetry)
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
        writePixel (x, y);
        if (D <= 0) D += 2 * dx;
        else
        {
            D += 2*(dx - dy);
            if(symmetry == ASYMMETRIC)
                x++;
            else x--;
        }
    }
    printf("\n");
}

void
drawBresenhamLine(Point pStart, Point pEnd)
{
    if(pEnd.x > pStart.x &&
       pEnd.y >= pStart.y &&
       (pEnd.x - pStart.x) >= (pEnd.y - pStart.y))
        //octant 1
    {
        printf("Octant 1:");
        bresenhamX(pStart,pEnd,ASYMMETRIC);
    }
    else if(pEnd.x >= pStart.x &&
	    pEnd.y > pStart.y && 
	    (pEnd.x - pStart.x) < (pEnd.y - pStart.y))
        //octant 2
    {
        printf("Octant 2:");
        bresenhamY(pStart,pEnd,ASYMMETRIC);
    }
    else if(pEnd.x < pStart.x &&
	    pEnd.y > pStart.y && 
	    (pStart.x - pEnd.x) < (pEnd.y - pStart.y))
        //octant 3
    {
        printf("Octant 3:");
        bresenhamY(pStart,pEnd,SYMMETRIC);
    }
    else if(pEnd.x < pStart.x &&
	    pEnd.y >= pStart.y &&
	    (pStart.x - pEnd.x) >= (pEnd.y - pStart.y))
        //octant 4
    {
        printf("Octant 4:");
        bresenhamX(pEnd,pStart,SYMMETRIC);
    }
    else if (pEnd.x < pStart.x &&
	     pEnd.y < pStart.y &&
	     (pStart.x - pEnd.x) >= (pStart.y - pEnd.y))
        //octant 5
    {
        printf("Octant 5:");
        bresenhamX(pEnd,pStart,ASYMMETRIC);
    }
    else if (pEnd.x <= pStart.x &&
	     pEnd.y < pStart.y &&
	     (pStart.x - pEnd.x) < (pStart.y - pEnd.y))
        //octant 6
    {
        printf("Octant 6:");
        bresenhamY(pEnd,pStart,ASYMMETRIC);
    }
    else if(pEnd.x > pStart.x &&
	    pEnd.y < pStart.y &&
	    (pEnd.x - pStart.x) <= (pStart.y - pEnd.y))
        //octant 7
    {
        printf("Octant 7:");
        bresenhamY(pEnd,pStart,SYMMETRIC);
    }
    else //octant 8
    {
        printf("Octant 8:");
        bresenhamX(pStart,pEnd,SYMMETRIC);
    }
}

int
main(int argc, char *argv[]){
  processPSFile("test.ps", &printGElement);
  return 0;
}
