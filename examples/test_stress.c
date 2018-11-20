#include "test.h"
#include <VG/openvg.h>
#include <VG/vgu.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>

typedef struct _ControlPoint {
   VGfloat x, y;
   VGfloat dx, dy;
} ControlPoint;

bool doDraw = true;
bool openVGInitialized = false;
VGint windowWidth = 512;
VGint windowHeight = 512;
VGfloat windowScaleX, windowScaleY;
VGRenderingQuality quality = VG_RENDERING_QUALITY_BETTER;
VGint drawnFrames, totalFrames;
unsigned timeRand;

VGPath path;
VGPaint paint;
VGubyte pathSegs[128];
VGfloat pathData[256];
ControlPoint controlPoints[12];

#define BOUNCE_DIMENSION 201.0f



void genPaints(void)
{
   VGfloat col[4];

   paint = vgCreatePaint();
   col[0] = 0.0f;
   col[1] = 0.1f;
   col[2] = 0.4f;
   col[3] = 1.0f;
   vgSetParameteri(paint, VG_PAINT_TYPE, VG_PAINT_TYPE_COLOR);
   vgSetParameterfv(paint, VG_PAINT_COLOR, 4, col);
}

void genPaths(void)
{
   VGint i, j;

   path = vgCreatePath(VG_PATH_FORMAT_STANDARD, VG_PATH_DATATYPE_F, 1.0f, 0.0f, 0, 0, VG_PATH_CAPABILITY_ALL);

   controlPoints[0].x = 0.0f; controlPoints[0].y = -200.0f;
   controlPoints[1].x = -100.0f; controlPoints[1].y = -200.0f;
   controlPoints[2].x = -200.0f; controlPoints[2].y = -200.0f;
   controlPoints[3].x = -200.0f; controlPoints[3].y = 0.0f;
   controlPoints[4].x = -100.0f; controlPoints[4].y = 200.0f;
   controlPoints[5].x = 0.0f; controlPoints[5].y = 200.0f;
   controlPoints[6].x = 200.0f; controlPoints[6].y = 200.0f;
   controlPoints[7].x = 200.0f; controlPoints[7].y = 0.0f;
   for (i = 0; i < 8; ++i)
   {
      controlPoints[i].dx = (VGfloat)(2.0f * (rand() % 256) / 256.0f) - 1.0f;
      controlPoints[i].dy = (VGfloat)(2.0f * (rand() % 256) / 256.0f) - 1.0f;
   }

   // this frame rise a bug
   totalFrames = 8999;
   //totalFrames = 0;
   for (j = 0; j < totalFrames; ++j)
   {
      for (i = 0; i < 8; ++i)
      {
         if (controlPoints[i].x < -BOUNCE_DIMENSION)
         {
            controlPoints[i].dx = -controlPoints[i].dx;
         }
         else
            if (controlPoints[i].x > BOUNCE_DIMENSION)
            {
               controlPoints[i].dx = -controlPoints[i].dx;
            }
         if (controlPoints[i].y < -BOUNCE_DIMENSION)
         {
            controlPoints[i].dy = -controlPoints[i].dy;
         }
         else
            if (controlPoints[i].y > BOUNCE_DIMENSION)
            {
               controlPoints[i].dy = -controlPoints[i].dy;
            }
         controlPoints[i].x += controlPoints[i].dx;
         controlPoints[i].y += controlPoints[i].dy;
      }
   }
}

void initApp(void)
{
   windowScaleX = (VGfloat)windowWidth / 512.0f;
   windowScaleY = (VGfloat)windowHeight / 512.0f;
   VGfloat clearColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
   drawnFrames = 0;
   totalFrames = 0;
   genPaths();
   genPaints();
   vgSeti(VG_FILL_RULE, VG_EVEN_ODD);
   vgSeti(VG_BLEND_MODE, VG_BLEND_SRC_OVER);
   vgSetfv(VG_CLEAR_COLOR, 4, clearColor);
}

