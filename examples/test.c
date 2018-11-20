#include "test.h"
#include <jpeglib.h>
#include <alloca.h>

static int testW = 0;
static int testH = 0;

static int timeinit = 0;
static int lastdraw = 0;
static int lastfps = 0;
static unsigned int fps = 0;
static unsigned int fpsdraw = 0;
static char *overtext = NULL;
static float overcolor[4] = { 0, 0, 0, 1 };

static CallbackFunc callbacks[TEST_CALLBACK_COUNT];

VGPath
testCreatePath()
{
   return vgCreatePath(VG_PATH_FORMAT_STANDARD, VG_PATH_DATATYPE_F,
                       1, 0, 0, 0, VG_PATH_CAPABILITY_ALL);
}

void
testMoveTo(VGPath p, float x, float y, VGPathAbsRel absrel)
{
   VGubyte seg = VG_MOVE_TO | absrel;
   VGfloat data[2];

   data[0] = x;
   data[1] = y;
   vgAppendPathData(p, 1, &seg, data);
}

void
testLineTo(VGPath p, float x, float y, VGPathAbsRel absrel)
{
   VGubyte seg = VG_LINE_TO | absrel;
   VGfloat data[2];

   data[0] = x;
   data[1] = y;
   vgAppendPathData(p, 1, &seg, data);
}

void
testHlineTo(VGPath p, float x, VGPathAbsRel absrel)
{
   VGubyte seg = VG_HLINE_TO | absrel;
   VGfloat data = x;

   vgAppendPathData(p, 1, &seg, &data);
}

void
testVlineTo(VGPath p, float y, VGPathAbsRel absrel)
{
   VGubyte seg = VG_VLINE_TO | absrel;
   VGfloat data = y;

   vgAppendPathData(p, 1, &seg, &data);
}

void
testQuadTo(VGPath p, float x1, float y1, float x2, float y2,
           VGPathAbsRel absrel)
{
   VGubyte seg = VG_QUAD_TO | absrel;
   VGfloat data[4];

   data[0] = x1;
   data[1] = y1;
   data[2] = x2;
   data[3] = y2;
   vgAppendPathData(p, 1, &seg, data);
}

void
testCubicTo(VGPath p, float x1, float y1, float x2, float y2, float x3,
            float y3, VGPathAbsRel absrel)
{
   VGubyte seg = VG_CUBIC_TO | absrel;
   VGfloat data[6];

   data[0] = x1;
   data[1] = y1;
   data[2] = x2;
   data[3] = y2;
   data[4] = x3;
   data[5] = y3;
   vgAppendPathData(p, 1, &seg, data);
}

void
testSquadTo(VGPath p, float x2, float y2, VGPathAbsRel absrel)
{
   VGubyte seg = VG_SQUAD_TO | absrel;
   VGfloat data[2];

   data[0] = x2;
   data[1] = y2;
   vgAppendPathData(p, 1, &seg, data);
}

void
testScubicTo(VGPath p, float x2, float y2, float x3, float y3,
             VGPathAbsRel absrel)
{
   VGubyte seg = VG_SCUBIC_TO | absrel;
   VGfloat data[4];

   data[0] = x2;
   data[1] = y2;
   data[2] = x3;
   data[3] = y3;
   vgAppendPathData(p, 1, &seg, data);
}

void
testArcTo(VGPath p, float rx, float ry, float rot, float x, float y,
          VGPathSegment type, VGPathAbsRel absrel)
{
   VGubyte seg = type | absrel;
   VGfloat data[5];

   data[0] = rx;
   data[1] = ry;
   data[2] = rot;
   data[3] = x;
   data[4] = y;
   vgAppendPathData(p, 1, &seg, data);
}

void
testClosePath(VGPath p)
{
   VGubyte seg = VG_CLOSE_PATH;
   VGfloat data = 0.0f;
   vgAppendPathData(p, 1, &seg, &data);
}

void
testOverlayColor(float r, float g, float b, float a)
{
   overcolor[0] = r;
   overcolor[1] = g;
   overcolor[2] = b;
   overcolor[3] = a;
}

void
testOverlayString(const char *format, ...)
{
   int len;
   va_list ap;

   if (overtext) {
      free(overtext);
      overtext = NULL;
   }

   va_start(ap, format);
   len = vsnprintf(NULL, 0, format, ap);
   overtext = (char *) malloc(len + 1);
   if (!overtext) {
      va_end(ap);
      return;
   }
   vsnprintf(overtext, len + 1, format, ap);
   va_end(ap);
}

