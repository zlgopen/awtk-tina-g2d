/**
 * File:   graphic_buffer_tina.c
 * Author: AWTK Develop Team
 * Brief:  graphic_buffer_tina 
 *
 * Copyright (c) 2024 - 2024  Guangzhou ZHIYUAN Electronics Co.,Ltd.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * License file for more details.
 *
 */

/**
 * History:
 * ================================================================
 * 2024-12-20 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include <stdio.h>
#include "tkc/mem.h"
#include "base/bitmap.h"
#include "base/graphic_buffer.h"

#include "g2d_tina_mem.h"

/**
 * @class graphic_buffer_tina_t
 * graphic_buffer_tina
 */
typedef struct _graphic_buffer_tina_t {
  graphic_buffer_t graphic_buffer;

  uint8_t* data;
  uint32_t w;
  uint32_t h;
  uint32_t line_length;
  g2d_tina_mem_t* mem;
} graphic_buffer_tina_t;

static graphic_buffer_tina_t* graphic_buffer_tina_cast(graphic_buffer_t* buffer);

#define GRAPHIC_BUFFER_DEFAULT(buffer) graphic_buffer_tina_cast(buffer)

static bool_t graphic_buffer_tina_is_valid_for(graphic_buffer_t* buffer, bitmap_t* bitmap) {
  graphic_buffer_tina_t* b = GRAPHIC_BUFFER_DEFAULT(buffer);
  return_value_if_fail(b != NULL && bitmap != NULL, FALSE);
  if (bitmap->orientation == LCD_ORIENTATION_0 || bitmap->orientation == LCD_ORIENTATION_180) {
    return b->w == bitmap->w && b->h == bitmap->h;
  } else {
    return b->w == bitmap->h && b->h == bitmap->w;
  }
}

static uint8_t* graphic_buffer_tina_lock_for_read(graphic_buffer_t* buffer) {
  graphic_buffer_tina_t* b = GRAPHIC_BUFFER_DEFAULT(buffer);
  return_value_if_fail(b != NULL, NULL);

  return b->data;
}

static uint8_t* graphic_buffer_tina_lock_for_write(graphic_buffer_t* buffer) {
  graphic_buffer_tina_t* b = GRAPHIC_BUFFER_DEFAULT(buffer);
  return_value_if_fail(b != NULL, NULL);

  return b->data;
}

static ret_t graphic_buffer_tina_unlock(graphic_buffer_t* buffer) {
  return RET_OK;
}

static ret_t graphic_buffer_tina_attach(graphic_buffer_t* buffer, void* data, uint32_t w,
                                        uint32_t h) {
  graphic_buffer_tina_t* b = GRAPHIC_BUFFER_DEFAULT(buffer);
  return_value_if_fail(b != NULL, RET_BAD_PARAMS);

  b->w = w;
  b->h = h;
  b->data = data;
  assert(!"not supported.");

  return RET_OK;
}

static ret_t graphic_buffer_tina_destroy(graphic_buffer_t* buffer) {
  graphic_buffer_tina_t* b = GRAPHIC_BUFFER_DEFAULT(buffer);
  return_value_if_fail(b != NULL, RET_BAD_PARAMS);

  g2d_tina_mem_destroy(b->mem);
  memset(b, 0x00, sizeof(*b));
  TKMEM_FREE(b);

  return RET_OK;
}

static uint32_t graphic_buffer_tina_get_physical_width(graphic_buffer_t* buffer) {
  graphic_buffer_tina_t* b = GRAPHIC_BUFFER_DEFAULT(buffer);
  return_value_if_fail(b != NULL, 0);

  return b->w;
}

static uint32_t graphic_buffer_tina_get_physical_height(graphic_buffer_t* buffer) {
  graphic_buffer_tina_t* b = GRAPHIC_BUFFER_DEFAULT(buffer);
  return_value_if_fail(b != NULL, 0);

  return b->h;
}

static uint32_t graphic_buffer_tina_get_physical_line_length(graphic_buffer_t* buffer) {
  graphic_buffer_tina_t* b = GRAPHIC_BUFFER_DEFAULT(buffer);
  return_value_if_fail(b != NULL, 0);

  return b->line_length;
}

