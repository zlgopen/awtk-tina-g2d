/**
 * File:   g2d_tina.c
 * Author: AWTK Develop Team
 * Brief:  hardware 2d for tina
 *
 * Copyright (c) 2024 - 2024  Guangzhou ZHIYUAN Electronics Co.,Ltd.
 *
 * this program is distributed in the hope that it will be useful,
 * but without any warranty; without even the implied warranty of
 * merchantability or fitness for a particular purpose.  see the
 * license file for more details.
 *
 */

/**
 * history:
 * ================================================================
 * 2024-12-20 li xianjing <xianjimli@hotmail.com> created
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <string.h>
#include <signal.h>
#include <signal.h>
#include <time.h>
#include <linux/fb.h>
#include <linux/kernel.h>
#include <sys/ioctl.h>
#include <errno.h>
#include "include/g2d_driver_enh.h"
#include "include/ion_head.h"

#include "tkc/mem.h"
#include "base/bitmap.h"

#include "g2d_tina.h"
#include "g2d_tina_mem.h"
#include "graphic_buffer_tina.h"

static int s_g2d_fd;

ret_t tk_g2d_init(void) {
  tk_g2d_tina_mem_init();

  s_g2d_fd = open("/dev/g2d", O_RDWR);
  goto_error_if_fail(s_g2d_fd > 0);

  return RET_OK;
error:
  return RET_FAIL;
}

static g2d_fmt_enh bitmap_get_g2d_format(bitmap_t* fb) {
  switch (fb->format) {
    case BITMAP_FMT_RGBA8888: {
      return G2D_FORMAT_ABGR8888;
    }
    case BITMAP_FMT_ABGR8888: {
      return G2D_FORMAT_RGBA8888;
    }
    case BITMAP_FMT_BGRA8888: {
      return G2D_FORMAT_ARGB8888;
    }
    case BITMAP_FMT_RGB565: {
      return G2D_FORMAT_BGR565;
    }
    case BITMAP_FMT_BGR565: {
      return G2D_FORMAT_RGB565;
    }
    case BITMAP_FMT_RGB888: {
      return G2D_FORMAT_BGR888;
    }
    case BITMAP_FMT_BGR888: {
      return G2D_FORMAT_RGB888;
    }
    default: {
      assert(!"not supported format");
      /*TODO*/
      break;
    }
  }

  return G2D_FORMAT_RGBA8888;
}

static uint32_t color_to_value(color_t c) {
  uint32_t v = (c.rgba.a << 24) | (c.rgba.r << 16) | (c.rgba.g << 8) | (c.rgba.b);

  return v;
}

ret_t g2d_tina_fill_rect(bitmap_t* fb, const rect_t* dst, color_t c) {
  g2d_fillrect_h info;
  g2d_tina_mem_t* mem = NULL;
  memset(&info, 0x00, sizeof(info));
  return_value_if_fail(fb != NULL && dst != NULL, RET_BAD_PARAMS);
  mem = graphic_buffer_tina_get_mem(fb->buffer);
  return_value_if_fail(mem != NULL, RET_BAD_PARAMS);

  if (dst->w < 2 || dst->h < 2) {
    return RET_NOT_IMPL;
  }

  info.dst_image_h.format = bitmap_get_g2d_format(fb);
  info.dst_image_h.width = fb->w;
  info.dst_image_h.height = fb->h;

  info.dst_image_h.clip_rect.x = dst->x;
  info.dst_image_h.clip_rect.y = dst->y;
  info.dst_image_h.clip_rect.w = dst->w;
  info.dst_image_h.clip_rect.h = dst->h;

  info.dst_image_h.color = color_to_value(c);
  info.dst_image_h.mode = G2D_GLOBAL_ALPHA;
  info.dst_image_h.alpha = c.rgba.a;

  if (g2d_tina_mem_init_image(&info.dst_image_h, mem) != RET_OK) {
    return RET_NOT_IMPL;
  }

  if (ioctl(s_g2d_fd, G2D_CMD_FILLRECT_H, (unsigned long)(&info)) < 0) {
    printf("G2D_CMD_FILLRECT_H fail: mem=%p format=%d (%u %u) (%u %u %u %u) color=%x \n", mem,
           info.dst_image_h.format, info.dst_image_h.width, info.dst_image_h.height,
           info.dst_image_h.clip_rect.x, info.dst_image_h.clip_rect.y, info.dst_image_h.clip_rect.w,
           info.dst_image_h.clip_rect.h, info.dst_image_h.color);
    return RET_FAIL;
  } else {
    g2d_tina_mem_flush(mem);
    return RET_OK;
  }
}

