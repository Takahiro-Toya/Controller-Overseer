CFLAGS =
CC = gcc $(CFLAGS)
LDFLAGS = 

all: overseer controller

overseer: overseer.c output_manage.c thread_manage.c extensions.c hashtab.c
	$(CC) overseer.c output_manage.c thread_manage.c extensions.c hashtab.c -o overseer

controller: controller.c usage.c
	$(CC) controller.c usage.c extensions.c -o controller

clean:
	rm -f *.o

.PHONY: clean
