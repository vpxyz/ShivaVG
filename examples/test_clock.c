#include "test.h"
#include <VG/openvg.h>
#include <VG/vgu.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>
#include <sys/time.h>
#include <time.h>
#include <ctype.h>

bool openVGInitialized = false;
VGint windowWidth = 384;
VGint windowHeight = 384;
VGfloat windowScaleX, windowScaleY;
VGRenderingQuality quality = VG_RENDERING_QUALITY_BETTER;
VGint drawnFrames;


VGPath shadowPath, quadrantPath, innerBevelPath, outerBevelPath, secondTagPath, hourTagPath, msTagPath;
VGPath glassPath, secondCursorPath, hourCursorPath, minuteCursorPath, msCursorPath, cursorScrewPath;
VGPaint shadowPaint, quadrantPaint, bevelsPaint, msQuadrantPaint, colorPaint;


void genClockPaints(void)
{
   VGfloat colKeys[50];
   VGfloat radGrad[5];

   // shadow gradient
   colKeys[0] = 0.00f; colKeys[1] = 0.30f; colKeys[2] = 0.30f; colKeys[3] = 0.30f; colKeys[4] = 1.00f;
   colKeys[5] = 0.80f; colKeys[6] = 0.30f; colKeys[7] = 0.30f; colKeys[8] = 0.30f; colKeys[9] = 1.00f;
   colKeys[10] = 0.95f; colKeys[11] = 0.30f; colKeys[12] = 0.30f; colKeys[13] = 0.30f; colKeys[14] = 0.00f;
   colKeys[15] = 1.00f; colKeys[16] = 0.30f; colKeys[17] = 0.30f; colKeys[18] = 0.30f; colKeys[19] = 0.00f;
   radGrad[0] = 260.0f; radGrad[1] = 250.0f;
   radGrad[2] = 260.0f; radGrad[3] = 250.0f;
   radGrad[4] = 270.0f;
   shadowPaint = vgCreatePaint();
   vgSetParameteri(shadowPaint, VG_PAINT_TYPE, VG_PAINT_TYPE_RADIAL_GRADIENT);
   vgSetParameterfv(shadowPaint, VG_PAINT_COLOR_RAMP_STOPS, 20, colKeys);
   vgSetParameterfv(shadowPaint, VG_PAINT_RADIAL_GRADIENT, 5, radGrad);
   vgSetParameteri(shadowPaint, VG_PAINT_COLOR_RAMP_SPREAD_MODE, VG_COLOR_RAMP_SPREAD_PAD);

   // quadrant background gradient
   colKeys[0] = 0.00f; colKeys[1] = 0.89f; colKeys[2] = 0.90f; colKeys[3] = 0.91f; colKeys[4] = 1.00f;
   colKeys[5] = 0.55f; colKeys[6] = 0.76f; colKeys[7] = 0.77f; colKeys[8] = 0.78f; colKeys[9] = 1.00f;
   colKeys[10] = 1.00f; colKeys[11] = 0.36f; colKeys[12] = 0.36f; colKeys[13] = 0.40f; colKeys[14] = 1.00f;
   radGrad[0] = 236.0f; radGrad[1] = 276.0f;
   radGrad[2] = 235.0f; radGrad[3] = 275.0f;
   radGrad[4] = 256.0f;
   quadrantPaint = vgCreatePaint();
   vgSetParameteri(quadrantPaint, VG_PAINT_TYPE, VG_PAINT_TYPE_RADIAL_GRADIENT);
   vgSetParameterfv(quadrantPaint, VG_PAINT_COLOR_RAMP_STOPS, 15, colKeys);
   vgSetParameterfv(quadrantPaint, VG_PAINT_RADIAL_GRADIENT, 5, radGrad);
   vgSetParameteri(quadrantPaint, VG_PAINT_COLOR_RAMP_SPREAD_MODE, VG_COLOR_RAMP_SPREAD_PAD);

   // outer/inner bevel ring gradient
   colKeys[0] = 0.00f; colKeys[1] = 1.00f; colKeys[2] = 1.00f; colKeys[3] = 1.00f; colKeys[4] = 1.00f;
   colKeys[5] = 0.40f; colKeys[6] = 0.90f; colKeys[7] = 0.90f; colKeys[8] = 0.92f; colKeys[9] = 1.00f;
   colKeys[10] = 0.80f; colKeys[11] = 0.46f; colKeys[12] = 0.47f; colKeys[13] = 0.50f; colKeys[14] = 1.00f;
   colKeys[15] = 1.00f; colKeys[16] = 0.16f; colKeys[17] = 0.17f; colKeys[18] = 0.20f; colKeys[19] = 1.00f;
   radGrad[0] = 120.0f; radGrad[1] = 390.0f;
   radGrad[2] = -121.0f; radGrad[3] = 591.0f;
   radGrad[4] = 400.0f;
   bevelsPaint = vgCreatePaint();
   vgSetParameteri(bevelsPaint, VG_PAINT_TYPE, VG_PAINT_TYPE_RADIAL_GRADIENT);
   vgSetParameterfv(bevelsPaint, VG_PAINT_COLOR_RAMP_STOPS, 20, colKeys);
   vgSetParameterfv(bevelsPaint, VG_PAINT_RADIAL_GRADIENT, 5, radGrad);
   vgSetParameteri(bevelsPaint, VG_PAINT_COLOR_RAMP_SPREAD_MODE, VG_COLOR_RAMP_SPREAD_PAD);

   // color paint
   colorPaint = vgCreatePaint();
   vgSetParameteri(colorPaint, VG_PAINT_TYPE, VG_PAINT_TYPE_COLOR);
}

