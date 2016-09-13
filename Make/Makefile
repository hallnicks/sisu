SISU := recipe/

include $(SISU)/before.mk

LINKMODES    := static shared exe

#Arbitrary number of "main" entry points 
#Need to optimize it such that test library links only to test 
#and runtime to runtime vice versa etc etc
TARGET_NAME  := sisu sisuTest
#TARGET_LIBS := rt pthread # for when renabled test
TARGET_LIBS  += opengl32 freeglut glew32

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
