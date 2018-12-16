#include "test.h"
#include <VG/openvg.h>
#include <VG/vgu.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

bool openVGInitialized = false;
bool imageGrabbed = false;
VGint windowWidth = 512;
VGint windowHeight = 512;
VGRenderingQuality quality = VG_RENDERING_QUALITY_BETTER;

VGPath flower, circle, square;
VGPaint paintSrc, paintDst, solidColor;
VGImage srcImage, dstImage;
VGfloat srcCenter[2];
VGfloat dstCenter[2];
VGBlendMode blendMode;
VGint pickedPoint, lastPickedPoint;


void genPaints(void)
{
   VGfloat gradient[4], colStops[10], col[4];

   // create a solid color paint
   solidColor = vgCreatePaint();
   col[0] = 1.0f;
   col[1] = 1.0f;
   col[2] = 1.0f;
   col[3] = 1.0f;
   vgSetParameterfv(solidColor, VG_PAINT_COLOR, 4, col);

   // create and set source paint
   paintSrc = vgCreatePaint();
   colStops[0] = 0.00f; colStops[1] = 0.6f; colStops[2] = 0.05f; colStops[3] = 0.1f; colStops[4] = 1.0f;
   colStops[5] = 1.00f; colStops[6] = 0.3f; colStops[7] = 0.9f; colStops[8] = 0.1f; colStops[9] = 0.3f;
   gradient[0] = -160.0f; gradient[1] = 0.0f;
   gradient[2] = 160.0f; gradient[3] = 0.0f;
   vgSetParameteri(paintSrc, VG_PAINT_TYPE, VG_PAINT_TYPE_LINEAR_GRADIENT);
   vgSetParameterfv(paintSrc, VG_PAINT_COLOR_RAMP_STOPS, 10, colStops);
   vgSetParameterfv(paintSrc, VG_PAINT_LINEAR_GRADIENT, 4, gradient);
   vgSetParameteri(paintSrc, VG_PAINT_COLOR_RAMP_SPREAD_MODE, VG_COLOR_RAMP_SPREAD_PAD);

   // create and set dest paint
   paintDst = vgCreatePaint();
   colStops[0] = 0.00f; colStops[1] = 0.9f; colStops[2] = 0.8f; colStops[3] = 0.0f; colStops[4] = 0.9f;
   colStops[5] = 1.00f; colStops[6] = 0.0f; colStops[7] = 0.2f; colStops[8] = 0.8f; colStops[9] = 0.4f;
   gradient[0] = -160.0f; gradient[1] = 0.0f;
   gradient[2] = 160.0f; gradient[3] = 0.0f;
   vgSetParameteri(paintDst, VG_PAINT_TYPE, VG_PAINT_TYPE_LINEAR_GRADIENT);
   vgSetParameterfv(paintDst, VG_PAINT_COLOR_RAMP_STOPS, 10, colStops);
   vgSetParameterfv(paintDst, VG_PAINT_LINEAR_GRADIENT, 4, gradient);
   vgSetParameteri(paintDst, VG_PAINT_COLOR_RAMP_SPREAD_MODE, VG_COLOR_RAMP_SPREAD_PAD);
}

void genImages(void)
{
   srcImage = vgCreateImage(VG_sRGBA_8888, 384, 384, VG_IMAGE_QUALITY_FASTER);
   dstImage = vgCreateImage(VG_sRGBA_8888, 384, 384, VG_IMAGE_QUALITY_FASTER);
}

void grabImages(void)
{
   VGfloat clearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

   vgSeti(VG_BLEND_MODE, VG_BLEND_SRC);
   vgSetfv(VG_CLEAR_COLOR, 4, clearColor);

   // draw and grab source image
   vgClear(0, 0, windowWidth, windowHeight);
   vgSetPaint(paintSrc, VG_FILL_PATH);
   vgSeti(VG_MATRIX_MODE, VG_MATRIX_PATH_USER_TO_SURFACE);
   vgLoadIdentity();
   vgTranslate(192.0f, 192.0f);
   vgRotate(180.0f);
   vgScale(1.2f, 1.2f);
   vgDrawPath(flower, VG_FILL_PATH);
   vgGetPixels(srcImage, 0, 0, 0, 0, 384, 384);

   // draw and grab dest image
   vgClear(0, 0, windowWidth, windowHeight);
   vgSetPaint(paintDst, VG_FILL_PATH);
   vgLoadIdentity();
   vgTranslate(192.0f, 192.0f);
   vgRotate(45.0f);
   vgScale(1.2f, 1.2f);
   vgDrawPath(flower, VG_FILL_PATH);
   vgGetPixels(dstImage, 0, 0, 0, 0, 384, 384);
}

