#include "test.h"
#include <ctype.h>

#define SCREEN_WIDTH	320
#define SCREEN_HEIGHT   240

char color = 'r';

void
display(void)
{
   VGImage foreGround;

   VGfloat redClear[] = { 1.0f, 0.0f, 0.0f, 1.0f };
   VGfloat greenClear[]  = { 0.0f, 1.0f, 0.0f, 1.0f };
   VGfloat blueClear[]  = { 0.0f, 0.0f, 1.0f, 1.0f };
   VGfloat clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };

   vgSetfv(VG_CLEAR_COLOR, 4, clearColor);
   vgClear(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

   vgSeti(VG_MATRIX_MODE, VG_MATRIX_IMAGE_USER_TO_SURFACE);
   foreGround = vgCreateImage(VG_sRGBA_8888, 200, 260, VG_IMAGE_QUALITY_BETTER);

   switch (color) {
   case 'r':
      vgSetfv(VG_CLEAR_COLOR, 4, redClear);
      vgClearImage(foreGround, 0, 0, 200, 260);
      break;
   case 'g':
      vgSetfv(VG_CLEAR_COLOR, 4, greenClear);
      vgClearImage(foreGround, 0, 0, 200, 260);
      break;
   case 'b':
   default:
      vgSetfv(VG_CLEAR_COLOR, 4, blueClear);
      vgClearImage(foreGround, 0, 0, 200, 260);
      break;
   }

   vgLoadIdentity();
   vgTranslate(85, 0);
   vgScale(0.7f, 0.7f);

   vgDrawImage(foreGround);
   vgDestroyImage(foreGround);
}

const char commands[] =
   "Commands\n"
   "h - this help\n"
   "c - change color mode\n"
   "q - quit";

const char help[] = "Press H for a list of commands";

void
key(unsigned char code, int x, int y)
{
   static int open = 0;
   switch (tolower(code)) {
   case 'q':
      exit(EXIT_SUCCESS);
   case 'c':
      switch (color) {
      case 'r':
         color = 'g';
         break;
      case 'g':
         color = 'b';
         break;
      case 'b':
         color = 'r';
         break;
      }
      testOverlayString("%s, color: %c", help, color);
      printf("new color mode: %c\n", color);
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
   testInit(argc, argv, SCREEN_WIDTH, SCREEN_HEIGHT, "ShivaVG: sRGB Test");
   testCallback(TEST_CALLBACK_DISPLAY, (CallbackFunc) display);
   testCallback(TEST_CALLBACK_KEY, (CallbackFunc) key);
   testOverlayColor(1.0f, 1.0f, 1.0f, 1.0f);
   testOverlayString("%s, color: %c", help, color);

   testRun();

   return EXIT_SUCCESS;
}
