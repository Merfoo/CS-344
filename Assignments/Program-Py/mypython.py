#!/usr/bin/env python

import string
import random

if __name__ == "__main__":
    # Create 3 files with a string of 10 random upper/lower case letters
    for i in range(3):
        # Filename for file to be created and file data for the file
        filename = "File" + str(i) + ".txt"
        fileData = ""

        # Generate 10 random characters with a newline at the end
        for j in range(10):
            fileData +=  random.choice(string.ascii_lowercase)

        fileData += "\n"

        # Create the file with the file data in it
        f = open(filename, "w+")
        f.write(fileData)
        f.close()

        # Output the data that was written to the file, 
        # exclude the newline at the end
        print(fileData[:-1])

    # Generate random numbers 'a' and 'b' between [1, 42],
    # display them and the their product
    numA = random.randint(1, 42)
    numB = random.randint(1, 42)

    print(numA)
    print(numB)
    print(numA * numB)

