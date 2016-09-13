$(foreach dir,$(SOURCE_DIRS), $(shell cp -R $(dir)/resources/* $(BIN_DIR)))
