include time.mk
getFiles = $(shell find $(1) -type f -name \*.$(2) -printf \%h/%f\\n)
stripBaseFolder = $(subst $(abspath $(PWD)/..)/,,$(abspath $(1)))

TARGET_LINKMODES = exe
TARGET_NAME := sisu
TARGET_LIBS := rt pthread
SOURCE_DIRS := ../Source
SOURCE_EXTS = cpp
OBJECTS_DIR := ../Objects
DEPEDENCIES_DIR := ../Depends

ifeq ($(SOURCE_DIRS),"")
	$(error "Source dir was empty.")
endif

TARGET_NAME_STRIPPED := $(strip $(TARGET_NAME))

ifeq ($(TARGET_NAME_STRIPPED),"")
	$(error "Target name was empty.")
endif

# Set up targets
ifneq (,$(findstring exe,$(TARGET_LINKMODES)))
TARGET_EXE_NAME := $(TARGET_NAME)
TARGETS += $(TARGET_EXE_NAME)
endif

ifneq (,$(findstring static,$(TARGET_LINKMODES)))
TARGETS += lib$(TARGET_NAME).a
endif

ifneq (,$(findstring shared,$(TARGET_LINKMODES)))
TARGETS += lib$(TARGET_NAME).so
LDFLAGS += -fPIC
endif

CXXFLAGS += -Werror -O3
LDFLAGS +=
CXX ?= clang++
CXX_OBJ_FLAG ?= -o
CXX_FILE_FLAG ?= -c

LINK_LIBRARIES += $(strip $(foreach lib,$(TARGET_LIBS),-l$(lib) ))

SOURCES := $(foreach dir,$(SOURCE_DIRS),\
		$(foreach ext,$(SOURCE_EXTS),\
			$(call getFiles,$(dir),$(ext))))

# Not actual location of obj files, for compilation recipe only.
PHONY_OBJECTS := $(foreach dir,$(SOURCE_DIRS),\
			$(subst dir,$(OBJECTS_DIR),\
				$(patsubst %.cpp,%.o,$(SOURCES))))

BUILD_COMMAND_EXE = $(CXX) $(LINK_LIBRARIES) $(CXX_OBJ_FLAG) $(TARGET_NAME) $(OBJECTS) $(LDFLAGS) > /dev/null
BUILD_COMMAND_STATIC = ar rvs lib$(TARGET_NAME).a $(OBJECTS) > /dev/null
BUILD_COMMAND_SHARED = $(CXX) -shared $(LINK_LIBRARIES) $(CXX_OBJ_FLAG) lib$(TARGET_NAME).so $(OBJECTS) > /dev/null

.PHONY: help all

help:	@echo "make all" - build exe, static and shared libs
	@echo "make exe" - build exe
	@echo "make static" - build static lib
	@echo "make shared" - build shared lib

%.o:	$(info Compiling $@ ...)
	$(eval SRC_FILE = $(patsubst %.o,%.cpp,$@))
	$(eval OBJ_DIR  = $(abspath $(OBJECTS_DIR)/$(call stripBaseFolder,$(@D)))/)
	$(eval OBJ_FILE = $(OBJ_DIR)$(notdir $@))
	$(shell test -d $OBJ_DIR || mkdir -p $(OBJ_DIR) > /dev/null)
	$(info $(CXX) $(CXXFLAGS) $(CXX_FILE_FLAG) $(SRC_FILE) $(CXX_OBJ_FLAG) $(OBJ_FILE) = $@)
	$(shell $(CXX) $(CXXFLAGS) $(CXX_FILE_FLAG) $(SRC_FILE) $(CXX_OBJ_FLAG) $(OBJ_FILE))

%.a: $(PHONY_OBJECTS)
	$(eval OBJECTS = $(call getFiles,$(OBJECTS_DIR),o))
	$(info $(BUILD_COMMAND_STATIC) = $@.a)
	$(shell $(BUILD_COMMAND_STATIC))

%.so: $(PHONY_OBJECTS)
	$(eval OBJECTS = $(call getFiles,$(OBJECTS_DIR),o))
	$(info $(BUILD_COMMAND_SHARED) = $@.so)
	$(shell $(BUILD_COMMAND_SHARED))

$(TARGET_EXE_NAME): $(PHONY_OBJECTS)
	$(eval OBJECTS = $(call getFiles,$(OBJECTS_DIR),o))
	$(info $(BUILD_COMMAND_EXE) = $@.exe)
	$(shell $(BUILD_COMMAND_EXE))

all:	$(TARGETS)

clean:  $(shell rm -rf *.o *.a *.so $(TARGET_NAME))
	$(shell rm -rf $(OBJECTS_DIR))
	$(shell rm -rf $(DEPENDENCIES_DIR))

