#include "gtransforms.h"

struct trans *
loadTStructure(char *tsFile){
  FILE *fhandle = fopen(tsFile, "r");
  struct trans *tsList = NULL;
  struct trans *pts = NULL;
  struct trans *newts = NULL;
  char transType;

  if(NULL != fhandle){
    /* process the file */
    fscanf(fhandle, "%c", &transType);
    while(!feof(fhandle)){
      newts = (struct trans *)malloc(sizeof(struct trans));
      newts->next = NULL;

      switch(transType){
      case 't':
	newts->tType = TRANSLATION;
	fscanf(fhandle, "%d %d", &newts->data.t.tx, &newts->data.t.ty);
	break;
      case 's':
	newts->tType = SCALING;
	fscanf(fhandle, "%d %d %lf %lf", &newts->data.s.px, &newts->data.s.py, &newts->data.s.sx, &newts->data.s.sy);
	break;
      case 'r':
	newts->tType = ROTATION;
	fscanf(fhandle, "%d %d %d", &newts->data.r.px, &newts->data.r.py, &newts->data.r.u);
	break;
      }

      if(NULL == tsList) tsList = pts = newts;
      else {
	pts->next = newts;
	pts = newts;
      }
      fgetc(fhandle);
      fscanf(fhandle, "%c", &transType);
    }
  }
  fclose(fhandle);
  
  return tsList;
}

void
freeTStrcuture(struct trans **tlist){
  struct trans *ptl = (*tlist);
  struct trans *nextptl = NULL;

  while(NULL != ptl){
    nextptl = ptl->next;
    free(ptl);
    ptl = nextptl;
  }
}

static void
applyTranslation(GElement *el, int tx, int ty){
  /* Used formula :
   * XNew = XOld + tx
   * YNew = XOld + ty
   */
  switch(el->type){
  case LINE:
    el->data.line.st.x += tx;
    el->data.line.en.x += tx;
    el->data.line.st.y += ty;
    el->data.line.en.y += ty;
    break;
  default:
    break;
  }
}

static void
applyScaling(GElement *el, int px, int py, double sx, double sy){
  /* Used formula :
   * XNew = XOld * sx - sx * px + px
   * YNew = XOld * sy - sy * py + py
   */
  double constx = -1 * sx * px + px;
  double consty = -1 * sy * py + py;

  switch(el->type){
  case LINE:
    el->data.line.st.x = el->data.line.st.x * sx + constx;
    el->data.line.en.x = el->data.line.en.x * sx + constx;
    el->data.line.st.y = el->data.line.st.y * sy + consty;
    el->data.line.en.y = el->data.line.en.y * sy + consty;
    break;
  default:
    break;
  }
}

static void
applyRotation(GElement *el, int px, int py, int u){
  /* Used formula :
   * XNew = XOld * cos(u) - YOld * sin(u) - px * cos(u) + py * sin(u) + px
   * YNew = XOld * sin(u) + YOld * cos(u) - px * sin(u) - py * cos(u) + py
   */
  double radu = u * PI/180;
  double constx = -1 * px * cos(radu) + py * sin(radu) + px;
  double consty = -1 * px * sin(radu) - py * cos(radu) + py;

  switch(el->type){
  case LINE:
    el->data.line.st.x = el->data.line.st.x * cos(radu) - el->data.line.st.y * sin(radu) + constx;
    el->data.line.en.x = el->data.line.en.x * cos(radu) - el->data.line.en.y * sin(radu) + constx;
    el->data.line.st.y = el->data.line.st.x * sin(radu) + el->data.line.st.y * cos(radu) + consty;
    el->data.line.en.y = el->data.line.en.x * sin(radu) + el->data.line.en.y * cos(radu) + consty;
    break;
  default:
    break;
  }
}

void 
applyTransforms(struct trans *tlist, struct GENode *glist){
  struct trans *ptl = tlist;
  struct GENode *pgl = NULL;

  while(NULL != ptl){
    pgl = glist;
    while(NULL != pgl){
      switch(ptl->tType){
      case TRANSLATION:
	applyTranslation(&pgl->el, ptl->data.t.tx, ptl->data.t.ty);
	break;
      case SCALING:
	applyScaling(&pgl->el, ptl->data.s.px, ptl->data.s.py, ptl->data.s.sx, ptl->data.s.sy);
	break;
      case ROTATION:
	applyRotation(&pgl->el, ptl->data.r.px, ptl->data.r.py, ptl->data.r.u);
	break;
      default:
	break;
      }
      pgl = pgl->next;
    }
    ptl = ptl->next;
  }
}
