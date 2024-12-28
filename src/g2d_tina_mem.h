/**
 * File:   g2d_tina_mem.h
 * Author: AWTK Develop Team
 * Brief:  hardware 2d allocator
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

#ifndef TK_G2D_TINA_MEM_H
#define TK_G2D_TINA_MEM_H

#include "tkc/types_def.h"
#include "include/ion_head.h"
#include "include/g2d_driver_enh.h"

BEGIN_C_DECLS

struct _g2d_tina_mem_t;
typedef struct _g2d_tina_mem_t g2d_tina_mem_t;

ret_t tk_g2d_tina_mem_init(void);

g2d_tina_mem_t* g2d_tina_mem_create(uint32_t size);
g2d_tina_mem_t* g2d_tina_mem_create_with_data(void* vaddr, void* paddr, uint32_t size);
ret_t g2d_tina_mem_flush(g2d_tina_mem_t* mem);
void* g2d_tina_mem_get_vaddr(g2d_tina_mem_t* mem);
void* g2d_tina_mem_get_paddr(g2d_tina_mem_t* mem);
uint32_t g2d_tina_mem_get_size(g2d_tina_mem_t* mem);
ret_t g2d_tina_mem_init_image(g2d_image_enh* image, g2d_tina_mem_t* mem);
ret_t g2d_tina_mem_destroy(g2d_tina_mem_t* mem);

ret_t g2d_tina_mem_deinit(void);

END_C_DECLS

#endif /*TK_G2D_TINA_MEM_H*/
