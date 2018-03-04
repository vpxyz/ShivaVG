#include "test.h"

#ifndef IMAGE_DIR
#define IMAGE_DIR "./"
#endif


struct Image cover;
VGfloat *warpMatrix;

void
display(float interval)
{
   VGfloat white[] = { 0, 0, 0, 1 };
   VGint c;

   vgSetfv(VG_CLEAR_COLOR, 4, white);
   vgClear(0, 0, testWidth(), testHeight());

// calculate warp matrix
   vguComputeWarpQuadToQuad(    // destination quadrilateral
                              0.0f, 0.0f,
                              cover.width + 10.0f, 115.0f,
                              cover.width + 20.0f, cover.height + 25.0f,
                              25.0f, cover.height + 40.0f,
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

   return EXIT_SUCCESS;
}
