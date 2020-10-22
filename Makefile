CFLAGS =
CC = gcc $(CFLAGS)
LDFLAGS = 

all: overseer controller

overseer: overseer.c output_manage.c helper.c thread_manage.c extensions.c mem_regulation.c
	$(CC) overseer.c output_manage.c helper.c thread_manage.c extensions.c mem_regulation.c -pthread -o overseer

controller: controller.c usage.c helper.c
	$(CC) controller.c usage.c helper.c extensions.c -o controller

clean:
	rm -f overseer
	rm -f controller

.PHONY: clean
