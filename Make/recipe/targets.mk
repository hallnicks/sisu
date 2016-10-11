ifneq (,$(findstring exe,$(LINKMODES)))
TARGET_EXE_NAME := $(foreach target,${TARGET_NAME},$(BIN_DIR)/$(target))
EXE_SOURCE_DIRS += $(foreach target,${TARGET_NAME},$(abspath ../$(target)Main))
endif

ifneq (,$(findstring static,$(LINKMODES)))
TARGET_STATICLIB_NAME   := $(foreach target,${TARGET_NAME},$(LIB_DIR)/lib$(target).a)
LDFLAGS               += -fPIC
endif

ifneq (,$(findstring shared,$(LINKMODES)))
TARGET_SHAREDLIB_NAME := $(foreach target,${TARGET_NAME},$(LIB_DIR)/lib$(target).so)
endif

CXXFLAGS += --std=c++11

TARGETS := $(TARGET_EXE_NAME) $(TARGET_STATICLIB_NAME) $(TARGET_SHAREDLIB_NAME)


