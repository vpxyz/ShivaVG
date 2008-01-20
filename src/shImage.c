/*
 * Copyright (c) 2007 Ivan Leben
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library in the file COPYING;
 * if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#define VG_API_EXPORT
#include "openvg.h"
#include "shImage.h"
#include "shContext.h"
#include <string.h>
#include <stdio.h>

#define _ITEM_T SHColor
#define _ARRAY_T SHColorArray
#define _FUNC_T shColorArray
#define _ARRAY_DEFINE
#define _COMPARE_T(c1,c2) 0
#include "shArrayBase.h"

#define _ITEM_T SHImage*
#define _ARRAY_T SHImageArray
#define _FUNC_T shImageArray
#define _ARRAY_DEFINE
#include "shArrayBase.h"

void SHColor_ctor(SHColor *c)
{
  c->r = 0.0f;
  c->g = 0.0f;
  c->b = 0.0f;
  c->a = 0.0f;
  c->f = sRGBA;
}

void SHColor_dtor(SHColor *c) {
}

void SHImage_ctor(SHImage *i)
{
  i->data = NULL;
  i->width = 0;
  i->height = 0;
  glGenTextures(1, &i->texture);
}

void SHImage_dtor(SHImage *i)
{
  if (i->data != NULL)
    free(i->data);
  
  if (glIsTexture(i->texture))
    glDeleteTextures(1, &i->texture);
}

int shIsValidImageFormat(VGImageFormat format)
{
  SHint aOrderBit = (1 << 6);
  SHint rgbOrderBit = (1 << 7);
  SHint baseFormat = format & 0xFF;
  SHint unorderedRgba = format & (~(aOrderBit | rgbOrderBit));
  SHint isRgba = (baseFormat == VG_sRGBX_8888     ||
                  baseFormat == VG_sRGBA_8888     ||
                  baseFormat == VG_sRGBA_8888_PRE ||
                  baseFormat == VG_sRGBA_5551     ||
                  baseFormat == VG_sRGBA_4444     ||
                  baseFormat == VG_lRGBX_8888     ||
                  baseFormat == VG_lRGBA_8888     ||
                  baseFormat == VG_lRGBA_8888_PRE);
  
  SHint check = isRgba ? unorderedRgba : format;
  return check >= VG_sRGBX_8888 && check <= VG_BW_1;
}

void shUpdateImageTextureSize(SHImage *i)
{
  i->texwidth = i->width;
  i->texheight = i->height;
  i->texwidthK = 1.0f;
  i->texheightK = 1.0f;
  
  /* Round size to nearest power of 2 */
  /* TODO: might be dropped out if it works without  */
  
  /*i->texwidth = 1;
  while (i->texwidth < i->width)
    i->texwidth *= 2;
  
  i->texheight = 1;
  while (i->texheight < i->height)
    i->texheight *= 2;
  
  i->texwidthK  = (SHfloat)i->width  / i->texwidth;
  i->texheightK = (SHfloat)i->height / i->texheight; */
}

void shUpdateImageTexture(SHImage *i, VGContext *c)
{
  SHint potwidth;
  SHint potheight;
  SHint8 *potdata;
  

  /* Find nearest power of two size */

  potwidth = 1;
  while (potwidth < i->width)
    potwidth *= 2;
  
  potheight = 1;
  while (potheight < i->height)
    potheight *= 2;
  
  
  /* Scale into a temp buffer if image not a power of two (pot)
     and non-power-of-two textures are not supported by OpenGL */  
  
  if ((i->width < potwidth || i->height < potheight) &&
      !c->isGLAvailable_TextureNonPowerOfTwo) {
    
    potdata = (SHint8*)malloc( potwidth * potheight * 4 );
    if (!potdata) return;
    
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glBindTexture(GL_TEXTURE_2D, i->texture);
    
    
    gluScaleImage(GL_RGBA, i->width, i->height, GL_UNSIGNED_BYTE, i->data,
                  potwidth, potheight, GL_UNSIGNED_BYTE, potdata);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, potwidth, potheight, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, potdata);
    
    free(potdata);
    return;
  }
  
  /* Store pixels to texture */
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glBindTexture(GL_TEXTURE_2D, i->texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, i->texwidth, i->texheight, 0,
               GL_RGBA, GL_UNSIGNED_BYTE, i->data);
}

