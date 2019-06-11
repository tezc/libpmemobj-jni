CC = gcc
ROOT_DIR:=$(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
LIBPMEMDIR:=$(ROOT_DIR)/src/main/resources/lib/

JNI_INCLUDES = $(JAVA_HOME)/include $(JAVA_HOME)/include/linux

CFLAGS = -O3 -DNDEBUG -fPIC
LINK_FLAGS = -fPIC -O3 -DNDEBUG -shared -Wl,--whole-archive $(LIBPMEMDIR)/libpmem.a $(LIBPMEMDIR)/libvmem.a -Wl,--no-whole-archive

C_SOURCE_DIR = src/main/c
TARGET_DIR = bin
C_BUILD_DIR = $(TARGET_DIR)/cbuild
BASE_CLASSPATH = $(CLASSES_DIR):lib
ARCH = $(shell $(CC) -dumpmachine | awk -F'[/-]' '{print $$1}')
ALL_C_SOURCES = $(wildcard $(C_SOURCE_DIR)/*.c)
ALL_OBJ = $(addprefix $(C_BUILD_DIR)/, $(notdir $(ALL_C_SOURCES:.c=.o)))
SO_FILE_NAME = libpmem-$(ARCH).so
LIBRARIES = $(addprefix $(C_BUILD_DIR)/, $(SO_FILE_NAME))


all: sources
sources: c
c: $(LIBRARIES)

clean:
	rm -rf $(TARGET_DIR)

$(LIBRARIES): | $(C_BUILD_DIR)
$(ALL_OBJ): | $(C_BUILD_DIR)

$(C_BUILD_DIR)/%.so: $(ALL_OBJ)
	$(CC) -Wl,-soname,$@ -o $@ $(ALL_OBJ) $(LINK_FLAGS)
	cp $(C_BUILD_DIR)/$(SO_FILE_NAME) $(ROOT_DIR)/src/main/resources/lib/$(SO_FILE_NAME)

$(C_BUILD_DIR)/%.o: $(C_SOURCE_DIR)/%.c
ifndef JAVA_HOME
	$(error JAVA_HOME not set)
endif
	$(CC) $(CFLAGS) $(addprefix -I, $(JNI_INCLUDES)) -o $@ -c $<

$(C_BUILD_DIR):
	mkdir -p $(C_BUILD_DIR)
