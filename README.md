# GThreads
A library to model the implementation of POSIX threads provided by UNIX.

Steps to compile the library
  -In the parent directory of gtthreads, type 'make' to generate the library file gtthread.a.
  -To run your program, copy your program in the parent directory of gtthreads, then run the following code to compile your program
    gcc -Wall -pedantic -I{...} -o object_name your_filename.c gtthread.a .
    Important note-
    -Plz ignore the warnings they are of no use. If there are no errors then your program would be compiled correctly.
    -Please use “gtthread.h” instead of <gtthread.h> in order to ensure no compiling errors.
