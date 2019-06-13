CC = gcc
ROOT_DIR:=$(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
JNI_INCLUDES = $(JAVA_HOME)/include $(JAVA_HOME)/include/linux
CFLAGS = -O3 -DNDEBUG -fPIC
LINK_FLAGS = -fPIC -O3 -DNDEBUG -shared -Wl,--whole-archive libpmem.a libvmem.a -Wl,--no-whole-archive
C_BUILD_DIR = build
ALL_C_SOURCES = $(wildcard $(ROOT_DIR)/*.c)
ALL_OBJ = $(addprefix $(C_BUILD_DIR)/, $(notdir $(ALL_C_SOURCES:.c=.o)))
SO_FILE_NAME = libpmdk-x86_64.so
LIBRARIES = $(addprefix $(C_BUILD_DIR)/, $(SO_FILE_NAME))


all: $(LIBRARIES) clean

clean:
	rm -rf $(C_BUILD_DIR)

$(LIBRARIES): | $(C_BUILD_DIR)

$(C_BUILD_DIR)/%.so: $(ALL_OBJ)
	$(CC) -Wl,-soname,$@ -o $@ $(ALL_OBJ) $(LINK_FLAGS)
	mv $(C_BUILD_DIR)/$(SO_FILE_NAME) $(ROOT_DIR)/$(SO_FILE_NAME)

$(C_BUILD_DIR)/%.o: $(ROOT_DIR)/%.c
ifndef JAVA_HOME
	$(error JAVA_HOME not set)
endif
	$(CC) $(CFLAGS) $(addprefix -I, $(ROOT_DIR) $(JNI_INCLUDES)) -o $@ -c $<

$(C_BUILD_DIR):
	mkdir -p $(C_BUILD_DIR)
