#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#if defined(WIN32)
#define vsnprintf _vsnprintf
#include <windows.h>
#endif

#if defined(__APPLE__)
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <OpenGL/glext.h>
#include <GLUT/glut.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include "GL/glext.h"
#endif


#include <VG/openvg.h>
#include <VG/vgu.h>

/*
 * #ifdef DEBUG
 * #define SH_DEBUG(M, ...) fprintf(stderr, "DEBUG %s:%d: " M "\n", __FILE__, __LINE__, ##__VA_ARGS__)
 * #else
 * #define SH_DEBUG(M, ...)
 * #endif
 */

#define SH_DEBUG(M, ...) fprintf(stderr, "DEBUG %s:%s:%d: " M "\n", __FILE__, __func__, __LINE__, ##__VA_ARGS__)

typedef enum
{
   TEST_CALLBACK_DISPLAY = 0,
   TEST_CALLBACK_RESHAPE = 1,
   TEST_CALLBACK_KEY = 2,
   TEST_CALLBACK_SPECIALKEY = 3,
   TEST_CALLBACK_BUTTON = 4,
   TEST_CALLBACK_MOVE = 5,
   TEST_CALLBACK_DRAG = 6,
   TEST_CALLBACK_CLEANUP = 7
} TestCallbackType;

#define TEST_CALLBACK_COUNT 8

typedef void (*CallbackFunc) (void);
typedef void (*DisplayFunc) (float interval);
typedef void (*ReshapeFunc) (int w, int h);
typedef void (*KeyFunc) (unsigned char key, int x, int y);
typedef void (*SpecialKeyFunc) (int key, int x, int y);
typedef void (*ButtonFunc) (int button, int state, int x, int y);
typedef void (*MoveFunc) (int x, int y);
typedef void (*DragFunc) (int x, int y);
typedef void (*CleanupFunc) (void);

struct Image
{
   VGImage img;
   unsigned int width;
   unsigned int height;
};


struct Image testCreateImageFromJpeg(const char *filename);

VGPath testCreatePath(void);
void testMoveTo(VGPath p, float x, float y, VGPathAbsRel absrel);
void testLineTo(VGPath p, float x, float y, VGPathAbsRel absrel);
void testHlineTo(VGPath p, float x, VGPathAbsRel absrel);
void testVlineTo(VGPath p, float y, VGPathAbsRel absrel);

void testQuadTo(VGPath p, float x1, float y1, float x2, float y2,
                VGPathAbsRel absrel);

void testCubicTo(VGPath p, float x1, float y1, float x2, float y2, float x3,
                 float y3, VGPathAbsRel absrel);

void testSquadTo(VGPath p, float x2, float y2, VGPathAbsRel absrel);

void testScubicTo(VGPath p, float x2, float y2, float x3, float y3,
                  VGPathAbsRel absrel);

void testArcTo(VGPath p, float rx, float ry, float rot, float x, float y,
               VGPathSegment type, VGPathAbsRel absrel);

void testClosePath(VGPath p);

void testOverlayString(const char *format, ...);

void testOverlayColor(float r, float g, float b, float a);

void testCallback(TestCallbackType type, CallbackFunc func);

void testInit(int argc, char **argv, int w, int h, const char *title);

void testRun(void);

VGint testWidth(void);
VGint testHeight(void);

VGuint nextBlendMode(VGuint blendMode);
const char *blendModeStr(const VGint blendMode);
