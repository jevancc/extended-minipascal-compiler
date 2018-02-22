.PHONY: all install uninstall test

CC = g++
SRCS = \
	$(SRC_DIR)/node.cc \
	$(SRC_DIR)/symtab.cc \
	$(SRC_DIR)/symtab_type.cc \
	$(SRC_DIR)/logger.cc \
	$(SRC_DIR)/codegen.cc

OBJS = $(SRCS:.cc=.o)
INCLUDES = -Iinc -Ibuild
CFLAGS = -W -std=c++11

PREFIX ?= /usr
BIN_DIR ?= $(PREFIX)/bin
LIB_DIR ?= $(PREFIX)/lib/mini-pascal
BUILD_DIR = build
SRC_DIR = src
UTIL_DIR = util

all:

install:
	mkdir -p $(BUILD_DIR)
	flex -o $(BUILD_DIR)/lex.yy.cc $(SRC_DIR)/lex.l
	yacc -o $(BUILD_DIR)/y.tab.cc --defines=$(BUILD_DIR)/y.tab.h $(SRC_DIR)/yacc.y
	$(CC) $(INCLUDES) $(CFLAGS) $(BUILD_DIR)/y.tab.cc $(SRCS) -o $(BUILD_DIR)/mini-pascal -ly -lfl \
	-Wl,-Bstatic -lboost_system -lboost_filesystem -lboost_program_options -Wl,-Bdynamic  -Wl,--as-needed
	install -m755 -d $(DESTDIR)$(BIN_DIR)
	install -m755 $(BUILD_DIR)/mini-pascal $(DESTDIR)$(BIN_DIR)
	install -m755 -d $(DESTDIR)$(LIB_DIR)
	install -m755 $(UTIL_DIR)/jasmin-2.4/jasmin.jar $(DESTDIR)$(LIB_DIR)
	rm -rf $(BUILD_DIR)

uninstall:
	rm -f $(DESTDIR)$(BIN_DIR)/mini-pascal
	rm -rf $(DESTDIR)$(LIB_DIR)

test:
	mini-pascal -h
