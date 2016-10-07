$(foreach dir,$(SOURCE_DIRS), $(shell cp -Rf $(dir)/resources $(BIN_DIR) && cp -Rf $(dir)/dll/* $(BIN_DIR) ))
