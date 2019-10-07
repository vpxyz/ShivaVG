#include "test.h"
#include <ctype.h>

#define SCREEN_WIDTH	320
#define SCREEN_HEIGHT   240

VGBlendMode blendMode = VG_BLEND_SRC;

void
display(void)
{
   VGImage backGround, foreGround;

   VGfloat srcColor[4] = { 0.12f, 0.58f, 0.95f, 1.0f}; // #2196f3
   VGfloat dstColor[4] = { 0.91f, 0.11f, 0.38f, 1.0f}; // #e91e63

   /*
    * VGfloat redClear[]   = { 1.0f, 0.0f, 0.0f, 0.6f };
    * VGfloat blueClear[]  = { 0.0f, 0.0f, 1.0f, 0.6f };
    */
   VGfloat clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };

   vgSetfv(VG_CLEAR_COLOR, 4, clearColor);
   vgClear(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

   vgSeti(VG_MATRIX_MODE, VG_MATRIX_IMAGE_USER_TO_SURFACE);
   backGround = vgCreateImage(VG_sRGBA_8888, 200, 260, VG_IMAGE_QUALITY_BETTER);
   foreGround = vgCreateImage(VG_sRGBA_8888, 200, 260, VG_IMAGE_QUALITY_BETTER);

   /*
    * vgSetfv(VG_CLEAR_COLOR, 4, redClear);
    */
   vgSetfv(VG_CLEAR_COLOR, 4, dstColor);
   vgClearImage(backGround, 0, 0, 200, 260);

   /*
    * vgSetfv(VG_CLEAR_COLOR, 4, blueClear);
    */
   vgSetfv(VG_CLEAR_COLOR, 4, srcColor);
   vgClearImage(foreGround, 0, 0, 200, 260);

   vgLoadIdentity();
   vgTranslate(85, 0);
   vgScale(0.7f, 0.7f);

   vgSeti(VG_BLEND_MODE, VG_BLEND_SRC);
   vgDrawImage(backGround);

   vgTranslate(60, 45);
   vgRotate(30.0f);
   vgSeti(VG_BLEND_MODE, blendMode);

   vgDrawImage(foreGround);

   vgDestroyImage(backGround);
   vgDestroyImage(foreGround);

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
   testInit(argc, argv, SCREEN_WIDTH, SCREEN_HEIGHT, "ShivaVG: Porter Duff Test");
   testCallback(TEST_CALLBACK_DISPLAY, (CallbackFunc) display);
   testCallback(TEST_CALLBACK_KEY, (CallbackFunc) key);
   testOverlayColor(1.0f, 1.0f, 1.0f, 1.0f);
   testOverlayString("%s, bm: %s", help, blendModeStr(blendMode));

   testRun();

   return EXIT_SUCCESS;
}
