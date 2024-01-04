CC:=$(shell which gcc)

CFLAGS_COMMON:=-Wall -Werror -Wextra -pedantic -fsanitize=address -std=gnu11
LDFLAGS:=-lcurl -lsqlite3

SRC_DIR:=src
SRC:=$(wildcard $(SRC_DIR)/*.c)

PROD_OUT_DIR:=/var/www/cgi-bin
# PROD_OUT:=moviedb

DEV_OUT_DIR:=./out
# DEV_OUT:=dev-moviedb

VALID_TARGETS:=build clean help

BUILD_TYPE:=dev # default build
EXECUTABLES:=$(SRC:$(SRC_DIR)/%.c=%)

ifeq ($(BUILD_TYPE), prod)
  CFLAGS:=$(CFLAGS_COMMON) -DPROD
	BUILD:=$(PROD_OUT_DIR)
else
	CFLAGS:=$(CFLAGS_COMMON) -DDEV -g
	BUILD:=$(DEV_OUT_DIR)
endif

all: $(EXECUTABLES)

$(EXECUTABLES): % : $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -o $(BUILD)/$@ $< $(LDFLAGS)

.PHONY:
clean:
	$(info Removing $(BUILD))
	@rm -f $(BUILD)

.PHONY:
help: ;
	$(info Valid targets: $(VALID_TARGETS))
