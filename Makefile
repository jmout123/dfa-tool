CC = gcc
CFLAGS = -Wall
LDFLAGS =
OBJFILES = ./src/scanner.o ./src/dfa.o
TARGET = dfa

all: $(TARGET)

$(TARGET): $(OBJFILES)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJFILES) $(LDFLAGS)

clean:
	rm -f $(OBJFILES) $(TARGET) *~