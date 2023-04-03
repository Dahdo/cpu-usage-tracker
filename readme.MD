CPU usage tracker
-------------------------------------------------------------
- The program computes the cpu usage every second.
- It prints the total cpus usage and individually per each cpu.
- The program terminates upon SIGTERM delivery to it [kill -15 PID] or when at least one of its threads exceedes 2 sec response time.
- The log file "log-cputracker" is created in the working directory.
- The program makes use 5 thread in addition to the main: the reader, analyzer, printer, logger & the watchdog.
----------------------------------------------------------

To build project:
---------------------------------------
make

To run project without valgrind
-------------------------------------------
make run

To run project with valgrind
-------------------------------------------
make vrun

This will usually cause one of its threads to timeout due to additional valgrind load.


To run tests
--------------------------------------------
make test

Licenses and Attributions
-------------------------------------------

Implementation code of the circular buffer, by Phillip Johnston from EmbeddedArtistry. Licensed under Creative Commons Zero v1.0 Universal
https://github.com/embeddedartistry/embedded-resources/blob/master/examples/c/circular_buffer/circular_buffer.c

Creating a Circular Buffer in C and C++, 17th May 2017 by Phillip Johnston
https://embeddedartistry.com/blog/2017/05/17/creating-a-circular-buffer-in-c-and-c/