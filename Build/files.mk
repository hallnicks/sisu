getFiles                = $(shell find $(1) -type f -name \*.$(2) -printf \%h/%f\\n)

stripBaseFolder         = $(subst $(abspath $(PWD)/..)/,,$(abspath $(1)))

coalesceFiles           = $(foreach dir,$(1),$(foreach ext,$(2),$(call getFiles,$(dir),$(ext))))

coalesceObjects         = $(foreach srcfile,$(2), \
				$(abspath $(1)/$(call stripBaseFolder,\
							$(patsubst %.cpp,%.o,$(srcfile)))))

coalesceDirectory	= $(shell test -d $(dir $(1)) || mkdir -p $(dir $(1)) > /dev/null)


toTheVoid		= $(shell test -d $(1) && rm -R $(1) || test -b $(1) && rm $(1))

prefixAll		= $(strip $(foreach lib,$(1),$(2)$(lib)))

prefixAllPaths		= $(strip $(foreach lib,$(1),$(2)$(abspath $(lib))))

getObjectSource		= $(patsubst %.o,%.cpp,$(1))

touchObjectSource	= $(shell touch $(call getObjectSource,$(1)))

coalesceSources 	= $(foreach dir,$(1),\
				$(foreach ext,$(2),\
					$(call getFiles,$(dir),$(ext))))
