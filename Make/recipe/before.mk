include $(SISU)/os.mk
include $(SISU)/time.mk
include $(SISU)/files.mk
include $(SISU)/goals.mk

NIL           := /dev/null
SOURCE_EXT    := cpp
HEADER_EXT    := hpp
DEP_EXT	      := dep
OBJ_EXT       := o
OBJ_META      := obj
BIN_META      := bin
LIB_META      := lib
BUILD_META    := build
AR	      ?= ar
CXX	      ?= g++
CXX_OBJ_FLAG  ?= -o
CXX_FILE_FLAG ?= -c
#CXXFLAGS      += -Werror -O3
CXXFLAGS      += -Werror -g3
META_DIR      := $(BUILD_META)/$(OBJ_META)/
BUILD_DIR     := $(call coalesceDirectory,../$(BUILD_META))
#OBJDIR        := $(call coalesceDirectory,$(BUILD_DIR)/$(OBJ_META)) #OBJ_DIR is no longer used since windows doesn't allow symlinks
BIN_DIR       := $(call coalesceDirectory,$(BUILD_DIR)/$(BIN_META))
LIB_DIR       := $(call coalesceDirectory,$(BUILD_DIR)/$(LIB_META))

