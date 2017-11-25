#include "Keygen.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Generate random upper case letters and space
// of length passed in as 1st argument
int main(int argc, char* argv[])
{
    // Only generate letters if the key length 
    // argument is present
    if(argc >= 2)
    {
        // Seed time and parse 1st argument for
        // key length
        srand(time(NULL));
        int keyLength = atoi(argv[1]);

        // Loop for key length times generating a
        // upper case letter or space
        for(int i = 0; i < keyLength; i++)
        {
            // Generate random number to determine
            // the letter or space to output
            int randNum = rand() % 27;
            char output;

            // If less than 26, output an upper case
            // letter
            if(randNum < 26)
                output = 'A' + randNum;

            // Otherwise, output a space
            else
                output = ' ';

            // Display the random letter/space
            printf("%c", output);
        }

        // Append newline
        printf("\n");
        return 0;
    }

    // Otherwise, display usage message
    else
    {
        printf("usage: keygen key_length\n");
        return 1;
    }
}

