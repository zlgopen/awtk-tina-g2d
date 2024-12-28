#include "base/g2d.h"
#include "../src/g2d_tina.h"
#include "../src/g2d_tina_mem.h"

#include "tkc/platform.h"
#include "base/graphic_buffer.h"

graphic_buffer_t* graphic_buffer_tina_create(uint32_t w, uint32_t h,
                                                       bitmap_format_t format,
                                                       uint32_t line_length);

#include <stdio.h>

//
// * BITMAP_FMT_RGB565 rotate not work
// * BITMAP_FMT_BGR565 rotate not work
// * BITMAP_FMT_RGB888 rotate not work
// * BITMAP_FMT_BGR888 rotate not work
// * BITMAP_FMT_ABGR8888 rotate not work
//
static void test_fill_rect(bitmap_t* fb, bitmap_t* img) {
  rect_t r = rect_init(0, 0, img->w, img->h);
  rect_t dst = rect_init(0, 0, img->w, img->h);

  color_t gray = color_init(0xd0, 0xd0, 0xd0, 0x80);
  color_t red = color_init(0xff, 0, 0, 0xff);
  color_t green = color_init(0, 0xff, 0, 0xff);
  color_t blue = color_init(0, 0, 0xff, 0xff);
  color_t bg = color_init(0xe0, 0xe0, 0xe0, 0xff);

  r = rect_init(0, 0, fb->w, fb->h);
  g2d_tina_fill_rect(fb, &r, bg); 
  
  g2d_tina_fill_rect(img, &r, gray); 

  r = rect_init(0, 0, 20, 20);
  g2d_tina_fill_rect(img, &r, red); 

  r = rect_init(20, 20, 20, 20);
  g2d_tina_fill_rect(img, &r, green); 

  r = rect_init(40, 40, 20, 20);
  g2d_tina_fill_rect(img, &r, blue); 

  r = rect_init(0, 0, 60, 60);
  g2d_tina_copy_image(fb, img, &r, 0, 0);

#if 0
  g2d_tina_rotate_image(fb, img, &r, LCD_ORIENTATION_0, 60, 0);
  g2d_tina_rotate_image(fb, img, &r, LCD_ORIENTATION_90, 120, 0);
  g2d_tina_rotate_image(fb, img, &r, LCD_ORIENTATION_180, 180, 0);
  g2d_tina_rotate_image(fb, img, &r, LCD_ORIENTATION_270, 240, 0);
#endif

#if 1
  dst = rect_init(0, 60, img->w, img->h);
  r = rect_init(0, 0, 60, 60);
  g2d_tina_blend_image(fb, img, &dst, &r, 0xff);

  dst = rect_init(60, 60, img->w, img->h);
  r = rect_init(0, 0, 20, 20);
  g2d_tina_blend_image(fb, img, &dst, &r, 0xff);
  
  dst = rect_init(80, 60, img->w, img->h);
  r = rect_init(20, 20, 20, 20);
  g2d_tina_blend_image(fb, img, &dst, &r, 0xff);
  
  dst = rect_init(100, 60, img->w, img->h);
  r = rect_init(40, 40, 20, 20);
  g2d_tina_blend_image(fb, img, &dst, &r, 0xff);

  r = rect_init(0, 0, img->w, img->h);
  gray = color_init(0xd0, 0xd0, 0xd0, 0x10);
  g2d_tina_fill_rect(img, &r, gray); 
  dst = rect_init(0, 120, img->w, img->h);
  r = rect_init(0, 0, 60, 60);
  g2d_tina_blend_image(fb, img, &dst, &r, 0xff);
  
  gray = color_init(0xd0, 0xd0, 0xd0, 0xff);
  g2d_tina_fill_rect(img, &r, gray); 
  dst = rect_init(60, 120, img->w, img->h);
  r = rect_init(0, 0, 60, 60);
  g2d_tina_blend_image(fb, img, &dst, &r, 0xff);
#endif
}

int main(void) {
  platform_prepare();

  log_set_log_level(LOG_LEVEL_DEBUG);

  if (tk_g2d_init() == RET_OK) {
    g2d_tina_mem_t* mem = g2d_tina_mem_create(1024 * 1024);
    if (mem != NULL) {
      printf("%p %u\n", g2d_tina_mem_get_vaddr(mem), g2d_tina_mem_get_size(mem));
      g2d_tina_mem_destroy(mem);
    }
    graphic_buffer_t* buffer = graphic_buffer_tina_create(100, 100, BITMAP_FMT_RGBA8888, 400);
    if (buffer != NULL) {
      printf("buffer=%p\n", buffer);
      graphic_buffer_destroy(buffer);
    }

    bitmap_t* fb = bitmap_create_ex(400, 400, 4 * 400, BITMAP_FMT_RGBA8888);
    //bitmap_t* img = bitmap_create_ex(100, 100, 400, BITMAP_FMT_RGBA8888);
    //bitmap_t* img = bitmap_create_ex(100, 100, 200, BITMAP_FMT_RGB565);
    //bitmap_t* img = bitmap_create_ex(100, 100, 200, BITMAP_FMT_BGR565);
    //bitmap_t* img = bitmap_create_ex(100, 100, 200, BITMAP_FMT_RGB888);
    //bitmap_t* img = bitmap_create_ex(100, 100, 200, BITMAP_FMT_BGR888);
    bitmap_t* img = bitmap_create_ex(100, 100, 200, BITMAP_FMT_ABGR8888);
    if (fb != NULL && img != NULL) {
      printf("bitmap=%p %p\n", fb, img);
      test_fill_rect(fb, img);
      bitmap_save_png(fb, "/tmp/fb.png");
      bitmap_save_png(img, "/tmp/img.png");
    }
    
    bitmap_destroy(fb);
    bitmap_destroy(img);
  }
  tk_g2d_deinit();
  return 0;
}
