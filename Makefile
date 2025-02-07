TARGET = MyProject
CC = gcc

FLAGS = -Wall -Werror -Wextra

PATH_CONSOLE = ./console/

all : $(TARGET)

$(TARGET) : build
	$(CC) $(FLAGS) $(wildcard $(PATH_CONSOLE)*.o) -o $@ 

build :
	$(MAKE) -C $(PATH_CONSOLE)

clean:
	$(MAKE) -C $(PATH_CONSOLE) clean
	rm -rf $(TARGET)

.PHONY: all build clean $(TARGET)
