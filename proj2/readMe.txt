1) 
Cristian Guarino

2) 
In my proj2 I developed the raid.c program to read the characters one by one and get their equivalent 2 digit HEX value.
The first digit will be the equivalent of the first nibble and the second digit will be the equivalent of the second nibble.
Once I have each nibble I calculate their parity bits and write the resulting hamming code to the separate files xyz.part[0-6].

When I am writing I am writing to text files. So when I am decoding I am also reading from text files.

To develop the diar.c program used the same approach and used in raid.c. I collect the bits from all the part files to get 
my nibbles back and extract the hamming codes. Everytime I check a Hamming Code for errors and corrections. 
By combining 2 nibbles at the time I'm able to get the first digit of the HEX 
value of the character with the first nibble and the second digit of the HEX value of the HEX value.
I convert the HEX to the corresponding character and print it to the outputfile.

In my folder there are test runs made on two identical files testing.txt and testing2.txt. 
In testing.txt I applied raid and diar without modifing the partFiles. So that it can decode it without using 
error detection and correction.
In testing2.txt I applied raid and then modified testing2.txt.part2 flipping the bits at the beginning and the end of the file.
I then ran diar on with the corrupted files and generated testing2.txt.2 to confirm that it was equal to the initial input file
testing2.txt.

3) 

// Selecting the compiler we will be using
CC=cc
// CFLAGS selects compiler flags to use for compilation
// in our case we use -Wall to turn on all warnings and -02 to set the optimization
CFLAGS=-Wall -O2               

// Targets to build
all: raid diar

// Targets .c files to run
%: %.c
// Runs the GCC compiler that we selected earlier with CC adding the flags to the command with CFLAGS and
// making the output files same name as the .c programs.
	$(CC) $(CFLAGS) -o $@ $<

// command to cleanup
clean:
// removing the files a.out all the ones with a .part? suffix and all the ones with a .2 suffix
	rm -f a.out *.part? *.2

4)

getopt.h is C library function used to parse command-line options of the UNIX/POSIX systems. It's functions and macros 
facilitate the processing of command-line options and arguments in a consistent manner across various programs.

In our program we can use it to get the command line arguments and the filemane entered in the command line for our program to run.
In my implementation I used instead the value at index 2 of argv (argv[2]) in order to get the filename entered in the command line to
work with our program (raid or diar).