void genClockPaths(void)
{
   VGubyte pathSegs[128];
   VGfloat pathData[256];

   // shadow
   shadowPath = vgCreatePath(VG_PATH_FORMAT_STANDARD, VG_PATH_DATATYPE_F, 1.0f, 0.0f, 0, 0, VG_PATH_CAPABILITY_ALL);
   pathSegs[0] = VG_MOVE_TO_ABS;
   pathData[0] = 0.0f; pathData[1] = 242.0f;
   pathSegs[1] = VG_LCWARC_TO_ABS;
   pathData[2] = 270.0f; pathData[3] = 270.0f; pathData[4] = 0.0f; pathData[5] = 540.0f; pathData[6] = 242.0f;
   pathSegs[2] = VG_LCWARC_TO_ABS;
   pathData[7] = 270.0f; pathData[8] = 270.0f; pathData[9] = 0.0f; pathData[10] = 0.0f; pathData[11] = 242.0f;
   pathSegs[3] = VG_CLOSE_PATH;
   vgAppendPathData(shadowPath, 4, pathSegs, pathData);

   // quadrant
   quadrantPath = vgCreatePath(VG_PATH_FORMAT_STANDARD, VG_PATH_DATATYPE_F, 1.0f, 0.0f, 0, 0, VG_PATH_CAPABILITY_ALL);
   pathSegs[0] = VG_MOVE_TO_ABS;
   pathData[0] = 16.0f; pathData[1] = 256.0f;
   pathSegs[1] = VG_LCWARC_TO_ABS;
   pathData[2] = 240.0f; pathData[3] = 240.0f; pathData[4] = 0.0f; pathData[5] = 496.0f; pathData[6] = 256.0f;
   pathSegs[2] = VG_LCWARC_TO_ABS;
   pathData[7] = 240.0f; pathData[8] = 240.0f; pathData[9] = 0.0f; pathData[10] = 16.0f; pathData[11] = 256.0f;
   pathSegs[3] = VG_CLOSE_PATH;
   vgAppendPathData(quadrantPath, 4, pathSegs, pathData);

   // inner bevel ring
   innerBevelPath = vgCreatePath(VG_PATH_FORMAT_STANDARD, VG_PATH_DATATYPE_F, 1.0f, 0.0f, 0, 0, VG_PATH_CAPABILITY_ALL);
   pathSegs[0] = VG_MOVE_TO_ABS;
   pathData[0] = 48.0f; pathData[1] = 256.1f;
   pathSegs[1] = VG_LCWARC_TO_ABS;
   pathData[2] = 208.0f; pathData[3] = 208.0f; pathData[4] = 0.0f; pathData[5] = 464.0f; pathData[6] = 256.0f;
   pathSegs[2] = VG_LCWARC_TO_ABS;
   pathData[7] = 208.0f; pathData[8] = 208.0f; pathData[9] = 0.0f; pathData[10] = 48.0f; pathData[11] = 255.9f;
   pathSegs[3] = VG_CLOSE_PATH;
   vgAppendPathData(innerBevelPath, 4, pathSegs, pathData);

   // outer bevel ring
   outerBevelPath = vgCreatePath(VG_PATH_FORMAT_STANDARD, VG_PATH_DATATYPE_F, 1.0f, 0.0f, 0, 0, VG_PATH_CAPABILITY_ALL);
   pathSegs[0] = VG_MOVE_TO_ABS;
   pathData[0] = 26.0f; pathData[1] = 256.1f;
   pathSegs[1] = VG_LCWARC_TO_ABS;
   pathData[2] = 230.0f; pathData[3] = 230.0f; pathData[4] = 0.0f; pathData[5] = 486.0f; pathData[6] = 256.0f;
   pathSegs[2] = VG_LCWARC_TO_ABS;
   pathData[7] = 230.0f; pathData[8] = 230.0f; pathData[9] = 0.0f; pathData[10] = 26.0f; pathData[11] = 255.9f;
   pathSegs[3] = VG_CLOSE_PATH;
   vgAppendPathData(outerBevelPath, 4, pathSegs, pathData);

   // seconds tag
   secondTagPath = vgCreatePath(VG_PATH_FORMAT_STANDARD, VG_PATH_DATATYPE_F, 1.0f, 0.0f, 0, 0, VG_PATH_CAPABILITY_ALL);
   pathSegs[0] = VG_MOVE_TO_ABS;
   pathData[0] = 256.0f; pathData[1] = 442.0f;
   pathSegs[1] = VG_VLINE_TO_ABS;
   pathData[2] = 446.0f;
   vgAppendPathData(secondTagPath, 2, pathSegs, pathData);

   // hours tag
   hourTagPath = vgCreatePath(VG_PATH_FORMAT_STANDARD, VG_PATH_DATATYPE_F, 1.0f, 0.0f, 0, 0, VG_PATH_CAPABILITY_ALL);
   pathSegs[0] = VG_MOVE_TO_ABS;
   pathData[0] = 256.0f; pathData[1] = 430.0f;
   pathSegs[1] = VG_VLINE_TO_ABS;
   pathData[2] = 446.0f;
   vgAppendPathData(hourTagPath, 2, pathSegs, pathData);

   // 1000 / 12 milliseconds tag
   msTagPath = vgCreatePath(VG_PATH_FORMAT_STANDARD, VG_PATH_DATATYPE_F, 1.0f, 0.0f, 0, 0, VG_PATH_CAPABILITY_ALL);
   pathSegs[0] = VG_MOVE_TO_ABS;
   pathData[0] = 356.0f; pathData[1] = 274.0f;
   pathSegs[1] = VG_VLINE_TO_ABS;
   pathData[2] = 276.0f;
   vgAppendPathData(msTagPath, 2, pathSegs, pathData);

   // glass
   glassPath = vgCreatePath(VG_PATH_FORMAT_STANDARD, VG_PATH_DATATYPE_F, 1.0f, 0.0f, 0, 0, VG_PATH_CAPABILITY_ALL);
   pathSegs[0] = VG_MOVE_TO_ABS;
   pathData[0] = 162.0f; pathData[1] = 447.0f;
   pathSegs[1] = VG_CUBIC_TO_ABS;
   pathData[2] = 365.0f; pathData[3] = 540.0f; pathData[4] = 564.0f; pathData[5] = 309.0f; pathData[6] = 419.0f; pathData[7] = 118.0f;
   pathSegs[2] = VG_QUAD_TO_ABS;
   pathData[8] = 400.0f; pathData[9] = 350.0f; pathData[10] = 162.0f; pathData[11] = 447.0f;

   pathSegs[3] = VG_MOVE_TO_ABS;
   pathData[12] = 256.0f; pathData[13] = 43.0f;
   pathSegs[4] = VG_CUBIC_TO_ABS;
   pathData[14] = 83.0f; pathData[15] = 45.0f; pathData[16] = 4.0f; pathData[17] = 231.0f; pathData[18] = 62.0f; pathData[19] = 340.0f;
   pathSegs[5] = VG_QUAD_TO_ABS;
   pathData[20] = 80.0f; pathData[21] = 110.0f; pathData[22] = 256.0f; pathData[23] = 43.0f;
   vgAppendPathData(glassPath, 6, pathSegs, pathData);

   // milliseconds	cursor
   msCursorPath = vgCreatePath(VG_PATH_FORMAT_STANDARD, VG_PATH_DATATYPE_F, 1.0f, 0.0f, 0, 0, VG_PATH_CAPABILITY_ALL);
   pathSegs[0] = VG_MOVE_TO_ABS;
   pathData[0] = 355.5f; pathData[1] = 250.0f;
   pathSegs[1] = VG_VLINE_TO_ABS;
   pathData[2] = 253.5f;
   pathSegs[2] = VG_LCWARC_TO_ABS;
   pathData[3] = 2.0f; pathData[4] = 2.5f; pathData[5] = 0.0f; pathData[6] = 355.5f; pathData[7] = 258.5f;
   pathSegs[3] = VG_LINE_TO_ABS;
   pathData[8] = 356.0f; pathData[9] = 270.0f;
   pathSegs[4] = VG_LINE_TO_ABS;
   pathData[10] = 356.5f; pathData[11] = 258.5f;
   pathSegs[5] = VG_LCWARC_TO_ABS;
   pathData[12] = 2.0f; pathData[13] = 2.5f; pathData[14] = 0.0f; pathData[15] = 356.5f; pathData[16] = 253.5f;
   pathSegs[6] = VG_VLINE_TO_ABS;
   pathData[17] = 250.0f;
   pathSegs[7] = VG_HLINE_TO_ABS;
   pathData[18] = 355.5f;
   pathSegs[8] = VG_MOVE_TO_ABS;
   pathData[19] = 356.0f; pathData[20] = 254.5f;
   pathSegs[9] = VG_LCWARC_TO_ABS;
   pathData[21] = 1.5f; pathData[22] = 1.5f; pathData[23] = 0.0f; pathData[24] = 356.0f; pathData[25] = 257.5f;
   pathSegs[10] = VG_LCWARC_TO_ABS;
   pathData[26] = 1.5f; pathData[27] = 1.5f; pathData[28] = 0.0f; pathData[29] = 356.0f; pathData[30] = 254.5f;
   vgAppendPathData(msCursorPath, 11, pathSegs, pathData);

   // minute cursor
   minuteCursorPath = vgCreatePath(VG_PATH_FORMAT_STANDARD, VG_PATH_DATATYPE_F, 1.0f, 0.0f, 0, 0, VG_PATH_CAPABILITY_ALL);
   pathSegs[0] = VG_MOVE_TO_ABS;
   pathData[0] = 253.0f; pathData[1] = 218.0f;
   pathSegs[1] = VG_LINE_TO_ABS;
   pathData[2] = 254.0f; pathData[3] = 410.0f;
   pathSegs[2] = VG_LINE_TO_ABS;
   pathData[4] = 258.0f; pathData[5] = 410.0f;
   pathSegs[3] = VG_LINE_TO_ABS;
   pathData[6] = 259.0f; pathData[7] = 218.0f;
   pathSegs[4] = VG_CLOSE_PATH;
   vgAppendPathData(minuteCursorPath, 5, pathSegs, pathData);

   // hour cursor
   hourCursorPath = vgCreatePath(VG_PATH_FORMAT_STANDARD, VG_PATH_DATATYPE_F, 1.0f, 0.0f, 0, 0, VG_PATH_CAPABILITY_ALL);
   pathSegs[0] = VG_MOVE_TO_ABS;
   pathData[0] = 251.0f; pathData[1] = 228.0f;
   pathSegs[1] = VG_LINE_TO_ABS;
   pathData[2] = 252.0f; pathData[3] = 370.0f;
   pathSegs[2] = VG_LINE_TO_ABS;
   pathData[4] = 260.0f; pathData[5] = 370.0f;
   pathSegs[3] = VG_LINE_TO_ABS;
   pathData[6] = 261.0f; pathData[7] = 228.0f;
   pathSegs[4] = VG_CLOSE_PATH;
   vgAppendPathData(hourCursorPath, 5, pathSegs, pathData);

   // seconds
   secondCursorPath = vgCreatePath(VG_PATH_FORMAT_STANDARD, VG_PATH_DATATYPE_F, 1.0f, 0.0f, 0, 0, VG_PATH_CAPABILITY_ALL);
   pathSegs[0] = VG_MOVE_TO_ABS;
   pathData[0] = 254.0f; pathData[1] = 210.0f;
   pathSegs[1] = VG_VLINE_TO_ABS;
   pathData[2] = 247.0f;
   pathSegs[2] = VG_LCWARC_TO_ABS;
   pathData[3] = 7.75f; pathData[4] = 9.0f; pathData[5] = 0.0f; pathData[6] = 254.5f; pathData[7] = 265.0f;
   pathSegs[3] = VG_VLINE_TO_ABS;
   pathData[8] = 394.5f;
   pathSegs[4] = VG_LCWARC_TO_ABS;
   pathData[9] = 5.0f; pathData[10] = 6.0f; pathData[11] = 0.0f; pathData[12] = 255.0f; pathData[13] = 407.5f;
   pathSegs[5] = VG_LINE_TO_ABS;
   pathData[14] = 256.0f; pathData[15] = 444.0f;
   pathSegs[6] = VG_LINE_TO_ABS;
   pathData[16] = 257.0f; pathData[17] = 407.5f;
   pathSegs[7] = VG_LCWARC_TO_ABS;
   pathData[18] = 5.0f; pathData[19] = 6.0f; pathData[20] = 0.0f; pathData[21] = 257.5f; pathData[22] = 394.5f;
   pathSegs[8] = VG_VLINE_TO_ABS;
   pathData[23] = 265.0f;
   pathSegs[9] = VG_LCWARC_TO_ABS;
   pathData[24] = 7.75f; pathData[25] = 9.0f; pathData[26] = 0.0f; pathData[27] = 258.0f; pathData[28] = 247.0f;
   pathSegs[10] = VG_VLINE_TO_ABS;
   pathData[29] = 210.0f;
   pathSegs[11] = VG_CLOSE_PATH;
   pathSegs[12] = VG_MOVE_TO_ABS;
   pathData[30] = 256.0f; pathData[31] = 249.5f;
   pathSegs[13] = VG_LCWARC_TO_ABS;
   pathData[32] = 7.0f; pathData[33] = 6.5f; pathData[34] = 0.0f; pathData[35] = 256.0f; pathData[36] = 262.5f;
   pathSegs[14] = VG_LCWARC_TO_ABS;
   pathData[37] = 7.0f; pathData[38] = 6.5f; pathData[39] = 0.0f; pathData[40] = 256.0f; pathData[41] = 249.5f;
   pathSegs[15] = VG_MOVE_TO_ABS;
   pathData[42] = 256.0f; pathData[43] = 396.25f;
   pathSegs[16] = VG_LCWARC_TO_ABS;
   pathData[44] = 4.75f; pathData[45] = 4.75f; pathData[46] = 0.0f; pathData[47] = 256.0f; pathData[48] = 405.75f;
   pathSegs[17] = VG_LCWARC_TO_ABS;
   pathData[49] = 4.75f; pathData[50] = 4.75f; pathData[51] = 0.0f; pathData[52] = 256.0f; pathData[53] = 396.25f;
   vgAppendPathData(secondCursorPath, 18, pathSegs, pathData);

   // cursors screw
   cursorScrewPath = vgCreatePath(VG_PATH_FORMAT_STANDARD, VG_PATH_DATATYPE_F, 1.0f, 0.0f, 0, 0, VG_PATH_CAPABILITY_ALL);
   pathSegs[0] = VG_MOVE_TO_ABS;
   pathData[0] = 249.0f; pathData[1] = 256.0f;
   pathSegs[1] = VG_LCWARC_TO_ABS;
   pathData[2] = 7.0f; pathData[3] = 7.0f; pathData[4] = 0.0f; pathData[5] = 263.0f; pathData[6] = 256.0f;
   pathSegs[2] = VG_LCWARC_TO_ABS;
   pathData[7] = 7.0f; pathData[8] = 7.0f; pathData[9] = 0.0f; pathData[10] = 249.0f; pathData[11] = 256.0f;
   pathSegs[3] = VG_CLOSE_PATH;
   vgAppendPathData(cursorScrewPath, 4, pathSegs, pathData);
}

