CC = gcc

PATH_INCLUDE = ../include/

FLAGS = -Wall -Werror -Wextra -MMD -MP -I $(PATH_INCLUDE) -I ..

SRC = $(wildcard *.c)
OBJ = $(patsubst %.c, %.o, $(SRC));

all: $(OBJ)
	
%.o : %.c
	$(CC) $(FLAGS) -c $^

-include *.d

clean:
	rm -rf *.o *.d *.gch