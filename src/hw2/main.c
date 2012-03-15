#include <stdio.h>

#include "xpmimage.h"
#include "xpmps.h"

int
main(int argc, char *argv[]){
  const unsigned char clrTable[][3] = {{255, 255, 255}, {0, 0, 0}};
  XPM *img = newXPM(300, 300, 1, sizeof(clrTable)/(sizeof(unsigned char) * 3));

  assignColorTable(img, clrTable, sizeof(clrTable)/(sizeof(unsigned char) * 3));
  renderPSFile(img, "test1.ps", &renderGElement);
  saveXPMtofile(img, "tema2.xpm");

  freeXPM(&img);
  return 0;
}
