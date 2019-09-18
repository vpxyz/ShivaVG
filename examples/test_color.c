#include "test.h"
#include <VG/openvg.h>
#include <VG/vgu.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <ctype.h>

#ifndef IMAGE_DIR
#define IMAGE_DIR "./"
#endif

unsigned int screenWidth, screenHeight;
enum colorType {
   NONE,
   R,
   G,
   B,
} colorType;


struct Image img;
void initImage(void)
{
   img = testCreateImageFromJpeg(IMAGE_DIR "test_color.jpg");
}
void cleanUp(void)
{
   vgDestroyImage(img.img);
}

void display(void)
{
   VGImage dstImage;
   VGfloat black[] = { 0.0f, 0.0f, 0.0f, 1.0f };

   VGfloat RedMatrix[] = { 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                           0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                           0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                           0.0f, 0.0f, 0.0f, 1.0f, 0.0f };

   VGfloat GreenMatrix[] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                           0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
                           0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                           0.0f, 0.0f, 0.0f, 1.0f, 0.0f };

   VGfloat BlueMatrix[] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                           0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                           0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
                           0.0f, 0.0f, 0.0f, 1.0f, 0.0f };

   vgSetfv(VG_CLEAR_COLOR, 4, black);
   vgClear(0, 0, img.width,img.height);

   dstImage = vgCreateImage(VG_lABGR_8888, img.width, img.height, VG_IMAGE_QUALITY_BETTER);

   switch (colorType) {
   case R:
      vgColorMatrix(dstImage, img.img, RedMatrix);
      vgDrawImage(dstImage);
      break;
   case G:
      vgColorMatrix( dstImage, img.img, GreenMatrix);
      vgDrawImage(dstImage);
      break;
   case B:
      vgColorMatrix( dstImage, img.img, BlueMatrix);
      vgDrawImage(dstImage);
      break;
   case NONE:
   default:
      vgDrawImage(img.img);
      break;

   }
   vgDestroyImage(dstImage);
}

const char commands[] =
   "Commands\n"
   "H - this help\n"
   "r - only red\n"
   "g - only green\n"
   "b - only blue\n"
   "n - none \n"
   "q - quit";

const char help[] = "Press H for a list of commands";
void key(unsigned char code, int x, int y)
{
   static int open = 0;
   switch (tolower(code)) {
   case 'r':
      colorType = R;
      break;
   case 'g':
      colorType = G;
      break;
   case 'b':
      colorType = B;
      break;
   case 'n':
      colorType = NONE;
      break;
   case 'q':
      exit(EXIT_SUCCESS);
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

   testInit(argc, argv, 1280, 720, "ShivaVG: test color example");
   testCallback(TEST_CALLBACK_DISPLAY, (CallbackFunc) display);
   testCallback(TEST_CALLBACK_KEY, (CallbackFunc) key);
   testOverlayColor(1.0f, 1.0f, 1.0f, 1.0f);
   testOverlayString(help);
   initImage();
   testRun();
   cleanUp();

   return EXIT_SUCCESS;
}
