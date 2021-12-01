# An admittedly primitive Makefile
# To compile, type "make" or make "all"
# To remove files, type "make clean"

CC = gcc
CFLAGS = -Wall -Wno-format-overflow
OBJS = wserver.o wclient.o request.o io_helper.o request_fifo.o request_priority_queue.o
SUBMITDIR = webserverlab

.SUFFIXES: .c .o

all: wserver wclient spin.cgi

wserver: wserver.o request.o io_helper.o request_fifo.o request_priority_queue.o
	$(CC) $(CFLAGS) -o wserver wserver.o request.o io_helper.o request_fifo.o request_priority_queue.o

wclient: wclient.o io_helper.o
	$(CC) $(CFLAGS) -o wclient wclient.o io_helper.o

spin.cgi: spin.c
	$(CC) $(CFLAGS) -o spin.cgi spin.c

.c.o:
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	-rm -rf $(OBJS) wserver wclient spin.cgi $(SUBMITDIR) $(SUBMITDIR).tar 

submit: clean
	-@rm -rf $(SUBMITDIR)
	-@rm -rf .tmp 
	@mkdir .tmp
	@cp * .tmp
	@mv .tmp $(SUBMITDIR)
	tar cvf $(SUBMITDIR).tar $(SUBMITDIR)
	-@rm -rf $(SUBMITDIR)