#ifndef _XPMIMAGE_H_
#define _XPMIMAGE_H_

#include <stdio.h>
#include <stdlib.h>

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

#endif
