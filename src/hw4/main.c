#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "xpmimage.h"
#include "xpmps.h"

struct option{
  char *name;
  int expectedArgCount;
  int isOptional;
  int visitCount;
};

void
printUsage(void){
  printf("Usage: lineout <options>\n"
	 "Where <options> is a combination of the following values (order doesn't matter):\n"
	 "-f <ps_input_file> : PS simplified input file\n"
	 "-w <output_width>  : desired output width of image\n"
	 "-h <output_height> : desired output height of image\n"
	 "-o <output_xpm>    : XPM desired output file\n"
	 "ALL of the above options are mandatory!\n\n"
	 "Beseides mandatory options, there are the optional ones:\n"
	 "-wl <window_left_margin>  : left margin clipping value\n"
	 "-wr <window_right_margin> : right margin clipping value\n"
	 "-wb <window_bottom_margin>: bottom margin clipping value\n"
	 "-wt <window_top_margin>   : top margin clipping value\n"
	 "Please try again ...\n");
}

int
main(int argc, char *argv[]){
  if(argc == 1){
    printUsage();
    exit(1);
  }

  const unsigned char clrTable[][3] = {
    {255, 255, 255}, /* colorIndex = 0 : WHITE */ 
    {0, 0, 0}        /* colorIndex = 1 : BLACK */
  };
  int optWidth = 1;
  int optHeight = 1;
  int optwLeft = 0;
  int optwRight = 0;
  int optwBottom = 0;
  int isClipSet = 0;
  int optwTop = 0;
  char *psInput = NULL;
  char *xpmOut = NULL;
  char *transformFile = NULL;
  XPM *img = NULL;
  struct GENode *psList = NULL;

  struct option options[] = {
    {"-wt", 1, 1, 0},
    {"-wl", 1, 1, 0},
    {"-wr", 1, 1, 0},
    {"-wb", 1, 1, 0},
    {"-t",  1, 0, 0},
    {"-f",  1, 0, 0},
    {"-w",  1, 0, 0},
    {"-h",  1, 0, 0},
    {"-o",  1, 0, 0},
    {0,    0,  0, 0}
  };

  int argIndex = 1;
  int optIndex = 0;
  for(argIndex = 1; argIndex < argc; ++argIndex){
    for(optIndex = 0; options[optIndex].name != NULL; ++optIndex){
      if(strcmp(argv[argIndex], options[optIndex].name) == 0){
	if(strcmp(argv[argIndex], "-wt") == 0){
	  /* set clip top margin */
	  optwTop = atoi(argv[argIndex + 1]);
	  isClipSet = 1;
	} else if(strcmp(argv[argIndex], "-wl") == 0){
	  /* set clip left margin */
	  optwLeft = atoi(argv[argIndex + 1]);
	  isClipSet = 1;
	} else if(strcmp(argv[argIndex], "-wr") == 0){
	  /* set clip right margin */
	  optwRight = atoi(argv[argIndex + 1]);
	  isClipSet = 1;
	} else if(strcmp(argv[argIndex], "-wb") == 0){
	  optwBottom = atoi(argv[argIndex + 1]);
	  isClipSet = 1;
	} else if(strcmp(argv[argIndex], "-t") == 0){
	  transformFile = argv[argIndex + 1];
	} else if(strcmp(argv[argIndex], "-f") == 0){
	  psInput = argv[argIndex + 1];
	} else if(strcmp(argv[argIndex], "-w") == 0){
	  optWidth = atoi(argv[argIndex + 1]);
	} else if(strcmp(argv[argIndex], "-h") == 0){
	  optHeight = atoi(argv[argIndex + 1]);
	} else if(strcmp(argv[argIndex], "-o") == 0){
	  xpmOut = argv[argIndex + 1];
	} else {
	  fprintf(stderr, "Option not handled yet!\n");
	}
	argIndex += options[optIndex].expectedArgCount;
	break;
      }
    }
  }

  if(optwRight == 0) optwRight = optWidth;
  if(optwTop == 0)  optwTop = optHeight;

  img = newXPM(optWidth, optHeight, 1, sizeof(clrTable)/(sizeof(unsigned char) * 3));
  /*printf("%d l: %d r: %d, b: %d t: %d", isClipSet, optwLeft, optwRight, optwBottom, optwTop);*/
  if(0 != isClipSet) assignXPMdisplayRegion(img, optwLeft, optwTop, optwRight, optwBottom);
  assignXPMColorTable(img, clrTable, sizeof(clrTable)/(sizeof(unsigned char) * 3));

  /* load the PS data into the XPM structure */
  psList = loadPSFile(img, psInput);
  /* apply the desired graphical transformations on the XPM structure */
  if(NULL != transformFile){
    
  }
  saveXPMtofile(img, xpmOut);
  printf("Program finished ...\n");

  freeXPM(&img);
  return 0;
}
