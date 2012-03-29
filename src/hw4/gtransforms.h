#ifndef _GTRANSFORMS_H_
#define _GTRANSFORMS_H_

#include "xpmimage.h"
#include "xpmps.h"

#define PI 3.14159265

enum TransformType{ TRANSLATION, SCALING, ROTATION };

struct trans{
  union{
    struct{
      int px, py;
      double sx, sy;
    }s;
    struct{
      int tx, ty;
    }t;
    struct{
      int px, py;
      int u;
    }r;
  }data;
  enum TransformType tType;
  struct trans *next;
};

struct transMatrix
{
	double matrix[3][3];
};

struct homoCoord
{
	double coord[3];
};

extern struct trans *
loadTStructure(char *);

extern void
freeTStrcuture(struct trans **);

extern void 
applyTransforms(struct trans *, struct GENode *);

extern void 
initTraslation(struct transMatrix *, int, int);

extern void 
initRotation(struct transMatrix *, int);

extern void 
initScale(struct transMatrix *, double , double);

extern void 
matrixProduct(struct transMatrix *, struct transMatrix, struct transMatrix);

extern double 
degToRad(int);

extern void 
matrixVectorProduct(struct homoCoord *, struct transMatrix, struct homoCoord);

extern void 
initHomoVector(struct homoCoord *, double , double);

extern void 
twoDCoord(GElement *, struct homoCoord, struct homoCoord);

#endif
