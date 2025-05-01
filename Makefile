CC = gcc
CFLAGS = -std=c99 -Wall -O2
LDFLAGS = -lvulkan -lglfw

TARGET = triangle

all: $(TARGET)

$(TARGET): main.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(TARGET)