void drawCursors(const VGuint hours, const VGuint minutes, const VGuint seconds, const VGuint milliseconds)
{
   VGfloat hh, mm, ss, ks, col[4];

   if (hours < 13)
      hh = 360.0f - ( ((VGfloat)hours / 12.0f + (VGfloat)minutes / 720.0f) * 360.0f);
   else
      hh = 360.0f - ( ((VGfloat)(hours - 12.0f) / 12.0f + (VGfloat)minutes / 720.0f) * 360.0f);

   mm = 360.0f - (((VGfloat)minutes / 60.0f + (VGfloat)seconds / 3600.0f) * 360.0f);
   ss = 360.0f - (((VGfloat)seconds / 60.0f + (VGfloat)milliseconds / 60000.0f) * 360.0f);
   ks = 360.0f - (((VGfloat)milliseconds / 1000.0f) * 360.0f);

   // milliseconds
   col[0] = 0.0f; col[1] = 0.2f; col[2] = 0.6f; col[3] = 1.0f;
   vgSetParameterfv(colorPaint, VG_PAINT_COLOR, 4, col);
   vgLoadIdentity();
   vgScale(windowScaleX, windowScaleY);
   vgTranslate(356.0f, 256.0f);
   vgRotate(ks);
   vgTranslate(-356.0f, -256.0f);
   vgDrawPath(msCursorPath, VG_FILL_PATH);

   // seconds shadow
   col[0] = 0.3f; col[1] = 0.3f; col[2] = 0.3f; col[3] = 0.25f;
   vgSetParameterfv(colorPaint, VG_PAINT_COLOR, 4, col);
   vgLoadIdentity();
   vgScale(windowScaleX, windowScaleY);
   vgTranslate(255.0f, 257.0f);
   vgRotate(ss);
   vgScale(1.0f , 1.0f);
   vgTranslate(-255.0f, -257.0f);
   vgDrawPath(secondCursorPath, VG_FILL_PATH);

   // minutes shadow
   vgLoadIdentity();
   vgScale(windowScaleX, windowScaleY);
   vgTranslate(255.5f, 256.5f);
   vgRotate(mm);
   vgScale(1.25f , 1.01f);
   vgTranslate(-255.5f, -256.5f);
   vgDrawPath(minuteCursorPath, VG_FILL_PATH);

   // hours shadow
   vgSetPaint(colorPaint, VG_FILL_PATH | VG_STROKE_PATH);

   vgLoadIdentity();
   vgScale(windowScaleX, windowScaleY);
   vgTranslate(255.5f, 256.5f);
   vgRotate(hh);
   vgScale(1.25f , 1.01f);
   vgTranslate(-255.5f, -256.5f);
   vgDrawPath(hourCursorPath, VG_FILL_PATH);


   col[0] = 0.20f; col[1] = 0.20f; col[2] = 0.22f; col[3] = 1.0f;
   vgSetParameterfv(colorPaint, VG_PAINT_COLOR, 4, col);
   // hours
   vgLoadIdentity();
   vgScale(windowScaleX, windowScaleY);
   vgTranslate(256.0f, 256.0f);
   vgRotate(hh);
   vgTranslate(-256.0f, -256.0f);
   vgDrawPath(hourCursorPath, VG_FILL_PATH);

   // draw screw
   vgLoadIdentity();
   vgScale(windowScaleX, windowScaleY);
   vgDrawPath(cursorScrewPath, VG_FILL_PATH);

   // minutes
   vgLoadIdentity();
   vgScale(windowScaleX, windowScaleY);
   vgTranslate(256.0f, 256.0f);
   vgRotate(mm);
   vgTranslate(-256.0f, -256.0f);
   vgDrawPath(minuteCursorPath, VG_FILL_PATH);

   // seconds
   col[0] = 0.65f; col[1] = 0.1f; col[2] = 0.075f; col[3] = 1.0f;
   vgSetParameterfv(colorPaint, VG_PAINT_COLOR, 4, col);
   vgLoadIdentity();
   vgScale(windowScaleX, windowScaleY);
   vgTranslate(256.0f, 256.0f);
   vgRotate(ss);
   vgTranslate(-256.0f, -256.0f);
   vgDrawPath(secondCursorPath, VG_FILL_PATH);
}

