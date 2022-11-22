CC = gcc
CC_FLAGS = -Wall -Wextra -Werror -Wpedantic

image-viewer: main.o png_reader.o common.o
	$(CC) main.o png_reader.o common.o -o image-viewer

main.o: main.c common.h
	$(CC) $(CC_FLAGS) -c main.c -g

common.o: common.c common.h
	$(CC) $(CC_FLAGS) -c common.c -g

png_reader.o: png_reader.c png_reader.h common.h
	$(CC) $(CC_FLAGS) -c png_reader.c -g

clean:
	@echo Cleaning all files
	rm -f *.o image-viewer
