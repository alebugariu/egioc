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

procPSFileReturnVal
renderPSFile(XPM *canvas, const char *file, void (*lineCallback)(XPM *, GElement *)){
  procPSFileReturnVal returnCode = PSFILE_OK;
  char currentLine[255];
  GElement aquiredEl;
  FILE *fhandle = fopen(file, "r");

  if(NULL == fhandle) returnCode = ERROR_ON_OPEN;
  else{
    /* file got opened ok, go past the header */
    do{
      /* search for the beginning of relevant PS data structures */
      fgets(currentLine, sizeof(currentLine), fhandle);
    }while(!feof(fhandle) && strcmp(currentLine, PS_BEGIN_MARK) != 0);

    /* process the file */
    fgets(currentLine, sizeof(currentLine), fhandle);
    while(!feof(fhandle) && strcmp(currentLine, PS_END_MARK) != 0){
      parsePSLine(currentLine, &aquiredEl);
      if(NULL != lineCallback) (*lineCallback)(canvas, &aquiredEl);
      
      fgets(currentLine, sizeof(currentLine), fhandle);
    }

    fclose(fhandle);
  }

  return returnCode;
}
