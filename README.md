# Simple MIPS Simulator by C
by Chanyoung Park

## Dev Note
I followed a book, [Computer Organization Design MIPS Architecture](https://www.amazon.com/Computer-Organization-Design-MIPS-Architecture/dp/0124077269). It is fully implemented in C language.


* If you want to see what instructions are implemented, Look at Instruction.c file. I give trouble that you manually register commands through this file. But, this way gives extensibility to add/remove commands through this file. 

* There is a huge difference between Phase 1 and Phase 2. Phase 2 is designed with pipelining.

* I deliver example output for testing. However, Phase 2 can get a slightly different output. You need to find "test" folder and get outputs by ikjae and taeyun.