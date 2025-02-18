TARGET = pr01
CC = gcc

PATH_CONSOLE = ./console/
PATH_MYSIMPLECOMPUTER = ./mySimpleComputer/
PATH_INCLUDE = ./include/

PATH_TARGET = $(PATH_CONSOLE)$(TARGET)

FLAGS = -Wall -Werror -Wextra -I $(PATH_INCLUDE)

LIB_MYSIMPLECOMPUTER = $(PATH_MYSIMPLECOMPUTER)libMySimpleComputer.a
OBJ_CONSOLE = $(PATH_CONSOLE)main.o $(PATH_CONSOLE)print.o

all : build $(PATH_TARGET)

$(PATH_TARGET) : $(OBJ_CONSOLE) $(LIB_MYSIMPLECOMPUTER)
	$(CC) $(FLAGS) $^ -o $@ 

build:
	$(MAKE) -C $(PATH_MYSIMPLECOMPUTER) all
	$(MAKE) -C $(PATH_CONSOLE) all 

clean:
	$(MAKE) -C $(PATH_MYSIMPLECOMPUTER) clean
	$(MAKE) -C $(PATH_CONSOLE) clean
	rm -rf $(PATH_INCLUDE)*.gch $(PATH_TARGET)

.PHONY: all build clean
