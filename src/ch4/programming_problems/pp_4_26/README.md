This is my solution to programming problem 4.26 from Operating System Concepts, Galvin.


Problem Statement.
===



![](imgs/img0.png)


Problem Notes.
===


Command line argument = number of Fibonacci numbers to print.


Child thread computes sequence, stores it in a external variable.


Parent outputs the sequence when child terminates.



Solution Plan.
===


Implement a single threaded solution, then add a thread.


Solution Test Cases.
===

`./a.out 7`



![](imgs/img1.png)


`./a.out 20`



![](imgs/img2.png)


`./a.out 0`, `./a.out -1`, `./a.out`



![](imgs/img3.png)


Solution Description and Results.
===


On Mac OS X,


To compile: `cc prog_prob_4_26.c`


To run: `./a.out 7`


End.
===