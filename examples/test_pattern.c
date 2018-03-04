#include "test.h"
#include <ctype.h>
#include <math.h>
#include <jpeglib.h>

#ifndef IMAGE_DIR
#define IMAGE_DIR "./"
#endif

VGfloat tx = 0.0f, ty = 0.0f;
VGfloat sx = 1.0f, sy = 1.0f;
VGfloat a = 45.0f;

VGint tindex = 3;
VGint tsize = 4;
VGint tile[] = {
   VG_TILE_FILL,
   VG_TILE_PAD,
   VG_TILE_REPEAT,
   VG_TILE_REFLECT
};

VGfloat sqx = 200;
VGfloat sqy = 200;

VGfloat clickX;
VGfloat clickY;
VGfloat startX;
VGfloat startY;
char mode = 't';

VGImage backImage;
VGImage patternImage;
VGPaint patternFill;
VGPaint blackFill;
VGPath org;
VGPath p;

VGfloat black[] = { 1, 1, 1, 1 };

const char commands[] =
   "Click & drag mouse to change\n"
   "value for current mode\n\n"
   "H - this help\n"
   "TAB - pattern tiling mode\n"
   "T - translate mode\n"
   "X - scale X mode\n" "Y - scale Y mode\n" "BACKSPACE - reset value\n";

#if defined(__APPLE__)
#define BACKSPACE 127
#else
#define BACKSPACE 8
#endif

#define TAB 9

void
display(float interval)
{
   VGfloat cc[] = { 0, 0, 0, 1 };

   vgSetfv(VG_CLEAR_COLOR, 4, cc);
   vgClear(0, 0, testWidth(), testHeight());

   vgSeti(VG_MATRIX_MODE, VG_MATRIX_FILL_PAINT_TO_USER);
   vgLoadIdentity();
   vgTranslate(tx, ty);
   vgScale(sx, sy);
   vgRotate(a);

   vgSeti(VG_MATRIX_MODE, VG_MATRIX_PATH_USER_TO_SURFACE);
   vgLoadIdentity();

   vgSeti(VG_MATRIX_MODE, VG_MATRIX_IMAGE_USER_TO_SURFACE);
   vgSeti(VG_IMAGE_MODE, VG_DRAW_IMAGE_MULTIPLY);
   vgLoadIdentity();

   vgSetPaint(patternFill, VG_FILL_PATH);
   /*vgDrawPath(p, VG_FILL_PATH); */
   vgDrawImage(backImage);

   vgSeti(VG_MATRIX_MODE, VG_MATRIX_PATH_USER_TO_SURFACE);
   vgLoadIdentity();
   vgTranslate(tx, ty);
   vgScale(sx, sy);
   vgRotate(a);

   vgSetPaint(blackFill, VG_FILL_PATH | VG_STROKE_PATH);
   vgDrawPath(org, VG_FILL_PATH);
}

void
createPattern()
{
   VGfloat tileFill[] = { 0, 0, 1, 1 };
   vgSetParameteri(patternFill, VG_PAINT_TYPE, VG_PAINT_TYPE_PATTERN);
   vgSetParameteri(patternFill, VG_PAINT_PATTERN_TILING_MODE, tile[tindex]);
   vgSetfv(VG_TILE_FILL_COLOR, 4, tileFill);
   vgPaintPattern(patternFill, patternImage);
}

void
updateOverlayString()
{
   switch (mode) {
   case 't':
      testOverlayString("Translate mode: %f,%f", tx, ty);
      break;
   case 'x':
      testOverlayString("Scale X mode: %f", sx);
      break;
   case 'y':
      testOverlayString("Scale Y mode: %f", sy);
      break;
   }
}

void
drag(int x, int y)
{
   VGfloat dx, dy;
   y = testHeight() - y;
   dx = x - clickX;
   dy = y - clickY;

   switch (mode) {
   case 't':{
         tx = startX + dx;
         ty = startY + dy;
         break;
      }
   case 'x':
      sx = startY + dy * 0.01;
      break;
   case 'y':
      sy = startY + dy * 0.01;
      break;
   }

   updateOverlayString();
   glutPostRedisplay();
}

void
click(int button, int state, int x, int y)
{
   y = testHeight() - y;
   clickX = x;
   clickY = y;

   switch (mode) {
   case 't':
      startX = tx;
      startY = ty;
      break;
   case 'x':
      startY = sx;
      break;
   case 'y':
      startY = sy;
      break;
   }

   glutPostRedisplay();
}

void
key(unsigned char key, int x, int y)
{
   glutPostRedisplay();

   if (key == BACKSPACE) {
      /* Reset value */
      switch (mode) {
      case 't':
         tx = 0;
         ty = 0;
         break;
      case 'x':
         sx = 1.0f;
         break;
      case 'y':
         sy = 1.0f;
         break;
      }
      updateOverlayString();
      return;
   }


   switch (tolower(key)) {
   case 't':
   case 'x':
   case 'y':
      break;

   case TAB:

      /* Cycle spread mode */
      tindex = (tindex + 1) % tsize;
      createPattern();
      switch (tindex) {
      case 0:
         testOverlayString("Pattern Tiling mode: FILL\n");
         break;
      case 1:
         testOverlayString("Pattern Tiling mode: PAD\n");
         break;
      case 2:
         testOverlayString("Pattern Tiling mode: REPEAT\n");
         break;
      case 3:
         testOverlayString("Pattern Tiling mode: REFLECT\n");
         break;
      }
      return;

   case 'h':
      /* Show help */
      testOverlayString(commands);
      return;

   default:
      return;
   }

   /* Switch mode */
   mode = tolower(key);
   updateOverlayString();
}

void
createSquare(VGPath p)
{
   testMoveTo(p, (testWidth() - sqx) / 2, (testHeight() - sqy) / 2,
              VG_ABSOLUTE);
   testLineTo(p, sqx, 0, VG_RELATIVE);
   testLineTo(p, 0, sqy, VG_RELATIVE);
   testLineTo(p, -sqx, 0, VG_RELATIVE);
   testClosePath(p);
}

void
createOrigin(VGPath p)
{
   vguEllipse(p, 0, 0, 10, 10);
}

void
reshape(int x, int y)
{
   vgClearPath(p, VG_PATH_CAPABILITY_ALL);
   createSquare(p);
}

int
main(int argc, char **argv)
{
   testInit(argc, argv, 400, 400, "ShivaVG: Linear Gradient Test");
   testCallback(TEST_CALLBACK_DISPLAY, (CallbackFunc) display);
   testCallback(TEST_CALLBACK_BUTTON, (CallbackFunc) click);
   testCallback(TEST_CALLBACK_DRAG, (CallbackFunc) drag);
   testCallback(TEST_CALLBACK_KEY, (CallbackFunc) key);
   testCallback(TEST_CALLBACK_RESHAPE, (CallbackFunc) reshape);

   p = testCreatePath();
   org = testCreatePath();

   blackFill = vgCreatePaint();
   vgSetParameterfv(blackFill, VG_PAINT_COLOR, 4, black);

   backImage = (testCreateImageFromJpeg(IMAGE_DIR "test_img_violin.jpg")).img;
   patternImage =
      (testCreateImageFromJpeg(IMAGE_DIR "test_img_shivavg.jpg")).img;
   patternFill = vgCreatePaint();

   createSquare(p);
   createOrigin(org);
   createPattern();

   testOverlayString("Press H for a list of commands");
   testOverlayColor(1, 1, 1, 1);
   testRun();

   return EXIT_SUCCESS;
}