static ret_t g2d_tina_blit_ioctrl(g2d_blt_h* blit) {
  if (ioctl(s_g2d_fd, G2D_CMD_BITBLT_H, blit) < 0) {
    printf("G2D_CMD_BITBLT_H failed: src(%d %d %d %d %d) dst(%d %d %d %d %d)\n",
           blit->src_image_h.format, blit->src_image_h.clip_rect.x, blit->src_image_h.clip_rect.y,
           blit->src_image_h.clip_rect.w, blit->src_image_h.clip_rect.h, blit->dst_image_h.format,
           blit->dst_image_h.clip_rect.x, blit->dst_image_h.clip_rect.y,
           blit->dst_image_h.clip_rect.w, blit->dst_image_h.clip_rect.h);
    return RET_FAIL;
  } else {
    return RET_OK;
  }
}

static ret_t g2d_tina_init_blt_h(g2d_blt_h* blit, bitmap_t* fb, bitmap_t* img, const rect_t* src,
                                 xy_t x, xy_t y) {
  g2d_tina_mem_t* fb_mem = NULL;
  g2d_tina_mem_t* img_mem = NULL;
  memset(blit, 0x00, sizeof(*blit));
  return_value_if_fail(fb != NULL && img != NULL && src != NULL, RET_BAD_PARAMS);
  fb_mem = graphic_buffer_tina_get_mem(fb->buffer);
  return_value_if_fail(fb_mem != NULL, RET_BAD_PARAMS);
  img_mem = graphic_buffer_tina_get_mem(img->buffer);
  return_value_if_fail(img_mem != NULL, RET_BAD_PARAMS);

  blit->flag_h = G2D_ROT_0;
  if (g2d_tina_mem_init_image(&blit->src_image_h, img_mem) != RET_OK) {
    return RET_NOT_IMPL;
  }
  blit->src_image_h.format = bitmap_get_g2d_format(img);
  blit->src_image_h.mode = G2D_PIXEL_ALPHA;
  blit->src_image_h.clip_rect.x = src->x;
  blit->src_image_h.clip_rect.y = src->y;
  blit->src_image_h.clip_rect.w = src->w;
  blit->src_image_h.clip_rect.h = src->h;
  blit->src_image_h.width = img->w;
  blit->src_image_h.height = img->h;
  blit->src_image_h.alpha = 0xff;

  if (g2d_tina_mem_init_image(&blit->dst_image_h, fb_mem) != RET_OK) {
    return RET_NOT_IMPL;
  }
  blit->dst_image_h.format = bitmap_get_g2d_format(fb);
  blit->dst_image_h.mode = G2D_GLOBAL_ALPHA;
  blit->dst_image_h.clip_rect.x = x;
  blit->dst_image_h.clip_rect.y = y;
  blit->dst_image_h.clip_rect.w = src->w;
  blit->dst_image_h.clip_rect.h = src->h;
  blit->dst_image_h.alpha = 0xff;
  blit->dst_image_h.width = fb->w;
  blit->dst_image_h.height = fb->h;

  return RET_OK;
}

ret_t g2d_tina_copy_image(bitmap_t* fb, bitmap_t* img, const rect_t* src, xy_t x, xy_t y) {
  g2d_blt_h blit;
  
  if (src->w < 2 || src->h < 2) {
    return RET_NOT_IMPL;
  }

  if (g2d_tina_init_blt_h(&blit, fb, img, src, x, y) != RET_OK) {
    return RET_NOT_IMPL;
  }

  blit.flag_h = G2D_BLT_NONE_H;

  return g2d_tina_blit_ioctrl(&blit);
}

static g2d_blt_flags_h lcd_orientation_to_blt_flags_h(lcd_orientation_t o) {
  switch (o) {
    case LCD_ORIENTATION_0: {
      return G2D_ROT_0;
    }
    case LCD_ORIENTATION_90: {
      return G2D_ROT_90;
    }
    case LCD_ORIENTATION_180: {
      return G2D_ROT_180;
    }
    case LCD_ORIENTATION_270: {
      return G2D_ROT_270;
    }
    default: { return G2D_ROT_0; }
  }
}

ret_t g2d_tina_rotate_image(bitmap_t* fb, bitmap_t* img, const rect_t* src, lcd_orientation_t o,
                            xy_t x, xy_t y) {
  g2d_blt_h blit;
  
  if (src->w < 2 || src->h < 2) {
    return RET_NOT_IMPL;
  }

  if (g2d_tina_init_blt_h(&blit, fb, img, src, x, y) != RET_OK) {
    return RET_NOT_IMPL;
  }

  blit.flag_h = lcd_orientation_to_blt_flags_h(o);

  return g2d_tina_blit_ioctrl(&blit);
}

