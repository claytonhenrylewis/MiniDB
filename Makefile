
CC = gcc -g -Wall -Werror -std=gnu99

all: miniDB

miniDB: miniDB.c
	$(CC) -o miniDB miniDB.c

