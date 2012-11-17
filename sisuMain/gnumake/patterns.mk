CXXFLAGS             += $(INCLUDE_PATHS)
PHONY_OBJECTS        := $(call coalesceObjects,$(OBJDIR),$(call getFiles,$(SOURCE_DIRS),$(SOURCE_EXT)))
LINK_LIBRARIES       := $(call prefixAll,$(TARGET_LIBS),-l)
INCLUDE_PATHS        := $(call prefixAllPaths,$(INCLUDE_DIRS) $(SOURCE_DIRS),-I)
BUILD_COMMAND_EXE    = $(CXX) $(LINK_LIBRARIES) $(CXX_OBJ_FLAG) $(BIN_DIR)/$(TARGET_NAME) $(OBJECTS) $(LDFLAGS) > $(NIL)
BUILD_COMMAND_STATIC = $(AR) rvs $(LIB_DIR)/lib$(TARGET_NAME).a $(OBJECTS) > $(NIL)
BUILD_COMMAND_SHARED = $(CXX) -shared $(LINK_LIBRARIES) $(CXX_OBJ_FLAG) $(LIB_DIR)/lib$(TARGET_NAME).so $(OBJECTS) > $(NIL)

$(TARGET_STATICLIB_NAME): $(PHONY_OBJECTS)
	$(eval OBJECTS = $(call getFiles,$(OBJECTS_DIR),$(OBJ_EXT)))
	$(OUT)$(BUILD_COMMAND_STATIC)

$(TARGET_SHAREDLIB_NAME): $(PHONY_OBJECTS)
	$(eval OBJECTS = $(call getFiles,$(OBJECTS_DIR),$(OBJ_EXT)))
	$(OUT)$(BUILD_COMMAND_SHARED)

$(TARGET_EXE_NAME): $(PHONY_OBJECTS)
	$(eval OBJECTS = $(call getFiles,../$(OBJECTS_DIR),$(OBJ_EXT)))
	$(OUT)$(BUILD_COMMAND_EXE)

%.dep: %.cpp
	$(OUT)mkdir -p $(@D)
	$(OUT)$(CXX) $(CXXFLAGS) -MM $*.cpp > $*.dep

%.o: %.dep
	$(OUT)$(CXX) $(CXXFLAGS) $(CXX_FILE_FLAG) $*.cpp $(CXX_OBJ_FLAG) $@

-include $(PHONY_OBJECTS:.o=.dep)

clean:
	$(call toTheVoid,$(BUILD_DIR))

.PHONY: all objects static exe shared verbose

objects: $(PHONY_OBJECTS)
static: $(TARGET_STATICLIB_NAME)
exe: $(TARGET_EXE_NAME)
shared: $(TARGET_SHAREDLIB_NAME)
all: objects static shared exe
libs: objects static shared
verbose:

.SECONDARY:

%.cpp:
	$(OUT)mkdir -p $(@D)
	$(OUT)ln -s $(subst $(META_DIR),,$@) $@