VG_API_CALL VGImage vgCreateImage(VGImageFormat format,
                                  VGint width, VGint height,
                                  VGbitfield allowedQuality)
{
  SHImage *i = NULL;
  SHImageFormatDesc fd;
  VG_GETCONTEXT(VG_INVALID_HANDLE);
  
  /* Reject invalid formats */
  VG_RETURN_ERR_IF(!shIsValidImageFormat(format),
                   VG_UNSUPPORTED_IMAGE_FORMAT_ERROR,
                   VG_INVALID_HANDLE);
  
  /* Just one image format currently supported */
  VG_RETURN_ERR_IF(format != VG_sRGBA_8888,
                   VG_UNSUPPORTED_IMAGE_FORMAT_ERROR,
                   VG_INVALID_HANDLE);
  
  /* Reject invalid sizes */
  VG_RETURN_ERR_IF(width  <= 0 || width > SH_MAX_IMAGE_WIDTH ||
                   height <= 0 || height > SH_MAX_IMAGE_HEIGHT ||
                   width * height > SH_MAX_IMAGE_PIXELS,
                   VG_ILLEGAL_ARGUMENT_ERROR, VG_INVALID_HANDLE);
  
  /* TODO: create descriptor for image format and check if byte size
     exceeds SH_MAX_IMAGE_BYTES */
  memset(&fd, 0, sizeof(fd));
  fd.format = format;
  
  /* Reject invalid quality bits */
  VG_RETURN_ERR_IF(allowedQuality &
                   ~(VG_IMAGE_QUALITY_NONANTIALIASED |
                     VG_IMAGE_QUALITY_FASTER | VG_IMAGE_QUALITY_BETTER),
                   VG_ILLEGAL_ARGUMENT_ERROR, VG_INVALID_HANDLE);
  
  /* Create new image object */
  SH_NEWOBJ(SHImage, i);
  VG_RETURN_ERR_IF(!i, VG_OUT_OF_MEMORY_ERROR, VG_INVALID_HANDLE);
  i->width = width;
  i->height = height;
  i->fd = fd;
  
  /* Allocate data memory */
  shUpdateImageTextureSize(i);
  i->data = (SHuint8*)malloc( i->texwidth * i->texheight * 4 );
  
  if (i->data == NULL) {
    SH_DELETEOBJ(SHImage, i);
    VG_RETURN_ERR(VG_OUT_OF_MEMORY_ERROR, VG_INVALID_HANDLE); }
  
  /* Initialize data by zeroing-out */
  memset(i->data, 0, width * height * 4);
  shUpdateImageTexture(i, context);
  
  /* Add to resource list */
  shImageArrayPushBack(&context->images, i);
  
  VG_RETURN((VGImage)i);
}

VG_API_CALL void vgDestroyImage(VGImage image)
{
  SHint index;
  VG_GETCONTEXT(VG_NO_RETVAL);
  
  /* Check if valid resource */
  index = shImageArrayFind(&context->images, (SHImage*)image);
  VG_RETURN_ERR_IF(index == -1, VG_ILLEGAL_ARGUMENT_ERROR, VG_NO_RETVAL);
  
  /* Delete object and remove resource */
  SH_DELETEOBJ(SHImage, (SHImage*)image);
  shImageArrayRemoveAt(&context->images, index);
  
  VG_RETURN(VG_NO_RETVAL);
}

void shStoreColor(SHColor *c, SHuint8 *data, SHImageFormatDesc *d)
{
  if (d->format != VG_sRGBA_8888) return;
  data[0] = COL2INTCOORD(c->r, 255);
  data[1] = COL2INTCOORD(c->g, 255);
  data[2] = COL2INTCOORD(c->b, 255);
  data[3] = COL2INTCOORD(c->a, 255);
}

