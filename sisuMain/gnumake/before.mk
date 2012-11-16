include $(SISU)/time.mk
include $(SISU)/files.mk
include $(SISU)/asserts.mk

NIL           := /dev/null
SOURCE_EXT    := cpp
OBJ_EXT       := o
OBJ_META      := obj
BIN_META      := bin
LIB_META      := lib
BUILD_META    := build
CXX_OBJ_FLAG  ?= -o
CXX_FILE_FLAG ?= -c
CXXFLAGS      += -Werror -O3
META_DIR      := $(BUILD_META)/$(OBJ_META)/
BUILD_DIR     := $(call coalesceDirectory,../$(BUILD_META))
OBJDIR        := $(call coalesceDirectory,$(BUILD_DIR)/$(OBJ_META))
BIN_DIR       := $(call coalesceDirectory,$(BUILD_DIR)/$(BIN_META))
LIB_DIR       := $(call coalesceDirectory,$(BUILD_DIR)/$(LIB_META))

