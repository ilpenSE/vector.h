CC = gcc
CFLAGS = -Wall -Wextra -I..

TARGET = app
SRC = main.c ints.c strings.c

ifeq ($(debug),1)
	CFLAGS += -g -O0
else
	CFLAGS += -O2
endif

all: $(TARGET)

$(TARGET): $(SRC) ../vector.h
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) 
