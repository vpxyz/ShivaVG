#include "test.h"
#include <stdbool.h>

#ifndef IMAGE_DIR
#define IMAGE_DIR "./"
#endif

#define MAXOFFSET  30.0f
#define MINOFFSET  -30.0f
#define OFFSET 0.1f

struct Image cover;
VGfloat *warpMatrix;

enum direction {
   GROW,
   DECREASE,
   ENLARGE,
   RESTRICT
};

void
display(float interval)
{
   // simple animation params
   static float grow_descr = 0.0f;
   static float enlarge_restrict = 0.0f;
   static enum direction dir = GROW;

   VGfloat white[] = { 0, 0, 0, 1 };

   vgSetfv(VG_CLEAR_COLOR, 4, white);
   vgClear(0, 0, testWidth(), testHeight());


// calculate warp matrix
   vguComputeWarpQuadToQuad(  // destination quadrilateral
      0.0f , 0.0f,
      cover.width + 10.0f + grow_descr, 115.0f,
      cover.width + 20.0f ,cover.height + 25.0f + grow_descr,
      25.0f + enlarge_restrict, cover.height + 40.0f + enlarge_restrict,
      // source image bounds
      0.0f, 0.0f,
      cover.width, 0.0f,
      cover.width, cover.height, 0.0f, cover.height,
      // the output matrix
      warpMatrix);

   // upload the warp matrix to the OpenVG backend
   vgSeti(VG_MATRIX_MODE, VG_MATRIX_IMAGE_USER_TO_SURFACE);
   vgLoadIdentity();
   vgLoadMatrix(warpMatrix);
   vgTranslate(50, 150);
   // draw image
   vgDrawImage(cover.img);

   // simple animation
   switch (dir) {
   case GROW:
      if (grow_descr >= MAXOFFSET) {
         dir = DECREASE;
         grow_descr -= OFFSET;
      }
      grow_descr += OFFSET;
      break;
      
   case DECREASE:
      if (grow_descr <= MINOFFSET) {
         dir = ENLARGE;
      }
      grow_descr -= OFFSET;
      break;
      
   case ENLARGE:
      if (enlarge_restrict >= MAXOFFSET) {
         dir = RESTRICT;
         enlarge_restrict -= OFFSET;
      }
      enlarge_restrict += OFFSET;
      break;
      
   case RESTRICT:
      if (enlarge_restrict <= MINOFFSET) {
         dir = GROW;
      }
      enlarge_restrict -= OFFSET;
      break;
   }
}

int
main(int argc, char **argv)
{
   testInit(argc, argv, 1000, 800, "ShivaVG: Warp Test");
   // Tell testRun() to run display function
   testCallback(TEST_CALLBACK_DISPLAY, (CallbackFunc) display);
   testOverlayColor(1, 1, 1, 1);
   cover = testCreateImageFromJpeg(IMAGE_DIR "test_img_guitar.jpg");

   warpMatrix = (VGfloat *) malloc(9 * sizeof(VGfloat));

   testRun();
   
   free(warpMatrix);

   return EXIT_SUCCESS;
}
