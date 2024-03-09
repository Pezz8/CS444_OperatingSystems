#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
// For getopt
#include <unistd.h>
#include <getopt.h>

// Function that returns the size of the file passed as an argument.
int getFileSize(FILE *file)
{
    fseek(file, 0, SEEK_END); // Move the file pointer to the end of the file
    int size = ftell(file);   // Get the current byte offset in the file
    rewind(file);             // Move file pointer back to the start of the file
    return size;
}

// Function that prints a 4 bit binary number (first/second nibble in our case)
void printBin(int *input)
{
    for (int i = 0; i < 4; i++)
    {
        printf("%d", input[i]);
    }
    printf("\n");
}

// Function to print our 2 digit HEX number
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

// Function that gets a 4 bit binary number (first/second nibble in our case) and finds
// the equivalent value in HEX. Afterwards it assigns that value to our 2 digit HEX number
// at the position indicated by pos.
void binToHex(int *binNumber, int *hexCh, int pos)
{
    int number = 0;
    if (binNumber[3] > 0)
    {
        number += 1;
    }
    if (binNumber[2] > 0)
    {
        number += 2;
    }
    if (binNumber[1] > 0)
    {
        number += 4;
    }
    if (binNumber[0] > 0)
    {
        number += 8;
    }
    hexCh[pos] = number;
}

// Function that converts a 2 digit HEX number into the equivalent in decimal and returns it.
int hexToChar(int *input)
{
    int character_number = (int)(input[0] * (pow(16, 1)) + input[1] * (pow(16, 0)));
    // printf("Integer equivalent of HexNum = %d\n", character_number);
    return character_number;
}

// Function that loads the bits found in all 7 of the partFiles into a 2-D array
void loadToDecodeArr(int **arr, int numOfFiles, int numOfBits, char *fileName[])
{
    int ch = 0;
    FILE *temp;

    for (int i = 0; i < numOfFiles; i++)
    {
        // printf("Opening %s\n", fileName[i]);
        temp = fopen(fileName[i], "r");
        for (int j = 0; i < numOfBits; j++)
        {
            ch = fgetc(temp);
            if (ch == -1)
                break;
            // printf("Reading %c from %s\n", ch, fileName[i]);
            if (ch == '0')
                ch = 0;
            else if (ch == '1')
                ch = 1;
            arr[i][j] = ch;
        }
        fclose(temp);
    }
}

// Function that loads the databits in a 4 bit nibble
void loadNibble(int *nibble, int d3, int d5, int d6, int d7)
{
    nibble[0] = d3;
    nibble[1] = d5;
    nibble[2] = d6;
    nibble[3] = d7;
}

// Function that detects the position of the error in our Hamming Code (7 4)
int hammingErrorDetection(int p1, int p2, int d3, int p4, int d5, int d6, int d7)
{
    // Check bits
    int c1 = p1 ^ d3 ^ d5 ^ d7;
    int c2 = p2 ^ d3 ^ d6 ^ d7;
    int c3 = p4 ^ d5 ^ d6 ^ d7;

    // Return the position of the error. If there are no errors it will return 0.
    return (c3 * pow(2, 2) + c2 * pow(2, 1) + c1 * pow(2, 0));
}

// Function to make correction on the 2-D array containing all our bits collected from
// the 7 files. It will flip the bit at the position specified.
void hammingErrorCorrection(int **arr, int partNumber, int bitNumber)
{
    if (partNumber > 0)
    {
        if (arr[partNumber][bitNumber] == 0)
            arr[partNumber][bitNumber] = 1;
        else if (arr[partNumber][bitNumber] == 1)
            arr[partNumber][bitNumber] = 0;
    }
}

