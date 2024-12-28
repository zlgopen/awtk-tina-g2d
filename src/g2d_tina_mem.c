/**
 * File:   g2d_tina_mem.c
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
#include "g2d_tina_mem.h"

static int s_ion_fd;

ret_t tk_g2d_tina_mem_init(void) {
  s_ion_fd = open("/dev/ion", O_RDONLY | O_CLOEXEC);
  goto_error_if_fail(s_ion_fd > 0);

  return RET_OK;
error:
  return RET_FAIL;
}

static int is_iommu_enabled(void) {
  struct stat iommu_sysfs;
  if (stat("/sys/class/iommu", &iommu_sysfs) == 0 && S_ISDIR(iommu_sysfs.st_mode))
    return 1;
  else
    return 0;
}

typedef struct _g2d_tina_mem_t {
  struct ion_info ion;
  int fd;
  void* vaddr;
  void* paddr;
  uint32_t size;
} g2d_tina_mem_t;

void* g2d_tina_mem_get_vaddr(g2d_tina_mem_t* mem) {
  return_value_if_fail(mem != NULL, NULL);

  return mem->vaddr;
}

void* g2d_tina_mem_get_paddr(g2d_tina_mem_t* mem) {
  return_value_if_fail(mem != NULL, NULL);

  return mem->paddr;
}

ret_t g2d_tina_mem_flush(g2d_tina_mem_t* mem) {
  int fd = 0;
  sunxi_cache_range range;
  return_value_if_fail(mem != NULL, RET_BAD_PARAMS);

  fd = mem->fd;
  range.start = (long)(mem->vaddr);
  range.end = range.start + mem->size;

  /*flush总是失败*/
  (void)fd;
#if 0
  if (ioctl(fd, ION_IOC_SUNXI_FLUSH_ALL, 0) < 0) {
    printf("ION_IOC_SUNXI_FLUSH_ALL failed\n");
    return RET_FAIL;
  }
  if (ioctl(fd, ION_IOC_SUNXI_FLUSH_RANGE, &range) < 0) {
    printf("ION_IOC_SUNXI_FLUSH_RANGE failed\n");
    return RET_FAIL;
  }
#endif

  return RET_OK;
}

uint32_t g2d_tina_mem_get_size(g2d_tina_mem_t* mem) {
  return_value_if_fail(mem != NULL, 0);

  return mem->size;
}

g2d_tina_mem_t* g2d_tina_mem_create_with_data(void* vaddr, void* paddr, uint32_t size) {
  g2d_tina_mem_t* mem = NULL;
  return_value_if_fail(vaddr != NULL, NULL);
  mem = TKMEM_ZALLOC(g2d_tina_mem_t);
  return_value_if_fail(mem != NULL, NULL);

  mem->vaddr = vaddr;
  mem->paddr = paddr;
  mem->size = size;

  return mem;
}

g2d_tina_mem_t* g2d_tina_mem_create(uint32_t size) {
  int ret = -1;
  g2d_tina_mem_t* mem = NULL;
  int ion_fd = s_ion_fd;
  struct ion_allocation_data data;
  struct ion_info* ion = NULL;
  return_value_if_fail(ion_fd > 0, NULL);
  mem = TKMEM_ZALLOC(g2d_tina_mem_t);
  return_value_if_fail(mem != NULL, NULL);

  ion = &(mem->ion);
  /* alloc buffer */
  if (is_iommu_enabled())
    data.heap_id_mask = ION_SYSTEM_HEAP_MASK;
  else
    data.heap_id_mask = ION_DMA_HEAP_MASK;

  data.len = size;
  data.flags = ION_FLAG_CACHED;

  printf("%s(%d): ION_HEAP_TYPE 0x%x\n", __func__, __LINE__, data.heap_id_mask);

  ret = ioctl(ion_fd, ION_IOC_ALLOC, &data);
  if (ret < 0) {
    log_warn("%s(%d): ION_IOC_ALLOC err, ret = %d\n", __func__, __LINE__, ret);
    data.fd = -1;
    goto out;
  }
  printf("%s(%u): ION_IOC_ALLOC succes, dmabuf-fd = %d, size = %d\n", __func__, __LINE__, ret,
         size);

  ion->virt_addr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, data.fd, 0);

  if (ion->virt_addr == MAP_FAILED) {
    log_warn("%s(%d): mmap err, ret %p\n", __func__, __LINE__, ion->virt_addr);
    data.fd = -1;
  } else {
    log_info("ion->virt_addr=%p\n", ion->virt_addr);
  }

  ion->alloc_data = data;
  mem->size = size;
  mem->fd = data.fd;
  mem->vaddr = ion->virt_addr;

  return mem;
out:
  return NULL;
}

ret_t g2d_tina_mem_destroy(g2d_tina_mem_t* mem) {
  printf("g2d_tina_mem_destroy: %p %u\n", mem->vaddr, mem->size);

  if (mem->fd > 0) {
    munmap(mem->vaddr, mem->size);
    close(mem->fd);
  }

  memset(mem, 0x00, sizeof(*mem));
  TKMEM_FREE(mem);
  return RET_OK;
}

ret_t g2d_tina_mem_deinit(void) {
  if (s_ion_fd > 0) {
    close(s_ion_fd);
  }
  s_ion_fd = -1;

  return RET_OK;
}

ret_t g2d_tina_mem_init_image(g2d_image_enh* image, g2d_tina_mem_t* mem) {
  void* paddr = g2d_tina_mem_get_paddr(mem);
  return_value_if_fail(image != NULL && mem != NULL, RET_BAD_PARAMS);

  if (paddr != NULL) {
    image->use_phy_addr = TRUE;
    image->laddr[0] = (__u32)(paddr);
    return RET_OK;
  } else if (mem->fd > 0) {
    image->fd = mem->fd;
    return RET_OK;
  } else {
    return RET_FAIL;
  }
}
