CXXFLAGS       += $(INCLUDE_PATHS)
PHONY_OBJECTS  := $(call coalesceObjects,$(OBJDIR),$(call getFiles,$(SOURCE_DIRS),$(SOURCE_EXT))) 
LINK_LIBRARIES := $(call prefixAll,$(TARGET_LIBS),-l)
INCLUDE_PATHS  := $(call prefixAllPaths,$(INCLUDE_DIRS) $(SOURCE_DIRS),-I) 
DEPFLAGS        = -MT $(cpp_file) -MMD -MP -MF $*.dep

buildCommandExe     = $(shell $(CXX) $(CXX_OBJ_FLAG) $(BIN_DIR)/$(1) $(OBJECTS) $(LDFLAGS) $(LINK_LIBRARIES) > $(NIL)) 
buildCommandStatic  = $(shell $(AR) rvs $(LIB_DIR)/$(1) $(OBJECTS) > $(NIL)) 
buildCommandShared  = $(shell $(CXX) -shared $(CXX_OBJ_FLAG) $(LIB_DIR)/$(1) $(OBJECTS) $(LDFLAGS) $(LINK_LIBRARIES) > $(NIL))
coalesceEntryPoints = $(call coalesceObjects,$(OBJDIR),$(call getFiles,$(1),$(SOURCE_EXT)))

#absolute hack
BUILD_PREFIX_DIR := $(subst build,,$(BUILD_DIR))

$(TARGET_STATICLIB_NAME): $(PHONY_OBJECTS)
	$(eval OBJECTS = $(PHONY_OBJECTS))
	$(eval target = $(shell basename $@))
	$(OUT)$(call buildCommandStatic,$(target))

$(TARGET_SHAREDLIB_NAME): $(PHONY_OBJECTS)
	$(eval target = $(shell basename $@))
	$(eval OBJECTS = $(PHONY_OBJECTS))
	$(OUT)$(call buildCommandShared,$(target))

$(TARGET_EXE_NAME): $(PHONY_OBJECTS)
	$(eval target = $(shell basename $@))
	$(eval ENTRY_POINT = $(call coalesceEntryPoints,../$(target)Main))
	$(shell mkdir -p $(dir $(ENTRY_POINT)))
	$(shell $(CXX) $(CXXFLAGS) $(DEPFLAGS) $(CXX_FILE_FLAG) ../$(target)Main/main_sisu.cpp $(CXX_OBJ_FLAG) $(ENTRY_POINT))
	$(eval OBJECTS = $(PHONY_OBJECTS) $(ENTRY_POINT))
	$(OUT)$(call buildCommandExe,$(target))

clean:
	$(call toTheVoid,$(BUILD_DIR))

$(BUILD_PREFIX_DIR)/%.cpp:
	$(info Possibly missing file: $@?)

$(BUILD_DIR)/obj/%.cpp: $(BUILD_PREFIX_DIR)/%.cpp
	$(eval cpp_file = $(subst $(META_DIR),,$*.cpp))
	$(info CPP += $(cpp_file))

# original symlink strategy does not work on M$
%.dep: %.cpp
	$(OUT)mkdir -p $(@D)
	$(eval cpp_file = $(subst $(META_DIR),,$*.cpp))
	$(OUT)$(CXX) $(CXXFLAGS) $(DEPFLAGS) -MM $(cpp_file) > $*.dep

%.o: %.dep %.cpp
	$(eval cpp_file = $(subst $(META_DIR),,$*.cpp))
	$(OUT)$(CXX) $(CXXFLAGS) $(DEPFLAGS) $(CXX_FILE_FLAG) $(cpp_file) $(CXX_OBJ_FLAG) $@

.PHONY: all objects static exe shared verbose

objects: $(PHONY_OBJECTS)
static:  $(TARGET_STATICLIB_NAME)
exe:	 $(TARGET_EXE_NAME)
shared:  $(TARGET_SHAREDLIB_NAME)
all: 	 objects static shared exe
libs: 	 objects static shared
verbose:

.PRECIOUS: %.dep

-include $(PHONY_OBJECTS:.o=.dep)
