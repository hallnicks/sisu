#Deficiencies: YOu have to do a make clean to rebuild binaries. Make links shared and creates static.

SISU := recipe/

include $(SISU)/before.mk

LINKMODES    := static shared exe

#Arbitrary number of "main" entry points 
#Need to optimize it such that test library links only to test 
#and runtime to runtime vice versa etc etc
TARGET_NAME  := sisu sisuTest
TARGET_LIBS  += pthread # rt # for when renabled test

#This is now windows specific. Need to test on linux again.
TARGET_LIBS  += opengl32 freeglut glew32 Dbghelp

# for SDL
SDL_CONFIG   := sdl2-config --prefix=/usr
CXXFLAGS     += $(shell $(SDL_CONFIG) --cflags)
LDFLAGS      += $(shell $(SDL_CONFIG) --libs)

#for freeglut
FREEGLUT_FLAGS  := -Wl,--subsystem,windows
CXXFLAGS 	+= $(FREEGLUT_FLAGS)
LDFLAGS		+= $(FREEGLUT_FLAGS)

# for GLEW
CXXFLAGS     += -I/usr/include

# for GLM
CXXFLAGS     += -I/usr/local/include

SOURCE_DIRS   := ../Source ../Tests

include $(SISU)/after.mk
