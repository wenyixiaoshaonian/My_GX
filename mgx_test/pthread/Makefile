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
LDFLAGS := -lpthread

SRCS = $(wildcard *.cpp)
OBJS = $(SRCS:.cpp=.o)

BIN =pthread

all:  $(OBJS) $(BIN)


$(BIN):$(OBJS)
	$(CC) $(CFLAGS)  -o $@ $^ $(LDFLAGS)

$(OBJS):$(SRCS)
	$(CC) $(CFLAGS)   -o $@ -c $^


PHONY += clean
clean:
	rm -f $(shell find -name "*.o") $(BIN)

PHONY += distclean
distclean:
	rm -f $(shell find -name "*.o") $(BIN)

.PHONY: $(PHONY)
