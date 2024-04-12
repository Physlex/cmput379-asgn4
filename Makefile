# AUTHOR: Alexander Williams-Letondre, arwillia@ualberta.ca 1775685

CC := gcc
PROJECT_NAME := a4p1

CFLAGS := -std=c99 -Wall -lpthread -lc
DEBUG := -Og # Debug flag by default
OPTIM := -O2
INCLUDES := include/
BIN :=  # Root folder, unfortunately. You can replace this with build/ or bin/
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
OPTIMAL := $(BIN)$(PROJECT_NAME)_optimal

COMPILE_C := $(CC) $(C_FILES) -o $(BUILD) $(CFLAGS) $(C_LIBS) $(DEBUG)
COMPILE_O := $(CC) $(C_FILES) -o $(OPTIMAL) $(CFLAGS) $(C_LIBS) $(OPTIM)

.PHONY: $(BUILD) $(OPTIMAL)

all:
	make a4p1
	make optimal

a4p1:
	$(COMPILE_C)

optimal:
	$(COMPILE_O)

####============================================================================
## Utilities

clean:
	rm -rf *.o $(BUILD) $(OPTIMAL) *.tar *.tar.gz

tar:
	tar -cvf williams-letondre_asgn4.tar.gz *

reset:
	make clean
	make all
