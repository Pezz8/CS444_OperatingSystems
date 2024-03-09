Cristian Guarino
CS444 Project 1
Due Date: 10/03/2023 @ 2:00 pm

We are building a program tha reads charcaters from an input file. We will check
all the characters used in the file and their frequency in order to calculate their
Huffman Codes. And output the binary code of the characters based on their huffman code.

DATA COLLECTION

First we create a structure CHARACTER to hold the character and their frequency.
We will collect them in an array of CHARACTER of size 256 (the possible characters in ASCII).
Initially our index for the array will match the integer value of
each characters (this will change when we sort the array).
Once the array is created we will increase the count of each characters based on their
frequencies. 
Finally, when all the characters frequencies are accounted for we can sort
the array in order to have the elements in the array in increasing order of frequency.

HUFFMAN ENCODING

Having our array sorted in increasing order we can work on building the tree for our
HUFFMAN encoding.
We will use the two least frequent characters and place the smallest one amongst the two
as a left node and the largest amongst the two as the right node.
We will create a parent node for the two child nodes that will hold the sum of the two
frequencies and no character value.
We recursively go through the steps until we reach the root node which holds the sum of all
the frequencies.

Once our tree is built we can traverse each node to get the equivalent huffman code for
each characters. If we traverse on a right node we will add a 1 if we traverse to a left
node we will add a 0 until the leaf is reached.

ENCODING INTO A .out file

Once we have all the huffman codes for all the characters we can read the input file And
replace all the characters with their corresponding huffman code at the bit level.


