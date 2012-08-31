#include "test.h"

VGPath backdrop;
VGPaint backdrop_p;
VGImage kirpicz;
VGPath wall;
VGPaint wall_p;

void display(float phase)
{
  VGfloat cc[] = {0,0,0,1};
  
  vgSetfv(VG_CLEAR_COLOR, 4, cc);
  vgClear(0,0,testWidth(),testHeight());

  /* Poor man's flip */
  vgLoadIdentity();
  vgScale(1,-1);
  vgTranslate(0,-512);
  
//   drawKirpicz();
 
  
  vgSetPaint(backdrop_p, VG_FILL_PATH);
  vgDrawPath(backdrop,VG_FILL_PATH);
  vgSetPaint(wall_p, VG_FILL_PATH);
  vgDrawPath(wall,VG_FILL_PATH);
}

void createPaths()
{
  /* Very silly, I know */
  VGubyte segsbk[]= { 
    VG_MOVE_TO_ABS, VG_LINE_TO_REL, VG_LINE_TO_REL, VG_LINE_TO_REL, VG_CLOSE_PATH };
  VGfloat databk[]= {
    0,0, 832,0, 0,512, -832,0  };
  if (!backdrop)
    backdrop=testCreatePath();
  vgAppendPathData(backdrop,sizeof(segsbk),segsbk,databk);
  
  VGubyte segwls[]= {
    VG_MOVE_TO_ABS, VG_LINE_TO_REL, VG_LINE_TO_REL, VG_LINE_TO_REL, VG_LINE_TO_REL, VG_LINE_TO_REL, VG_CLOSE_PATH,
    VG_MOVE_TO_ABS, VG_LINE_TO_REL, VG_LINE_TO_REL, VG_LINE_TO_REL, VG_LINE_TO_REL, VG_LINE_TO_REL, VG_LINE_TO_REL, VG_LINE_TO_REL, VG_CLOSE_PATH };
    
  VGfloat datawls[] = {
    512,128, 320,0, 0,192, -128,0, 0,-64, -192,0,
    0,128, 128,0, 0,256, 576,0, 0,64, 128,0, 0,64, -832,0 };
  
  if (!wall)
    wall=testCreatePath();
  vgAppendPathData(wall,sizeof(segwls),segwls,datawls);
}

VGPath getRectangle(float x, float y, float w, float h)
{
  VGPath ret=testCreatePath();
  VGubyte segs[]= { VG_MOVE_TO_ABS, VG_LINE_TO_REL, VG_LINE_TO_REL, VG_LINE_TO_REL, VG_CLOSE_PATH };
  VGfloat data[]= { x,y, w,0, 0,h, -w,0 };
  vgAppendPathData(ret,sizeof(segs),segs,data);
  return ret;
}

void drawKirpicz()
{
  VGfloat dark[] = { 0.1411765, 0.2666667, 0.3294118, 1.0 };
  VGfloat border[] = { 0.1686275, 0.5019608, 0.5647059, 0.4745098 };
  VGfloat light[] = { 0.1254902, 0.6470588, 0.4784314, 0.2039216 };
  VGPaint back=vgCreatePaint();
  VGPaint stroke=vgCreatePaint();
  vgSetParameterfv(back, VG_PAINT_COLOR, 4, dark);
  
  VGPath path=getRectangle(0,0,64,64);
  vgSetPaint(back, VG_FILL_PATH);
  vgDrawPath(path, VG_FILL_PATH);
  vgDestroyPath(path);
  
  vgSetParameterfv(back, VG_PAINT_COLOR, 4, light);
  vgSetParameterfv(stroke, VG_PAINT_COLOR, 4, border); 
  vgSetPaint(stroke, VG_STROKE_PATH);
  path=getRectangle(2.459,43.874,36.831,17.326);
  vgSetf(VG_STROKE_LINE_WIDTH, 2.825);
  vgDrawPath(path, VG_FILL_PATH); vgDrawPath(path, VG_STROKE_PATH); vgDestroyPath(path);
  
  path=getRectangle(43.598,24.212,16.250,35.305);
  vgSetf(VG_STROKE_LINE_WIDTH, 2.679);
  vgDrawPath(path, VG_FILL_PATH); vgDrawPath(path, VG_STROKE_PATH); vgDestroyPath(path);
  
  path=getRectangle(3.641,4.286,16.250,35.305);
  vgSetf(VG_STROKE_LINE_WIDTH, 2.679);
  vgDrawPath(path, VG_FILL_PATH); vgDrawPath(path, VG_STROKE_PATH); vgDestroyPath(path);
  
  path=getRectangle(23.859,2.912,37.539,17.694);
  vgSetf(VG_STROKE_LINE_WIDTH, 2.882);
  vgDrawPath(path, VG_FILL_PATH); vgDrawPath(path, VG_STROKE_PATH); vgDestroyPath(path);
  
  path=getRectangle(23.572,23.753,16.932,16.932);
  vgSetf(VG_STROKE_LINE_WIDTH, 1.938);
  vgDrawPath(path, VG_FILL_PATH); vgDrawPath(path, VG_STROKE_PATH); vgDestroyPath(path);
}

void createTextures()
{
  VGfloat lightblue[] = {0,0.4,0.8,1};
  if (!wall_p)
    wall_p = vgCreatePaint();
  vgSetParameterfv(wall_p, VG_PAINT_COLOR, 4, lightblue);
  
  if (!backdrop_p)
    backdrop_p = vgCreatePaint();
  vgSetParameteri(backdrop_p, VG_PAINT_TYPE, VG_PAINT_TYPE_PATTERN);
  vgSetParameteri(backdrop_p, VG_PAINT_PATTERN_TILING_MODE, VG_TILE_REPEAT);
  
  if (!kirpicz)
    kirpicz=vgCreateImage(VG_lRGBA_8888,64,64,VG_IMAGE_QUALITY_BETTER);
  /* Rendering takes place */
  VGfloat cc[] = {0,0,0,1};
  
  vgSetfv(VG_CLEAR_COLOR, 4, cc);
  vgClear(0,0,testWidth(),testHeight());
  
  drawKirpicz();
  vgGetPixels(kirpicz,0,0,0,0,64,64);
  
  vgPaintPattern(backdrop_p,kirpicz);
//   vgClear(0,0,testWidth(),testHeight());
}

int main(int argc,char argv[])
{
  testInit(argc, argv, 832,512, "ShivaVG: Electro Body scene test");
  testCallback(TEST_CALLBACK_DISPLAY, (CallbackFunc)display);
  createPaths();
  createTextures();

  testRun();
  return EXIT_SUCCESS;
}
