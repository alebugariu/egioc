#ifndef _GTRANSFORMS_H_
#define _GTRANSFORMS_H_

#include "xpmimag.h"
#include "xpmps.h"

enum TransformType{ TRANSLATION, SCALING, ROTATION };

struct trans{
  union{
    struct{
      int sx, sy;
    }s;
    struct{
      int tx, ty;
    }t;
    struct{
      int t;
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
applyTransforms(struct trans *, GElement *);

#endif
