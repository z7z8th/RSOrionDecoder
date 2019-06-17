######################configure##################
# OS=Windows,Linux,Android
# ARCH=x86,x86_64,arm,aarch64
# CC
# CXX
# CPPFLAGS += $(addprefix -D, $(DEFINES))
# CPPFLAGS += $(addprefix -I, $(INCDIRS))
# CFLAGS
# CXXFLAGS
# LDFLAGS += $(addprefix -L, $(LIBDIRS))
# ENABLE_SHARED=true,false

MAKEFILE_PATH := $(abspath $(lastword $(MAKEFILE_LIST)))
SRC_ROOT_DIR := $(dir $(MAKEFILE_PATH))

ifeq ($(OS), Windows_NT)
	OS=Windows
endif

MKDIR = mkdir -p
RM = rm -r
CP = cp -r

CPPFLAGS += $(addprefix -D, $(DEFINES))
ifeq ($(OS), Windows)
	CPPFLAGS += -D_WIN32_WINNT=0x600
ifeq ($(ENABLE_SHARED),true)
	CPPFLAGS += -DDLL_EXPORTS
endif
endif

#CFLAGS += -g -Wall -O3
CFLAGS += -g -O0 -Wall # -w
ifeq ($(ENABLE_SHARED),true)
	CFLAGS += -shared -fPIC -fvisibility=hidden
endif
DEPFLAGS += -MMD -MP
CXXFLAGS += $(CFLAGS) $(DEPFLAGS) -std=c++11
ARFLAGS := cr

INCDIR = include
LIBDIR = lib
SRCDIR = src demo include
BINDIR = bin
DEPDIR = 3rd
CONFDIR = etc
DISTDIR = dist
DOCDIR  = doc

TARGET = libOrionDecoder

DIRS += $(shell find $(SRCDIR) -type d ! -path '$(SRCDIR)/.*')
#DIRS += $(shell find $(SRCDIR/hw) -type d)
SRCS += $(foreach dir, $(DIRS), $(wildcard $(dir)/*.c $(dir)/*.cc $(dir)/*.cpp  $(dir)/*.cxx  $(dir)/*.C))
#OBJS := $(patsubst %.cpp, %.o, $(SRCS))
OBJS := $(addsuffix .o, $(basename $(SRCS)))
DEPS := $(addsuffix .d, $(basename $(SRCS)))
RS_LIBS := 
#RS_LIBS := -lOrionPortal -lReadFace2

$(info TARGET=$(TARGET))
$(info SRCDIR=$(SRCDIR))
$(info DIRS=$(DIRS))
$(info SRCS=$(SRCS))
$(info OBJS=$(OBJS))

INCDIRS  += $(INCDIR) $(SRC_ROOT_DIR) $(DEPDIR)/include $(DIRS) /Users/cheewing/3rd/ffmpeg/include $(SRC_ROOT_DIR)/../srs-librtmp/objs/include/
CPPFLAGS += $(addprefix -I, $(INCDIRS))

LIBDIRS += $(LIBDIR) $(DEPDIR)/lib /Users/cheewing/3rd/ffmpeg/lib $(SRC_ROOT_DIR)/../srs-librtmp/objs/lib/
LDFLAGS += $(addprefix -L, $(LIBDIRS))
ifeq ($(OS), Windows)
	LDFLAGS += -static-libgcc -static-libstdc++
endif

#common LIBS

ifeq ($(OS), Windows)
	LDFLAGS += -Wl,-Bdynamic -lwinmm -liphlpapi -lws2_32
	LDFLAGS += -Wl,-Bstatic -lstdc++ -lpthread -lm
else
ifeq ($(OS), Android)
	LDFLAGS += -Wl,-Bdynamic -llog -lm
else
	LDFLAGS += $(RS_LIBS) -lavcodec -lavfilter -lavutil -lavformat -lswresample -lswscale -l:srs_librtmp.a -Wl,-lstdc++ -lpthread -lm -ldl 
endif
endif

$(info OS=$(OS))
$(info ARCH=$(ARCH))
$(info MAKE=$(MAKE))
$(info CC=$(CC))
$(info CXX=$(CXX))
$(info CPPFLAGS=$(CPPFLAGS))
$(info CFLAGS=$(CFLAGS))
$(info CXXFLAGS=$(CXXFLAGS))
$(info LDFLAGS=$(LDFLAGS))

default: all

all: prepare $(TARGET)

prepare:
	$(MKDIR) $(BINDIR) $(LIBDIR)

$(TARGET): $(OBJS)

ifeq ($(ENABLE_SHARED),true)
# dynamic
# archive
	#$(AR) $(ARFLAGS) $(LIBDIR)/$@.a $^
ifeq ($(OS), Windows)
	$(CXX) $(CXXFLAGS) $^ -o $(LIBDIR)/$@.dll $(LDFLAGS) -Wl,--output-def,$(LIBDIR)/$@.def
else
	$(CXX) $(CXXFLAGS) $^ -o $(LIBDIR)/$@.so $(LDFLAGS)
endif
else
# executable
ifeq ($(OS), Windows)
	$(CXX) $(CXXFLAGS) $^ -o $(BINDIR)/$@.exe $(LDFLAGS)
else
	$(CXX) $(CXXFLAGS) $^ -o $(BINDIR)/$@ $(LDFLAGS)
endif
endif

clean:
	-$(RM) $(LIBDIR)/$(TARGET).so
	-$(RM) $(BINDIR)/$(TARGET)
	-$(RM) $(OBJS)
	-$(RM) $(BINDIR)
	-$(RM) $(DEPS)
	#$(RM) $(LIBDIR)

install:

uninstall:

dist:
	$(MKDIR) $(DISTDIR)
	$(CP) $(INCDIR) $(LIBDIR) $(BINDIR) $(DISTDIR)

undist:
	$(RM) $(DISTDIR)

rebuild: clean
	+make all

.PHONY: default all prepare clean install uninstall dist undist rebuild

-include $(DEPS)