void
testDrawString(float x, float y, const char *format, ...)
{
   int i, len;
   va_list ap;
   char *text;
   float k = 0.15;

   x += 0.5f;
   y += 0.5f;

   va_start(ap, format);
   len = vsnprintf(NULL, 0, format, ap);
   va_end(ap);

   if (len < 0)
      return;
   len++;                       // for \0
   text = (char *) alloca(len);

   if (!text)
      return;

   va_start(ap, format);
   if (vsnprintf(text, len, format, ap) < 0) {
      va_end(ap);
      return;
   }
   va_end(ap);

   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glEnable(GL_LINE_SMOOTH);
   glDisable(GL_MULTISAMPLE);

   glMatrixMode(GL_MODELVIEW);
   glPushMatrix();
   glLoadIdentity();
   glTranslatef(x, y, 0);
   glScalef(k, k, k);
   glLineWidth(1.0f);


   for (i = 0; i < len; ++i) {
      if (text[i] == '\n') {
         y -= 20;
         glLoadIdentity();
         glTranslatef(x, y, 0);
         glScalef(k, k, k);
         continue;
      }
      glutStrokeCharacter(GLUT_STROKE_ROMAN, text[i]);
   }

   glPopMatrix();
   glDisable(GL_LINE_SMOOTH);
}

void
testAnimate(void)
{
   glutPostRedisplay();
}

void
testDisplay(void)
{
   int now;
   unsigned int msinterval;
   float interval;

   DisplayFunc callback = (DisplayFunc) callbacks[TEST_CALLBACK_DISPLAY];

   /* Get interval from last redraw */
   now = glutGet(GLUT_ELAPSED_TIME);
   if (!timeinit)
      lastdraw = now;
   msinterval = (unsigned int) (now - lastdraw);
   interval = (float) msinterval / 1000;
   lastdraw = now;

   /* Draw scene */
   if (callback)
      (*callback) (interval);

   /* Draw overlay text */
   if (overtext != NULL) {
      glColor4fv(overcolor);
      testDrawString(10, testHeight() - 25, overtext);
   }

   /* Draw fps */
   glColor4fv(overcolor);
   testDrawString(10, 10, "FPS: %d", fpsdraw);

   /* Swap */
   glutSwapBuffers();

   /* Count frames per second */
   ++fps;

   if (timeinit) {
      if (now - lastfps > 1000) {
         lastfps = now;
         fpsdraw = fps;
         fps = 0;
      }
   }
   else {
      lastfps = now;
      timeinit = 1;
   }

}

void
testDrag(int x, int y)
{
   DragFunc callback = (DragFunc) callbacks[TEST_CALLBACK_DRAG];

   if (callback)
      (*callback) (x, y);
}

void
testMove(int x, int y)
{
   MoveFunc callback = (MoveFunc) callbacks[TEST_CALLBACK_MOVE];

   if (callback)
      (*callback) (x, y);
}

void
testButton(int button, int state, int x, int y)
{
   ButtonFunc callback = (ButtonFunc) callbacks[TEST_CALLBACK_BUTTON];

   if (callback)
      (*callback) (button, state, x, y);
}

void
testKeyboard(unsigned char key, int x, int y)
{
   KeyFunc callback = (KeyFunc) callbacks[TEST_CALLBACK_KEY];

   if (key == 27)
      exit(EXIT_SUCCESS);

   if (callback)
      (*callback) (key, x, y);
}

void
testSpecialKeyboard(int key, int x, int y)
{
   SpecialKeyFunc callback =
      (SpecialKeyFunc) callbacks[TEST_CALLBACK_SPECIALKEY];

   if (callback)
      (*callback) (key, x, y);
}

void
testReshape(int w, int h)
{
   ReshapeFunc callback = (ReshapeFunc) callbacks[TEST_CALLBACK_RESHAPE];

   testW = w;
   testH = h;

   vgResizeSurfaceSH(w, h);


   if (callback)
      (*callback) (w, h);
}

void
testCleanup(void)
{
   CleanupFunc callback = (CleanupFunc) callbacks[TEST_CALLBACK_CLEANUP];

   if (callback)
      (*callback) ();

   vgDestroyContextSH();
   if (overtext)
      free(overtext);
}

void
testCallback(TestCallbackType type, CallbackFunc func)
{
   if (type < 0 || type > TEST_CALLBACK_COUNT - 1)
      return;

   callbacks[type] = func;
}

VGint
testWidth()
{
   return testW;
}

VGint
testHeight()
{
   return testH;
}

