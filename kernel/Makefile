TARGET = zerovsh_patcher
OBJS = main.o import.o blacklist.o resolver.o hook.o
INCDIR =
EXTRA_WARNS= -Wextra -Wfloat-equal -Wundef -Wshadow -Wpointer-arith -Wwrite-strings -Wunreachable-code
CFLAGS = -O2 -G0 -Wall -std=c99 $(EXTRA_WARNS)  

ifeq ($(DEBUG), 1)
CFLAGS+=-DDEBUG
endif

ifeq ($(USE_EF0), 1)
CFLAGS+=-DUSE_EF0
endif

ASFLAGS = $(CFLAGS)

BUILD_PRX = 1
PRX_EXPORTS = exports.exp

PSP_FW_VERSION = 371

LDFLAGS = -nostartfiles

USE_KERNEL_LIBS=1
USE_KERNEL_LIBC=1

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak