#include "test.h"

// FIXME unsafe
#define min(x,y) ((x<y)?(x):(y))

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

  /* Detecting the aspect ratio */
  float ratio=min(testWidth()/13,testHeight()/8);
  
  /* Poor man's flip */
  vgSeti(VG_MATRIX_MODE, VG_MATRIX_FILL_PAINT_TO_USER);
  vgLoadIdentity();
  vgScale(1.0/ratio,1.0/ratio);
  
  vgSeti(VG_MATRIX_MODE, VG_MATRIX_PATH_USER_TO_SURFACE);
  vgLoadIdentity();
  vgScale(ratio,-ratio);
  vgTranslate(0,-8); /* TODO center */
  
  
  vgSetPaint(backdrop_p, VG_FILL_PATH);
  vgDrawPath(backdrop,VG_FILL_PATH);
  vgSetPaint(wall_p, VG_FILL_PATH);
  vgDrawPath(wall,VG_FILL_PATH);
}

void reshapeFunc(int x,int y)
{
  createPaths();
  createTextures();
}

void createPaths()
{
  /* Very silly, I know */
  VGubyte segsbk[]= { 
    VG_MOVE_TO_ABS, VG_LINE_TO_REL, VG_LINE_TO_REL, VG_LINE_TO_REL, VG_CLOSE_PATH };
  VGfloat databk[]= {
    0,0, 13,0, 0,8, -13,0  };
  if (!backdrop)
  {
    backdrop=testCreatePath();
  }
  else vgClearPath(backdrop, VG_PATH_CAPABILITY_ALL);
  vgAppendPathData(backdrop,sizeof(segsbk),segsbk,databk);
  
  VGubyte segwls[]= {
    VG_MOVE_TO_ABS, VG_LINE_TO_REL, VG_LINE_TO_REL, VG_LINE_TO_REL, VG_LINE_TO_REL, VG_LINE_TO_REL, VG_CLOSE_PATH,
    VG_MOVE_TO_ABS, VG_LINE_TO_REL, VG_LINE_TO_REL, VG_LINE_TO_REL, VG_LINE_TO_REL, VG_LINE_TO_REL, VG_LINE_TO_REL, VG_LINE_TO_REL, VG_CLOSE_PATH };
    
  VGfloat datawls[] = {
    8,2, 5,0, 0,3, -2,0, 0,-1, -3,0,
    0,2, 2,0, 0,4, 9,0, 0,1, 2,0, 0,1, -13,0 };
  
  if (!wall)
    wall=testCreatePath();
  else
    vgClearPath(wall, VG_PATH_CAPABILITY_ALL);
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

void drawKirpicz(int ratio)
{
  VGfloat dark[] = { 0.1411765, 0.2666667, 0.3294118, 1.0 };
  VGfloat border[] = { 0.1686275, 0.5019608, 0.5647059, 0.4745098 };
  VGfloat light[] = { 0.1254902, 0.6470588, 0.4784314, 0.2039216 };
  VGPaint back=vgCreatePaint();
  VGPaint stroke=vgCreatePaint();
  vgSetParameterfv(back, VG_PAINT_COLOR, 4, dark);
  
  vgScale((float)ratio/64.0,(float)ratio/64.0);
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
  
  int ratio=min(testWidth()/13,testHeight()/8);
  if (kirpicz)
    vgDestroyImage(kirpicz);
  kirpicz=vgCreateImage(VG_sRGB_565,ratio,ratio,VG_IMAGE_QUALITY_BETTER);
  /* Rendering takes place */
  VGfloat cc[] = {0,0,0,1};
  
  vgSetfv(VG_CLEAR_COLOR, 4, cc);
  vgClear(0,0,testWidth(),testHeight());
  vgLoadIdentity();
  drawKirpicz(ratio);
  vgGetPixels(kirpicz,0,0,0,0,ratio,ratio);
  
  vgPaintPattern(backdrop_p,kirpicz);

}

int main(int argc,char argv[])
{
  testInit(argc, argv, 832,512, "ShivaVG: Electro Body scene test");
  testCallback(TEST_CALLBACK_DISPLAY, (CallbackFunc)display);
  testCallback(TEST_CALLBACK_RESHAPE, (CallbackFunc)reshapeFunc);
  reshapeFunc(832,512);
  testRun();
  return EXIT_SUCCESS;
}
