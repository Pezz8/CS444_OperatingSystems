#include <stdlib.h>
#include <stdio.h>

// Struct that holds bot characters and their frequencies
typedef struct CHARACTER
{
    char character;
    int frequency;
} CHARACTER;

// Compare function that compares frequencies of two elements in an array.
int compare_char(const void *v1, const void *v2)
{
    const CHARACTER *char1 = (CHARACTER *)v1;
    const CHARACTER *char2 = (CHARACTER *)v2;

    if (char1->frequency < char2->frequency)
        return -1;
    else if (char1->frequency > char2->frequency)
        return 1;
    else
        return 0;
}

// Max number of characters
const int INT_MAX_CHAR = 256;

int main(int argc, char **argv)
{
    CHARACTER charArr[INT_MAX_CHAR];
    int charCounter = 0;

    // Loading Array with default values
    // Character will be the character corresponding to the value of i at each iteration
    // Frequency will be set to 0 as default
    for (int i = 0; i < INT_MAX_CHAR; i++)
    {
        // printf("%c \n", (char)i);
        CHARACTER c = {.character = (char)i, .frequency = 0};
        charArr[i] = c;
    }

    // File input
    FILE *fi;

    // Open the file in argv[2]
    fi = fopen(argv[2], "r");

    // If the file opens correctly we will keep loading c with all the characters we read
    // from the file and assign it to their corresponding character in the array. We
    // also will increase their frequency every time we encounter the character.
    if (fi != NULL)
    {
        char c;
        while ((c = fgetc(fi)) != EOF)
        {
            charArr[(int)c].character = c;
            charArr[(int)c].frequency++;
        }
        fclose(fi);
    }
    else
        printf("Error opening file.\n");

    // Sort the array using qsort and sort the array in increasing order.
    qsort(charArr, INT_MAX_CHAR, sizeof(CHARACTER), compare_char);

    // Calculating the number of charcters that have frequency more than 0 to use for
    // the Huffman encoding.
    for (int i = 0; i < INT_MAX_CHAR; i++)
    {
        if (charArr[i].frequency > 0)
        {
            // printf("%c %i\n", charArr[i].character, charArr[i].frequency);
            charCounter++;
        }
    }

    return 0;
}