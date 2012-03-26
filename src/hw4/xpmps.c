#include "xpmps.h"

void
renderGElement(XPM *canvas, GElement *el){
  switch(el->type){
  case LINE:
    printf("Original Line := start{x:%4d y:%4d}, end{x:%4d y:%4d}\n", el->data.line.st.x, el->data.line.st.y, el->data.line.en.x, el->data.line.en.y);
    if(cohenSutherlandFrameLine(canvas, &el->data.line.st, &el->data.line.en) == 1){
      printf("Trimed Line := start{x:%4d y:%4d}, end{x:%4d y:%4d}\n", el->data.line.st.x, el->data.line.st.y, el->data.line.en.x, el->data.line.en.y);
      drawBresenhamLine(canvas, el->data.line.st, el->data.line.en, 1);
    }
    else{
      fprintf(stderr, "*** Line was totally outside the canvas.\n");
    }
    break;
  default:
    fprintf(stderr, "Unhandled type!\n");
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

struct GENode *
loadPSFile(XPM *canvas, const char *file){
  char currentLine[255];
  FILE *fhandle = fopen(file, "r");
  struct GENode *glist = NULL;
  struct GENode *plastNode = NULL;
  struct GENode *newNode = NULL;

  if(NULL != fhandle){
    /* file got opened ok, go past the header */
    do{
      /* search for the beginning of relevant PS data structures */
      fgets(currentLine, sizeof(currentLine), fhandle);
    }while(!feof(fhandle) && strcmp(currentLine, PS_BEGIN_MARK) != 0);

    /* process the file */
    fgets(currentLine, sizeof(currentLine), fhandle);
    while(!feof(fhandle) && strcmp(currentLine, PS_END_MARK) != 0){
      newNode = (struct GENode *)malloc(sizeof(struct GENode));
      parsePSLine(currentLine, &newNode->el);
      
      if(NULL == glist){
	glist = plastNode = newNode;
      } else {
	newNode = plastNode->next;
	plastNode = newNode;
      }
      
      fgets(currentLine, sizeof(currentLine), fhandle);
    }

    fclose(fhandle);
  }

  return glist;
}

void
freePSFile(struct GENode **glist){
  struct GENode *pge = (*glist);
  struct GENode *nextpge;

  while(NULL != pge){
    nextpge = pge->next;
    free(pge);
    pge = nextpge;
  }

  free(*glist);
}
