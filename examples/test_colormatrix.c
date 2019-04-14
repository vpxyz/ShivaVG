#include "test.h"
#include <VG/openvg.h>
#include <VG/vgu.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <ctype.h>

#include "./img_data.c"

#define SCREEN_WIDTH	320
#define SCREEN_HEIGHT   240

enum matrixType {
   NONE,
   RGB_SWAP,
   GREY_SCALE,
   BIAS
} matrixType;

struct Image cover;

void display(void)
{
   VGImage srcImage, dstImage;

   VGfloat RGBSwapMatrix[] = { 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
                               1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                               0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
                               1.0f, 1.0f, 1.0f, 0.0f, 0.0f };

   VGfloat GrayScaleMatrix[] = { 0.2126f, 0.7152f, 0.0722f, 0.0f, 0.0f,
                                 0.2126f, 0.7152f, 0.0722f, 0.0f, 0.0f,
                                 0.2126f, 0.7152f, 0.0722f, 0.0f, 0.0f,
                                 0.2126f, 0.7152f, 0.0722f, 0.0f, 0.0f };

   VGfloat BiasMatrix[] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                            1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                            0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                            0.0f, 0.0f, 0.0f, 1.0f, 0.0f };


   VGfloat white[] = { 0.0f, 0.0f, 0.0f, 1.0f };

   vgSetfv(VG_CLEAR_COLOR, 4, white);
   vgClear(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

   VGImageFormat imgFormat;
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
   imgFormat = VG_sABGR_8888;
#else
   imgFormat = VG_sRGBA_8888;
#endif

   srcImage = vgCreateImage(imgFormat, SCREEN_WIDTH, SCREEN_HEIGHT, VG_IMAGE_QUALITY_BETTER );

   vgImageSubData(srcImage, cimg, SCREEN_WIDTH * 4, VG_sRGBA_8888, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT );

   dstImage = vgCreateImage(imgFormat, SCREEN_WIDTH, SCREEN_HEIGHT, VG_IMAGE_QUALITY_BETTER );

   switch (matrixType) {
   case RGB_SWAP:
      vgColorMatrix( dstImage, srcImage, RGBSwapMatrix);
      vgDrawImage(dstImage);
      break;
   case GREY_SCALE:
      vgColorMatrix( dstImage, srcImage, GrayScaleMatrix);
      vgDrawImage(dstImage);
      break;
   case BIAS:
      vgColorMatrix( dstImage, srcImage, BiasMatrix);
      vgDrawImage(dstImage);
      break;
   case NONE:
   default:
      vgDrawImage(srcImage);
      break;
   }

   vgDrawImage(dstImage);
   vgDestroyImage(srcImage);
   vgDestroyImage(dstImage);

}

const char commands[] =
   "Commands\n"
   "H - this help\n"
   "s - rgb swap\n"
   "g - grey scale\n"
   "b - bias \n"
   "n - none \n"
   "q - quit";

const char help[] = "Press H for a list of commands";
void key(unsigned char code, int x, int y)
{
   static int open = 0;
   switch (tolower(code)) {
   case 's':
      matrixType = RGB_SWAP;
      break;
   case 'g':
      matrixType = GREY_SCALE;
      break;
   case 'b':
      matrixType = BIAS;
      break;
   case 'n':
      matrixType = NONE;
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

}

#ifndef IMAGE_DIR
#define IMAGE_DIR "./"
#endif

int main(int argc, char *argv[])
{
   testInit(argc, argv, SCREEN_WIDTH,SCREEN_HEIGHT , "ShivaVG: colormatrix example");
   testCallback(TEST_CALLBACK_DISPLAY, (CallbackFunc) display);
   testCallback(TEST_CALLBACK_KEY, (CallbackFunc) key);

   testOverlayString(help);
   testOverlayColor(1, 1, 1, 1);

   matrixType = NONE;

   testRun();
   return EXIT_SUCCESS;
}
