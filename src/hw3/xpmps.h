#ifndef _XPMPS_H_
#define _XPMPS_H_

#include <math.h>
#include <string.h>

#include "xpmimage.h"

#define PS_BEGIN_MARK "%%%BEGIN\n"
#define PS_END_MARK "%%%END\n"

typedef enum {PSFILE_OK, ERROR_ON_OPEN, ERROR_ON_PARSE} procPSFileReturnVal;
typedef enum {PSLINE_OK, UNRECOGNIZED_ELEMENT} procPSLineReturnVal;
typedef enum {UNDEFINED_ELEMENT, BLANK, LINE, POINT} GElementType;

/* PS Graphics Element structure containing various primitive elements */
typedef struct{
  union{
    Point point;
    struct{
      Point st;
      Point en;
    } line;
  } data;
  GElementType type;
}GElement;


/* Renders a GElement to a XPM canvas */
extern void
renderGElement(XPM *canvas, GElement *el);

/* Extracts a GElement from a simple PS text line */
extern procPSLineReturnVal
parsePSLine(const char *line, GElement *dest);

/* Loads and processes a simple PS file format through GElement structures */
extern procPSFileReturnVal
renderPSFile(XPM *canvas, const char *file, void (*lineCallback)(XPM *, GElement *));

#endif