void hammingDecoding(int numFiles, char *fileName[], char *outFile)
{
    // Data bits
    int d3, d5, d6, d7;
    // Parity bits
    int p1, p2, p4;
    // Position of the error
    int errorPos;
    // Character in decimal value
    int ch;
    // Character in HEX
    int hexCh[2];
    // Databits from the left HEX digit in hexCh
    int firstNibble[4];
    // Databits from the right HEX digit in hexCh
    int secondNibble[4];
    // Number of characters in a file;
    int numCharacters;
    // Output file
    FILE *out;
    // File buffer
    FILE *temp;

    // Finding the number of characters in a part file (all the part files should be the same size)
    // printf("Opening file %s\n", fileName[0]);
    temp = fopen(fileName[0], "r");
    numCharacters = getFileSize(temp);
    fclose(temp);
    // printf("Number of characters in %s is %d\n", fileName[0], numCharacters);

    // MEMORY ALLOCATION FOR ARRAY
    int **toDecode = malloc(numFiles * sizeof(int *));
    if (toDecode == NULL)
    {
        printf("Row allocation failure\n");
    }
    for (int i = 0; i < numFiles; i++)
    {
        toDecode[i] = malloc(numCharacters * sizeof(int));
        if (toDecode[i] == NULL)
        {
            printf("Column allocation failure\n");
        }
    }

    // Load the toDecode 2-D array with all the bits in the 7 files provided.
    loadToDecodeArr(toDecode, numFiles, numCharacters, fileName);

    // Write the output file with the decoded text.
    out = fopen(outFile, "a");

    // Loop all the files for as many charaters are there and increment the loop
    // in multiples of 2 (j is going to be the first nibble and j+1 will be the second
    // nibble)
    for (int j = 0; j < numCharacters; j += 2)
    {
        // Load all the parity bits and data bits with the 0s and 1s provided by the files;
        p1 = toDecode[0][j];
        p2 = toDecode[1][j];
        d3 = toDecode[2][j];
        p4 = toDecode[3][j];
        d5 = toDecode[4][j];
        d6 = toDecode[5][j];
        d7 = toDecode[6][j];

        // Find the position of the error among the hamming code provided
        errorPos = hammingErrorDetection(p1, p2, d3, p4, d5, d6, d7);
        // printf("\nFound Error in file %d at position %d\n", (errorPos), (j));
        // If the position is 0 then there are no errors in the data bits and we use them for
        // the decoding. If it is more than 0 we correct them and update the array.
        if (errorPos > 0)
        {
            hammingErrorCorrection(toDecode, errorPos - 1, j);
            p1 = toDecode[0][j];
            p2 = toDecode[1][j];
            d3 = toDecode[2][j];
            p4 = toDecode[3][j];
            d5 = toDecode[4][j];
            d6 = toDecode[5][j];
            d7 = toDecode[6][j];
        }

        // Loading data bits for the first nibble
        loadNibble(firstNibble, d3, d5, d6, d7);
        // printBin(firstNibble);

        // Load all the parity bits and data bits with the 0s and 1s provided by the files;
        p1 = toDecode[0][j + 1];
        p2 = toDecode[1][j + 1];
        d3 = toDecode[2][j + 1];
        p4 = toDecode[3][j + 1];
        d5 = toDecode[4][j + 1];
        d6 = toDecode[5][j + 1];
        d7 = toDecode[6][j + 1];

        errorPos = hammingErrorDetection(p1, p2, d3, p4, d5, d6, d7);
        if (errorPos > 0)
        {
            hammingErrorCorrection(toDecode, errorPos - 1, j + 1);
            p1 = toDecode[0][j + 1];
            p2 = toDecode[1][j + 1];
            d3 = toDecode[2][j + 1];
            p4 = toDecode[3][j + 1];
            d5 = toDecode[4][j + 1];
            d6 = toDecode[5][j + 1];
            d7 = toDecode[6][j + 1];
        }

        // Load data bits for the second nibble
        loadNibble(secondNibble, d3, d5, d6, d7);
        // printBin(secondNibble);

        // Turning the first nibble to the corresponding HEX value and assigning it
        // to the left digit of our 2 digit HEX number
        binToHex(firstNibble, hexCh, 0);
        // Turning the first nibble to the corresponding HEX value and assigning it
        // to the right digit of our 2 digit HEX number
        binToHex(secondNibble, hexCh, 1);
        // printHex(hexCh);

        // Assigning to ch the decimal value of our 2 digit HEX number.
        ch = hexToChar(hexCh);
        // Printing to our out file using the character equivalient of our ch.
        fprintf(out, "%c", ch);
        // printf("%c", ch);
    }

    // DEALLOCATING MEMORY
    for (int i = 0; i < numFiles; i++)
    {
        free(toDecode[i]);
    }
    free(toDecode);
}

int main(int argc, char *argv[])
{

    FILE *out;
    // For opt
    int opt;
    int numberOfBytes = 0;
    // Character in HEX
    int hexCh[2];
    // Number of part files
    int numFiles = 7;
    // Variables for the file names
    char *fileName = NULL;
    char *suffix = ".2";
    // Array that holds the names of the partFiles
    char *partFiles[numFiles];

    // Process the arguments with getopt.
    while ((opt = getopt(argc, argv, "f:s:")) != -1)
    {
        switch (opt)
        {
        // Option -f followed by a filename
        case 'f':
            fileName = optarg;
            break;
        // Option -s followed by the number of bytes
        case 's':
            numberOfBytes = atoi(optarg);
            if (numberOfBytes < 0)
            {
                fprintf(stderr, "The number of bytes must be non-negative.\n");
                return EXIT_FAILURE;
            }
            break;
        // An unrecognized option is encountered
        case '?':
            fprintf(stderr, "Usage: %s -f filename -s numberOfBytes\n", argv[0]);
            return EXIT_FAILURE;
        }
    }

    // Ensure both -f and -s are provided.
    if (!fileName || !numberOfBytes)
    {
        fprintf(stderr, "Both -f and -s options must be specified.\n");
        fprintf(stderr, "Usage: %s -f filename -s numberOfBytes\n", argv[0]);
        return EXIT_FAILURE;
    }

    // If the number of bytes provided is not a multiple of 4 close the programs
    if (numberOfBytes % 4 != 0)
    {
        printf("Incorrect or corrupted partFiles!\n");
        return 0;
    }

    // Generate outFile name fileName + ".2"
    char *outFile = malloc(strlen(fileName) + strlen(suffix) + 1);

    if (outFile == NULL)
    {
        perror("Failed to allocate memory");
        return 1;
    }

    strcpy(outFile, fileName);
    strcat(outFile, suffix);

    // Generate the names for the partFiles based on the original name of the file
    for (int i = 0; i < numFiles; ++i)
    {
        char part[10];               // Buffer to hold "partX"
        sprintf(part, ".part%d", i); // Create the string "partX"

        // Allocate memory for combined string
        partFiles[i] = malloc(strlen(argv[2]) + strlen(part) + 1);
        if (partFiles[i] == NULL)
        {
            perror("Failed to allocate memory");
            // Free previously allocated memory
            for (int j = 0; j < i; ++j)
            {
                free(partFiles[j]);
            }
            return 1;
        }
        // Concatenate argv[1] and part
        strcpy(partFiles[i], fileName);
        strcat(partFiles[i], part);
    }

    // Decoding
    hammingDecoding(numFiles, partFiles, outFile);

    // FREE ALLOCATED MEMORY
    for (int i = 0; i < numFiles; ++i)
    {
        free(partFiles[i]);
    }

    // End Program
    return 0;
}