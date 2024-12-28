# 变量定义
CC = arm-openwrt-linux-gcc
AR = arm-openwrt-linux-ar
CFLAGS = -fPIC -I../awtk/src -DHAS_STDIO -DWITH_STB_IMAGE -Wall -g
LDFLAGS= -Llib -lg2d -L../awtk-linux-fb/lib -lbase -ltkc_core 
ARFLAGS = rcs
SRC = src/graphic_buffer_tina.c src/g2d_tina.c src/g2d_tina_mem.c
OBJ = $(SRC:.c=.o)
LIBNAME = lib/libg2d.a

all: $(LIBNAME) test 

$(LIBNAME): $(OBJ)
	@rm -rf lib
	@mkdir -p lib
	$(AR) $(ARFLAGS) $@ $^

%.o: %.c mylib.h
	$(CC) $(CFLAGS) -c $< -o $@

test:
	@rm -rf bin
	@mkdir -p bin
	$(CC) tests/main.c $(CFLAGS) $(LDFLAGS) -o bin/g2d_test

clean:
	rm -f $(OBJ) $(LIBNAME)

.PHONY: all clean
	