void shLoadColor(SHColor *c, SHuint8 *data, SHImageFormatDesc *d)
{
  if (d->format != VG_sRGBA_8888) return;
  c->r = INT2COLCOORD(data[0], 255);
  c->g = INT2COLCOORD(data[1], 255);
  c->b = INT2COLCOORD(data[2], 255);
  c->a = INT2COLCOORD(data[3], 255);
}

/*---------------------------------------------------
 * Clear given rectangle area in the image data with
 * color set via vgSetfv(VG_CLEAR_COLOR, ...)
 *---------------------------------------------------*/

VG_API_CALL void vgClearImage(VGImage image,
                              VGint x, VGint y, VGint width, VGint height)
{
  SHImage *i;
  SHColor clear;
  SHuint8 *data;
  SHint X,Y, ix, iy, dx, dy, stride;
  VG_GETCONTEXT(VG_NO_RETVAL);
  
  VG_RETURN_ERR_IF(!shIsValidImage(context, image),
                   VG_BAD_HANDLE_ERROR, VG_NO_RETVAL);
  
  /* TODO: check if image current render target */
  
  i = (SHImage*)image;
  VG_RETURN_ERR_IF(width <= 0 || height <= 0,
                   VG_ILLEGAL_ARGUMENT_ERROR, VG_NO_RETVAL);
  
  /* Nothing to do if target rectangle out of bounds */
  if (x >= i->width || y >= i->height)
    VG_RETURN(VG_NO_RETVAL);
  if (x + width < 0 || y + height < 0)
    VG_RETURN(VG_NO_RETVAL);

  /* Clamp to image bounds */
  ix = SH_MAX(x, 0); dx = ix - x;
  iy = SH_MAX(y, 0); dy = iy - y;
  width  = SH_MIN( width  - dx, i->width  - ix);
  height = SH_MIN( height - dy, i->height - iy);
  stride = i->texwidth * 4;
  
  /* Walk pixels and clear*/
  clear = context->clearColor;
  clear.f = sRGBA;
  
  for (Y=iy; Y<iy+height; ++Y) {
    data = i->data + ( Y*stride + ix * 4 );
    
    for (X=ix; X<ix+width; ++X) {
      shStoreColor(&clear, data, &i->fd);
      data += 4;
    }}
  
  shUpdateImageTexture(i, context);
  VG_RETURN(VG_NO_RETVAL);
}

/*------------------------------------------------------------
 * Generic function for copying a rectangle area of pixels
 * of size (width,height) among two data buffers. The size of
 * source (swidth,sheight) and destination (dwidth,dheight)
 * images may vary as well as the source coordinates (sx,sy)
 * and destination coordinates(dx, dy).
 *------------------------------------------------------------*/

