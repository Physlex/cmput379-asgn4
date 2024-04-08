# AUTHOR: Alexander Williams-Letondre, arwillia@ualberta.ca 1775685

CC := gcc
PROJECT_NAME := a4p1

CFLAGS := -std=c99 -Wall -Og -DTEST_LVL=0
INCLUDES := include/
BIN := bin/
SRC := src/
TST := test/

####============================================================================
## Linking header files

C_LIBS := -I$(INCLUDES)

####============================================================================
## Collecting src files

C_FILES := $(wildcard $(SRC)*.c)
C_TESTS := $(wildcard $(TST)*.c)

####============================================================================
## Compilation

BUILD := $(BIN)$(PROJECT_NAME)
TESTS := $(BIN)$(PROJECT_NAME)_Test

COMPILE_C := $(CC) $(C_FILES) -o $(BUILD) $(CFLAGS) $(C_LIBS)
COMPILE_T := $(CC) $(C_TESTS) -o $(TESTS) $(CFLAGS) $(C_LIBS)

.PHONY: $(BUILD) $(TESTS)

all:
	make a4p1
	make tests

a4p1:
	$(COMPILE_C)

tests:
	$(COMPILE_T)

####============================================================================
## Utilities

clean:
	rm -rf *.o $(BUILD) $(TESTS) *.tar *.tar.gz

tar:
	tar -xcf * williams-letondre_asgn4.tar.gz

reset:
	make clean
	make all
