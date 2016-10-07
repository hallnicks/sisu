
CXXFLAGS       += $(INCLUDE_PATHS)
PHONY_OBJECTS  := $(call coalesceObjects,..,$(call getFiles,$(SOURCE_DIRS),$(SOURCE_EXT))) 
PHONY_DEPS     := $(PHONY_OBJECTS:.o=.dep)
PHONY_SRC      := $(PHONY_OBJECTS:.o=.cpp)

#$(info PHONY_OBJECTS = $(PHONY_OBJECTS))
#$(info PHONY_DEPS    = $(PHONY_DEPS))
#$(info PHONY_SRC     = $(PHONY_SRC))

LINK_LIBRARIES := $(call prefixAll,$(TARGET_LIBS),-l)
INCLUDE_PATHS  := $(call prefixAllPaths,$(INCLUDE_DIRS) $(SOURCE_DIRS),-I) 

buildCommandExe     = \
$(info $(CXX) $(CXX_OBJ_FLAG) $(BIN_DIR)/$(1) $(OBJECTS) $(LDFLAGS) $(LINK_LIBRARIES) > $(NIL))\
$(shell $(CXX) $(CXX_OBJ_FLAG) $(BIN_DIR)/$(1) $(OBJECTS) $(LDFLAGS) $(LINK_LIBRARIES) > $(NIL))

buildCommandStatic  = \
$(info $(AR) rvs $(LIB_DIR)/$(1) $(OBJECTS) > $(NIL))\
$(shell $(AR) rvs $(LIB_DIR)/$(1) $(OBJECTS) > $(NIL))

buildCommandShared  = \
$(info $(CXX) -shared $(CXX_OBJ_FLAG) $(LIB_DIR)/$(1) $(OBJECTS) $(LDFLAGS) $(LINK_LIBRARIES) > $(NIL))\
$(shell $(CXX) -shared $(CXX_OBJ_FLAG) $(LIB_DIR)/$(1) $(OBJECTS) $(LDFLAGS) $(LINK_LIBRARIES) > $(NIL))

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
	$(eval ENTRY_POINT = $(call coalesceEntryPoints,../$(target)Main))
	$(shell mkdir -p $(dir $(ENTRY_POINT)))
	$(shell $(CXX) $(CXXFLAGS) $(CXX_FILE_FLAG) ../$(target)Main/main_sisu.cpp $(CXX_OBJ_FLAG) $(ENTRY_POINT))
	$(eval OBJECTS = $(PHONY_OBJECTS) $(ENTRY_POINT))
	$(OUT)$(call buildCommandExe,$(target))

%.dep: 
	$(OUT)mkdir -p $(@D)
	$(eval cpp_file = $(subst $(META_DIR),,$*.cpp))
	$(info DEP += $(cpp_file) )
	$(OUT)$(CXX) $(CXXFLAGS) -MT ' $*.cpp' -MM $(cpp_file) > $*.dep
	$(OUT)sed -i 's/C:\/msys64//g' $*.dep
	$(OUT)sed -i 's/\\//g' $*.dep
	$(OUT)sed -i 's/  / /g' $*.dep
	$(OUT)sed -i ':begin;$!N;s/\n/ /;tbegin' $*.dep
	$(OUT)cat $*.dep

ifneq (,$(findstring clean,$(MAKECMDGOALS)))
	-include $(PHONY_DEPS)
endif

%.o: %.dep %.cpp
	$(eval cpp_file = $(subst $(META_DIR),,$*.cpp))
	$(info CPP += $(cpp_file))
	$(OUT)$(CXX) $(LINK_LIBRARIES) $(CXXFLAGS) $(CXX_FILE_FLAG) $(cpp_file) $(CXX_OBJ_FLAG) $@

depclean:
	$(foreach dep,$(PHONY_DEPS),$(shell rm -rf $(dep)))

clean: depclean
	$(foreach object,$(PHONY_OBJECTS),$(shell rm -rf $(object)))
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