void shCopyPixels(SHuint8 *dst, VGImageFormat dstFormat, SHint dstStride,
                  SHuint8 *src, VGImageFormat srcFormat, SHint srcStride,
                  SHint dwidth, SHint dheight, SHint swidth, SHint sheight,
                  SHint dx, SHint dy, SHint sx, SHint sy,
                  SHint width, SHint height)
{
  SHint dxold, dyold;
  SHint SX, SY, DX, DY;
  SHuint8 *SD, *DD;
  SHColor c;

  SHImageFormatDesc dfd;
  SHImageFormatDesc sfd;

  /* TODO: generate real format descriptors */
  SH_ASSERT(dstFormat == VG_sRGBA_8888);
  SH_ASSERT(srcFormat == VG_sRGBA_8888);
  memset(&dfd, 0, sizeof(dfd));
  memset(&sfd, 0, sizeof(sfd));
  dfd.format = dstFormat;
  sfd.format = srcFormat;

  /*
    In order to optimize the copying loop and remove the
    if statements from it to check whether target pixel
    is in the source and destination surface, we clamp
    copy rectangle in advance. This is quite a tedious
    task though. Here is a picture of the scene. Note that
    (dx,dy) is actually an offset of the copy rectangle
    (clamped to src surface) from the (0,0) point on dst
    surface. A negative (dx,dy) (as in this picture) also
    affects src coords of the copy rectangle which have
    to be readjusted again (sx,sy,width,height).

                          src
    *----------------------*
    | (sx,sy)  copy rect   |
    | *-----------*        |
    | |\(dx, dy)  |        |          dst
    | | *------------------------------*
    | | |xxxxxxxxx|        |           |
    | | |xxxxxxxxx|        |           |
    | *-----------*        |           |
    |   |   (width,height) |           |
    *----------------------*           |
        |           (swidth,sheight)   |
        *------------------------------*
                                (dwidth,dheight)
  */

  /* Cancel if copy rect out of src bounds */
  if (sx >= swidth || sy >= sheight) return;
  if (sx + width < 0 || sy + height < 0) return;
  
  /* Clamp copy rectangle to src bounds */
  sx = SH_MAX(sx, 0);
  sy = SH_MAX(sy, 0);
  width = SH_MIN(width, swidth - sx);
  height = SH_MIN(height, sheight - sy);
  
  /* Cancel if copy rect out of dst bounds */
  if (dx >= dwidth || dy >= dheight) return;
  if (dx + width < 0 || dy + height < 0) return;
  
  /* Clamp copy rectangle to dst bounds */
  dxold = dx; dyold = dy;
  dx = SH_MAX(dx, 0);
  dy = SH_MAX(dy, 0);
  sx += dx - dxold;
  sy += dy - dyold;
  width -= dx - dxold;
  height -= dy - dyold;
  width = SH_MIN(width, dwidth  - dx);
  height = SH_MIN(height, dheight - dy);
  
  /* Calculate stride from format if not given */
  if (dstStride == -1) dstStride = dwidth * 4;
  if (srcStride == -1) srcStride = swidth * 4;
  
  /* Walk pixels and copy */
  for (SY=sy, DY=dy; SY < sy+height; ++SY, ++DY) {
    SD = src + SY * srcStride + sx * 4;
    DD = dst + DY * dstStride + dx * 4;
    
    for (SX=sx, DX=dx; SX < sx+width; ++SX, ++DX) {
      shLoadColor(&c, SD, &sfd);
      shStoreColor(&c, DD, &dfd);
      SD += 4; DD += 4;
    }}
}

/*---------------------------------------------------------
 * Copies a rectangle area of pixels of size (width,height)
 * from given data buffer to image surface at destination
 * coordinates (x,y)
 *---------------------------------------------------------*/

VG_API_CALL void vgImageSubData(VGImage image,
                                const void * data, VGint dataStride,
                                VGImageFormat dataFormat,
                                VGint x, VGint y, VGint width, VGint height)
{
  SHImage *i;
  VG_GETCONTEXT(VG_NO_RETVAL);
  
  VG_RETURN_ERR_IF(!shIsValidImage(context, image),
                   VG_BAD_HANDLE_ERROR, VG_NO_RETVAL);
  
  /* TODO: check if image current render target */
  i = (SHImage*)image;
  
  /* Reject invalid formats */
  VG_RETURN_ERR_IF(!shIsValidImageFormat(dataFormat),
                   VG_UNSUPPORTED_IMAGE_FORMAT_ERROR,
                   VG_NO_RETVAL);
  
  /* Just one image format currently supported */
  VG_RETURN_ERR_IF(dataFormat != VG_sRGBA_8888,
                   VG_UNSUPPORTED_IMAGE_FORMAT_ERROR,
                   VG_NO_RETVAL);
  
  VG_RETURN_ERR_IF(width <= 0 || height <= 0 || !data,
                   VG_ILLEGAL_ARGUMENT_ERROR, VG_NO_RETVAL);
  
  /* TODO: check data array alignment */
  
  shCopyPixels(i->data, i->fd.format, i->texwidth * 4,
               (SHuint8*)data, dataFormat,dataStride,
               i->width, i->height, width, height,
               x, y, 0, 0, width, height);
  
  shUpdateImageTexture(i, context);
  VG_RETURN(VG_NO_RETVAL);
}

