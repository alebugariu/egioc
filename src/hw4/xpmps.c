#include "xpmps.h"

void
renderGElements(XPM *canvas, struct GENode *glist){
  struct GENode *pgl = glist;
  GElement *pge = NULL;

  while(NULL != pgl){
    pge = &pgl->el;
    switch(pge->type){
    case LINE:
      printf("Original Line := start{x:%4d y:%4d}, end{x:%4d y:%4d}\n", pge->data.line.st.x, pge->data.line.st.y, pge->data.line.en.x, pge->data.line.en.y);
      if(cohenSutherlandFrameLine(canvas, &pge->data.line.st, &pge->data.line.en) == 1){
	printf("Trimed Line := start{x:%4d y:%4d}, end{x:%4d y:%4d}\n", pge->data.line.st.x, pge->data.line.st.y, pge->data.line.en.x, pge->data.line.en.y);
	drawBresenhamLine(canvas, pge->data.line.st, pge->data.line.en, 1);
      }
      else{
	fprintf(stderr, "*** Line was totally outside the canvas.\n");
      }
      break;
    default:
      fprintf(stderr, "Unhandled type (%d) !\n", pge->type);
      break;
    }
    
    pgl = pgl->next;
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
      if(strlen(currentLine) != 1){
	newNode = (struct GENode *)malloc(sizeof(struct GENode));
	newNode->next = NULL;
	parsePSLine(currentLine, &newNode->el);
	
	if(NULL == glist){
	  glist = plastNode = newNode;
	} else {
	  plastNode->next = newNode;
	  plastNode = newNode;
	}
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
}
