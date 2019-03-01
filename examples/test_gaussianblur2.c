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

#define SCREEN_WIDTH	320
#define SCREEN_HEIGHT   240

unsigned int screenWidth, screenHeight;
enum deviationType {
   NONE,
   DEFAULT,
   X,
   Y,
   M,
   S
} deviationType;

struct Image img;
void initImage(void)
{
   img = testCreateImageFromJpeg(IMAGE_DIR "milano.jpg");
}
void cleanUp(void)
{
   vgDestroyImage(img.img);
}

void display(void)
{
   VGImage dstImage;
   VGfloat white[] = { 0.0f, 0.0f, 0.0f, 1.0f };

   vgSetfv(VG_CLEAR_COLOR, 4, white);
   vgClear(0, 0, img.width,img.height);

   dstImage = vgCreateImage(VG_lABGR_8888, img.width, img.height, VG_IMAGE_QUALITY_BETTER);

   switch (deviationType) {
   case DEFAULT:
      vgGaussianBlur(dstImage, img.img, 10.0f, 10.0f, VG_TILE_PAD);
      vgDrawImage(dstImage);
      break;
   case X:
      vgGaussianBlur( dstImage, img.img, 10.0f, 1.0f, VG_TILE_PAD);
      vgDrawImage(dstImage);
      break;
   case Y:
      vgGaussianBlur( dstImage, img.img, 1.0f, 10.0f, VG_TILE_PAD);
      vgDrawImage(dstImage);
      break;
   case M:
      vgGaussianBlur( dstImage, img.img, 5.0f, 5.0f, VG_TILE_PAD);
      vgDrawImage(dstImage);
      break;
   case S:
      vgGaussianBlur(dstImage, img.img, 1.0f, 1.0f, VG_TILE_PAD);
      vgDrawImage(dstImage);
      break;
   default:
      vgDrawImage(img.img);
      break;

   }
   vgDestroyImage(dstImage);
}

const char commands[] =
   "Commands\n"
   "H - this help\n"
   "t - stdDev == 10.0\n"
   "x - stdDevX > stdDevY\n"
   "y - sdtDevY > stdDevX\n"
   "5 - stdDev == 5.0\n"
   "1 - stdDev == 1.0\n"
   "n - none \n"
   "q - quit \n";

const char help[] = "Press H for a list of commands";
void key(unsigned char code, int x, int y)
{
   static int open = 0;
   switch (tolower(code)) {
   case 't':
      deviationType = DEFAULT;
      break;
   case 'x':
      deviationType = X;
      break;
   case 'y':
      deviationType = Y;
      break;
   case '5':
      deviationType = M;
      break;
   case '1':
      deviationType = S;
      break;
   case 'n':
      deviationType = NONE;
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
   testInit(argc, argv, 1600, 1066, "ShivaVG: gaussian blur advanced example");
   testCallback(TEST_CALLBACK_DISPLAY, (CallbackFunc) display);
   testCallback(TEST_CALLBACK_KEY, (CallbackFunc) key);
   testOverlayColor(1, 1, 1, 1);
   testOverlayString(help);
   initImage();
   testRun();
   cleanUp();

   return EXIT_SUCCESS;
}
