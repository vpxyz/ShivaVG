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

#define SCREEN_WIDTH	240
#define SCREEN_HEIGHT   320

enum KernelType {
   NONE,
   SCALING_KERNEL,
   BIASING_KERNEL,
   SHARPENING_KERNEL,
   SMOOTHING_KERNEL,
   EMBOSSING_KERNEL
} kernelType;

VGImage srcImage;

void display(void)
{
   VGImage dstImage;
   VGfloat white[] = { 0.0f, 0.0f, 0.0f, 1.0f };
   VGshort originalKernel[9] = {0, 0, 0,
                                0, 1, 0,
                                0, 0, 0};

   VGshort sharpeningKernel[9] = { -1, -1, -1,
                                   -1,  9, -1,
                                   -1, -1, -1 };

   VGshort smoothingKernel[9] = { 1,  2,  1,
                                  2,  4,  2,
                                  1,  2,  1};

   VGshort embossingKernel[9] = { 2,  0,  0,
                                  0, -1,  0,
                                  0,  0, -1 };


   vgSetfv(VG_CLEAR_COLOR, 4, white);
   vgClear(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

   if (kernelType == NONE) {
      vgDrawImage(srcImage);
      return;
   }

   dstImage = vgCreateImage(imgFormat, SCREEN_WIDTH, SCREEN_HEIGHT, VG_IMAGE_QUALITY_BETTER);


   switch (kernelType) {
   case SCALING_KERNEL:
      vgConvolve(dstImage, srcImage, 3, 3, 1, 1, originalKernel, 1.0f, 0.0f, VG_TILE_FILL);
      /*
       * VGErrorCode err = vgGetError();
       * if (err != 0) {
       *    printf("Something goes wrong %x\n", err);
       * }
       */
      vgDrawImage(dstImage);
      break;
   case BIASING_KERNEL:
      vgConvolve(dstImage, srcImage, 3, 3, 1, 1, originalKernel, 1.0f, 0.4f, VG_TILE_FILL);
      vgDrawImage(dstImage);
      break;
   case SHARPENING_KERNEL:
      vgConvolve(dstImage, srcImage, 3, 3, 1, 1, sharpeningKernel, 1.0f, 0.0f, VG_TILE_FILL);
      vgDrawImage(dstImage);
      break;
   case SMOOTHING_KERNEL:
      vgConvolve(dstImage, srcImage, 3, 3, 1, 1, smoothingKernel, 0.0625f, 0.0f, VG_TILE_FILL);
      vgDrawImage(dstImage);
      break;
   case EMBOSSING_KERNEL:
      vgConvolve(dstImage, srcImage, 3, 3, 1, 1, embossingKernel, 1.0f, 0.3f, VG_TILE_FILL);
      vgDrawImage(dstImage);
      break;
   case NONE:
   default:
      break;

   }
   vgDestroyImage( dstImage );

}

const char commands[] =
   "Commands\n"
   "H - this help\n"
   "s - scaling kernel\n"
   "b - biasing kernel\n"
   "p - sharpening kernel\n"
   "m - smooting kernel\n"
   "e - embossing kernel\n"
   "n - none \n"
   "q - quit \n";

const char help[] = "Press H for a list of commands";
void key(unsigned char code, int x, int y)
{
   static int open = 0;
   switch (tolower(code)) {
   case 's':
      kernelType = SCALING_KERNEL;
      break;
   case 'b':
      kernelType = BIASING_KERNEL;
      break;
   case 'p':
      kernelType = SHARPENING_KERNEL;
      break;
   case 'm':
      kernelType = SMOOTHING_KERNEL;
      break;
   case 'e':
      kernelType = EMBOSSING_KERNEL;
      break;
   case 'n':
      kernelType = NONE;
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

int main(int argc, char *argv[])
{
   testInit(argc, argv, SCREEN_WIDTH,SCREEN_HEIGHT , "ShivaVG: convolve example");
   testCallback(TEST_CALLBACK_DISPLAY, (CallbackFunc) display);
   testCallback(TEST_CALLBACK_KEY, (CallbackFunc) key);

   testOverlayString(help);
   testOverlayColor(1, 1, 1, 1);

   // init source image
   srcImage = vgCreateImage(imgFormat, SCREEN_WIDTH, SCREEN_HEIGHT, VG_IMAGE_QUALITY_BETTER );
   vgImageSubData(srcImage, cimgGirl, SCREEN_WIDTH * 4, VG_sRGBA_8888, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

   kernelType = NONE;
   testRun();
   return EXIT_SUCCESS;
}