static const graphic_buffer_vtable_t s_graphic_buffer_tina_vtable = {
    .lock_for_read = graphic_buffer_tina_lock_for_read,
    .lock_for_write = graphic_buffer_tina_lock_for_write,
    .unlock = graphic_buffer_tina_unlock,
    .attach = graphic_buffer_tina_attach,
    .is_valid_for = graphic_buffer_tina_is_valid_for,
    .get_width = graphic_buffer_tina_get_physical_width,
    .get_height = graphic_buffer_tina_get_physical_height,
    .get_line_length = graphic_buffer_tina_get_physical_line_length,
    .destroy = graphic_buffer_tina_destroy};

graphic_buffer_t* graphic_buffer_tina_create(uint32_t w, uint32_t h, bitmap_format_t format,
                                             uint32_t line_length) {
  uint32_t size = 0;
  graphic_buffer_tina_t* buffer = NULL;
  uint32_t min_line_length = bitmap_get_bpp_of_format(format) * w;
  return_value_if_fail(w > 0 && h > 0, NULL);

  buffer = TKMEM_ZALLOC(graphic_buffer_tina_t);
  return_value_if_fail(buffer != NULL, NULL);

  buffer->line_length = tk_max(min_line_length, line_length);
  size = buffer->line_length * h;
  buffer->mem = g2d_tina_mem_create(size);

  if (buffer->mem != NULL) {
    buffer->data = g2d_tina_mem_get_vaddr(buffer->mem);
    buffer->w = w;
    buffer->h = h;
    buffer->graphic_buffer.vt = &s_graphic_buffer_tina_vtable;

    printf("graphic_buffer_tina_create ok: %u %u\n", w, h);
    return GRAPHIC_BUFFER(buffer);
  } else {
    printf("graphic_buffer_tina_create fail: %u %u\n", w, h);
    TKMEM_FREE(buffer);
    return NULL;
  }
}

/*public functions*/
graphic_buffer_t* graphic_buffer_create_with_data(const uint8_t* data, uint32_t w, uint32_t h,
                                                  bitmap_format_t format) {
  return graphic_buffer_create_with_data_ex(data, NULL, w, h, 0, format);
}

ret_t graphic_buffer_create_for_bitmap(bitmap_t* bitmap) {
  uint32_t line_length = bitmap_get_line_length(bitmap);
  return_value_if_fail(bitmap != NULL && bitmap->buffer == NULL, RET_BAD_PARAMS);

  bitmap->buffer = graphic_buffer_tina_create(bitmap->w, bitmap->h,
                                              (bitmap_format_t)(bitmap->format), line_length);
  bitmap->should_free_data = bitmap->buffer != NULL;

  return bitmap->buffer != NULL ? RET_OK : RET_OOM;
}

static graphic_buffer_tina_t* graphic_buffer_tina_cast(graphic_buffer_t* buffer) {
  return_value_if_fail(buffer != NULL && buffer->vt == &s_graphic_buffer_tina_vtable, NULL);

  return (graphic_buffer_tina_t*)(buffer);
}

g2d_tina_mem_t* graphic_buffer_tina_get_mem(graphic_buffer_t* buffer) {
  graphic_buffer_tina_t* b = graphic_buffer_tina_cast(buffer);
  return_value_if_fail(b != NULL, NULL);

  return b->mem;
}

graphic_buffer_t* graphic_buffer_create_with_data_ex(const uint8_t* virtual_data,
                                                     const uint8_t* physical_data, uint32_t w,
                                                     uint32_t h, uint32_t line_length,
                                                     bitmap_format_t format) {
  uint32_t size = 0;
  graphic_buffer_tina_t* buffer = NULL;
  uint32_t min_line_length = bitmap_get_bpp_of_format(format) * w;
  return_value_if_fail(w > 0 && h > 0, NULL);
  return_value_if_fail(virtual_data != NULL, NULL);

  buffer = TKMEM_ZALLOC(graphic_buffer_tina_t);
  return_value_if_fail(buffer != NULL, NULL);

  buffer->line_length = tk_max(min_line_length, line_length);
  size = buffer->line_length * h;
  buffer->mem = g2d_tina_mem_create_with_data((void*)virtual_data, (void*)physical_data, size);

  if (buffer->mem != NULL) {
    buffer->data = g2d_tina_mem_get_vaddr(buffer->mem);
    buffer->w = w;
    buffer->h = h;
    buffer->graphic_buffer.vt = &s_graphic_buffer_tina_vtable;

    printf("graphic_buffer_tina_create ok: %u %u\n", w, h);
    return GRAPHIC_BUFFER(buffer);
  } else {
    printf("graphic_buffer_tina_create fail: %u %u\n", w, h);
    TKMEM_FREE(buffer);
    return NULL;
  }
}
