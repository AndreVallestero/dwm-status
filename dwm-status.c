#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/resource.h>
#include <limits.h>

char timeString[32]; 

void print_status(void);
void update_time(void);

int main(void) {
    // Set process to lowest priority for less CPU usage, needs privileges?
    setpriority(PRIO_PROCESS, 0, INT_MAX);

    int timeInterval = 1;
    int timeNextUpdate = 0;

    while(1) {
        int printFlag = 0;
        int currTime = time(NULL);

        if(timeNextUpdate < currTime) {
            timeNextUpdate = currTime + timeInterval;
            update_time();
            printFlag = 1;
        }

        if(printFlag)
            print_status();

        //struct timespec ts;
        //ts.tv_sec = 1;
        //ts.tv_nsec = 0;
        //nanosleep(&ts, NULL);
        sleep(1);
    }
    return 0;
}

void print_status(void) {
    char execString[256];
    sprintf(execString, "xsetroot -name \"%s\"", timeString);
    system(execString);
}

void update_time(void) {
    time_t timer;
    struct tm* tm_info;
    time(&timer);
    tm_info = localtime(&timer);
    strftime(timeString, 26, "%Y-%m-%d %H:%M:%S", tm_info);
}
