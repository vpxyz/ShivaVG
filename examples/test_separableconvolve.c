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

enum KernelType {
   NONE,
   SHARPENING_KERNEL,
   SMOOTHING_KERNEL,
   SOBEL_KERNEL
} kernelType = NONE;

void display(void)
{
   VGImage srcImage, dstImage;
   VGfloat white[] = { 0.0f, 0.0f, 0.0f, 1.0f };

   // 4 elements in order to align data. We use only the firt 3 values
   VGshort sharpeningKernelX[4] = { -1,  3, -1, 0 };
   VGshort sharpeningKernelY[4] = { -1,  3, -1, 0 };

   VGshort smoothingKernelX[4] = { 1, 2, 1, 0 };
   VGshort smoothingKernelY[4] = { 1, 2, 1, 0 };

   VGshort sobelKernelX[4] = { -1, 0, -1, 0 };
   VGshort sobelKernelY[4] = { 1, 2, 1, 0 };

   VGImageFormat imgFormat;
   #if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
   imgFormat = VG_sABGR_8888;
   #else
   imgFormat = VG_sRGBA_8888;
   #endif

   vgSetfv(VG_CLEAR_COLOR, 4, white);
   vgClear(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

   srcImage = vgCreateImage(imgFormat, SCREEN_WIDTH, SCREEN_HEIGHT, VG_IMAGE_QUALITY_BETTER);
   vgImageSubData(srcImage, cimg, SCREEN_WIDTH*4, VG_sRGBA_8888, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

   dstImage = vgCreateImage(imgFormat, SCREEN_WIDTH, SCREEN_HEIGHT, VG_IMAGE_QUALITY_BETTER );

   switch (kernelType) {
   case SHARPENING_KERNEL:
      vgSeparableConvolve(dstImage, srcImage, 3, 3, 1, 1, sharpeningKernelX, sharpeningKernelY, 1.0f, 0.0f, VG_TILE_FILL);
      vgDrawImage(dstImage);
      break;
   case SMOOTHING_KERNEL:
      vgSeparableConvolve(dstImage, srcImage, 3, 3, 1, 1, smoothingKernelX, smoothingKernelY, 0.1625f, 0.0f, VG_TILE_FILL);
      vgDrawImage(dstImage);
      break;
   case SOBEL_KERNEL:
      vgSeparableConvolve(dstImage, srcImage, 3, 3, 1, 1, sobelKernelX, sobelKernelY, 0.1f, 0.0f, VG_TILE_FILL);
      vgDrawImage(dstImage);
      break;
   case NONE:
   default:
      vgDrawImage(srcImage);
      break;
   }

   vgDestroyImage(srcImage);
   vgDestroyImage(dstImage);
}

const char commands[] =
   "Commands\n"
   "H - this help\n"
   "s - sharpening kernel\n"
   "m - smooting kernel\n"
   "b - sobel kernel\n"
   "n - none \n"
   "q - quit \n";

const char help[] = "Press H for a list of commands";
void key(unsigned char code, int x, int y)
{
   static int open = 0;
   switch (tolower(code)) {
   case 's':
      kernelType = SHARPENING_KERNEL;
      break;
   case 'm':
      kernelType = SMOOTHING_KERNEL;
      break;
   case 'b':
      kernelType = SOBEL_KERNEL;
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

   testInit(argc, argv, SCREEN_WIDTH,SCREEN_HEIGHT , "ShivaVG: separable convolve example");
   testCallback(TEST_CALLBACK_DISPLAY, (CallbackFunc) display);
   testCallback(TEST_CALLBACK_KEY, (CallbackFunc) key);

   testOverlayString(help);
   testOverlayColor(1, 1, 1, 1);
   kernelType = NONE;
   testRun();
   return EXIT_SUCCESS;

}
