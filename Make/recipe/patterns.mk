CXXFLAGS             += $(INCLUDE_PATHS)
PHONY_OBJECTS        := $(call coalesceObjects,$(OBJDIR),$(call getFiles,$(SOURCE_DIRS),$(SOURCE_EXT)))
LINK_LIBRARIES       := $(call prefixAll,$(TARGET_LIBS),-l)
INCLUDE_PATHS        := $(call prefixAllPaths,$(INCLUDE_DIRS) $(SOURCE_DIRS),-I)

buildCommandExe     = $(shell $(CXX) $(CXX_OBJ_FLAG) $(BIN_DIR)/$(1) $(OBJECTS) $(LDFLAGS) $(LINK_LIBRARIES) > $(NIL))
buildCommandStatic  = $(shell $(AR) rvs $(LIB_DIR)/lib$(1).a $(OBJECTS) > $(NIL))
buildCommandShared  = $(shell $(CXX) -shared $(LINK_LIBRARIES) $(CXX_OBJ_FLAG) $(LIB_DIR)/lib$(1).so $(OBJECTS) > $(NIL))
coalesceEntryPoints = $(call coalesceObjects,$(OBJDIR),$(call getFiles,$(1),$(SOURCE_EXT)))


$(TARGET_STATICLIB_NAME): $(PHONY_OBJECTS)
		$(eval OBJECTS = $(PHONY_OBJECTS))\
		$(eval target = $(shell basename $@))\
		$(OUT)$(call buildCommandStatic,$(target))

$(TARGET_SHAREDLIB_NAME): $(PHONY_OBJECTS)
		$(eval target = $(shell basename $@))\
		$(eval OBJECTS = $(PHONY_OBJECTS))\
		$(OUT)$(call buildCommandShared,$(target))

$(TARGET_EXE_NAME): $(PHONY_OBJECTS)
	$(eval target = $(shell basename $@))\
	$(info Building target = $(shell basename $@))\
	$(eval ENTRY_POINT = $(call coalesceEntryPoints,../$(target)Main))\
	$(shell mkdir -p $(dir $(ENTRY_POINT)))\
	$(info $(CXX) $(CXXFLAGS) $(CXX_FILE_FLAG) ../$(target)Main/main_sisu.cpp $(CXX_OBJ_FLAG) $(ENTRY_POINT))\
	$(shell $(CXX) $(CXXFLAGS) $(CXX_FILE_FLAG) ../$(target)Main/main_sisu.cpp $(CXX_OBJ_FLAG) $(ENTRY_POINT))\
	$(eval OBJECTS = $(PHONY_OBJECTS) $(ENTRY_POINT))\
	$(OUT)$(call buildCommandExe,$(target))

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

