#include "test.h"
#include <VG/openvg.h>
#include <VG/vgu.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <ctype.h>

typedef struct _particle {
   VGfloat x, y;
   VGfloat dx, dy;
   VGfloat vel;
   VGfloat rotf;
   VGfloat rot;
   VGfloat sclf;
   VGfloat scl;
} particle;

#define MAX_FLOWERS 350
#define MAX_PAINTS 100

bool openVGInitialized = false;
VGint windowWidth = 832;
VGint windowHeight = 832;
VGfloat zoomFactor;
VGint oldMouseX, oldMouseY;
VGRenderingQuality quality = VG_RENDERING_QUALITY_BETTER;
VGint drawnFrames;
VGint flowerIndex = 0;

VGPath path0;
VGPaint flower_paints[MAX_PAINTS];
particle flowers[MAX_FLOWERS];


void genPaints(void)
{
   VGfloat col[4];

   for (VGint i = 0; i < MAX_PAINTS; ++i) {
      flower_paints[i] = vgCreatePaint();
      for (VGint j = 0; j < 4; ++j) {
         col[j] = (VGfloat)(rand() % 255) / 255.0f;
         vgSetParameterfv(flower_paints[i], VG_PAINT_COLOR, 4, col);
      }
   }
}

void genPaths(void)
{
   VGubyte pathSegs[6];
   VGfloat pathData[26];

   // create a path
   path0 = vgCreatePath(VG_PATH_FORMAT_STANDARD, VG_PATH_DATATYPE_F, 1.0f, 0.0f, 0, 0, VG_PATH_CAPABILITY_ALL);
   pathSegs[0] = VG_MOVE_TO_ABS;
   pathData[0] = -20.0f; pathData[1] = 20.0f;
   pathSegs[1] = VG_CUBIC_TO_ABS;
   pathData[2] = -200.0f; pathData[3] = 170.0f; pathData[4] = -200.0f; pathData[5] = -170.0f; pathData[6] = -20.0f; pathData[7] = -20.0f;
   pathSegs[2] = VG_CUBIC_TO_ABS;
   pathData[8] = -170.0f; pathData[9] = -200.0f; pathData[10] = 170.0f; pathData[11] = -200.0f; pathData[12] = 20.0f; pathData[13] = -20.0f;
   pathSegs[3] = VG_CUBIC_TO_ABS;
   pathData[14] = 200.0f; pathData[15] = -170.0f; pathData[16] = 200.0f; pathData[17] = 170.0f; pathData[18] = 20.0f; pathData[19] = 20.0f;
   pathSegs[4] = VG_CUBIC_TO_ABS;
   pathData[20] = 170.0f; pathData[21] = 200.0f; pathData[22] = -170.0f; pathData[23] = 200.0f; pathData[24] = -20.0f; pathData[25] = 20.0f;
   pathSegs[5] = VG_CLOSE_PATH;
   vgAppendPathData(path0, 6, pathSegs, pathData);
}

void initFlowers(void)
{
   for (VGint i = 0; i < MAX_FLOWERS; ++i) {
      flowers[i].dx = 0.0f;
      flowers[i].dy = 0.0f;
      flowers[i].scl = 0.0f;
      flowers[i].sclf = 0.0f;
      flowers[i].rot = 0.0f;
      flowers[i].rotf = 0.0f;
   }
}

void display(void)
{
   VGfloat clearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
   vgSeti(VG_RENDERING_QUALITY, quality);
   vgSetfv(VG_CLEAR_COLOR, 4, clearColor);
   vgClear(0, 0, windowWidth, windowHeight);
   vgSeti(VG_BLEND_MODE, VG_BLEND_ADDITIVE);

   vgSeti(VG_MATRIX_MODE, VG_MATRIX_PATH_USER_TO_SURFACE);
   for (VGint i = 0; i < MAX_FLOWERS; ++i) {
      flowers[i].rot += flowers[i].rotf;
      flowers[i].scl += flowers[i].sclf;
      flowers[i].x += flowers[i].dx;
      flowers[i].y += flowers[i].dy;

      vgLoadIdentity();
      vgTranslate((VGfloat)windowWidth / 2.0f, (VGfloat)windowHeight / 2.0f);
      vgScale(zoomFactor, zoomFactor);
      vgTranslate(-(VGfloat)windowWidth / 2.0f, -(VGfloat)windowHeight / 2.0f);
      vgTranslate(flowers[i].x, flowers[i].y);
      vgRotate(flowers[i].rot);
      vgScale(1.35f * flowers[i].sclf * (VGfloat)cos(0.003f * flowers[i].scl), 1.35f * flowers[i].sclf * (VGfloat)cos(0.003f * flowers[i].scl));
      vgSetPaint(flower_paints[(i % MAX_PAINTS)], VG_FILL_PATH);
      vgDrawPath(path0, VG_FILL_PATH);
   }
}