void display(void)
{

   VGfloat clearColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
   VGfloat col[4], rot;

   vgSeti(VG_RENDERING_QUALITY, quality);
   vgSetfv(VG_CLEAR_COLOR, 4, clearColor);
   vgSetf(VG_STROKE_CAP_STYLE, VG_CAP_BUTT);
   vgClear(0, 0, windowWidth, windowHeight);
   vgSeti(VG_BLEND_MODE, VG_BLEND_SRC_OVER);

   vgSeti(VG_MATRIX_MODE, VG_MATRIX_PATH_USER_TO_SURFACE);
   vgLoadIdentity();
   vgScale(windowScaleX, windowScaleY);

   vgSeti(VG_MATRIX_MODE, VG_MATRIX_FILL_PAINT_TO_USER);
   vgLoadIdentity();
   vgSeti(VG_MATRIX_MODE, VG_MATRIX_STROKE_PAINT_TO_USER);
   vgLoadIdentity();

   // draw background
   vgSetPaint(shadowPaint, VG_FILL_PATH);
   vgDrawPath(shadowPath, VG_FILL_PATH);

   // draw quadrant
   vgSetPaint(quadrantPaint, VG_FILL_PATH);
   vgDrawPath(quadrantPath, VG_FILL_PATH);

   // draw inner bevel
   vgSetf(VG_STROKE_LINE_WIDTH, 10.0f);
   vgSetPaint(bevelsPaint, VG_STROKE_PATH);
   vgDrawPath(innerBevelPath, VG_STROKE_PATH);

   // draw outer bevel
   vgSeti(VG_MATRIX_MODE, VG_MATRIX_STROKE_PAINT_TO_USER);
   vgLoadIdentity();
   vgTranslate(256.0f, 256.0f);
   vgRotate(180.0f);
   vgTranslate(-256.0f, -256.0f);
   vgSetf(VG_STROKE_LINE_WIDTH, 35.0f);
   vgDrawPath(outerBevelPath, VG_STROKE_PATH);

   // draw seconds tags
   col[0] = 0.11f; col[1] = 0.12f; col[2] = 0.13f; col[3] = 0.65f;
   vgSetf(VG_STROKE_LINE_WIDTH, 2.0f);
   vgSetf(VG_STROKE_CAP_STYLE, VG_CAP_ROUND);
   vgSetParameterfv(colorPaint, VG_PAINT_COLOR, 4, col);
   vgSetPaint(colorPaint, VG_FILL_PATH | VG_STROKE_PATH);

   vgSeti(VG_MATRIX_MODE, VG_MATRIX_PATH_USER_TO_SURFACE);
   for (VGint i = 0; i < 60; ++i)
   {
      if ((i % 5) != 0)
      {
         vgLoadIdentity();
         vgScale(windowScaleX, windowScaleY);
         vgTranslate(256.0f, 256.0f);
         vgRotate(360.0f - (((VGfloat)i / 60.0f) * 360.0f));
         vgTranslate(-256.0f, -256.0f);
         vgDrawPath(secondTagPath, VG_STROKE_PATH);
      }
   }

   // draw hours and milliseconds tags
   for (VGint i = 0; i < 12; ++i)
   {
      rot = 360.0f - (((VGfloat)i / 12.0f) * 360.0f);
      vgSetf(VG_STROKE_LINE_WIDTH, 7.0f);
      vgLoadIdentity();
      vgScale(windowScaleX, windowScaleY);
      vgTranslate(256.0f, 256.0f);
      vgRotate(rot);
      vgTranslate(-256.0f, -256.0f);
      vgDrawPath(hourTagPath, VG_STROKE_PATH);
      vgSetf(VG_STROKE_LINE_WIDTH, 2.0f);
      vgLoadIdentity();
      vgScale(windowScaleX, windowScaleY);
      vgTranslate(356.0f, 256.0f);
      vgRotate(rot);
      vgTranslate(-356.0f, -256.0f);
      vgDrawPath(msTagPath, VG_STROKE_PATH);
   }

   // draw cursors
   struct timeval tv;
   gettimeofday(&tv, NULL);
   struct tm *ptm = localtime(&tv.tv_sec);

   drawCursors(ptm->tm_hour, ptm->tm_min, ptm->tm_sec, tv.tv_usec / 1000);

   // draw glass
   vgSeti(VG_BLEND_MODE, VG_BLEND_ADDITIVE);
   col[0] = 0.04f; col[1] = 0.045f; col[2] = 0.05f; col[3] = 0.8f;
   vgSetParameterfv(colorPaint, VG_PAINT_COLOR, 4, col);
   vgLoadIdentity();
   vgScale(windowScaleX, windowScaleY);
   vgDrawPath(glassPath, VG_FILL_PATH);
}

