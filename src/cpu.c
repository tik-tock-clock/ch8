#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* denToHex(int value)
{
    int denary = value;
    char hexadec[10];

    if (denary == 0)
    {
        printf("00 ");
        return NULL;
    }

    int remainder = denary % 16;

    //printf("%i ", remainder);

    int byte1 = (denary - remainder)/16;

    switch (byte1)
    {
        case 0:
            hexadec[0] = '0';
        case 1:
            hexadec[0] = '1';
        case 2:
            hexadec[0] = '2';
        case 3:
            hexadec[0] = '3';
        case 4:
            hexadec[0] = '4';
        case 5:
            hexadec[0] = '5';
        case 6:
            hexadec[0] = '6';
        case 7:
            hexadec[0] = '7';
        case 8:
            hexadec[0] = '8';
        case 9:
            hexadec[0] = '9';
        case 10:
            hexadec[0] = 'A';
        case 11:
            hexadec[0] = 'B';
        case 12:
            hexadec[0] = 'C';
        case 13:
            hexadec[0] = 'D';
        case 14:
            hexadec[0] = 'E';
        case 15:
            hexadec[0] = 'F';    
    }

    switch (remainder)
    {
        case 0:
            hexadec[1] = '0';
        case 1:
            hexadec[1] = '1';
        case 2:
            hexadec[1] = '2';
        case 3:
            hexadec[1] = '3';
        case 4:
            hexadec[1] = '4';
        case 5:
            hexadec[1] = '5';
        case 6:
            hexadec[1] = '6';
        case 7:
            hexadec[1] = '7';
        case 8:
            hexadec[1] = '8';
        case 9:
            hexadec[1] = '9';
        case 10:
            hexadec[1] = 'A';
        case 11:
            hexadec[1] = 'B';
        case 12:
            hexadec[1] = 'C';
        case 13:
            hexadec[1] = 'D';
        case 14:
            hexadec[1] = 'E';
        case 15:
            hexadec[1] = 'F';    
    }

    printf("%s ", hexadec);
    return NULL;
}

int main(int argc, char* argv[])
{
    FILE* fptr;

    fptr = fopen("roms/ch8-logo.ch8", "rb");

    if (fptr == NULL) {
        printf("File not opened!\n");
    }
    else{
        printf("File opened successfully!\n\n");

        fseek(fptr, 0L, SEEK_END);

        long int fileLength = ftell(fptr);

        fseek(fptr, 0L, SEEK_SET);

        for(int i=0; i<fileLength; i++)
        {
            int currentValue;
            fread(&currentValue, 1, 1, fptr);
            denToHex(currentValue);
        }
        printf("\n");
    }

    fclose(fptr);
    return 0;
}