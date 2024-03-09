Cristian Guarino
DATE = 12/15/2023
CS444 - proj4

In my proj4 I was able to implement the queing system. I followed the pseudocode present in the pdf and used it as guideline for the program.
However, I ran into issues when using drand48. 
I am running Ubuntu 22.04 on my laptop and apparently it is not present in my version of <stdlib.h>. And my version of gcc is 11.4.0.

I kept getting "incomplete type is not allowed C/C++(70)" as an error. 

I have tryied changing setting and version but I was unsuccesfull.

I also had issues compiling the program with "-log" present in the code. However, I found out that by adding the -lm flag when compiling
does the trick.

Also, since I could not use drand48 onlineAvgStd.c was failing to compile as well. Therefore, I could not use the tools in the file to
get my statistics.

I finally decide to discard it and work on the rest of the rest of the program. 

I was able to implement getopt.h to get the command line arguments to change the values we need to run the program.


To compile the program I run:

gcc -o q q.c -pthread -lm

I was also able to compile it with the instruction provided:

gcc -pthread q.c -lm

To run the program I use the same command present in the proj4 pdf:

./q -l 5 -m 7 -c 1000 -s 1


Sample output of the program running
...
Current queue lenght: 0
Current queue lenght: 0
Current queue lenght: 0
Current queue lenght: 0
Current queue lenght: 1
Current queue lenght: 1
Current queue lenght: 0
Current queue lenght: 1
Current queue lenght: 3
Current queue lenght: 3
Costumer served: 1000
Current queue lenght: 4

The program ends after serving the 1000th customer. Showing the remaining potential clients left in the queue at the end of execution.

