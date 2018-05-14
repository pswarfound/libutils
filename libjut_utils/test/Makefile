PRJ_ROOT = ../..
include $(PRJ_ROOT)/NR-RULES

GLOBAL_PREFIX ?= ERROR
GLOBAL_CFLAG ?= ERROR
GLOBAL_CFLAG ?= ERROR

CPPFLAGS = $(GLOBAL_CFLAG)
LDFLAGS = $(GLOBAL_LDFLAG)

VPATH += ./
VPATH += ../
VPATH += ../libini
CPPFLAGS += -I../
CPPFLAGS += -I./

CPPFLAGS += -I../libini

CPPFLAGS += -Wall -Os -g
LDFLAGS += -lpthread -lrt -lstdc++

TARGET = test

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
STRIP = $(GLOBAL_PREFIX)strip

%.d: %.c
	@set -e; rm -f $@; 
	@$(CC) -MM $(CPPFLAGS) $< > $@.$$$$;
	@sed 's,\($(notdir $*)\)\.o[ :]*,$*\.o $@ : ,g' < $@.$$$$ > $@;
	@ rm -f $@.$$$$

%.d: %.cpp
	@set -e; rm -f $@;
	@$(CXX) -MM $(CPPFLAGS) $< >  $@.$$$$;
	@sed 's,\($(notdir $*)\)\.o[ :]*,$*\.o $@ : ,g' < $@.$$$$ > $@;
	@ rm -f $@.$$$$


$(TARGET): $(OBJS)
	@$(CC) $(OBJS) $(LDFLAGS) -o $@

all: $(TARGET)

clean:
	@-rm -f $(TARGET) $(OBJS) $(DEPS)

