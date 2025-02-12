TARGET = test
CC = gcc

SRC = $(wildcard *.c)
OBJ = $(patsubst %.c, %.o, $(SRC));

all: $(TARGET)

$(TARGET) : $(OBJ)
	
%.o : %.c
	$(CC) $^ -c -o $@

clean:
	rm -rf *.o