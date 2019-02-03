#include <unistd.h>
#include <stdlib.h>
#include <string.h>

void print_status(char* command) {
    char execString[] = "xsetroot -name \"";
    strcat(execString, command);
    strcat(execString, "\"");
    system(execString);
}

int main(void) {
    print_status("Hello, World!");
    return 0;
}