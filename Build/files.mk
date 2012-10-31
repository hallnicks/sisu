getFiles 	= $(shell find $(1) -type f -name \*.$(2) -printf \%h/%f\\n)
stripBaseFolder = $(subst $(abspath $(PWD)/..)/,,$(abspath $(1)))

