#include "test.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <ctype.h>

#define SCREEN_WIDTH	320
#define SCREEN_HEIGHT   240

enum gradientType {
   NONE,
   LINEAR,
   RADIAL
} gradientType;



void display( void )
{
   VGPath  path;
   VGPaint fill;
   VGfloat black[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
   VGfloat color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

   VGubyte segments[] = { VG_MOVE_TO_ABS, VG_LINE_TO_REL, VG_LINE_TO_REL, VG_LINE_TO_REL, VG_CLOSE_PATH };

   VGfloat coords[]   = { 0.0f, 0.0f, 0.0f, 10.0f, 10.0f, 0.0f, 0.0f, -10.0f, -10.0f, 0.0f };

   VGfloat rampStops[20] = { 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
                             0.3f, 1.0f, 0.0f, 0.0f, 1.0f,
                             0.7f, 0.0f, 1.0f, 0.0f, 1.0f,
                             1.0f, 0.0f, 0.0f, 1.0f, 1.0f };

   VGfloat linearGradient[4] = { 2.0f, 2.0f, 8.0f, 8.0f };
   VGfloat radialGradient[5] = { 6.5f, 6.5f, 8.0f, 8.0f, 3.0f };

   vgSetfv( VG_CLEAR_COLOR, 4, color );
   vgClear( 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT );

   vgSeti ( VG_FILL_RULE, VG_NON_ZERO );

   vgLoadIdentity();
   vgScale( 8.0f, 8.0f );
   vgTranslate( 2.0f, 5.0f );

   path = vgCreatePath(VG_PATH_FORMAT_STANDARD, VG_PATH_DATATYPE_F, 1.0f, 0.0f, 0, 0, VG_PATH_CAPABILITY_ALL);
   vgAppendPathData( path, 4,segments, coords );

   fill = vgCreatePaint();
   vgSetPaint( fill, VG_FILL_PATH );

   switch (gradientType) {
   case RADIAL:
      vgSetParameteri( fill, VG_PAINT_TYPE, VG_PAINT_TYPE_RADIAL_GRADIENT );
      vgSetParameterfv( fill, VG_PAINT_RADIAL_GRADIENT, 5, radialGradient );
      break;
   case LINEAR:
   default:
      vgSetParameteri( fill, VG_PAINT_TYPE, VG_PAINT_TYPE_LINEAR_GRADIENT );
      vgSetParameterfv( fill, VG_PAINT_LINEAR_GRADIENT, 4, linearGradient );
      break;
   }

   vgSetParameterfv( fill, VG_PAINT_COLOR_RAMP_STOPS, 20, rampStops );
   vgSetParameteri( fill, VG_PAINT_COLOR_RAMP_SPREAD_MODE, VG_COLOR_RAMP_SPREAD_PAD );
   vgDrawPath( path, VG_FILL_PATH );

   vgTranslate( 12.0f, 0.0f );
   vgSetParameterfv( fill, VG_PAINT_COLOR_RAMP_STOPS, 20, rampStops );
   vgSetParameteri( fill, VG_PAINT_COLOR_RAMP_SPREAD_MODE, VG_COLOR_RAMP_SPREAD_REPEAT );
   vgDrawPath( path, VG_FILL_PATH );

   vgTranslate( 12.0f, 0.0f );
   vgSetParameterfv( fill, VG_PAINT_COLOR_RAMP_STOPS, 20, rampStops );
   vgSetParameteri( fill, VG_PAINT_COLOR_RAMP_SPREAD_MODE, VG_COLOR_RAMP_SPREAD_REFLECT );
   vgDrawPath( path, VG_FILL_PATH );

   vgDestroyPath( path );
   vgDestroyPaint( fill );

}

const char commands[] =
   "Commands\n"
   "H - this help\n"
   "l - linear\n"
   "r - radial\n"
   "q - quit";

const char help[] = "Press H for a list of commands";
void key(unsigned char code, int x, int y)
{
   static int open = 0;
   switch (tolower(code)) {
   case 'l':
     gradientType = LINEAR;
      break;
   case 'r':
     gradientType = RADIAL;
      break;
   case 'q':
      exit(EXIT_SUCCESS);
      break;
   case 'h':
      /* Show help */
      if (!open) {
         testOverlayString(commands);
         open = 1;
      } else {
         testOverlayString(help);
         open = 0;
      }
      break;
   }

}


int main(int argc, char *argv[])
{
   testInit(argc, argv, SCREEN_WIDTH,SCREEN_HEIGHT , "ShivaVG: gradient example");
   testCallback(TEST_CALLBACK_DISPLAY, (CallbackFunc) display);
   testCallback(TEST_CALLBACK_KEY, (CallbackFunc) key);

   testOverlayString(help);
   testOverlayColor(0.0f, 0.0f, 0.0f, 1.0f);

   gradientType = LINEAR;

   testRun();

   return 0;
}
