getFiles                = $(shell find $(1) -type f -name \*.$(2) -printf \%h/%f\\n)

stripBaseFolder         = $(subst $(abspath $(PWD)/..)/,,$(abspath $(1)))

coalesceFiles           = $(foreach dir,$(1),$(foreach ext,$(2),$(call getFiles,$(dir),$(ext))))

coalesceObjects         = $(foreach srcfile,$(2), \
				$(abspath $(1)/$(call stripBaseFolder,\
							$(patsubst %.cpp,%.o,$(srcfile)))))

coalesceDirectory	= $(shell mkdir -p $(abspath $(1)) && echo $(abspath $(1)))

toTheVoid		= $(shell rm -rf $(1))

prefixAll		= $(strip $(foreach lib,$(1),$(2)$(lib)))
suffixAll		= $(strip $(foreach lib,$(1),$(lib)$(2)))

prefixAllPaths		= $(strip $(foreach lib,$(1),$(2)$(abspath $(lib))))
suffixAllPaths		= $(strip $(foreach lib,$(1),$(abspath $(lib))$(2)))

getObjectSource		= $(patsubst %.o,%.cpp,$(1))

touchObjectSource	= $(shell touch $(call getObjectSource,$(1)))

coalesceSources 	= $(foreach dir,$(1),\
				$(foreach ext,$(2),\
					$(call getFiles,$(dir),$(ext))))
