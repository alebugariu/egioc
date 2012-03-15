#ifndef _XPMIMAGE_H_
#define _XPMIMAGE_H_

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

/* Macro definition region */
#define SYMMETRIC 1
#define ASYMMETRIC 0

#define BIT0IS1 1
#define BIT1IS1 2
#define BIT2IS1 4
#define BIT3IS1 8

#define WINDOWLEFT 0
#define WINDOWBOTTOM 0

#define COMPLETELYIN 1
#define COMPLETELYOUT 0

/* A simple point structure */
typedef struct{
  int x;
  int y;
}Point;

/* Color structure that holds pixel data */
typedef struct{
	unsigned short red;
	unsigned short green;
	unsigned short blue;
} Color;

/* XPM specific color structure (see Chapter 2 of "XPM Manual", v3.4i) */
typedef struct{
	Color clr;
	char *chars;
	char *key;
} XPMColor;

/* XPM structure that holds a complete XPM image segment */
typedef struct{
  unsigned int width; 
  unsigned int height; 
  unsigned short chrperpixel;
  unsigned int ncolors; 
  XPMColor *colta;
  unsigned short **data;
} XPM;

/* Initialize a XPM structure with custom
 * width, height, characters/pixel and number of colors
 */
extern XPM * 
newXPM(unsigned int width, unsigned int height, unsigned short cpp, unsigned int ncolors);

/* Frees up the memory used by a XPM heap alocated structure */
extern void
freeXPM(XPM **img);

/* Inserts a pixel value specified by it's table index at (x, y) with x mod chrperpixel = 0 
 * Returns 1 if succesfull and 0 if not
 */
extern int
putXPMpixel(XPM *img, unsigned int x, unsigned int y, unsigned short colorindex);

/* Assigns a color to a color table entry (colta) in RGB format 
 * Returns 1 if succesfull and 0 if not
 */
extern int
setXPMColor(XPM *img, unsigned int index, XPMColor pixdata);

/* Saves the XPM image to a disk file located at 'filepath'
 * Returns 1 if succesfull and 0 if not
 */
extern int
saveXPMtofile(XPM *img, char *filepath);

/* Assigns a color matrix of nx3 values to the desired XPM structure */
void
assignColorTable(XPM *img, const unsigned char vColors[][3], int clrCnt);

/* Draws a line onto the XPM image using Bresenham' algorithm */
extern void
drawBresenhamLine(XPM *canvas, Point pStart, Point pEnd, unsigned short colorIndex);

/* Trims a line so that it may fit inside the XPM image */
extern int 
cohenSutherlandFrameLine(XPM *canvas, Point *pStart, Point *pEnd);

#endif
