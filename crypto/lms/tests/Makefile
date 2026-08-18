CC ?= cc

LMS_ROOT := ..
TARGET := lms_q0_test

CFLAGS ?= -O2
CFLAGS += -Wall -Wextra -Werror \
	-DUSE_RAM_STORAGE \
	-I$(LMS_ROOT)/inc \
	-I$(LMS_ROOT)/src \
	-I$(LMS_ROOT)/imports \
	-I$(LMS_ROOT)/winbond_stubs

SOURCES := \
	lms_q0_test.c \
	$(LMS_ROOT)/src/lms.c \
	$(LMS_ROOT)/winbond_stubs/winbond_sha256.c \
	$(LMS_ROOT)/winbond_stubs/winbond_nonce.c \
	$(LMS_ROOT)/winbond_stubs/winbond_random.c \
	$(LMS_ROOT)/winbond_stubs/winbond_key_storage.c \
	$(LMS_ROOT)/winbond_stubs/winbond_key_storage_in_ram.c \
	$(LMS_ROOT)/winbond_stubs/platform_sha256.c

.PHONY: all test clean

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) -o $@ $^

test: $(TARGET)
	./$(TARGET)

clean:
	$(RM) $(TARGET)
