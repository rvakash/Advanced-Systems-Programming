# Makefile for Assignment 2, Single Producer Multiple Consumer problem
# Author Akash R Vasishta, UFID : 53955080

TARGETS=Combiner

CROSS_TOOL =
CC_CPP = $(CROSS_TOOL)g++
CC_C = $(CROSS_TOOL)gcc

#CFLAGS = -Wall -g -std=c99 -Werror -pthread	

CFLAGS =  -g -pthread

all: clean $(TARGETS)

$(TARGETS):
	$(CC_C) $(CFLAGS) $@.c -o $@

clean:
	rm -f $(TARGETS)


