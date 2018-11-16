CC=gcc
RC=windres
LDFLAGS=-mwindows
CFLAGS=-I ./include
TARGET=win32_test.exe
SRC=$(wildcard *.c)
default:
	$(CC) -o $(TARGET) $(SRC) $(CFLAGS) $(LDFLAGS)
