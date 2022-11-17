CC = gcc
CC_FLAGS = -Wall -Wextra -Werror -Wpedantic

image-viewer: main.o png_parser.o common.o
	$(CC) main.o png_parser.o common.o -o image-viewer

main.o: main.c common.h
	$(CC) $(CC_FLAGS) -c main.c -g

common.o: common.c common.h
	$(CC) $(CC_FLAGS) -c common.c -g

png_parser.o: png_parser.c png_parser.h common.h
	$(CC) $(CC_FLAGS) -c png_parser.c -g

clean:
	@echo Cleaning all files
	rm -f *.o image-viewer
