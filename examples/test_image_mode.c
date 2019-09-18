#include "test.h"
#include <ctype.h>

#define SCREEN_WIDTH	640
#define SCREEN_HEIGHT   480
VGBlendMode blendMode = VG_BLEND_SRC;

void
display(void)
{

   VGfloat blackColor[4]     = { 0.0f, 0.0f, 0.0f, 0.5f };
   VGfloat letterColor[4]    = { 1.0f, 1.0f, 1.0f, 0.5f };
   /*
    * VGfloat clearColor[4]     = { 1.0f, 1.0f, 1.0f, 0.7f };
    */
   VGfloat clearColor[4]     = { 0.7f, 0.8f, 0.4f, 0.7f };
   VGfloat fillColor[4]      = { 1.0f, 0.0f, 0.0f, 0.5f };

   vgSetfv(VG_CLEAR_COLOR, 4, clearColor);
   vgClear(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

   VGImage image = vgCreateImage(VG_sRGB_565, 200, 200, VG_IMAGE_QUALITY_BETTER);
   vgSetfv(VG_CLEAR_COLOR, 4, blackColor);
   vgClearImage(image, 0, 0, 200, 200);
   vgSetfv(VG_CLEAR_COLOR, 4, letterColor);
   vgClearImage(image, 0, 140, 200,  60);
   vgClearImage(image, 70,  0,  60, 200);

   VGPaint paint = vgCreatePaint();
   vgSetPaint(paint, VG_FILL_PATH);
   vgSetParameterfv(paint, VG_PAINT_COLOR, 4, fillColor);

   vgSeti(VG_BLEND_MODE, blendMode); // TEST: per capire perchè il default è diverso da AmanithVG

   vgSeti(VG_MATRIX_MODE, VG_MATRIX_IMAGE_USER_TO_SURFACE);
   vgLoadIdentity();
   vgScale(0.4f, 0.4f);

   vgTranslate(200, 200);
   vgSeti(VG_IMAGE_MODE, VG_DRAW_IMAGE_NORMAL);
   vgDrawImage(image);

   vgTranslate(260, 150);
   vgSeti(VG_IMAGE_MODE, VG_DRAW_IMAGE_MULTIPLY);
   vgDrawImage(image);

   vgTranslate(260, 150);
   vgSeti(VG_IMAGE_MODE, VG_DRAW_IMAGE_STENCIL);
   vgDrawImage(image);

   vgDestroyPaint(paint);
   vgDestroyImage(image);

}


const char commands[] =
   "Commands\n"
   "h - this help\n"
   "b - change blend mode\n"
   "q - quit";

const char help[] = "Press H for a list of commands";

void
key(unsigned char code, int x, int y)
{
   static int open = 0;
   switch (tolower(code)) {
   case 'q':
      exit(EXIT_SUCCESS);
   case 'b':
      blendMode = nextBlendMode(blendMode);
      testOverlayString("%s, bm: %s", help, blendModeStr(blendMode));
      printf("new blend mode: %s\n", blendModeStr(blendMode));
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
   default:
      break;
   }

}

int main(int argc, char *argv[])
{
   testInit(argc, argv, SCREEN_WIDTH, SCREEN_HEIGHT, "ShivaVG: Image mode Test");
   testCallback(TEST_CALLBACK_DISPLAY, (CallbackFunc) display);
   testCallback(TEST_CALLBACK_KEY, (CallbackFunc) key);
   testOverlayColor(1.0f, 0.0f, 0.0f, 1.0f);
   testOverlayString("%s, bm: %s", help, blendModeStr(blendMode));

   testRun();

   return EXIT_SUCCESS;
}