void
reshape(int x, int y)
{
   windowWidth = x;
   windowHeight = y;
   windowScaleX = (VGfloat)windowWidth / 512.0f;
   windowScaleY = (VGfloat)windowHeight / 512.0f;
   drawnFrames = 0;
   genClockPaths();
   genClockPaints();
   vgClear(0, 0, x, y);
}


void initApp(void)
{
   windowScaleX = (VGfloat)windowWidth / 512.0f;
   windowScaleY = (VGfloat)windowHeight / 512.0f;
   drawnFrames = 0;
   genClockPaths();
   genClockPaints();

}

const char commands[] =
   "Commands\n"
   "h - this help\n"
   "r - change display render quality\n"
   "q - quit";

const char help[] = "Press H for a list of commands";

void key(unsigned char code, int x, int y)
{
   static int open = 0;
   switch (tolower(code)) {
   case 'q':
      exit(EXIT_SUCCESS);
      break;
   case 'r':
      if (quality == VG_RENDERING_QUALITY_FASTER)
         quality = VG_RENDERING_QUALITY_BETTER;
      else
         quality = VG_RENDERING_QUALITY_FASTER;
      break;
   case 'h':
      if (!open) {
         testOverlayString(commands);
         open = 1;
      } else {
         testOverlayString(help);
         open = 0;
      }
   }
   glutPostRedisplay();
}


int main(int argc, char *argv[])
{

   testInit(argc, argv, windowWidth, windowHeight , "ShivaVG: clock example");
   initApp();

   testCallback(TEST_CALLBACK_DISPLAY, (CallbackFunc) display);
   testCallback(TEST_CALLBACK_KEY, (CallbackFunc) key);
   testCallback(TEST_CALLBACK_RESHAPE, (CallbackFunc) reshape);
   testOverlayString(help);
   testOverlayColor(0.0, 0.0, 0.0, 1.0);
   testRun();

   return EXIT_SUCCESS;

}
