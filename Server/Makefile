# define some variables for the compiler and compiler flags
# to use Makefile variables later in the Makefile: $()
#
#  -g    adds debugging information to the executable file
#  -Wall turns on most, but not all, compiler warnings
#  -c    Compile and assemble, but do not link.
CC = gcc
CFLAGS = -c -g -Wall

# Objects used to obtain the executable file
#
# Normaly this variable its called objects, obj, OBJECTS or OBJ
obj_folder = Obj
objects = $(addprefix $(obj_folder)/, main.o my_string.o client.o)
head = my_string.h client.h errono.h

main: $(objects)
	$(CC) $(objects) -o main.elf -lpthread

$(obj_folder)/main.o: main.c my_string.h client.h
	$(CC) $(CFLAGS) main.c -o $(obj_folder)/main.o

$(obj_folder)/client.o: client.c client.h
	$(CC) $(CFLAGS) client.c -o $(obj_folder)/client.o

$(obj_folder)/my_string.o: my_string.c my_string.h
	$(CC) $(CFLAGS) my_string.c -o $(obj_folder)/my_string.o

clean:
	rm $(objects) main.elf