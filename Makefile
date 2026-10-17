CC ?= gcc
CFLAGS ?= -Wall -Wextra -std=c99 -Iinclude -O2
LDFLAGS ?=

SRC = src/main.c \
      src/dashboard.c \
      src/courses.c \
      src/routine.c \
      src/exams.c \
      src/syllabus.c \
      src/storage.c \
      src/time_engine.c \
      src/ui.c \
      src/utilities.c

OBJ = $(SRC:.c=.o)
TARGET = academic_os

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET) $(TARGET).exe

.PHONY: all clean
