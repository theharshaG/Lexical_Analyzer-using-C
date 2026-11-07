#include <unknown_header.h>

int main()
{
    printf("Hello"); // Error: missing <stdio.h>
    int a = 10       // Error: missing semicolon
    int b = 12.3.4;  // Lexical error
    char (x = 5;     // Error: mismatched delimiter

    int switch(a)
    {
        case 1;
    }
}