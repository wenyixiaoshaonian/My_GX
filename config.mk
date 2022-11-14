export BUILD_ROOT = $(shell pwd)

export INCLUDE_PATH = $(BUILD_ROOT)/include 
export INCLUDE_SQL_PATH = $(BUILD_ROOT)/mysql/inc

export BIN = mgx

BUILD_DIR = $(BUILD_ROOT)/signal/  \
			$(BUILD_ROOT)/net/ \
			$(BUILD_ROOT)/conet/ \
			$(BUILD_ROOT)/process/ \
			$(BUILD_ROOT)/misc/ \
			$(BUILD_ROOT)/bussiness/ \
			$(BUILD_ROOT)/http/     \
			$(BUILD_ROOT)/app/     \

export DEBUG = false

export USE_HTTP = true

export USE_REDIS = false

export USE_CO = true
