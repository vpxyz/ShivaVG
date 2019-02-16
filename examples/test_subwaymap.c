#include "test.h"
#include <VG/openvg.h>
#include <VG/vgu.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <stdbool.h>
#include <ctype.h>


#define NUM_PATHS (sizeof(objl_paths) / sizeof(PathData))

typedef struct {
   const VGfloat *floats;
   const VGubyte *cmds;
   VGint nCommands;
   VGint nFloats;
   VGuint color;
   VGfloat transform[9];
} PathData;

#include "./milano5_data.c"

#define MAP_WIDTH 645.0f
#define MAP_HEIGHT 420.0f

VGint windowWidth = 645;
VGint windowHeight = 420;
VGPath paths[NUM_PATHS];
VGPaint solidCol;
VGfloat logicCenterX, logicCenterY;
VGfloat logicHalfWidth, logicHalfHeight;


void genPaints(void)
{
   solidCol = vgCreatePaint();
   vgSetParameteri(solidCol, VG_PAINT_TYPE, VG_PAINT_TYPE_COLOR);
}

void genPaths(void)
{
   for (VGint i = 0; i < NUM_PATHS; i++) {
      paths[i] = vgCreatePath(VG_PATH_FORMAT_STANDARD, VG_PATH_DATATYPE_F, 1.0f, 0.0f, 0, 0, VG_PATH_CAPABILITY_ALL);
      vgAppendPathData(paths[i], objl_paths[i].nCommands, (VGubyte *) objl_paths[i].cmds, objl_paths[i].floats);
   }
}


void display(void)
{
   VGfloat x0 = logicCenterX - logicHalfWidth;
   VGfloat y0 = logicCenterY - logicHalfHeight;
   VGfloat x1 = logicCenterX + logicHalfWidth;
   VGfloat y1 = logicCenterY + logicHalfHeight;

   VGfloat clearColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
   vgSetfv(VG_CLEAR_COLOR, 4, clearColor);

   vgClear(0, 0, windowWidth, windowHeight);
   vgSetPaint(solidCol, VG_FILL_PATH | VG_STROKE_PATH);
   vgSeti(VG_MATRIX_MODE, VG_MATRIX_PATH_USER_TO_SURFACE);
   vgLoadIdentity();
   vgTranslate(0.0f, (VGfloat)windowHeight);
   vgScale((VGfloat)windowWidth / (x1 - x0), - (VGfloat) windowHeight / (y1 - y0));
   vgTranslate(-x0, -y0);

   for (VGint i = 0; i < NUM_PATHS; ++i) {
      vgSetColor(solidCol, objl_paths[i].color | 255);
      vgDrawPath(paths[i], VG_FILL_PATH);
   }
}

void
reshape(int x, int y)
{
   windowWidth = x;
   windowHeight = y;
   vgClear(0, 0, x, y);
}

void initApp(void)
{
   logicCenterX = MAP_WIDTH / 2.0f;
   logicCenterY = MAP_HEIGHT / 2.0f;
   logicHalfWidth = 75.0f;
   logicHalfHeight = logicHalfWidth * MAP_HEIGHT / MAP_WIDTH;
   genPaths();
   genPaints();
   vgSeti(VG_FILL_RULE, VG_EVEN_ODD);
   vgSeti(VG_BLEND_MODE, VG_BLEND_SRC);
   vgSeti(VG_SCISSORING, VG_FALSE);
   vgSeti(VG_MASKING, VG_FALSE);
   vgSeti(VG_IMAGE_MODE, VG_DRAW_IMAGE_NORMAL);
}
const char commands[] =
   "Commands\n"
   "h - this help\n"
   "a - zoom in\n"
   "z - zoom out\n"
   "q - quit \n" ;

const char help[] = "Press H for a list of commands";
void key(unsigned char code, int x, int y)
{
   static int open = 0;
   switch (tolower(code)) {
   case 'a':
      logicHalfWidth /= 1.04f;
      if (logicHalfWidth < 25.0f) {
         logicHalfWidth = 25.0f;
      }
      logicHalfHeight = logicHalfWidth * MAP_HEIGHT / MAP_WIDTH;
      break;
   case 'z':
      logicHalfWidth *= 1.04f;
      if (logicHalfWidth > MAP_WIDTH * 0.5f) {
         logicHalfWidth = MAP_WIDTH * 0.5f;
      }
      logicHalfHeight = logicHalfWidth * MAP_HEIGHT / MAP_WIDTH;
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
   glutPostRedisplay();
}

void specialKey(int key, int x, int y )
{
   switch(key) {
   case GLUT_KEY_RIGHT:
      logicCenterX += logicHalfWidth * 0.05f;
      if (logicCenterX + logicHalfWidth > MAP_WIDTH)
         logicCenterX = MAP_WIDTH - logicHalfWidth;
      break;

   case GLUT_KEY_LEFT:
      logicCenterX -= logicHalfWidth * 0.05f;
      if (logicCenterX - logicHalfWidth < 0.0f)
         logicCenterX = logicHalfWidth;
      break;

   case GLUT_KEY_UP:
      logicCenterY -= logicHalfHeight * 0.05f;
      if (logicCenterY - logicHalfHeight < 0.0f)
         logicCenterY = logicHalfHeight;
      break;

   case GLUT_KEY_DOWN:
      logicCenterY += logicHalfHeight * 0.05f;
      if (logicCenterY + logicHalfHeight > MAP_HEIGHT)
         logicCenterY = MAP_HEIGHT - logicHalfHeight;
      break;
   }
   glutPostRedisplay();
}

void
drag(int x, int y)
{
   // detect drag direction
   if (logicCenterX - x < 0) {
      // move to right
      logicCenterX += logicHalfWidth * 0.05f;
      if (logicCenterX + logicHalfWidth > MAP_WIDTH)
         logicCenterX = MAP_WIDTH - logicHalfWidth;
   } else if (logicCenterX - x > 0) {
      // move to left
      logicCenterX -= logicHalfWidth * 0.05f;
      if (logicCenterX - logicHalfWidth < 0.0f)
         logicCenterX = logicHalfWidth;
   }

   if (logicCenterY - y > 0) {
      // move to up
      logicCenterY -= logicHalfHeight * 0.05f;
      if (logicCenterY - logicHalfHeight < 0.0f)
         logicCenterY = logicHalfHeight;
   } else if (logicCenterY - y < 0) {
      // move to down
      logicCenterY += logicHalfHeight * 0.05f;
      if (logicCenterY + logicHalfHeight > MAP_HEIGHT)
         logicCenterY = MAP_HEIGHT - logicHalfHeight;
   }
}


int main(int argc, char *argv[])
{
   testInit(argc, argv, windowWidth, windowHeight , "ShivaVG: subwaymap example");
   initApp();
   testCallback(TEST_CALLBACK_DISPLAY, (CallbackFunc) display);
   testCallback(TEST_CALLBACK_KEY, (CallbackFunc) key);
   testCallback(TEST_CALLBACK_SPECIALKEY, (CallbackFunc) specialKey);
   testCallback(TEST_CALLBACK_DRAG, (CallbackFunc) drag);
   testCallback(TEST_CALLBACK_RESHAPE, (CallbackFunc) reshape);
   testOverlayString("Press H for a list of commands");
   testOverlayColor(0.0, 0.0, 0.0, 1.0);
   testRun();

   return EXIT_SUCCESS;
}
