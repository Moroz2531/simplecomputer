TARGET = libBc.a
CC = gcc

CFLAGS = -Wall -Werror -Wextra -MMD -MP -I ../include 

SRC = $(wildcard *.c)
OBJ = $(patsubst %.c, %.o, $(SRC))

all : $(TARGET)

$(TARGET) : $(OBJ)
	ar rcs $@ $^

%.o : %.c
	$(CC) $(CFLAGS) -c $^

-include *.d

clean:
	rm -rf *.o *.d *.a