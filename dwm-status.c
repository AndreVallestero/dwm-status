#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/resource.h>
#include <limits.h>
#include <signal.h>

// Frequency per second to check for updates
#define UPDATE_RATE 10

// Interval in seconds to auto update modules
#define WEATHER_INTERVAL 600
#define CPU_INTERVAL 10
#define MEMORY_INTERVAL 10
#define NETWORK_INTERVAL 10
#define BATTERY_INTERVAL 60
#define TIME_INTERVAL 1

char recordString[8];
char musicString[32];
char newsString[8];
char pacmanString[8];
char torrentString[16];
char weatherString[24];
char cpuString[8];
char memString[8];
char volumeString[8];
char brightnessString[8];
char networkString[8];
char batteryString[8];
char timeString[24]; 

void sighandler(int);
void print_status(void);

void update_weather(void);
void update_cpu(void);
void update_memory(void);
void update_network(void);
void update_battery(void);
void update_time(time_t);

int main(void) {
    // Set process to lowest priority for less CPU usage, needs privileges?
    setpriority(PRIO_PROCESS, 0, INT_MAX);
    signal(SIGINT, sighandler);

    int weatherNextUpdate = 0;
    int cpuNextUpdate = 0;
    int memoryNextUpdate = 0;
    int networkNextUpdate = 0;
    int batteryNextUpdate = 0;
    int timeNextUpdate = 0;

    while (1) {
        int printFlag = 0;
        time_t currTime = time(NULL);

        if (weatherNextUpdate <= currTime) {
            weatherNextUpdate = currTime + WEATHER_INTERVAL;
            update_weather();
            printFlag = 1;
        } if (cpuNextUpdate <= currTime) {
            cpuNextUpdate = currTime + CPU_INTERVAL;
            update_cpu();
            printFlag = 1;
        } if (memoryNextUpdate <= currTime) {
            memoryNextUpdate = currTime + MEMORY_INTERVAL;
            update_memory();
            printFlag = 1;
        } if (networkNextUpdate <= currTime) {
            networkNextUpdate = currTime + NETWORK_INTERVAL;
            update_network();
            printFlag = 1;
        } if (batteryNextUpdate <= currTime) {
            batteryNextUpdate = currTime + BATTERY_INTERVAL;
            update_battery();
            printFlag = 1;
        } if (timeNextUpdate <= currTime) {
            timeNextUpdate = currTime + TIME_INTERVAL;
            update_time(currTime);
            printFlag = 1;
        }

        // Only print at most once per update and only if data needed updating
        if (printFlag) print_status();

        // Check for updates 10 times per second
        struct timespec ts;
        ts.tv_sec = 0;
        ts.tv_nsec = 1000000000L / UPDATE_RATE;
        nanosleep(&ts, NULL);
    }
    return 0;
}

void sighandler(int signum) {
   printf("Caught signal %d, coming out...\n", signum);
   exit(1);
}

void print_status(void) {
    char execString[256];
    sprintf(execString, "xsetroot -name \"%s\"", timeString);
    system(execString);
}

void update_weather(void) {

}

void update_cpu(void){

}

void update_memory(void) {

}

void update_network(void) {

}

void update_battery(void) {

}

void update_time(time_t currTime) {
    strftime(timeString, 26, "%Y-%m-%d %H:%M:%S", localtime(&currTime));
}