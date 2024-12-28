/**
 * File:   g2d_tina.h
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

#ifndef TK_G2D_TINA_H
#define TK_G2D_TINA_H

#include "tkc/rect.h"
#include "base/bitmap.h"

BEGIN_C_DECLS

/**
 * @class g2d_tina_t
 * tina linux 2d 加速。
 */

/**
 * @method tk_g2d_init
 * 初始化g2d。
 * @return {ret_t} 成功返回RET_OK，失败返回RET_FAIL。
 */
ret_t tk_g2d_init(void);

/**
 * @method tk_g2d_deinit
 * 反初始化g2d。
 * @return {ret_t} 成功返回RET_OK，失败返回RET_FAIL。
 */
ret_t tk_g2d_deinit(void);

/*public for tests */
ret_t g2d_tina_fill_rect(bitmap_t* fb, const rect_t* dst, color_t c);
ret_t g2d_tina_copy_image(bitmap_t* fb, bitmap_t* img, const rect_t* src, xy_t x, xy_t y);
ret_t g2d_tina_rotate_image(bitmap_t* fb, bitmap_t* img, const rect_t* src, lcd_orientation_t o,
                            xy_t x, xy_t y);
ret_t g2d_tina_blend_image(bitmap_t* fb, bitmap_t* img, const rect_t* dst, const rect_t* src,
                           uint8_t global_alpha);
END_C_DECLS

#endif /*TK_G2D_TINA_H*/