void genPaths(void)
{
   VGubyte pathSegs[6];
   VGfloat pathData[26];

   flower = vgCreatePath(VG_PATH_FORMAT_STANDARD, VG_PATH_DATATYPE_F, 1.0f, 0.0f, 0, 0, VG_PATH_CAPABILITY_ALL);
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
   vgAppendPathData(flower, 6, pathSegs, pathData);

   circle = vgCreatePath(VG_PATH_FORMAT_STANDARD, VG_PATH_DATATYPE_F, 1.0f, 0.0f, 0, 0, VG_PATH_CAPABILITY_ALL);
   vguEllipse(circle, 0.0f, 0.0f, 8.0f, 8.0f);
   square = vgCreatePath(VG_PATH_FORMAT_STANDARD, VG_PATH_DATATYPE_F, 1.0f, 0.0f, 0, 0, VG_PATH_CAPABILITY_ALL);
   vguRect(square, 0, 0, 346.0f, 346.0f);
}

void display(void)
{
   VGfloat clearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
   VGfloat dashPattern[4] = { 5.0f, 5.0f };
   VGfloat col[4];


   if (imageGrabbed == false) {
      imageGrabbed = true;
      grabImages();
   } else {
      vgSetfv(VG_STROKE_DASH_PATTERN, 0, NULL);
      vgSetfv(VG_CLEAR_COLOR, 4, clearColor);
      vgSeti(VG_RENDERING_QUALITY, quality);
      vgClear(0, 0, windowWidth, windowHeight);

      // draw destination image
      vgSeti(VG_BLEND_MODE, VG_BLEND_SRC);
      vgSeti(VG_MATRIX_MODE, VG_MATRIX_IMAGE_USER_TO_SURFACE);
      vgLoadIdentity();
      vgTranslate(dstCenter[0] - 173.0f, dstCenter[1] - 173.0f);
      vgScale(0.90f, 0.90f);
      vgDrawImage(dstImage);

      // draw source image according to the current blend mode
      vgSeti(VG_BLEND_MODE, blendMode);
      vgSeti(VG_MATRIX_MODE, VG_MATRIX_IMAGE_USER_TO_SURFACE);
      vgLoadIdentity();
      vgTranslate(srcCenter[0] - 173.0f, srcCenter[1] - 173.0f);
      vgScale(0.90f, 0.90f);
      vgDrawImage(srcImage);

      // draw images centers
      col[0] = 1.0f; col[1] = 1.0f; col[2] = 1.0f; col[3] = 1.0f;
      vgSetParameterfv(solidColor, VG_PAINT_COLOR, 4, col);
      vgSetPaint(solidColor, VG_STROKE_PATH);
      vgSeti(VG_BLEND_MODE, VG_BLEND_SRC_OVER);
      vgSeti(VG_MATRIX_MODE, VG_MATRIX_PATH_USER_TO_SURFACE);
      vgLoadIdentity();
      vgTranslate(srcCenter[0], srcCenter[1]);
      vgDrawPath(circle, VG_STROKE_PATH);
      vgLoadIdentity();
      vgTranslate(dstCenter[0], dstCenter[1]);
      vgDrawPath(circle, VG_STROKE_PATH);

      // draw images bounds, with color in accord with the current selected image
      if (lastPickedPoint == 1) {
         col[0] = 1.0f; col[1] = 0.0f; col[2] = 0.0f; col[3] = 1.0f;
         vgSetParameterfv(solidColor, VG_PAINT_COLOR, 4, col);
      } else {
         col[0] = 1.0f; col[1] = 1.0f; col[2] = 1.0f; col[3] = 1.0f;
         vgSetParameterfv(solidColor, VG_PAINT_COLOR, 4, col);
      }
      vgSetfv(VG_STROKE_DASH_PATTERN, 2, dashPattern);
      vgSetf(VG_STROKE_DASH_PHASE, 0.0f);
      vgLoadIdentity();
      vgTranslate(srcCenter[0] - 173.0f, srcCenter[1] - 173.0f);
      vgDrawPath(square, VG_STROKE_PATH);
      if (lastPickedPoint == 2) {
         col[0] = 1.0f; col[1] = 0.0f; col[2] = 0.0f; col[3] = 1.0f;
         vgSetParameterfv(solidColor, VG_PAINT_COLOR, 4, col);
      } else {
         col[0] = 1.0f; col[1] = 1.0f; col[2] = 1.0f; col[3] = 1.0f;
         vgSetParameterfv(solidColor, VG_PAINT_COLOR, 4, col);
      }
      vgLoadIdentity();
      vgTranslate(dstCenter[0] - 173.0f, dstCenter[1] - 173.0f);
      vgDrawPath(square, VG_STROKE_PATH);
   }
}

