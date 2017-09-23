# Brian Smith's Super Duper Makefile
# Change CC/CFLAGS/LDFLAGS to fit your needs
# Directory format it searchs for:
# src/ - have your .c/.cc/.cpp files in there
# include/ - have your .h/.hpp files in there
# build/ - empty folder for object files and the executable

# CHANGE THESE TO FIT YOUR NEEDS
CC = cc
CFLAGS = -c -Wall -Iinclude/
LDFLAGS =
# DONT TOUCH AFTER THIS - IT'S ALL AUTO

NAME = $(notdir $(patsubst %/,%,$(dir $(abspath $(lastword $(MAKEFILE_LIST))))))
EXT = .c
SOURCES = $(strip $(wildcard src/*.c))

ifeq ($(SOURCES),)
	EXT = .cpp
	SOURCES = $(strip $(wildcard src/*.cpp))
endif

ifeq ($(SOURCES),)
	EXT = .cc
	SOURCES = $(strip $(wildcard src/*.cc))
endif

ifeq ($(SOURCES),)
	$(error Unable to find the files!)
endif

OBJECTS = $(subst src/, build/, $(SOURCES:$(EXT)=.o))
EXECUTABLE = build/$(NAME)
TAR = $(NAME).tar

.PHONY: depend clean

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

$(OBJECTS):
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(subst .o,$(EXT), $(subst build/, src/, $@)) -o $@

clean:
	rm -f build/*.o $(EXECUTABLE) $(TAR)

tar:
	tar cfv $(TAR) $(SOURCES)
