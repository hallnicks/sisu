# Set up targets
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

TARGETS              := $(TARGET_EXE_NAME) $(TARGET_STATICLIB_NAME) $(TARGET_SHAREDLIB_NAME)
INCLUDE_PATHS        := $(call prefixAllPaths,$(INCLUDE_DIRS) $(SOURCE_DIRS),-I)
CXXFLAGS             += $(INCLUDE_PATHS)
PHONY_OBJECTS        := $(call coalesceObjects,$(OBJDIR),$(call getFiles,$(SOURCE_DIRS),$(SOURCE_EXT)))
LINK_LIBRARIES       := $(call prefixAll,$(TARGET_LIBS),-l)
BUILD_COMMAND_EXE    = $(CXX) $(LINK_LIBRARIES) $(CXX_OBJ_FLAG) $(BIN_DIR)/$(TARGET_NAME) $(OBJECTS) $(LDFLAGS) > $(NIL)
BUILD_COMMAND_STATIC = ar rvs $(LIB_DIR)/lib$(TARGET_NAME).a $(OBJECTS) > $(NIL)
BUILD_COMMAND_SHARED = $(CXX) -shared $(LINK_LIBRARIES) $(CXX_OBJ_FLAG) $(LIB_DIR)/lib$(TARGET_NAME).so $(OBJECTS) > $(NIL)

.SECONDARY:

%.cpp:
	$(info CPP = $@)
	mkdir -p $(@D)
	ln -s $(subst $(META_DIR),,$@) $@

%.o: %.cpp
	$(info OBJECT = $@)
	$(CXX) $(CXXFLAGS) $(CXX_FILE_FLAG) $(subst $(META_DIR),,$(patsubst %.o,%.cpp,$@)) $(CXX_OBJ_FLAG) $@

$(TARGET_STATICLIB_NAME): $(PHONY_OBJECTS)
	$(eval OBJECTS = $(call getFiles,$(OBJECTS_DIR),$(OBJ_EXT)))
	$(info $(BUILD_COMMAND_STATIC) = $@)
	$(shell $(BUILD_COMMAND_STATIC))

$(TARGET_SHAREDLIB_NAME): $(PHONY_OBJECTS)
	$(eval OBJECTS = $(call getFiles,$(OBJECTS_DIR),$(OBJ_EXT)))
	$(info $(BUILD_COMMAND_SHARED) = $@)
	$(shell $(BUILD_COMMAND_SHARED))

$(TARGET_EXE_NAME): $(PHONY_OBJECTS)
	$(eval OBJECTS = $(call getFiles,../$(OBJECTS_DIR),$(OBJ_EXT)))
	$(info $(BUILD_COMMAND_EXE) = $@)
	$(shell $(BUILD_COMMAND_EXE))

clean:
	$(call toTheVoid,$(BUILD_DIR))

.PHONY: all objects static exe shared

objects: $(PHONY_OBJECTS)
static: $(TARGET_STATICLIB_NAME)
exe: $(TARGET_EXE_NAME)
shared: $(TARGET_SHAREDLIB_NAME)
all: objects static shared exe
libs: objects static shared