void
reshape(int x, int y)
{
   windowWidth = x;
   windowHeight = y;
   srcCenter[0] = (VGfloat) x / 2.0f;
   srcCenter[1] = (VGfloat) y / 2.0f;
   dstCenter[0] = (VGfloat) x / 2.0f  + 100.0f;
   dstCenter[1] = (VGfloat) y / 2.0f + 50.0f;
   vgClear(0, 0, x, y);
}

const char commands[] =
   "Click & drag mouse to change\n"
   "value for current mode\n\n"
   "H - this help\n"
   "A - change blend mode\n"
   "Q - change display render quality\n";

const char help[] = "Press H for a list of commands";

void key(unsigned char code, int x, int y)
{
   static int open = 0;
   switch (tolower(code)) {
   case 'q':
      if (quality == VG_RENDERING_QUALITY_FASTER)
         quality = VG_RENDERING_QUALITY_BETTER;
      else
         quality = VG_RENDERING_QUALITY_FASTER;
      break;

   case 'a':
   {
      VGuint i = (VGuint) blendMode;
      i++;
      blendMode = (VGBlendMode) i;
      if (blendMode > VG_BLEND_ADDITIVE)
         blendMode = VG_BLEND_SRC;
   }
   break;

   case 'h':
      if (!open) {
         /* Show help */
         testOverlayString(commands);
         open = 1;
      } else {
         testOverlayString(help);
         open = 0;
      }
      return;
   }

}


void move(int button, int state, int x, int y)
{
   if (button != GLUT_LEFT_BUTTON)
      return;

   if (pickedPoint == 1) {
      srcCenter[0] = x;
      srcCenter[1] = y;
   } else if (pickedPoint == 2) {
      dstCenter[0] = x;
      dstCenter[1] = y;
   }

}

void click(int button, int state, int x, int y)
{
   if (state != GLUT_DOWN) {
      pickedPoint = 0;
      return;
   }

   VGfloat dist0 = ((x - srcCenter[0]) * (x - srcCenter[0])) +
      ((y - srcCenter[1]) * (y - srcCenter[1]));
   VGfloat dist1 = ((x - dstCenter[0]) * (x - dstCenter[0])) +
      ((y - dstCenter[1]) * (y - dstCenter[1]));

   if (dist0 < dist1) {
      if (dist0 < 36.0f) {
         pickedPoint = 1;
         lastPickedPoint = 1;
      } else {
         pickedPoint = 0;
         lastPickedPoint = 0;
      }
   } else if (dist1 < 36.0f) {
      pickedPoint = 2;
      lastPickedPoint = 2;
   } else {
      pickedPoint = 0;
      lastPickedPoint = 0;
   }
}

void initApp(void)
{
   srcCenter[0] = 200.0f;
   srcCenter[1] = 200.0f;
   dstCenter[0] = 312.0f;
   dstCenter[1] = 292.0f;
   /*
    * blendMode = VG_BLEND_SRC_OVER;
    */
   blendMode = VG_BLEND_SRC;
   pickedPoint = 0;
   lastPickedPoint = 0;

   genPaths();
   genPaints();
   vgSeti(VG_FILL_RULE, VG_EVEN_ODD);
   vgSeti(VG_SCISSORING, VG_FALSE);
   vgSeti(VG_MASKING, VG_FALSE);
   genImages();
   vgSeti(VG_IMAGE_MODE, VG_DRAW_IMAGE_NORMAL);

}

int main(int argc, char *argv[])
{

   testInit(argc, argv, windowWidth, windowHeight , "ShivaVG: compositing example");
   initApp();

   testCallback(TEST_CALLBACK_DISPLAY, (CallbackFunc) display);
   testCallback(TEST_CALLBACK_KEY, (CallbackFunc) key);
   testCallback(TEST_CALLBACK_BUTTON, (CallbackFunc) click);
   testCallback(TEST_CALLBACK_BUTTON, (CallbackFunc) move);
   testCallback(TEST_CALLBACK_RESHAPE, (CallbackFunc) reshape);
   
   testOverlayString(help);
   testOverlayColor(1, 1, 1, 1);

   testRun();

   return EXIT_SUCCESS;
}
