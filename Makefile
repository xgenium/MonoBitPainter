CC = cc
CFLAGS = -g -Wall -Werror -std=c23
LDFLAGS = -lraylib
TARGET = mono_bitpainter
SRC = mono_bitpainter.c
OBJ = $(SRC:.c=.o)
all: $(TARGET)
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LDFLAGS)
.PHONY: clean

clean:
	rm -f $(TARGET) $(OBJ)
