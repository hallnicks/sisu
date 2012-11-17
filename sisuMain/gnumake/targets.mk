ifneq (,$(findstring exe,$(LINKMODES)))
TARGET_EXE_NAME := $(BIN_DIR)/$(TARGET_NAME)
SOURCE_DIRS     += $(abspath ../${TARGET_NAME}Main)
endif

ifneq (,$(findstring static,$(LINKMODES)))
TARGET_STATICLIB_NAME   := $(LIB_DIR)/lib$(TARGET_NAME).a
endif

ifneq (,$(findstring shared,$(LINKMODES)))
TARGET_SHAREDLIB_NAME := $(LIB_DIR)/lib$(TARGET_NAME).so
LDFLAGS               += -fPIC
endif

TARGETS := $(TARGET_EXE_NAME) $(TARGET_STATICLIB_NAME) $(TARGET_SHAREDLIB_NAME)

