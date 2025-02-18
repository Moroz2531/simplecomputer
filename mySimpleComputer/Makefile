TARGET = libMySimpleComputer.a
CC = gcc

CFLAGS = -Wall -Werror -Wextra -MMD -MP -I ../include/

SRC = $(wildcard *.c)
OBJ = $(patsubst %.c, %.o, $(SRC))

SRC_SC = $(wildcard sc_*.c)
OBJ_SC = $(patsubst %.c, %.o, $(SRC_SC))


all: $(TARGET) $(OBJ)

$(TARGET) : $(OBJ_SC)
	ar rcs $@ $^

%.o : %.c
	$(CC) $(CFLAGS) -c $^

-include *.d

clean:
	rm -rf *.o *.d *.a