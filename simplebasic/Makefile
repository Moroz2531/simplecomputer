TARGET = translator
CC = gcc

CFLAGS = -Wall -Werror -Wextra -MMD -MP

SRC = $(wildcard *.c)
OBJ = $(patsubst %.c, %.o, $(SRC))

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< 

-include $(OBJ:.o=.d)

clean:
	rm -rf *.o *.d *.gch $(TARGET)