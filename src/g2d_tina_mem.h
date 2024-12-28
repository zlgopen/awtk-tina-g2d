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

/**
 * @class g2d_tina_mem_t
 * 2d内存。
 */
struct _g2d_tina_mem_t;
typedef struct _g2d_tina_mem_t g2d_tina_mem_t;

/**
 * @method tk_g2d_tina_mem_init
 * 初始化2d内存。
 * @return {ret_t} 成功返回RET_OK，失败返回RET_FAIL。
 */
ret_t tk_g2d_tina_mem_init(void);

/**
 * @method g2d_tina_mem_create
 * 创建2d内存。
 * @param {uint32_t} size 内存大小。
 * @return {g2d_tina_mem_t*} 返回2d内存对象。
 */
g2d_tina_mem_t* g2d_tina_mem_create(uint32_t size);

/**
 * @method g2d_tina_mem_create_with_data
 * 创建2d内存。
 * @param {void*} vaddr 内存虚拟地址。
 * @param {void*} paddr 内存物理地址。
 * @param {uint32_t} size 内存大小。
 * @return {g2d_tina_mem_t*} 返回2d内存对象。
 */
g2d_tina_mem_t* g2d_tina_mem_create_with_data(void* vaddr, void* paddr, uint32_t size);

/**
 * @method g2d_tina_mem_flush
 * 刷新2d内存。
 * @param {g2d_tina_mem_t*} mem 2d内存对象。
 * @return {ret_t} 成功返回RET_OK，失败返回RET_FAIL。
 */
ret_t g2d_tina_mem_flush(g2d_tina_mem_t* mem);

/**
 * @method g2d_tina_mem_get_vaddr
 * 获取2d内存虚拟地址。
 * @param {g2d_tina_mem_t*} mem 2d内存对象。
 * @return {void*} 返回2d内存虚拟地址。
 */
void* g2d_tina_mem_get_vaddr(g2d_tina_mem_t* mem);

/**
 * @method g2d_tina_mem_get_paddr
 * 获取2d内存物理地址。
 * @param {g2d_tina_mem_t*} mem 2d内存对象。
 * @return {void*} 返回2d内存物理地址。
 */
void* g2d_tina_mem_get_paddr(g2d_tina_mem_t* mem);

/**
 * @method g2d_tina_mem_get_size
 * 获取2d内存大小。
 * @param {g2d_tina_mem_t*} mem 2d内存对象。
 * @return {uint32_t} 返回2d内存大小。
 */
uint32_t g2d_tina_mem_get_size(g2d_tina_mem_t* mem);

/**
 * @method g2d_tina_mem_init_image
 * 初始化2d内存图像。
 * @param {g2d_image_enh*} image 图像对象。
 * @param {g2d_tina_mem_t*} mem 2d内存对象。
 * @return {ret_t} 成功返回RET_OK，失败返回RET_FAIL。
 */
ret_t g2d_tina_mem_init_image(g2d_image_enh* image, g2d_tina_mem_t* mem);

/**
 * @method g2d_tina_mem_destroy
 * 销毁2d内存。
 * @param {g2d_tina_mem_t*} mem 2d内存对象。
 * @return {ret_t} 成功返回RET_OK，失败返回RET_FAIL。
 */
ret_t g2d_tina_mem_destroy(g2d_tina_mem_t* mem);

/**
 * @method g2d_tina_mem_deinit
 * 反初始化2d内存。
 * @return {ret_t} 成功返回RET_OK，失败返回RET_FAIL。
 */
ret_t g2d_tina_mem_deinit(void);

END_C_DECLS

#endif /*TK_G2D_TINA_MEM_H*/