const char commands[] =
   "Click & drag mouse to change\n"
   "value for current mode\n\n"
   "h - this help\n"
   "q - change display rendere quality\n";


void key(unsigned char code, int x, int y)
{
   switch(tolower(code)) {
   case 'q':
      if (quality == VG_RENDERING_QUALITY_FASTER)
         quality = VG_RENDERING_QUALITY_BETTER;
      else
         quality = VG_RENDERING_QUALITY_FASTER;
      break;
   case 'h':
      testOverlayString(commands);
      break;
   }

}

void click(int button, int state, int x, int y)
{
   if (button == GLUT_LEFT_BUTTON) {
      flowerIndex = (flowerIndex % MAX_FLOWERS);
      flowers[flowerIndex].dx = (VGfloat)(0.5f * (rand() % 255) / 255.0f) - 0.5f;
      flowers[flowerIndex].dy = (VGfloat)(0.5f * (rand() % 255) / 255.0f) - 0.5f;
      flowers[flowerIndex].x = ((VGfloat) x);
      flowers[flowerIndex].y = ((VGfloat) windowHeight - y);
      flowers[flowerIndex].x = (((flowers[flowerIndex].x - (VGfloat)windowWidth * 0.5f)) / zoomFactor) + ((VGfloat)windowWidth * 0.5f);
      flowers[flowerIndex].y = (((flowers[flowerIndex].y - (VGfloat)windowHeight * 0.5f)) / zoomFactor) + ((VGfloat)windowHeight * 0.5f);
      flowers[flowerIndex].scl = (VGfloat)0.05f * ((rand() % 255) / 255.0f);
      flowers[flowerIndex].sclf = (VGfloat)0.5f * (((rand() % 255)/ 255.0f) - 0.5f);
      flowers[flowerIndex].rot = (VGfloat)(rand() % 360);
      flowers[flowerIndex].rotf = (VGfloat)((rand() % 255) / 255.0f) - 0.5f;
      flowerIndex++;
   } else if (button == GLUT_RIGHT_BUTTON) {
      VGint xPos = x;
      VGint yPos = y;
      VGint deltaX = xPos - oldMouseX;
      VGint deltaY = yPos - oldMouseY;
      VGint delta;

      if (abs(deltaX) > abs(deltaY))
         delta = deltaX;
      else
         delta = -deltaY;
      oldMouseX = xPos;
      oldMouseY = yPos;
      zoomFactor += (VGfloat)(delta * 0.003f * zoomFactor);
      if (zoomFactor < 0.1f)
         zoomFactor = 0.1f;
      if (zoomFactor > 10.000f)
         zoomFactor = 10.000f;
   }
   glutPostRedisplay();
}

void drag(int x, int y)
{
   flowerIndex = (flowerIndex % MAX_FLOWERS);
   flowers[flowerIndex].dx = (VGfloat)(0.5f * (rand() % 255) / 255.0f) - 0.5f;
   flowers[flowerIndex].dy = (VGfloat)(0.5f * (rand() % 255) / 255.0f) - 0.5f;
   flowers[flowerIndex].x = ((VGfloat) x);
   flowers[flowerIndex].y = ((VGfloat) windowHeight - y);
   flowers[flowerIndex].x = (((flowers[flowerIndex].x - (VGfloat)windowWidth * 0.5f)) / zoomFactor) + ((VGfloat)windowWidth * 0.5f);
   flowers[flowerIndex].y = (((flowers[flowerIndex].y - (VGfloat)windowHeight * 0.5f)) / zoomFactor) + ((VGfloat)windowHeight * 0.5f);
   flowers[flowerIndex].scl = (VGfloat)0.05f * ((rand() % 255) / 255.0f);
   flowers[flowerIndex].sclf = (VGfloat)0.5f * (((rand() % 255) / 255.0f) - 0.5f);
   flowers[flowerIndex].rot = (VGfloat)(rand() % 360);
   flowers[flowerIndex].rotf = (VGfloat)((rand() % 255) / 255.0f) - 0.5f;
   flowerIndex++;
   oldMouseX = x;
   oldMouseY = y;
   glutPostRedisplay();
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
   zoomFactor = 1.0f;
   drawnFrames = 0;
   genPaints();
   genPaths();
   initFlowers();
}

int main(int argc, char *argv[])
{
   testInit(argc, argv, windowWidth, windowHeight , "ShivaVG: flowers example");

   testCallback(TEST_CALLBACK_DISPLAY, (CallbackFunc) display);
   testCallback(TEST_CALLBACK_KEY, (CallbackFunc) key);
   testCallback(TEST_CALLBACK_DRAG, (CallbackFunc) drag);
   testCallback(TEST_CALLBACK_BUTTON, (CallbackFunc) click);
   testCallback(TEST_CALLBACK_RESHAPE, (CallbackFunc) reshape);
   testOverlayString("Press H for a list of commands");
   testOverlayColor(1, 1, 1, 1);

   initApp();
   testRun();
   return EXIT_SUCCESS;
}
