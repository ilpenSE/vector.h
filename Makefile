CC ?= gcc
CFLAGS = -Wall -Wextra

BUILD = build
TARGET = $(BUILD)/app
SRC = main.c tests.c

ifeq ($(windows),1)
	TARGET = $(BUILD)/app.exe
	CC = x86_64-w64-mingw32-gcc
endif

ifeq ($(debug),1)
	CFLAGS += -g -O0
else
	CFLAGS += -O2
endif

all: $(BUILD) $(TARGET)

$(BUILD):
	mkdir -p $(BUILD)

$(TARGET): $(SRC) vector.h | $(BUILD)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) 
