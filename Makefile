# Makefile for compiling project.c

# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -g

# Target executable
TARGET = project

# Source file
SRC = project.c

# Rules
all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

# To run the shell without a batch file
run: $(TARGET)
	./$(TARGET)

# To run the shell with a batch file
run-batch: $(TARGET)
	./$(TARGET) batch.txt

clean:
	rm -f $(TARGET) ./a.out
