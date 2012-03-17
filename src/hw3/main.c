#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>

#include "xpmimage.h"
#include "xpmps.h"

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
  char *psInput = NULL;
  char *xpmOut = NULL;
  XPM *img = NULL;
  int option = 0;

  while((option = getopt(argc, argv, "f:w:h:o:")) != -1){
    switch(option){
    case 'f':
      psInput = optarg;
      break;
    case 'w':
      optWidth = atoi(optarg);
      break;
    case 'h':
      optHeight = atoi(optarg);
      break;
    case 'o':
      xpmOut = optarg;
      break;
    case '?':
      if(isprint(optopt))
	fprintf (stderr, "Unknown option `-%c'.\n", optopt);
      else
	fprintf (stderr,"Unknown option character `\\x%x'.\n", optopt);
      printUsage();
      exit(1);
      break;
    default:
      /* it shouldn't reach this point */
      exit(1);
      break;
    }
  }

  img = newXPM(optWidth, optHeight, 1, sizeof(clrTable)/(sizeof(unsigned char) * 3));
  assignXPMdisplayRegion(img, 0, optHeight, optWidth, 0);
  assignXPMColorTable(img, clrTable, sizeof(clrTable)/(sizeof(unsigned char) * 3));
  renderPSFile(img, psInput, &renderGElement);
  saveXPMtofile(img, xpmOut);
  printf("Program finished ...\n");

  freeXPM(&img);
  return 0;
}