/*---------------------------------------------------------
 * Copies a rectangle area of pixels of size (width,height)
 * from image surface at source coordinates (x,y) to given
 * data buffer
 *---------------------------------------------------------*/

VG_API_CALL void vgGetImageSubData(VGImage image,
                                   void * data, VGint dataStride,
                                   VGImageFormat dataFormat,
                                   VGint x, VGint y,
                                   VGint width, VGint height)
{
  SHImage *i;
  VG_GETCONTEXT(VG_NO_RETVAL);
  
  VG_RETURN_ERR_IF(!shIsValidImage(context, image),
                   VG_BAD_HANDLE_ERROR, VG_NO_RETVAL);
  
  /* TODO: check if image current render target */
  i = (SHImage*)image;
  
  /* Reject invalid formats */
  VG_RETURN_ERR_IF(!shIsValidImageFormat(dataFormat),
                   VG_UNSUPPORTED_IMAGE_FORMAT_ERROR,
                   VG_NO_RETVAL);
  
  /* Just one image format currently supported */
  VG_RETURN_ERR_IF(dataFormat != VG_sRGBA_8888,
                   VG_UNSUPPORTED_IMAGE_FORMAT_ERROR,
                   VG_NO_RETVAL);
  
  VG_RETURN_ERR_IF(width <= 0 || height <= 0 || !data,
                   VG_ILLEGAL_ARGUMENT_ERROR, VG_NO_RETVAL);
  
  /* TODO: check data array alignment */
  
  shCopyPixels((SHuint8*)data, dataFormat, dataStride,
               i->data, i->fd.format, i->texwidth * 4,
               width, height, i->width, i->height,
               0,0,x,x,width,height);
  
  VG_RETURN(VG_NO_RETVAL);
}

/*----------------------------------------------------------
 * Copies a rectangle area of pixels of size (width,height)
 * from src image surface at source coordinates (sx,sy) to
 * dst image surface at destination cordinates (dx,dy)
 *---------------------------------------------------------*/

VG_API_CALL void vgCopyImage(VGImage dst, VGint dx, VGint dy,
                             VGImage src, VGint sx, VGint sy,
                             VGint width, VGint height,
                             VGboolean dither)
{
  SHImage *s, *d;
  SHuint8 *pixels;

  VG_GETCONTEXT(VG_NO_RETVAL);
  
  VG_RETURN_ERR_IF(!shIsValidImage(context, src) ||
                   !shIsValidImage(context, dst),
                   VG_BAD_HANDLE_ERROR, VG_NO_RETVAL);

  /* TODO: check if images current render target */

  s = (SHImage*)src; d = (SHImage*)dst;
  VG_RETURN_ERR_IF(width <= 0 || height <= 0,
                   VG_ILLEGAL_ARGUMENT_ERROR, VG_NO_RETVAL);

  /* In order to perform copying in a cosistent fashion
     we first copy to a temporary buffer and only then to
     destination image */
  
  /* TODO: rather check first if the image is really
     the same at and whether the regions overlap */

  pixels = (SHuint8*)malloc(width * height * 4);
  SH_RETURN_ERR_IF(!pixels, VG_OUT_OF_MEMORY_ERROR, SH_NO_RETVAL);

  shCopyPixels(pixels, VG_sRGBA_8888, -1,
               s->data, s->fd.format, s->texwidth * 4,
               width, height, s->width, s->height,
               0, 0, sx, sy, width, height);

  shCopyPixels(d->data, d->fd.format, d->texwidth * 4,
               pixels, VG_sRGBA_8888, -1,
               d->width, d->height, width, height,
               dx, dy, 0, 0, width, height);

  free(pixels);
  
  shUpdateImageTexture(d, context);
  VG_RETURN(VG_NO_RETVAL);
}

/*---------------------------------------------------------
 * Copies a rectangle area of pixels of size (width,height)
 * from src image surface at source coordinates (sx,sy) to
 * window surface at destination coordinates (dx,dy)
 *---------------------------------------------------------*/

