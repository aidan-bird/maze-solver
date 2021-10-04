CC = gcc
CC_FLAGS = -O0 -ggdb3 -Wall -fverbose-asm -fsanitize=address -lpng -lm
OBJS = ./src/*
OBJ_NAME = maze.out
all :
	$(CC) $(OBJS) $(CC_FLAGS) -o $(OBJ_NAME)
