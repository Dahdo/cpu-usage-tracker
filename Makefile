ifeq ($(CC),clang)
	CFLAGS = -Weverything
else
	CC=gcc
	CFLAGS = -Wall -Wextra
endif

OBJECTS = $(wildcard *.o)

cputracker: reader_thread.o circular_buffer.o analyzer_thread.o printer_thread.o logger_thread.o src/cpu_tracker.c
	$(CC) $(CFLAGS) -o $@ *.o src/cpu_tracker.c

logger_thread.o: include/logger_thread.h src/logger_thread.c
	$(CC) -c $(CFLAGS) -o $@ src/logger_thread.c

printer_thread.o: include/printer_thread.h src/printer_thread.c
	$(CC) -c $(CFLAGS) -o $@ src/printer_thread.c

analyzer_thread.o: include/analyzer_thread.h src/analyzer_thread.c
	$(CC) -c $(CFLAGS) -o $@ src/analyzer_thread.c

reader_thread.o: include/reader_thread.h src/reader_thread.c
	$(CC) -c $(CFLAGS) -o $@ src/reader_thread.c

circular_buffer.o: include/circular_buffer.h src/circular_buffer.c
	$(CC) -c $(CFLAGS) -o $@ src/circular_buffer.c

vrun: cputracker
	valgrind -s --tool=memcheck --leak-check=full ./cputracker

run: cputracker
	./cputracker

clear:
	-rm -f cputracker $(OBJECTS)