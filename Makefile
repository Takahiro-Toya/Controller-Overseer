CFLAGS =
CC = gcc $(CFLAGS)
LDFLAGS = 

all: overseer controller

overseer: overseer.c output_manage.c thread_manage.c
	$(CC) overseer.c output_manage.c thread_manage.c -o overseer

controller: controller.c
	$(CC) -o controller controller.c

clean:
	rm -f *.o

.PHONY: clean
