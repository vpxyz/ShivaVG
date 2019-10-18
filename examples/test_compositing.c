#include "test.h"
#include <VG/openvg.h>
#include <VG/vgu.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>


#define MOUSE_BUTTON_NONE  0
#define MOUSE_BUTTON_LEFT  1
#define MOUSE_BUTTON_RIGHT 2

#define X_COORD 0
#define Y_COORD 1

#define CONTROL_POINT_NONE      0
#define CONTROL_POINT_SRC_IMAGE 1
#define CONTROL_POINT_DST_IMAGE 2

bool openVGInitialized = false;
bool imageGrabbed = false;
VGint windowWidth = 512;
VGint windowHeight = 512;
VGRenderingQuality quality = VG_RENDERING_QUALITY_FASTER;

VGPath flower, circle, square;
VGPaint paintSrc, paintDst, solidColor;
VGImage srcImage, dstImage;
VGfloat srcCenter[2];
VGfloat dstCenter[2];
VGBlendMode blendMode = VG_BLEND_SRC;
VGint pickedPoint, lastPickedPoint;

VGint imageSize = 384;

// control points
static VGfloat controlPointsRadius = 14.0f;
static VGint pickedControlPoint = CONTROL_POINT_NONE;

// mouse state
static VGint oldMouseX = 0;
static VGint oldMouseY = 0;
static VGint mouseButton = MOUSE_BUTTON_NONE;

void genPaints(void)
{
   // create a solid color paint
   solidColor = vgCreatePaint();

   VGfloat white[] = {1.0f,1.0f,1.0f,1.0f};
   vgSetParameteri(solidColor, VG_PAINT_TYPE, VG_PAINT_TYPE_COLOR);
   vgSetParameterfv(solidColor, VG_PAINT_COLOR, 4, white);

   // color stops used to generate SRC image
   const VGfloat srcStops[] = {
      0.00f, 0.60f, 0.05f, 0.10f, 1.00f,
      1.00f, 0.30f, 0.90f, 0.10f, 0.30f
   };
   const VGfloat srcGrad[] = {
      -160.0f, 0.0f,
      160.0f, 0.0f
   };
   // color stops used to generate DST image
   const VGfloat dstStops[] = {
      0.00f, 0.90f, 0.80f, 0.00f, 0.90f,
      1.00f, 0.00f, 0.20f, 0.80f, 0.40f
   };
   const VGfloat dstGrad[] = {
      -160.0f, 0.0f,
      160.0f, 0.0f
   };

   // create paint that will be used to generate SRC image
   paintSrc = vgCreatePaint();
   vgSetParameteri(paintSrc, VG_PAINT_TYPE, VG_PAINT_TYPE_LINEAR_GRADIENT);
   vgSetParameterfv(paintSrc, VG_PAINT_COLOR_RAMP_STOPS, 10, srcStops);
   vgSetParameterfv(paintSrc, VG_PAINT_LINEAR_GRADIENT, 4, srcGrad);
   vgSetParameteri(paintSrc, VG_PAINT_COLOR_RAMP_SPREAD_MODE, VG_COLOR_RAMP_SPREAD_PAD);

   // create paint that will be used to generate DST image
   paintDst = vgCreatePaint();
   vgSetParameteri(paintDst, VG_PAINT_TYPE, VG_PAINT_TYPE_LINEAR_GRADIENT);
   vgSetParameterfv(paintDst, VG_PAINT_COLOR_RAMP_STOPS, 10, dstStops);
   vgSetParameterfv(paintDst, VG_PAINT_LINEAR_GRADIENT, 4, dstGrad);
   vgSetParameteri(paintDst, VG_PAINT_COLOR_RAMP_SPREAD_MODE, VG_COLOR_RAMP_SPREAD_PAD);
}


