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

extern struct trans *
loadTStructure(char *);

extern void
freeTStrcuture(struct trans **);

extern void 
applyTransforms(struct trans *, struct GENode *);

#endif
