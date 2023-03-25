CPU usage tracker
-------------------------------------------------------------
- The program computes the cpu usage every second.
- It shows total cpus(cores) usage and individually per cpu(core).
- It only stops upon SIGTERM delivery to it. [kill -15 PID]
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


To run tests
--------------------------------------------
make test
