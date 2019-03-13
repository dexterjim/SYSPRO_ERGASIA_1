OBJS	= main.o functions.o 
SOURCE	= src/main.c src/functions.c
HEADER	= header-files/functions.h
OUT	= bitcoin
CC	= gcc
FLAGS	= -g -c -Wall

$(OUT):	$(OBJS)
	$(CC) -g $(OBJS) -o $@

main.o: src/main.c
	$(CC) $(FLAGS) src/main.c

functions.o : src/functions.c
	$(CC) $(FLAGS) src/functions.c

clean: 
	rm -f $(OBJS) $(OUT)

count:
	wc $(SOURCE) $(HEADER)

