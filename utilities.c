#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

void clear_screen() {
    printf("\e[1;1H\e[2J");
}

void echo_allowed(bool allowed) {
    //determines whether user input should be displayed
    if(allowed) {
        system("stty echo");
    } else {
        system("stty -echo");
    }
}

int generate_random_number(int lower, int upper)
{
    int num = (rand() % (upper - lower + 1)) + lower;
    return num;
}

void remove_spaces(char* s) {
    const char* d = s;
    do {
        while (*d == ' ') {
            ++d;
        }
    } while (*s++ = *d++);
}