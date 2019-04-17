#include "test.h"
#include <ctype.h>

#define SCREEN_WIDTH	320
#define SCREEN_HEIGHT   240

void display(void)
{
   VGImage image;
   VGPaint paint;

   VGfloat blackcolor[4]     = { 0.0f, 0.0f, 0.0f, 0.5f };
   VGfloat lettercolor[4]    = { 1.0f, 1.0f, 1.0f, 0.5f };
   VGfloat clearColor[4]     = { 0.87f, 0.87f, 0.87f, 0.7f };
   VGfloat fillcolor[4]      = { 1.0f, 0.0f, 0.0f, 0.5f };

   vgSetfv( VG_CLEAR_COLOR, 4, clearColor );
   vgClear( 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT );

   image = vgCreateImage(VG_sRGB_565, 200, 200, VG_IMAGE_QUALITY_BETTER);
   vgSetfv( VG_CLEAR_COLOR, 4, blackcolor );
   vgClearImage( image, 0, 0, 200, 200 );
   vgSetfv( VG_CLEAR_COLOR, 4, lettercolor );
   vgClearImage( image, 0, 140, 200,  60 );
   vgClearImage( image, 70,  0,  60, 200 );

   paint = vgCreatePaint();
   vgSetPaint( paint, VG_FILL_PATH );
   vgSetParameterfv( paint, VG_PAINT_COLOR, 4, fillcolor );

   vgSeti( VG_MATRIX_MODE, VG_MATRIX_IMAGE_USER_TO_SURFACE );
   vgLoadIdentity();
   vgTranslate( 20, 20 );

   vgScale( 0.4f, 0.4f );
   vgSeti( VG_IMAGE_MODE, VG_DRAW_IMAGE_NORMAL );
   vgDrawImage( image );

   vgTranslate( 260, 150 );
   vgSeti( VG_IMAGE_MODE, VG_DRAW_IMAGE_MULTIPLY );
   vgDrawImage( image );

   vgTranslate( 260, 150 );
   vgSeti( VG_IMAGE_MODE, VG_DRAW_IMAGE_STENCIL );
   vgDrawImage( image );

   vgDestroyPaint( paint );
   vgDestroyImage( image );

}

void
key(unsigned char code, int x, int y)
{
   if (tolower(code) == 'q') {
      exit(EXIT_SUCCESS);
   }

}

int main(int argc, char *argv[])
{
   testInit(argc, argv, SCREEN_WIDTH, SCREEN_HEIGHT, "ShivaVG: Image mode Test");
   testCallback(TEST_CALLBACK_DISPLAY, (CallbackFunc) display);
   testCallback(TEST_CALLBACK_KEY, (CallbackFunc) key);
   testOverlayColor(1.0f, 0.1f, 0.1f, 1.0f);
   testRun();

   return EXIT_SUCCESS;
}
