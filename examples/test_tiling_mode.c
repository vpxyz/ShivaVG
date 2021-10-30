#include "test.h"
#include <ctype.h>

#include "./img_data.c"

/*
 * #define SCREEN_WIDTH	320
 * #define SCREEN_HEIGHT   240
 */
#define SCREEN_WIDTH	640
#define SCREEN_HEIGHT   480

enum tilingType {
   NONE,
   FILL,
   PAD,
   REPEAT,
   REFLECT
} tilingMode;

void
display(void)
{
   VGfloat blue[4]   = {0.0f, 0.0f, 1.0f, 1.0f};
   VGfloat yellow[4] = {1.0f, 1.0f, 0.0f, 1.0f};

   VGubyte segments[] = {VG_MOVE_TO_ABS, VG_LINE_TO_REL, VG_LINE_TO_REL, VG_LINE_TO_REL, VG_LINE_TO_REL, VG_CLOSE_PATH,
                          VG_MOVE_TO_ABS, VG_LINE_TO_REL, VG_LINE_TO_REL, VG_LINE_TO_REL, VG_LINE_TO_REL, VG_CLOSE_PATH};

   /*
    * VGfloat coords[]   = {100.0f, 200.0f, 50.0f, -200.0f, -125.0f, 100.0f, 150.0f,   0.0f, -125.0f, -100.0f,
    *                        79.0f,  97.0f, 42.0f,    0.0f,    7.0f, -29.0f, -28.0f, -23.0f,  -28.0f,   23.0f};
    */
   VGfloat coords[]   = {200.0f, 400.0f, 100.0f, -400.0f, -250.0f, 200.0f, 300.0f,   0.0f, -250.0f, -200.0f,
      158.0f,  194.0f, 84.0f,    0.0f,    14.0f, -58.0f, -56.0f, -46.0f,  -56.0f,   46.0f};

   vgSetfv(VG_CLEAR_COLOR, 4, blue);
   vgClear(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

   vgLoadIdentity();
   /*
    * vgTranslate(20.0f, 20.0f);
    */
   vgTranslate(40.0f, 40.0f);

   /*
    * VGPath path = vgCreatePath(VG_PATH_FORMAT_STANDARD, VG_PATH_DATATYPE_F, 1.0f, 0.0f, 50, 50, VG_PATH_CAPABILITY_ALL);
    */
   VGPath path = vgCreatePath(VG_PATH_FORMAT_STANDARD, VG_PATH_DATATYPE_F, 1.1f, 0.0f, 50, 50, VG_PATH_CAPABILITY_ALL);
   vgAppendPathData(path, 12, segments, coords);

   VGImage image = vgCreateImage(VG_sRGB_565, 200, 260, VG_IMAGE_QUALITY_BETTER);
   vgImageSubData(image, cimg, 400, VG_sRGB_565, 0, 0, 200, 260);
   /*
    * VGImage image = vgCreateImage(VG_sRGB_565, 400, 520, VG_IMAGE_QUALITY_BETTER);
    * vgImageSubData(image, cimg, 400, VG_sRGB_565, 0, 0, 400, 520);
    */

   VGPaint fill = vgCreatePaint();
   vgSetPaint(fill, VG_FILL_PATH);
   vgPaintPattern (fill, image);
   vgSetParameterfv(fill, VG_PAINT_COLOR, 4, yellow);
   vgSetfv(VG_TILE_FILL_COLOR, 4, yellow);

   vgSeti(VG_MATRIX_MODE, VG_MATRIX_FILL_PAINT_TO_USER);
   vgLoadIdentity();
   /*
    * vgTranslate(90.0f, 65.0f);
    * vgScale(0.1f, 0.1f);
    */
   vgTranslate(195.0f, 130.0f);
   vgScale(0.4f, 0.4f);
   vgSetParameteri(fill, VG_PAINT_TYPE, VG_PAINT_TYPE_PATTERN);

   switch (tilingMode) {
   case FILL:
      vgSetParameteri(fill, VG_PAINT_PATTERN_TILING_MODE, VG_TILE_FILL);
      break;

   case PAD:
      vgSetParameteri(fill, VG_PAINT_PATTERN_TILING_MODE, VG_TILE_PAD);
      break;

   case REPEAT:
      vgSetParameteri(fill, VG_PAINT_PATTERN_TILING_MODE, VG_TILE_REPEAT);
      break;

   case REFLECT:
      vgSetParameteri(fill, VG_PAINT_PATTERN_TILING_MODE, VG_TILE_REFLECT);
      break;
   case NONE:
   default:
      break;
   }

   vgPaintPattern(fill, image);
   vgDrawPath(path, VG_FILL_PATH);

   vgDestroyImage(image);
   vgDestroyPath(path);
   vgDestroyPaint(fill);

}

const char help[] = "Press H for a list of commands";
const char commands[] =
   "Commands\n"
   "h - this help\n"
   "f - fill\n"
   "p - pad\n"
   "r - repeat\n"
   "t - reflect\n"
   "n - none\n"
   "q - quit\n";
void
key(unsigned char code, int x, int y)
{
   static int open = 0;
   switch (tolower(code)) {
   case 'f':
      tilingMode = FILL;
      break;
   case 'p':
      tilingMode = PAD;
      break;
   case 'r':
      tilingMode = REPEAT;
      break;
   case 't':
      tilingMode = REFLECT;
      break;
   case 'n':
      tilingMode = NONE;
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
   case 'q':
      exit(EXIT_SUCCESS);
   }
}

int
main(int argc, char *argv[])
{
   testInit(argc, argv, SCREEN_WIDTH, SCREEN_HEIGHT, "ShivaVG: Image tiling mode");
   testCallback(TEST_CALLBACK_DISPLAY, (CallbackFunc) display);
   testCallback(TEST_CALLBACK_KEY, (CallbackFunc) key);
   testOverlayColor(1.0f, 1.0f, 1.0f, 1.0f);
   testOverlayString(help);
   testRun();

   return EXIT_SUCCESS;

}
