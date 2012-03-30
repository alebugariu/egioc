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
    Load the following matrix into 'm' :
    cos(t) -sin(t)  0
    sin(t)  cos(t)  0
      0       0     1
   */
  m->matrix[0][0] = cos(radAngle); m->matrix[0][1] = -1*sin(radAngle); m->matrix[0][2] = 0;
  m->matrix[1][0] = sin(radAngle); m->matrix[1][1] =    cos(radAngle); m->matrix[1][2] = 0;
  m->matrix[2][0] =             0; m->matrix[2][1] =                0; m->matrix[2][2] = 1;
}

void initScale(struct transMatrix * m, double sx, double sy)
{
   /*
    Load the following matrix into 'm' :
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

void twoDCoord(Point * el, struct homoCoord point)
{
	double factor = point.coord[2];
        el->x = point.coord[0]/factor;
        el->y = point.coord[1]/factor;
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
      twoDCoord(&pgl->el.data.line.st, start);
      twoDCoord(&pgl->el.data.line.en, end);     
      pgl = pgl->next;
    }
    ptl = ptl->next;
  }
}


