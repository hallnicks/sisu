
CXXFLAGS       += $(INCLUDE_PATHS)
PHONY_OBJECTS  := $(call coalesceObjects,..,$(call getFiles,$(SOURCE_DIRS),$(SOURCE_EXT)))
CLEAN_OBJECTS  := $(call coalesceObjects,..,$(call getFiles,../,$(SOURCE_EXT)))
PHONY_DEPS     := $(PHONY_OBJECTS:.o=.dep)
PHONY_SRC      := $(PHONY_OBJECTS:.o=.cpp)

#$(info PHONY_OBJECTS = $(PHONY_OBJECTS))
#$(info PHONY_DEPS    = $(PHONY_DEPS))
#$(info PHONY_SRC     = $(PHONY_SRC))

LINK_LIBRARIES        := $(call prefixAll,$(TARGET_LIBS),-l)
LINK_STATIC_LIBRARIES := $(call prefixAll,$(TARGET_STATIC_LIBS),-l)
INCLUDE_PATHS  	      := $(call prefixAllPaths,$(INCLUDE_DIRS) $(SOURCE_DIRS),-I)

buildCommandExe     = $(shell $(CXX) -Wl,-Bstatic $(LINK_STATIC_LIBRARIES) $(CXX_OBJ_FLAG) $(BIN_DIR)/$(1) $(OBJECTS) -Wl,-Bdynamic $(LDFLAGS) $(LINK_LIBRARIES) > $(NIL))

buildCommandStatic  = $(shell $(AR) rvs $(LIB_DIR)/$(1) $(OBJECTS) > $(NIL))

#TODO- Re-enable LDFLAGS for shared. Right now it is disabled since you can mix shared/static link targets to build both shared
# and static libs. We need a separate var for each like LDFLAGS_SHARED, LDFLAGS_STATIC
buildCommandShared  = $(shell $(CXX) -Wl,-Bstatic $(LINK_STATIC_LIBRARIES) -Wl,-Bdynamic -shared $(CXX_OBJ_FLAG) $(LIB_DIR)/$(1) $(OBJECTS)  $(LINK_LIBRARIES) > $(NIL))

coalesceEntryPoints = $(call coalesceObjects,$(OBJDIR),$(call getFiles,$(1),$(SOURCE_EXT)))

$(TARGET_STATICLIB_NAME): $(PHONY_OBJECTS)
	$(eval OBJECTS = $(PHONY_OBJECTS))
	$(eval target = $(shell basename $@))
	$(info STATIC += $@)
	$(OUT)$(call buildCommandStatic,$(target))

$(TARGET_SHAREDLIB_NAME): $(PHONY_OBJECTS)
	$(eval target = $(shell basename $@))
	$(eval OBJECTS = $(PHONY_OBJECTS))
	$(info SHARED += $@)
	$(OUT)$(call buildCommandShared,$(target))

$(TARGET_EXE_NAME): $(PHONY_OBJECTS)
	$(info EXE += $@)
	$(eval target = $(shell basename $@))
	$(info target = $(target))
	$(eval ENTRY_POINT = $(call coalesceEntryPoints,../$(target)Main))
	$(shell $(CXX) $(CXXFLAGS) $(CXX_FILE_FLAG) ../$(target)Main/main_sisu.cpp -Wl,-Bstatic $(LINK_STATIC_LIBRARIES) -Wl,-Bdynamic $(CXX_OBJ_FLAG) ../$(target)Main/main_sisu.o)
	$(eval OBJECTS = $(PHONY_OBJECTS) ../$(target)Main/main_sisu.o)
	$(OUT)$(call buildCommandExe,$(target))

%.dep:
	$(OUT)mkdir -p $(@D)
	$(eval cpp_file = $(subst $(META_DIR),,$*.cpp))
	$(info DEP += $(cpp_file) )
	$(OUT)$(CXX) $(CXXFLAGS) -MT ' $*.cpp' -MM $(cpp_file) > $*.dep

ifneq (,$(findstring clean,$(MAKECMDGOALS)))
	-include $(PHONY_DEPS)
endif

%.o: %.dep %.cpp
	$(eval cpp_file = $(subst $(META_DIR),,$*.cpp))
	$(info CPP += $(cpp_file))
	$(OUT)$(CXX) $(CXXFLAGS) $(CXX_FILE_FLAG) $(cpp_file) -Wl,-Bstatic $(LINK_STATIC_LIBRARIES) -Wl,-Bdynamic $(LINK_LIBRARIES) $(CXX_OBJ_FLAG) $@

depclean:
	$(foreach dep,$(PHONY_DEPS),$(shell rm -rf $(dep)))

clean: depclean
	$(foreach object,$(CLEAN_OBJECTS),$(shell rm -rf $(object)))
	$(call toTheVoid,$(BUILD_DIR))


.PHONY: all objects static exe shared verbose depends

depends: $(PHONY_DEPS) $(PHONY_SRC)
objects: depends $(PHONY_OBJECTS)
static:  depends $(TARGET_STATICLIB_NAME)
exe:	 depends $(TARGET_EXE_NAME)
shared:  depends $(TARGET_SHAREDLIB_NAME)
all: 	 depends objects static shared exe
libs: 	 depends objects static shared
verbose:

.PRECIOUS: %.dep