void genImages(void)
{
   VGfloat black[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
   // destroy previous images
   vgDestroyImage(srcImage);
   vgDestroyImage(dstImage);

   srcImage = vgCreateImage(VG_sRGBA_8888, imageSize, imageSize, VG_IMAGE_QUALITY_BETTER);
   dstImage = vgCreateImage(VG_sRGBA_8888, imageSize, imageSize, VG_IMAGE_QUALITY_BETTER);
   // clear surface with a transparent black
   vgSetfv(VG_CLEAR_COLOR, 4, black);
   vgSeti(VG_BLEND_MODE, VG_BLEND_SRC);
   vgSeti(VG_RENDERING_QUALITY, VG_RENDERING_QUALITY_BETTER);
   // generate SRC image
   vgClear(0, 0, windowWidth, windowHeight);
   vgSetPaint(paintSrc, VG_FILL_PATH);
   vgSeti(VG_MATRIX_MODE, VG_MATRIX_PATH_USER_TO_SURFACE);
   vgLoadIdentity();
   vgTranslate(173.0f, 173.0f);
   vgRotate(180.0f);
   /*
    * vgScale(scl, scl);
    */
   vgDrawPath(flower, VG_FILL_PATH);
   vgGetPixels(srcImage, 0, 0, 0, 0, imageSize, imageSize);
   // generate DST image
   vgClear(0, 0, windowWidth, windowHeight);
   vgSetPaint(paintDst, VG_FILL_PATH);
   vgLoadIdentity();
   vgTranslate(173.0f, 173.0f);
   vgRotate(45.0f);
   /*
    * vgScale(scl, scl);
    */
   vgDrawPath(flower, VG_FILL_PATH);
   vgGetPixels(dstImage, 0, 0, 0, 0, imageSize, imageSize);

}

void genPaths(void)
{
   /*
    * VGubyte pathSegs[6];
    * VGfloat pathData[26];
    *
    * flower = vgCreatePath(VG_PATH_FORMAT_STANDARD, VG_PATH_DATATYPE_F, 1.0f, 0.0f, 0, 0, VG_PATH_CAPABILITY_ALL);
    * pathSegs[0] = VG_MOVE_TO_ABS;
    * pathData[0] = -20.0f; pathData[1] = 20.0f;
    * pathSegs[1] = VG_CUBIC_TO_ABS;
    * pathData[2] = -200.0f; pathData[3] = 170.0f; pathData[4] = -200.0f; pathData[5] = -170.0f; pathData[6] = -20.0f; pathData[7] = -20.0f;
    * pathSegs[2] = VG_CUBIC_TO_ABS;
    * pathData[8] = -170.0f; pathData[9] = -200.0f; pathData[10] = 170.0f; pathData[11] = -200.0f; pathData[12] = 20.0f; pathData[13] = -20.0f;
    * pathSegs[3] = VG_CUBIC_TO_ABS;
    * pathData[14] = 200.0f; pathData[15] = -170.0f; pathData[16] = 200.0f; pathData[17] = 170.0f; pathData[18] = 20.0f; pathData[19] = 20.0f;
    * pathSegs[4] = VG_CUBIC_TO_ABS;
    * pathData[20] = 170.0f; pathData[21] = 200.0f; pathData[22] = -170.0f; pathData[23] = 200.0f; pathData[24] = -20.0f; pathData[25] = 20.0f;
    * pathSegs[5] = VG_CLOSE_PATH;
    * vgAppendPathData(flower, 6, pathSegs, pathData);
    */

   // flower-like path commands
   VGubyte flowerCmds[] = {
      VG_MOVE_TO,
      VG_CUBIC_TO,
      VG_CUBIC_TO,
      VG_CUBIC_TO,
      VG_CUBIC_TO,
      VG_CLOSE_PATH
   };
   // flower-like path coordinates
   VGfloat flowerCoords[] = {
      // move to
      -20.0f, 20.0f,
      // cubic to
      -200.0f, 170.0f, -200.0f, -170.0f, -20.0f, -20.0f,
      // cubic to
      -170.0f, -200.0f, 170.0f, -200.0f, 20.0f, -20.0f,
      // cubic to
      200.0f, -170.0f, 200.0f, 170.0f, 20.0f, 20.0f,
      // cubic to
      170.0f, 200.0f, -170.0f, 200.0f, -20.0f, 20.0f
   };

   // create the flower-like path: it will be used to generate images
   flower = vgCreatePath(VG_PATH_FORMAT_STANDARD, VG_PATH_DATATYPE_F, 1.0f, 0.0f, 0, 0, VG_PATH_CAPABILITY_ALL);
   vgAppendPathData(flower, 6, flowerCmds, flowerCoords);

   circle = vgCreatePath(VG_PATH_FORMAT_STANDARD, VG_PATH_DATATYPE_F, 1.0f, 0.0f, 0, 0, VG_PATH_CAPABILITY_ALL);
   vguEllipse(circle, 0.0f, 0.0f, 8.0f, 8.0f);

   square = vgCreatePath(VG_PATH_FORMAT_STANDARD, VG_PATH_DATATYPE_F, 1.0f, 0.0f, 0, 0, VG_PATH_CAPABILITY_ALL);
   vguRect(square, 0, 0, 346.0f, 346.0f);
}

void display(void)
{
   VGfloat clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
   VGfloat dashPattern[4] = { 10.0f, 10.0f };

   vgSetfv(VG_STROKE_DASH_PATTERN, 0, NULL);
   vgSeti(VG_RENDERING_QUALITY, quality);
   vgSetfv(VG_CLEAR_COLOR, 4, clearColor);
   vgClear(0, 0, windowWidth, windowHeight);

   // draw destination image
   vgSeti(VG_BLEND_MODE, VG_BLEND_SRC);
   vgSeti(VG_MATRIX_MODE, VG_MATRIX_IMAGE_USER_TO_SURFACE);
   vgLoadIdentity();
   vgTranslate(dstCenter[X_COORD] - 173.0f, dstCenter[Y_COORD] - 173.0f);
   vgDrawImage(dstImage);

   // draw source image according to the current blend mode
   vgSeti(VG_BLEND_MODE, blendMode);
   vgSeti(VG_MATRIX_MODE, VG_MATRIX_IMAGE_USER_TO_SURFACE);
   vgLoadIdentity();
   vgTranslate(srcCenter[X_COORD] - 173.0f, srcCenter[Y_COORD] - 173.0f);
   vgDrawImage(srcImage);

   // draw control points
   vgSetf(VG_STROKE_LINE_WIDTH, 2.0f);
   vgSetfv(VG_STROKE_DASH_PATTERN, 0, NULL);
   vgSeti(VG_BLEND_MODE, VG_BLEND_SRC);
   vgSetPaint(solidColor, VG_STROKE_PATH);
   vgSeti(VG_MATRIX_MODE, VG_MATRIX_PATH_USER_TO_SURFACE);
   vgLoadIdentity();
   vgTranslate(dstCenter[X_COORD], dstCenter[Y_COORD]);
   vgDrawPath(circle, VG_STROKE_PATH);
   vgLoadIdentity();
   vgTranslate(srcCenter[X_COORD], srcCenter[Y_COORD]);
   vgDrawPath(circle, VG_STROKE_PATH);

   // draw images bounds
   vgSetf(VG_STROKE_LINE_WIDTH, 2.0f);
   vgSetfv(VG_STROKE_DASH_PATTERN, 2, dashPattern);
   vgLoadIdentity();
   vgTranslate(dstCenter[X_COORD] - 173.0f, dstCenter[Y_COORD] - 173.0f);
   vgDrawPath(square, VG_STROKE_PATH);
   vgLoadIdentity();
   vgTranslate(srcCenter[X_COORD] - 173.0f, srcCenter[Y_COORD] - 173.0f);
   vgDrawPath(square, VG_STROKE_PATH);

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

// calculate the distance between two points
static VGfloat distance(const VGfloat x0,
                        const VGfloat y0,
                        const VGfloat x1,
                        const VGfloat y1) {

   VGfloat dx = x0 - x1;
   VGfloat dy = y0 - y1;
   return (VGfloat)hypot(dx, dy);
}

void mouseLeftButtonDown(const VGint x,
                         const VGint y) {

   VGfloat distSrc, distDst;
   VGfloat mouseX = (VGfloat)x;
   VGfloat mouseY = (VGfloat)y;

   // calculate mouse distance from control points
   distSrc = distance(mouseX, mouseY, srcCenter[X_COORD], srcCenter[Y_COORD]);
   distDst = distance(mouseX, mouseY, dstCenter[X_COORD], dstCenter[Y_COORD]);
   // check if we have picked a control point
   if (distSrc < distDst) {
      pickedControlPoint = (distSrc < controlPointsRadius * 1.1f) ? CONTROL_POINT_SRC_IMAGE : CONTROL_POINT_NONE;
   } else {
      pickedControlPoint = (distDst < controlPointsRadius * 1.1f) ? CONTROL_POINT_DST_IMAGE : CONTROL_POINT_NONE;
   }
   // keep track of current mouse position
   oldMouseX = x;
   oldMouseY = y;
   mouseButton = MOUSE_BUTTON_LEFT;
}

void mouseLeftButtonUp(const VGint x,
                       const VGint y) {

   oldMouseX = x;
   oldMouseY = y;
   mouseButton = MOUSE_BUTTON_NONE;
   pickedControlPoint = CONTROL_POINT_NONE;
}

void mouseRightButtonDown(const VGint x,
                          const VGint y) {

   // keep track of current mouse position
   oldMouseX = x;
   oldMouseY = y;
   mouseButton = MOUSE_BUTTON_RIGHT;
}

void mouseRightButtonUp(const VGint x,
                        const VGint y) {

   oldMouseX = x;
   oldMouseY = y;
   mouseButton = MOUSE_BUTTON_NONE;
}

void mouseMove(const VGint x,
               const VGint y) {

   if (mouseButton == MOUSE_BUTTON_LEFT) {
      if (pickedControlPoint != CONTROL_POINT_NONE) {
         VGfloat* controlPoint = (pickedControlPoint == CONTROL_POINT_SRC_IMAGE) ? srcCenter : dstCenter;       // assign the new control point position
         controlPoint[X_COORD] = (VGfloat)x;
         controlPoint[Y_COORD] = (VGfloat)y;
      }
   }

   // keep track of current mouse position
   oldMouseX = x;
   oldMouseY = y;
}

void click(int button, int state, int x, int y)
{
   // fix y coord according to axis direction
   y = windowHeight - y;
   switch (button) {
   case GLUT_LEFT_BUTTON:
   {
      printf("button left ");
      switch (state) {
      case GLUT_DOWN:
         printf("down");
         mouseLeftButtonDown(x, y);
         break;
      case GLUT_UP:
         printf("up");
         mouseLeftButtonUp(x, y);
         break;
      }
      break;
   }
   case GLUT_RIGHT_BUTTON: {
      printf("button right ");
      switch (state) {
      case GLUT_DOWN:
         printf("down");
         mouseRightButtonDown(x, y);
         break;
      case GLUT_UP:
         printf("up");
         mouseRightButtonUp(x, y);
         break;
      }
      break;
   }
   }
   printf("\n");
   glutPostRedisplay();
}

void drag(int x, int y)
{
   // fix y coord according to axis direction
   y = windowHeight - y;
   mouseMove(x, y);
   glutPostRedisplay();
}

const char commands[] =
   "Click & drag mouse to change\n"
   "value for current mode\n\n"
   "h - this help\n"
   "b - change blend mode\n"
   "r - change display render quality\n"
   "q - quit";

const char help[] = "Press H for a list of commands";

void key(unsigned char code, int x, int y)
{
   static int open = 0;
   switch (tolower(code)) {
   case 'r':
      if (quality == VG_RENDERING_QUALITY_FASTER)
         quality = VG_RENDERING_QUALITY_BETTER;
      else
         quality = VG_RENDERING_QUALITY_FASTER;
      break;
   case 'b':
      blendMode = nextBlendMode(blendMode);
      testOverlayString("%s, m: %s", help, blendModeStr(blendMode));
      printf("new blend mode: %s\n", blendModeStr(blendMode));
      break;
   case 'q':
      exit(EXIT_SUCCESS);
      break;
   case 'h':
      if (!open) {
         /* Show help */
         testOverlayString(commands);
         open = 1;
      } else {
         testOverlayString("%s, m: %s", help, blendModeStr(blendMode));
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

static void initApp(void)
{
   srcCenter[0] = 200.0f;
   srcCenter[1] = 200.0f;
   dstCenter[0] = 312.0f;
   dstCenter[1] = 292.0f;
   /*
    * blendMode = VG_BLEND_SRC_OVER;
    */
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
   testCallback(TEST_CALLBACK_DRAG, (CallbackFunc) drag);
   /*
    * testCallback(TEST_CALLBACK_BUTTON, (CallbackFunc) move);
    */
   testCallback(TEST_CALLBACK_RESHAPE, (CallbackFunc) reshape);

   testOverlayString("%s, m: %s", help, blendModeStr(blendMode));
   testOverlayColor(1, 1, 1, 1);

   testRun();

   return EXIT_SUCCESS;
}
