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

enum lookupChannel {
   NONE,
   RED_CHANNEL,
   GREEN_CHANNEL,
   BLUE_CHANNEL,
} lookupChannel;



void display(void)
{
   VGImage srcImage, dstImage;
   VGuint lookupTable[256];
   VGfloat white[] = { 0.0f, 0.0f, 0.0f, 1.0f };

   for (VGint index = 0; index < 256; index++ ) {
      switch (lookupChannel) {
      case RED_CHANNEL:
         lookupTable[index] = (index << 24) + index;
         break;

      case GREEN_CHANNEL:
         lookupTable[index] = (index << 16) + index;
         break;

      case BLUE_CHANNEL:
         lookupTable[index] = (index << 8) + index;
         break;

      default:
         break;
      }

   }

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

   switch (lookupChannel) {
   case RED_CHANNEL:
      vgLookupSingle( dstImage, srcImage, lookupTable, VG_RED, VG_TRUE, VG_FALSE );
      vgDrawImage(dstImage);
      break;
   case GREEN_CHANNEL:
      vgLookupSingle( dstImage, srcImage, lookupTable, VG_GREEN, VG_TRUE, VG_FALSE );
      vgDrawImage(dstImage);
      break;
   case BLUE_CHANNEL:
      vgLookupSingle( dstImage, srcImage, lookupTable, VG_BLUE, VG_TRUE, VG_FALSE );
      vgDrawImage(dstImage);
      break;
   default:
      vgDrawImage(srcImage);
   }

   vgDestroyImage(srcImage);
   vgDestroyImage(dstImage);

}

const char commands[] =
   "Commands\n"
   "H - this help\n"
   "r - red \n"
   "g - green \n"
   "b - blue \n"
   "n - none \n"
   "q - quit \n";

const char help[] = "Press H for a list of commands";
void key(unsigned char code, int x, int y)
{
   static int open = 0;
   switch (tolower(code)) {
   case 'r':
      lookupChannel = RED_CHANNEL;
      break;
   case 'g':
      lookupChannel = GREEN_CHANNEL;
      break;
   case 'b':
      lookupChannel = BLUE_CHANNEL;
      break;
   case 'n':
      lookupChannel = NONE;
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
   testInit(argc, argv, SCREEN_WIDTH,SCREEN_HEIGHT , "ShivaVG: color lockup single example");
   testCallback(TEST_CALLBACK_DISPLAY, (CallbackFunc) display);
   testCallback(TEST_CALLBACK_KEY, (CallbackFunc) key);

   testOverlayString(help);
   testOverlayColor(1, 1, 1, 1);
   lookupChannel = NONE;
   testRun();
   return EXIT_SUCCESS;

   return 0;
}
