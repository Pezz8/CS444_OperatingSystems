#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// For getopt
#include <unistd.h>
#include <getopt.h>

// Function that turns a character into the equivalent 2 digit HEX number
void charToHex(char input, int *output)
{
    int number = (int)input;
    int rem;

    for (int i = 1; i >= 0; i--)
    {
        rem = number % 16;
        number = number / 16;
        output[i] = rem;
    }
}

// Function that turns a single HEX digit into a 4 bit binary number
void hexToBin(int input, int *output)
{
    int number = input;
    int rem;

    for (int i = 3; i >= 0; i--)
    {
        rem = number % 2;
        number = number / 2;
        output[i] = rem;
    }
}

// Function that prints to the console our 4 bit binary number
void printBin(int *input)
{
    for (int i = 0; i < 4; i++)
    {
        printf("%d", input[i]);
    }
}

// Function that prints our 2 digit HEX number
void printHex(int *hexNum)
{
    for (int i = 0; i < 2; i++)
    {
        if (hexNum[i] > 9)
        {
            printf("%c", ('A' + (hexNum[i] - 10)));
        }
        else
        {
            printf("%d", hexNum[i]);
        }
    }
    printf("\n");
}

// Function that prints the 7 bit Hamming Code
void printHammingCode(int *hammingCode)
{
    for (int i = 0; i < 7; i++)
    {
        printf("%d", hammingCode[i]);
    }
    printf("\n");
}

// Function that writes to the part files the parity bits and the data bits
void writeToFiles(char *fileNames[], int numOfFiles, int *data)
{
    for (int i = 0; i < numOfFiles; i++)
    {
        FILE *file = fopen(fileNames[i], "ab");
        if (file == NULL)
        {
            printf("Error opening file");
            continue;
        }
        else
        {
            fprintf(file, "%d", data[i]);
            // fputc(data[i], file);
        }
        fclose(file);
    }
}

// Function that loads the parity bits and the data bits into an array holding the generated Hamming Code
void loadHammingCode(int *hammingCode, int p1, int p2, int d3, int p4, int d5, int d6, int d7)
{
    hammingCode[0] = p1;
    hammingCode[1] = p2;
    hammingCode[2] = d3;
    hammingCode[3] = p4;
    hammingCode[4] = d5;
    hammingCode[5] = d6;
    hammingCode[6] = d7;
}

// Function that generates parity bits from the data nibble and saves the generated codes to the partFiles
void hammingEncoding(int *hexNum, int pos, int num_files, char *fileNames[])
{
    // Data bits
    int d3, d5, d6, d7;
    // Parity bits
    int p1, p2, p4;

    // Binary number equivalent to first/second nibble
    int binNumber[4];
    // Generated Hamming code
    int hammingCode[7];

    hexToBin(hexNum[pos], binNumber);

    d3 = binNumber[0];
    d5 = binNumber[1];
    d6 = binNumber[2];
    d7 = binNumber[3];

    p1 = d3 ^ d5 ^ d7;
    p2 = d3 ^ d6 ^ d7;
    p4 = d5 ^ d6 ^ d7;

    loadHammingCode(hammingCode, p1, p2, d3, p4, d5, d6, d7);
    // printHammingCode(hammingCode);
    writeToFiles(fileNames, num_files, hammingCode);
}

int main(int argc, char **argv)
{
    // Input file
    FILE *in;
    // Integer value of a character
    int ch;
    int opt;
    // 2 digit HEX number
    int hexCh[2];
    // Number of part files
    int numFiles = 7;
    char *fileName;
    // Array holding the names of the partFiles
    char *files[numFiles];

    // Process the arguments with getopt.
    while ((opt = getopt(argc, argv, "f:")) != -1)
    {
        switch (opt)
        {
        // Option -f followed by a filename
        case 'f':
            fileName = optarg;
            break; // An unrecognized option is encountered
        case '?':
            fprintf(stderr, "Usage: %s -f filename -s numberOfBytes\n", argv[0]);
            return EXIT_FAILURE;
        }
    }

    // Generating the names for the partFiles
    for (int i = 0; i < numFiles; ++i)
    {
        char part[10];               // Buffer to hold "partX"
        sprintf(part, ".part%d", i); // Create the string "partX"

        // Allocate memory for combined string
        files[i] = malloc(strlen(fileName) + strlen(part) + 1);
        if (files[i] == NULL)
        {
            perror("Failed to allocate memory");
            // Free previously allocated memory
            for (int j = 0; j < i; ++j)
            {
                free(files[j]);
            }
            return 1;
        }

        // Concatenate argv[1] and part
        strcpy(files[i], fileName);
        strcat(files[i], part);
    }

    // Opening the input file
    in = fopen(fileName, "r");
    if (in == NULL)
        printf("File can't be opened.\n");
    // Read the input file
    while ((ch) != EOF)
    {
        ch = fgetc(in);
        if (ch == EOF)
            break;
        charToHex(ch, hexCh);
        // printf("Character: %c \n", ch);
        // printf("Character in dec: %d \n", (int)ch);
        // printf("Character in Hex: %x \n", ch);
        // printHex(hexCh);
        hammingEncoding(hexCh, 0, numFiles, files);
        hammingEncoding(hexCh, 1, numFiles, files);
    }
    fclose(in);

    // FREE ALLOCATED MEMORY
    for (int i = 0; i < numFiles; ++i)
    {
        free(files[i]);
    }
    // End program
    return 0;
}