static ret_t g2d_tina_init_bld(g2d_bld* bld, bitmap_t* fb, bitmap_t* img, const rect_t* dst,
                               const rect_t* src) {
  g2d_tina_mem_t* fb_mem = NULL;
  g2d_tina_mem_t* img_mem = NULL;
  memset(bld, 0x00, sizeof(*bld));
  return_value_if_fail(fb != NULL && img != NULL && src != NULL, RET_BAD_PARAMS);
  fb_mem = graphic_buffer_tina_get_mem(fb->buffer);
  return_value_if_fail(fb_mem != NULL, RET_BAD_PARAMS);
  img_mem = graphic_buffer_tina_get_mem(img->buffer);
  return_value_if_fail(img_mem != NULL, RET_BAD_PARAMS);

  bld->bld_cmd = G2D_BLD_SRCOVER;

  if (g2d_tina_mem_init_image(&bld->dst_image, fb_mem) != RET_OK) {
    return RET_NOT_IMPL;
  }

  bld->dst_image.format = bitmap_get_g2d_format(fb);
  bld->dst_image.mode = G2D_GLOBAL_ALPHA;
  bld->dst_image.clip_rect.x = dst->x;
  bld->dst_image.clip_rect.y = dst->y;
  bld->dst_image.clip_rect.w = dst->w;
  bld->dst_image.clip_rect.h = dst->h;
  bld->dst_image.alpha = 0xff;
  bld->dst_image.width = fb->w;
  bld->dst_image.height = fb->h;

  bld->src_image[0] = bld->dst_image;

  if (g2d_tina_mem_init_image(&bld->src_image[1], img_mem) != RET_OK) {
    return RET_NOT_IMPL;
  }
  bld->src_image[1].format = bitmap_get_g2d_format(img);
  bld->src_image[1].mode = G2D_PIXEL_ALPHA;
  bld->src_image[1].clip_rect.x = src->x;
  bld->src_image[1].clip_rect.y = src->y;
  bld->src_image[1].clip_rect.w = src->w;
  bld->src_image[1].clip_rect.h = src->h;
  bld->src_image[1].width = img->w;
  bld->src_image[1].height = img->h;
  bld->src_image[1].alpha = 0xff;

  return RET_OK;
}

static ret_t g2d_tina_bld_ioctrl(g2d_bld* bld) {
  if (ioctl(s_g2d_fd, G2D_CMD_BLD_H, bld) < 0) {
    printf("G2D_CMD_BLD_H failed\n");
    return RET_FAIL;
  } else {
    return RET_OK;
  }
}

ret_t g2d_tina_blend_image(bitmap_t* fb, bitmap_t* img, const rect_t* dst, const rect_t* src,
                           uint8_t global_alpha) {
  g2d_bld bld;

  if (global_alpha < 0xf0) {
    return RET_NOT_IMPL;
  }
  
  if (dst->w < 2 || dst->h < 2) {
    return RET_NOT_IMPL;
  }

  if (g2d_tina_init_bld(&bld, fb, img, dst, src) != RET_OK) {
    return RET_NOT_IMPL;
  }

  bld.src_image[0].mode = G2D_PIXEL_ALPHA;

  return g2d_tina_bld_ioctrl(&bld);
}

ret_t tk_g2d_deinit(void) {
  if (s_g2d_fd > 0) {
    close(s_g2d_fd);
  }

  g2d_tina_mem_deinit();
  return RET_OK;
}

ret_t g2d_fill_rect(bitmap_t* fb, const rect_t* dst, color_t c) {
  return g2d_tina_fill_rect(fb, dst, c);
}

ret_t g2d_copy_image(bitmap_t* fb, bitmap_t* img, const rect_t* src, xy_t x, xy_t y) {
  return g2d_tina_copy_image(fb, img, src, x, y);
}

ret_t g2d_rotate_image(bitmap_t* fb, bitmap_t* img, const rect_t* src, lcd_orientation_t o, xy_t x,
                       xy_t y) {
  return g2d_tina_rotate_image(fb, img, src, o, x, y);
}

ret_t g2d_blend_image(bitmap_t* fb, bitmap_t* img, const rect_t* dst, const rect_t* src,
                      uint8_t global_alpha) {
  return g2d_tina_blend_image(fb, img, dst, src, global_alpha);
}

ret_t TK_WEAK g2d_rotate_image_ex(bitmap_t* dst, bitmap_t* src, const rect_t* src_r, xy_t dx,
                                  xy_t dy, lcd_orientation_t o) {
  return RET_NOT_IMPL;
}

ret_t TK_WEAK g2d_blend_image_rotate(bitmap_t* dst, bitmap_t* src, const rectf_t* dst_r,
                                     const rectf_t* src_r, uint8_t alpha, lcd_orientation_t o) {
  return RET_NOT_IMPL;
}
