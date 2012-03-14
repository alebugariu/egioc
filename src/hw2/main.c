#include <stdio.h>
#include <string.h>
#include "xpmimage.h"

#define PS_BEGIN_MARK "%%%BEGIN\n"
#define PS_END_MARK "%%%END\n"

typedef enum {PSFILE_OK, ERROR_ON_OPEN, ERROR_ON_PARSE} procPSFileReturnVal;
typedef enum {PSLINE_OK, UNRECOGNIZED_ELEMENT} procPSLineReturnVal;
typedef enum {UNDEFINED_ELEMENT, BLANK, LINE, POINT} GElementType;
typedef struct{
  union{
    struct Point{
      int x;
      int y;
    } point;
    struct{
      struct Point st;
      struct Point en;
    } line;
  } data;
  GElementType type;
}GElement;

void
printGElement(GElement *el){
  switch(el->type){
  case LINE:
    printf("Line := start{x:%4d y:%4d}, end{x:%4d y:%4d}\n", el->data.line.st.x, el->data.line.st.y, el->data.line.en.x, el->data.line.en.y);
    break;
  default:
    printf("Unhandled type!\n");
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
processPSFile(const char *file, void (*lineCallback)(GElement *)){
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
      if(NULL != lineCallback) (*lineCallback)(&aquiredEl);
      
      fgets(currentLine, sizeof(currentLine), fhandle);
    }

    fclose(fhandle);
  }

  return returnCode;
}

int
main(int argc, char *argv[]){
  processPSFile("test.ps", &printGElement);
  return 0;
}