void display(void)
{
   VGint i, j, k;
   VGfloat c, s;

#define ROTATE_POINT(_x, _y) \
   pathData[j++] = c * (_x) + s * (_y);  \
   pathData[j++] = -s * (_x) + c * (_y);

   vgSeti(VG_RENDERING_QUALITY, quality);
   vgClear(0, 0, windowWidth, windowHeight);

   vgSeti(VG_MATRIX_MODE, VG_MATRIX_PATH_USER_TO_SURFACE);
   vgLoadIdentity();
   vgTranslate((VGfloat)(windowWidth / 2), (VGfloat)(windowHeight / 2));
   vgScale((VGfloat)windowWidth / 512.0f, (VGfloat)windowHeight / 512.0f);

   for (i = 0; i < 8; ++i) {
      if (controlPoints[i].x < -BOUNCE_DIMENSION) {
         controlPoints[i].dx = -controlPoints[i].dx;
      } else if (controlPoints[i].x > BOUNCE_DIMENSION) {
         controlPoints[i].dx = -controlPoints[i].dx;
      } if (controlPoints[i].y < -BOUNCE_DIMENSION) {
         controlPoints[i].dy = -controlPoints[i].dy;
      } else if (controlPoints[i].y > BOUNCE_DIMENSION) {
         controlPoints[i].dy = -controlPoints[i].dy;
      }
      controlPoints[i].x += controlPoints[i].dx;
      controlPoints[i].y += controlPoints[i].dy;
   }

   k = 0;
   j = 0;
   // path
   pathSegs[k++] = VG_MOVE_TO_ABS;
   pathSegs[k++] = VG_CUBIC_TO_ABS;
   pathSegs[k++] = VG_SCUBIC_TO_ABS;
   pathSegs[k++] = VG_SCUBIC_TO_ABS;
   pathSegs[k++] = VG_SQUAD_TO_ABS;
   for (i = 0; i < 8; ++i) {
      pathData[j++] = controlPoints[i].x;
      pathData[j++] = controlPoints[i].y;
   }
   pathData[j++] = controlPoints[0].x;
   pathData[j++] = controlPoints[0].y;

   // subpath
   c = (VGfloat)cos(0.01 * totalFrames / 180.0f * 3.14159265);
   s = (VGfloat)sin(0.01 * totalFrames / 180.0f * 3.14159265);

   pathSegs[k++] = VG_MOVE_TO_ABS;
   ROTATE_POINT(-200.0f, -200.0f)
      pathSegs[k++] = VG_LINE_TO_ABS;
   ROTATE_POINT(-200.0f, 200.0f)
      pathSegs[k++] = VG_LINE_TO_ABS;
   ROTATE_POINT(200.0f, 200.0f)
      pathSegs[k++] = VG_LINE_TO_ABS;
   ROTATE_POINT(200.0f, -200.0f)
      pathSegs[k++] = VG_LINE_TO_ABS;
   ROTATE_POINT(-67.0f, -200.0f)
      pathSegs[k++] = VG_LINE_TO_ABS;
   ROTATE_POINT(-67.0f, 67.0f)
      pathSegs[k++] = VG_LINE_TO_ABS;
   ROTATE_POINT(67.0f, 67.0f)
      pathSegs[k++] = VG_LINE_TO_ABS;
   ROTATE_POINT(67.0f, 0.0f)
      pathSegs[k++] = VG_LINE_TO_ABS;
   ROTATE_POINT(0.0f, 0.0f)
      pathSegs[k++] = VG_LINE_TO_ABS;
   ROTATE_POINT(0.0f, -133.0f)
      pathSegs[k++] = VG_LINE_TO_ABS;
   ROTATE_POINT(133.0f, -133.0f)
      pathSegs[k++] = VG_LINE_TO_ABS;
   ROTATE_POINT(133.0f, 133.0f)
      pathSegs[k++] = VG_LINE_TO_ABS;
   ROTATE_POINT(-133.0f, 133.0f)
      pathSegs[k++] = VG_LINE_TO_ABS;
   ROTATE_POINT(-133.0f, -200.0f)
      pathSegs[k++] = VG_CLOSE_PATH;

   pathSegs[k++] = VG_MOVE_TO_ABS;
   pathData[j++] = -200.0f;
   pathData[j++] = -200.0f;
   pathSegs[k++] = VG_HLINE_TO_ABS;
   pathData[j++] = 200.0f;
   pathSegs[k++] = VG_VLINE_TO_ABS;
   pathData[j++] = 200.0f;
   pathSegs[k++] = VG_HLINE_TO_ABS;
   pathData[j++] = -200.0f;
   pathSegs[k++] = VG_CLOSE_PATH;

   pathSegs[k++] = VG_MOVE_TO_ABS;
   ROTATE_POINT(20.0f, -67.0f)
      pathSegs[k++] = VG_LCWARC_TO_ABS;
   pathData[j++] = 46.0f;
   pathData[j++] = 46.0f;
   pathData[j++] = (VGfloat)totalFrames;
   ROTATE_POINT(67.0f, -113.0f)
      pathSegs[k++] = VG_LINE_TO_ABS;
   ROTATE_POINT(67.0f, -67.0f)
      pathSegs[k++] = VG_CLOSE_PATH;

   vgClearPath(path, VG_PATH_CAPABILITY_ALL);
   vgAppendPathData(path, k, pathSegs, pathData);

   vgSeti(VG_MATRIX_MODE, VG_MATRIX_FILL_PAINT_TO_USER);
   vgLoadIdentity();
   vgScale(0.18f, 1.0f);

   vgSetPaint(paint, VG_FILL_PATH);
   vgDrawPath(path, VG_FILL_PATH);

#undef ROTATE_POINT
}

const char commands[] =
   "Click & drag mouse to change\n"
   "value for current mode\n\n"
   "H - this help\n"
   "Q - change display rendere quality\n";


void key(unsigned char code, int x, int y)
{
   switch(code) {
   case 'Q':
   case 'q':
      if (quality == VG_RENDERING_QUALITY_FASTER)
         quality = VG_RENDERING_QUALITY_BETTER;
      else
         quality = VG_RENDERING_QUALITY_FASTER;
      break;
   }
   glutPostRedisplay();
  
}

int main(int argc, char *argv[])
{

   testInit(argc, argv, windowWidth, windowHeight , "ShivaVG: stress test example");
   initApp();

   testCallback(TEST_CALLBACK_DISPLAY, (CallbackFunc) display);
   testCallback(TEST_CALLBACK_KEY, (CallbackFunc) key);
   testOverlayString("Press H for a list of commands");
   testOverlayColor(0.0, 0.0, 0.0, 1.0);
   testRun();

   return EXIT_SUCCESS;
}
