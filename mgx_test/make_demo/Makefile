CROSS_COMPILE =
AS      = $(CROSS_COMPILE)as
LD      = $(CROSS_COMPILE)ld
CC      = $(CROSS_COMPILE)g++
CPP     = $(CC) -E
AR      = $(CROSS_COMPILE)ar
NM      = $(CROSS_COMPILE)nm

STRIP       = $(CROSS_COMPILE)strip
OBJCOPY     = $(CROSS_COMPILE)objcopy
OBJDUMP     = $(CROSS_COMPILE)objdump

CFLAGS := -Wall -O2 -std=c++11


SRCS = $(wildcard *.cpp)
OBJS = $(SRCS:.cpp=.o)
DEPS = $(SRCS:.cpp=.d)

BIN =test

export BUILD_ROOT = $(shell pwd)
export INCLUDE_PATH = $(BUILD_ROOT)/inc

all: $(DEPS) $(OBJS) $(BIN)

#include $(DEPS)

$(BIN):$(OBJS)
	$(CC) $(CFLAGS)  -o $@ $^

$(OBJS):$(SRCS)
	$(CC) $(CFLAGS)  -I$(INCLUDE_PATH) -o $@ -c $(filter %.cpp, $^)

$(DEPS): $(SRCS)
	$(CC) $(CFLAGS) -I$(INCLUDE_PATH) -MM $^ >> $@

PHONY += clean
clean:
	rm -f $(shell find -name "*.o") $(BIN)

PHONY += distclean
distclean:
	rm -f $(shell find -name "*.o") $(BIN)
	rm -f $(shell find -name "*.d")

.PHONY: $(PHONY)
