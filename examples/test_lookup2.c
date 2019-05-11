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
struct Image img;

void initImage(void)
{
   img = testCreateImageFromJpeg(IMAGE_DIR "test_color.jpg");
}
void cleanUp(void)
{
   vgDestroyImage(img.img);
}


enum lookupType {
   INVERT,
   DARKEN,
   LIGHTEN,
   RED_CHANNEL,
   GREEN_CHANNEL,
   BLUE_CHANNEL,
   TRANSPARENCY,
   NONE,
} lookupType;


void display(void)
{
   VGImage dstImage;
   VGfloat white[] = { 0.0f, 0.0f, 0.0f, 1.0f };

   VGubyte redLUT[256], greenLUT[256], blueLUT[256], alphaLUT[256];
   VGfloat variation = 4;
   for (VGint index = 0; index < 256; index++ ) {
      switch (lookupType) {
      case INVERT:
         redLUT[index]   = (VGubyte)( 255 - index );
         greenLUT[index] = (VGubyte)( 255 - index );
         blueLUT[index]  = (VGubyte)( 255 - index );
         alphaLUT[index] = (VGubyte)( index );
         break;
      case DARKEN:
         redLUT[index]   = (VGubyte)( index / variation );
         greenLUT[index] = (VGubyte)( index / variation );
         blueLUT[index]  = (VGubyte)( index / variation );
         alphaLUT[index] = (VGubyte)( index );
         break;
      case LIGHTEN:
         if ( index * 2 <= 255 ) {
            redLUT[index]   = (VGubyte)( index * variation );
            greenLUT[index] = (VGubyte)( index * variation );
            blueLUT[index]  = (VGubyte)( index * variation );
            alphaLUT[index] = (VGubyte)( index );
         } else {
            redLUT[index]   = 255;
            greenLUT[index] = 255;
            blueLUT[index]  = 255;
            alphaLUT[index] = (VGubyte)( index );
         }
         break;
      case RED_CHANNEL:
         redLUT[index]   = (VGubyte)( index );
         greenLUT[index] = 0;
         blueLUT[index]  = 0;
         alphaLUT[index] = (VGubyte)( index );
         break;
      case GREEN_CHANNEL:
         redLUT[index]   = 0;
         greenLUT[index] = (VGubyte)( index );
         blueLUT[index]  = 0;
         alphaLUT[index] = (VGubyte)( index );
         break;
      case BLUE_CHANNEL:
         redLUT[index]   = 0;
         greenLUT[index] = 0;
         blueLUT[index]  = (VGubyte)( index );
         alphaLUT[index] = (VGubyte)( index );
         break;
      case TRANSPARENCY:
         redLUT[index]   = (VGubyte)( index );
         greenLUT[index] = (VGubyte)( index );
         blueLUT[index]  = (VGubyte)( index );
         alphaLUT[index] = (VGubyte)( index / variation );
         break;
      case NONE:
      default:
         break;
      }
   }

   vgSetfv(VG_CLEAR_COLOR, 4, white);
   vgClear(0, 0, img.width,img.height);

   dstImage = vgCreateImage(VG_lABGR_8888, img.width, img.height, VG_IMAGE_QUALITY_BETTER);

   if (lookupType != NONE) {
      vgLookup(dstImage, img.img, redLUT, greenLUT, blueLUT, alphaLUT, VG_TRUE, VG_FALSE );
      vgDrawImage(dstImage);
   } else {
      vgDrawImage(img.img);
   }
   vgDestroyImage(dstImage);
}

const char commands[] =
   "Commands\n"
   "H - this help\n"
   "i - invert\n"
   "d - darken\n"
   "l - lighten \n"
   "r - red \n"
   "g - green \n"
   "b - blue \n"
   "t - transparency \n"
   "n - none \n"
   "q - quit";

const char help[] = "Press H for a list of commands";
void key(unsigned char code, int x, int y)
{
   static int open = 0;
   switch (tolower(code)) {
   case 'i':
      lookupType = INVERT;
      break;
   case 'd':
      lookupType = DARKEN;
      break;
   case 'l':
      lookupType = LIGHTEN ;
      break;
   case 'r':
      lookupType = RED_CHANNEL;
      break;
   case 'g':
      lookupType = GREEN_CHANNEL;
      break;
   case 'b':
      lookupType = BLUE_CHANNEL;
      break;
   case 't':
      lookupType = TRANSPARENCY;
      break;
   case 'n':
      lookupType = NONE;
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

   testInit(argc, argv, 1280, 720, "ShivaVG: color lockup2 example");
   testCallback(TEST_CALLBACK_DISPLAY, (CallbackFunc) display);
   testCallback(TEST_CALLBACK_KEY, (CallbackFunc) key);
   testOverlayString(help);
   testOverlayColor(1, 1, 1, 1);
   initImage();
   lookupType = NONE;
   testRun();
   cleanUp();
   return EXIT_SUCCESS;

}
