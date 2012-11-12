#include time.mk
include files.mk

ifeq (,$(findstring clean,$(MAKECMDGOALS)))
 ifeq (,$(findstring static,$(MAKECMDGOALS)))
  ifeq (,$(findstring shared,$(MAKECMDGOALS)))
   ifeq (,$(findstring objects,$(MAKECMDGOALS)))
    ifeq (,$(findstring all,$(MAKECMDGOALS)))
     ifeq (,$(findstring exe,$(MAKECMDGOALS)))
      $(error Invalid make command goals. Valid commands are make exe, make shared, make static and make clean.)
     endif
    endif
   endif
  endif
 endif
endif

THIS 	     := $(abspath $(CURDIR)/Makefile)
LINKMODES    := static shared exe
TARGET_NAME  := sisu
TARGET_LIBS  := rt pthread
SOURCE_DIRS  := ../Source ../Tests
INCLUDE_DIRS := ../Source
SOURCE_EXT   := cpp
OBJ_EXT	     := o
OBJDIR	     := $(call coalesceDirectory,../Objects)

# Set up targets
ifneq (,$(findstring exe,$(LINKMODES)))
TARGET_EXE_NAME := $(TARGET_NAME)
SOURCE_DIRS 	+= $(abspath ../${TARGET_NAME}Main)
endif

ifneq (,$(findstring static,$(LINKMODES)))
TARGET_STATICLIB_NAME 	:= lib$(TARGET_NAME).a
endif

ifneq (,$(findstring shared,$(LINKMODES)))
TARGET_SHAREDLIB_NAME 	:= lib$(TARGET_NAME).so
LDFLAGS 		+= -fPIC
endif

TARGETS := $(TARGET_EXE_NAME) $(TARGET_STATICLIB_NAME) $(TARGET_SHAREDLIB_NAME)

INCLUDE_PATHS := $(call prefixAllPaths,$(INCLUDE_DIRS),-I)
CXXFLAGS      += -Werror -O3 
CXXFLAGS      += $(INCLUDE_PATHS)
CXX_OBJ_FLAG  ?= -o
CXX_FILE_FLAG ?= -c

PHONY_OBJECTS	     := $(call coalesceObjects,$(OBJDIR),$(call getFiles,$(SOURCE_DIRS),$(SOURCE_EXT)))
LINK_LIBRARIES 	     := $(call prefixAll,$(TARGET_LIBS),-l)
BUILD_COMMAND_EXE    = $(CXX) $(LINK_LIBRARIES) $(CXX_OBJ_FLAG) $(TARGET_NAME) $(OBJECTS) $(LDFLAGS) > /dev/null
BUILD_COMMAND_STATIC = ar rvs lib$(TARGET_NAME).a $(OBJECTS) > /dev/null
BUILD_COMMAND_SHARED = $(CXX) -shared $(LINK_LIBRARIES) $(CXX_OBJ_FLAG) lib$(TARGET_NAME).so $(OBJECTS) > /dev/null

getSource = $(subst Objects/,,$(patsubst %.$(1),%.$(2),$(3)))

%.cpp: 
	$(info o hi $@)
	ln -s $@ $(subst Objects/,,$(patsubst %.o,%.cpp,$@))

%.o: %.cpp
	$(info OBJECT = $@)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(CXX_FILE_FLAG) $(subst Objects/,,$(patsubst %.o,%.cpp,$@)) $(CXX_OBJ_FLAG) $@

$(TARGET_STATICLIB_NAME): $(PHONY_OBJECTS)
	$(eval OBJECTS = $(call getFiles,$(OBJECTS_DIR),$(OBJ_EXT)))
	$(info $(BUILD_COMMAND_STATIC) = $@)
	$(shell $(BUILD_COMMAND_STATIC))
	$(shell touch $@)

$(TARGET_SHAREDLIB_NAME): $(PHONY_OBJECTS)
	$(eval OBJECTS = $(call getFiles,$(OBJECTS_DIR),$(OBJ_EXT)))
	$(info $(BUILD_COMMAND_SHARED) = $@)
	$(shell $(BUILD_COMMAND_SHARED))
	$(shell touch $@)

$(TARGET_EXE_NAME): $(PHONY_OBJECTS)
	$(eval OBJECTS = $(call getFiles,../Objects,$(OBJ_EXT)))
	$(info $(BUILD_COMMAND_EXE) = $@)
	$(shell $(BUILD_COMMAND_EXE))
	$(shell touch $@)

.PHONY: all objects static exe shared

objects: $(PHONY_OBJECTS)
static: $(TARGET_STATICLIB_NAME)
exe: $(TARGET_EXE_NAME)
shared: $(TARGET_SHAREDLIB_NAME)
all: exe static objects shared

clean:
	$(call toTheVoid,$(TARGET_EXE_NAME))
	$(call toTheVoid,$(TARGET_STATICLIB_NAME))
	$(call toTheVoid,$(TARGET_SHAREDLIB_NAME))
	$(call toTheVoid,../Objects)

