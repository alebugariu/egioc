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

double degToRad(int angle)
{
	return angle * PI/180;
}

void initTranslation(struct transMatrix * m, int tx, int ty)
{
  /*
   Load the following matrix into 'm' :
   1 0 tx
   0 1 tx
   0 0  1
  */
  m->matrix[0][0] = 1; m->matrix[0][1] = 0; m->matrix[0][2] = tx;
  m->matrix[1][0] = 0; m->matrix[1][1] = 1; m->matrix[1][2] = ty;
  m->matrix[2][0] = 0; m->matrix[2][1] = 0; m->matrix[2][2] = 1;	
}

void initRotation(struct transMatrix * m, int angle)
{
  double radAngle = degToRad(angle);
  /*
    Load the followin matrix into 'm' :
    cos -sin  0
    sin cos   0
      0   0   1
   */
  m->matrix[0][0] = cos(radAngle); m->matrix[0][1] = -1*sin(radAngle); m->matrix[0][2] = 0;
  m->matrix[1][0] = sin(radAngle); m->matrix[1][1] =    cos(radAngle); m->matrix[1][2] = 0;
  m->matrix[2][0] =             0; m->matrix[2][1] =                0; m->matrix[2][2] = 1;
}

void initScale(struct transMatrix * m, double sx, double sy)
{
   /*
    Load the followin matrix into 'm' :
    sx  0 0
     0 sy 0
     0  0 1
   */
  m->matrix[0][0] = sx; m->matrix[0][1] =  0; m->matrix[0][2] = 0;
  m->matrix[1][0] =  0; m->matrix[1][1] = sy; m->matrix[1][2] = 0;
  m->matrix[2][0] =  0; m->matrix[2][1] =  0; m->matrix[2][2] = 1;
}

void matrixProduct(struct transMatrix *prod, struct transMatrix a, struct transMatrix b)
{
	int lin, col, index;
        double sum;
        for(lin=0; lin<3; lin++)
            for(col=0; col<3; col++)
            {
                sum = 0;
                for(index=0; index<3; index++)
                    sum+= a.matrix[lin][index]*b.matrix[index][col];
                prod->matrix[lin][col] = sum;
            }
}

void matrixVectorProduct(struct homoCoord *prod, struct transMatrix m, struct homoCoord vect)
{
	int lin, col;
        double sum = 0;

        for(lin=0; lin<3; lin++)
           {
		sum = 0;
                for(col=0; col<3; col++)
                    sum+= m.matrix[lin][col]*vect.coord[col];
                prod->coord[lin] = sum; 
           }
}

void initHomoVector(struct homoCoord *point, double x, double y)
{
	point->coord[0] = x;
        point->coord[1] = y;
        point->coord[2] = 1;
}

void twoDCoord(GElement *el, struct homoCoord pointStart, struct homoCoord pointEnd)
{
	double factor = pointStart.coord[2];
        el->data.line.st.x = pointStart.coord[0]/factor;
        el->data.line.st.y = pointStart.coord[1]/factor;
        factor = pointEnd.coord[2];
        el->data.line.en.x = pointEnd.coord[0]/factor;
        el->data.line.en.y = pointEnd.coord[1]/factor;

}

void 
applyTransforms(struct trans *tlist, struct GENode *glist){
  struct trans *ptl = tlist;
  struct GENode *pgl = NULL;

  struct transMatrix m, aux;
  struct homoCoord start;
  struct homoCoord end;

  while(NULL != ptl){
    pgl = glist;
    while(NULL != pgl){
      switch(pgl->el.type)
      {case LINE: initHomoVector(&start, pgl->el.data.line.st.x, pgl->el.data.line.st.y);
                  initHomoVector(&end, pgl->el.data.line.en.x, pgl->el.data.line.en.y);
                  break;
       default: break;
      }
      switch(ptl->tType){
      case TRANSLATION: initTranslation(&m, ptl->data.t.tx, ptl->data.t.ty);
	                break;
      case SCALING: initScale(&m, ptl->data.s.sx, ptl->data.s.sy);
                    if (!(ptl->data.s.px == 0 && ptl->data.s.py == 0))
                        {
                          initTranslation(&aux, -1*ptl->data.s.px, -1*ptl->data.s.py);
                          matrixProduct(&m, m, aux);
                          initTranslation(&aux, ptl->data.s.px, ptl->data.s.py);
                          matrixProduct(&m, m, aux);
                        }
	             break;
      case ROTATION: initRotation(&m, ptl->data.r.u);
                     if (!(ptl->data.s.px == 0 && ptl->data.s.py == 0))
                        {
                          initTranslation(&aux, -1*ptl->data.r.px, -1*ptl->data.r.py);
                          matrixProduct(&m, m, aux);
                          initTranslation(&aux, ptl->data.s.px, ptl->data.s.py);
                          matrixProduct(&m, m, aux);
                        }
	             break;
      default: break;
      }
      matrixVectorProduct(&start, m, start);
      matrixVectorProduct(&end, m, end);
      twoDCoord(&pgl->el, start, end);     
      pgl = pgl->next;
    }
    ptl = ptl->next;
  }
}