VG_API_CALL void vgSetPixels(VGint dx, VGint dy,
                             VGImage src, VGint sx, VGint sy,
                             VGint width, VGint height)
{
  SHImage *i;
  SHuint8 *pixels;

  VG_GETCONTEXT(VG_NO_RETVAL);
  
  VG_RETURN_ERR_IF(!shIsValidImage(context, src),
                   VG_BAD_HANDLE_ERROR, VG_NO_RETVAL);
  
  /* TODO: check if image current render target */

  i = (SHImage*)src;
  VG_RETURN_ERR_IF(width <= 0 || height <= 0,
                   VG_ILLEGAL_ARGUMENT_ERROR, VG_NO_RETVAL);

  /* OpenGL doesn't allow us to use random stride. We have to
     manually copy the image data and write from a copy with
     normal row length (without power-of-two roundup pixels) */

  pixels = (SHuint8*)malloc(width * height * 4);
  SH_RETURN_ERR_IF(!pixels, VG_OUT_OF_MEMORY_ERROR, SH_NO_RETVAL);

  shCopyPixels(pixels, VG_sRGBA_8888, -1,
               i->data, i->fd.format, i->texwidth * 4,
               width, height, i->width, i->height,
               0,0,sx,sy, width, height);

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glRasterPos2i(dx, dy);
  glDrawPixels(width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
  glRasterPos2i(0,0);
  
  free(pixels);

  VG_RETURN(VG_NO_RETVAL);
}

/*---------------------------------------------------------
 * Copies a rectangle area of pixels of size (width,height)
 * from given data buffer at source coordinates (sx,sy) to
 * window surface at destination coordinates (dx,dy)
 *---------------------------------------------------------*/

VG_API_CALL void vgWritePixels(const void * data, VGint dataStride,
                               VGImageFormat dataFormat,
                               VGint dx, VGint dy,
                               VGint width, VGint height)
{
  SHuint8 *pixels;

  VG_GETCONTEXT(VG_NO_RETVAL);

  /* Reject invalid formats */
  VG_RETURN_ERR_IF(!shIsValidImageFormat(dataFormat),
                   VG_UNSUPPORTED_IMAGE_FORMAT_ERROR,
                   VG_NO_RETVAL);
  
  /* Just one image format currently supported */
  VG_RETURN_ERR_IF(dataFormat != VG_sRGBA_8888,
                   VG_UNSUPPORTED_IMAGE_FORMAT_ERROR,
                   VG_NO_RETVAL);

  /* TODO: check output data array alignment */
  
  VG_RETURN_ERR_IF(width <= 0 || height <= 0 || !data,
                   VG_ILLEGAL_ARGUMENT_ERROR, VG_NO_RETVAL);

  /* OpenGL doesn't allow us to use random stride. We have to
     manually copy the image data and write from a copy with
     normal row length */

  pixels = (SHuint8*)malloc(width * height * 4);
  SH_RETURN_ERR_IF(!pixels, VG_OUT_OF_MEMORY_ERROR, SH_NO_RETVAL);
  
  shCopyPixels(pixels, VG_sRGBA_8888, -1,
               (SHuint8*)data, dataFormat, dataStride,
               width, height, width, height,
               0,0,0,0, width, height);
  
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glRasterPos2i(dx, dy);
  glDrawPixels(width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
  glRasterPos2i(0,0);
  
  free(pixels);

  VG_RETURN(VG_NO_RETVAL); 
}

/*-----------------------------------------------------------
 * Copies a rectangle area of pixels of size (width, height)
 * from window surface at source coordinates (sx, sy) to
 * image surface at destination coordinates (dx, dy)
 *-----------------------------------------------------------*/

VG_API_CALL void vgGetPixels(VGImage dst, VGint dx, VGint dy,
                             VGint sx, VGint sy,
                             VGint width, VGint height)
{
  SHImage *i;
  SHuint8 *pixels;
  VG_GETCONTEXT(VG_NO_RETVAL);
  
  VG_RETURN_ERR_IF(!shIsValidImage(context, dst),
                   VG_BAD_HANDLE_ERROR, VG_NO_RETVAL);
  
   /* TODO: check if image current render target */

  i = (SHImage*)dst;
  VG_RETURN_ERR_IF(width <= 0 || height <= 0,
                   VG_ILLEGAL_ARGUMENT_ERROR, VG_NO_RETVAL);
  
  /* OpenGL doesn't allow us to read to random destination
     coordinates nor using random stride. We have to
     read first and then manually copy to the image data */

  pixels = (SHuint8*)malloc(width * height * 4);
  SH_RETURN_ERR_IF(!pixels, VG_OUT_OF_MEMORY_ERROR, SH_NO_RETVAL);

  glPixelStorei(GL_PACK_ALIGNMENT, 1);
  glReadPixels(sx, sy, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
  
  shCopyPixels(i->data, i->fd.format, i->texwidth * 4,
               pixels, VG_sRGBA_8888, -1,
               i->width, i->height, width, height,
               dx, dy, 0, 0, width, height);

  free(pixels);
  
  shUpdateImageTexture(i, context);
  VG_RETURN(VG_NO_RETVAL);
}

/*-----------------------------------------------------------
 * Copies a rectangle area of pixels of size (width, height)
 * from window surface at source coordinates (sx, sy) to
 * to given output data buffer.
 *-----------------------------------------------------------*/

VG_API_CALL void vgReadPixels(void * data, VGint dataStride,
                              VGImageFormat dataFormat,
                              VGint sx, VGint sy,
                              VGint width, VGint height)
{
  SHuint8 *pixels;
  VG_GETCONTEXT(VG_NO_RETVAL);

  /* Reject invalid formats */
  VG_RETURN_ERR_IF(!shIsValidImageFormat(dataFormat),
                   VG_UNSUPPORTED_IMAGE_FORMAT_ERROR,
                   VG_NO_RETVAL);
  
  /* Just one image format currently supported */
  VG_RETURN_ERR_IF(dataFormat != VG_sRGBA_8888,
                   VG_UNSUPPORTED_IMAGE_FORMAT_ERROR,
                   VG_NO_RETVAL);

  /* TODO: check output data array alignment */
  
  VG_RETURN_ERR_IF(width <= 0 || height <= 0 || !data,
                   VG_ILLEGAL_ARGUMENT_ERROR, VG_NO_RETVAL);

  /* OpenGL doesn't allow random data stride. We have to
     read first and then manually copy to the output buffer */

  pixels = (SHuint8*)malloc(width * height * 4);
  SH_RETURN_ERR_IF(!pixels, VG_OUT_OF_MEMORY_ERROR, SH_NO_RETVAL);

  glPixelStorei(GL_PACK_ALIGNMENT, 1);
  glReadPixels(sx, sy, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
  
  shCopyPixels(data, dataFormat, dataStride,
               pixels, VG_sRGBA_8888, -1,
               width, height, width, height,
               0, 0, 0, 0, width, height);

  free(pixels);
  
  VG_RETURN(VG_NO_RETVAL);
}

/*----------------------------------------------------------
 * Copies a rectangle area of pixels of size (width,height)
 * from window surface at source coordinates (sx,sy) to
 * windows surface at destination cordinates (dx,dy)
 *---------------------------------------------------------*/

VG_API_CALL void vgCopyPixels(VGint dx, VGint dy,
                              VGint sx, VGint sy,
                              VGint width, VGint height)
{
  VG_GETCONTEXT(VG_NO_RETVAL);
  
  VG_RETURN_ERR_IF(width <= 0 || height <= 0,
                   VG_ILLEGAL_ARGUMENT_ERROR, VG_NO_RETVAL);
  
  glPixelStorei(GL_PACK_ALIGNMENT, 1);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glRasterPos2i(dx, dy);
  glCopyPixels(sx, sy, width, height, GL_COLOR);
  glRasterPos2i(0, 0);
  
  VG_RETURN(VG_NO_RETVAL);
}