void
testInit(int argc, char **argv, int w, int h, const char *title)
{
   int i;
   glutInit(&argc, argv);

#if defined(__APPLE__) || defined(WIN32)
   /*glutInitDisplayString("rgba alpha double stencil samples=4"); */
   glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA |
                       GLUT_STENCIL | GLUT_MULTISAMPLE);
#else
   glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA |
                       GLUT_STENCIL | GLUT_MULTISAMPLE);
#endif

   glutInitWindowPosition(0, 0);
   glutInitWindowSize(w, h);
   glutCreateWindow(title);

   glutReshapeFunc(testReshape);
   glutDisplayFunc(testDisplay);
   glutIdleFunc(testAnimate);
   glutKeyboardFunc(testKeyboard);
   glutSpecialFunc(testSpecialKeyboard);
   glutMouseFunc(testButton);
   glutMotionFunc(testDrag);
   glutPassiveMotionFunc(testMove);
   atexit(testCleanup);

   vgCreateContextSH(w, h);

   testW = w;
   testH = h;

   for (i = 0; i < TEST_CALLBACK_COUNT; ++i)
      callbacks[i] = NULL;

}

void
testRun()
{
   glutMainLoop();
}


struct Image
testCreateImageFromJpeg(const char *filename)
{
   FILE *infile;
   struct jpeg_decompress_struct jdc;
   struct jpeg_error_mgr jerr;
   JSAMPARRAY buffer;
   unsigned int bstride;
   unsigned int bbpp;

   VGImage img;
   VGubyte *data;
   unsigned int width;
   unsigned int height;
   unsigned int dstride;
   unsigned int dbpp;

   VGubyte *brow;
   VGubyte *drow;
   unsigned int x;
   unsigned int lilEndianTest = 1;
   VGImageFormat rgbaFormat;

   struct Image image;

   /* Check for endianness */
   if (((unsigned char *) &lilEndianTest)[0] == 1) {
      rgbaFormat = VG_lABGR_8888;
      SH_DEBUG("testCreateImageFromJpeg: rgbaFormat VG_lABGR_8888\n");
   } else {
      rgbaFormat = VG_lRGBA_8888;
      SH_DEBUG("testCreateImageFromJpeg: rgbaFormat VG_lRGBA_8888\n");
   }

   /* Try to open image file */
   infile = fopen(filename, "rb");
   if (infile == NULL) {
      printf("Failed opening '%s' for reading!\n", filename);
      image.img = VG_INVALID_HANDLE;
      return image;
   }

   /* Setup default error handling */
   jdc.err = jpeg_std_error(&jerr);
   jpeg_create_decompress(&jdc);

   /* Set input file */
   jpeg_stdio_src(&jdc, infile);

   /* Read header and start */
   jpeg_read_header(&jdc, TRUE);
   jpeg_start_decompress(&jdc);
   width = jdc.output_width;
   height = jdc.output_height;

   /* Allocate buffer using jpeg allocator */
   bbpp = jdc.output_components;
   bstride = width * bbpp;
   buffer = (*jdc.mem->alloc_sarray)
      ((j_common_ptr) & jdc, JPOOL_IMAGE, bstride, 1);

   /* Allocate image data buffer */
   dbpp = 4;
   dstride = width * dbpp;
   data = (VGubyte *) malloc(dstride * height);

   /* Iterate until all scanlines processed */
   while (jdc.output_scanline < height) {

      /* Read scanline into buffer */
      jpeg_read_scanlines(&jdc, buffer, 1);
      drow = data + (height - jdc.output_scanline) * dstride;
      brow = buffer[0];

      /* Expand to RGBA */
      for (x = 0; x < width; ++x, drow += dbpp, brow += bbpp) {
         switch (bbpp) {
         case 4:
            drow[0] = brow[0];
            drow[1] = brow[1];
            drow[2] = brow[2];
            drow[3] = brow[3];
            break;
         case 3:
            drow[0] = brow[0];
            drow[1] = brow[1];
            drow[2] = brow[2];
            drow[3] = 255;
            break;
         }
      }
   }

   /* Create VG image */
   img = vgCreateImage(rgbaFormat, width, height, VG_IMAGE_QUALITY_BETTER);
   vgImageSubData(img, data, dstride, rgbaFormat, 0, 0, width, height);

   /* Cleanup */
   jpeg_destroy_decompress(&jdc);
   fclose(infile);
   free(data);

   image.img = img;
   image.width = width;
   image.height = height;
   return image;
}
