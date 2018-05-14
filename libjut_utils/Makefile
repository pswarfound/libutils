
PRJ_ROOT = ../..
include $(PRJ_ROOT)/NR-RULES

GLOBAL_PREFIX ?= ERROR
GLOBAL_CFLAG ?= ERROR

CPPFLAGS = $(GLOBAL_CFLAG)

VPATH = ./
CPPFLAGS += -I./


CPPFLAGS += $(GLOBAL_CFLAG)

#libini
VPATH += ./libini
CPPFLAGS += -I./libini


TARGET = libjut_utils.a

SOURCES := $(foreach dir,$(VPATH),$(wildcard $(dir)/*))
C_SRCS   = $(filter %.c,$(SOURCES))
CPP_SRCS = $(filter %.cpp,$(SOURCES))
SRCS = $(C_SRCS) $(CPP_SRCS)

C_OBJS   = $(C_SRCS:%.c=%.o)
CPP_OBJS = $(CPP_SRCS:%.cpp=%.o)
OBJS = $(C_OBJS) $(CPP_OBJS)
DEPS = $(OBJS:.o=.d)

CC = $(GLOBAL_PREFIX)gcc
CXX = $(GLOBAL_PREFIX)g++
AR = $(GLOBAL_PREFIX)ar
RANLIB = $(GLOBAL_PREFIX)ranlib


$(TARGET): $(OBJS)
	@$(AR) r $@ $(OBJS)
	@$(RANLIB) $@

.PHONY: all clean

all: $(TARGET)

clean:
	@rm -f $(TARGET) $(OBJS) $(DEPS)

