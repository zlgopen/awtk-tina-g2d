/**
 * File:   graphic_buffer_tina.h
 * Author: AWTK Develop Team
 * Brief:  graphic_buffer_tina
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

#ifndef TK_GRAPHIC_BUFFER_TINA_H
#define TK_GRAPHIC_BUFFER_TINA_H

#include "base/graphic_buffer.h"

BEGIN_C_DECLS

g2d_tina_mem_t* graphic_buffer_tina_get_mem(graphic_buffer_t* buffer);

END_C_DECLS

#endif /*TK_GRAPHIC_BUFFER_TINA_H*/
