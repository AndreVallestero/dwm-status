#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/resource.h>
#include <limits.h>
#include <signal.h>

#define WEATHER_API ""

// Frequency per second to check for updates and signals
#define UPDATE_RATE 10

// Interval in seconds to auto-update modules, other modules are updated manually using signal
#define NEWS_INTERVAL 300
#define PACMAN_INTERVAL 600
#define TORRENT_INTERVAL 10
#define WEATHER_INTERVAL 600
#define CPU_INTERVAL 10
#define MEMORY_INTERVAL 10
#define NETWORK_INTERVAL 10
#define BATTERY_INTERVAL 60
#define TIME_INTERVAL 1

// Signal IDs, use 34 and higher to avoid interfearing with critical signals
#define RECORD_SIGNAL 34
#define MUSIC_SIGNAL 35
#define TORRENT_SIGNAL 36
#define VOLUME_SIGNAL 37
#define BRIGHTNESS_SIGNAL 38

int printFlag = 0;

long prevCpuUsage = 0L;
long prevCpuIdle = 0L;

char recordString[8];
char musicString[32];
char newsString[8];
char pacmanString[8];
char torrentString[16];
char weatherString[24];
char cpuString[8];
char memoryString[8];
char volumeString[8];
char brightnessString[8];
char networkString[8];
char batteryString[8];
char timeString[24]; 

void print_status(void);

void update_news(void);
void update_pacman(void);
void update_torrent(void);
void update_weather(void);
void update_cpu(time_t);
void update_memory(void);
void update_network(void);
void update_battery(void);
void update_time(time_t);

// Signal handlers
void record_sighand(int);
void music_sighand(int);
void torrent_sighand(int);
void volume_sighand(int);
void brightness_sighand(int);

int main(void) {
    // Set process to lowest priority for less CPU usage, needs privileges?
    setpriority(PRIO_PROCESS, 0, INT_MAX);
    printf("%d", getpid());
    fflush(stdout);
    
    signal(RECORD_SIGNAL, record_sighand);
    signal(MUSIC_SIGNAL, music_sighand);
    signal(TORRENT_SIGNAL, torrent_sighand);
    signal(VOLUME_SIGNAL, volume_sighand);
    signal(BRIGHTNESS_SIGNAL, brightness_sighand);

    int newsNextUpdate = 0;
    int pacmanNextUpdate = 0;
    int torrentNextUpdate = 0;
    int weatherNextUpdate = 0;
    int cpuNextUpdate = 0;
    int memoryNextUpdate = 0;
    int networkNextUpdate = 0;
    int batteryNextUpdate = 0;
    int timeNextUpdate = 0;

    while (1) {
        printFlag = 0;
        time_t currTime = time(NULL);

        if (newsNextUpdate <= currTime) {
            newsNextUpdate = currTime + NEWS_INTERVAL;
            update_news();
        } if (pacmanNextUpdate <= currTime) {
            pacmanNextUpdate = currTime + PACMAN_INTERVAL;
            update_pacman();
        } if (torrentNextUpdate <= currTime) {
            torrentNextUpdate = currTime + TORRENT_INTERVAL;
            update_torrent();
        } if (weatherNextUpdate <= currTime) {
            weatherNextUpdate = currTime + WEATHER_INTERVAL;
            update_weather();
        } if (cpuNextUpdate <= currTime) {
            cpuNextUpdate = currTime + CPU_INTERVAL;
            update_cpu(currTime);
        } if (memoryNextUpdate <= currTime) {
            memoryNextUpdate = currTime + MEMORY_INTERVAL;
            update_memory();
        } if (networkNextUpdate <= currTime) {
            networkNextUpdate = currTime + NETWORK_INTERVAL;
            update_network();
        } if (batteryNextUpdate <= currTime) {
            batteryNextUpdate = currTime + BATTERY_INTERVAL;
            update_battery();
        } if (timeNextUpdate <= currTime) {
            timeNextUpdate = currTime + TIME_INTERVAL;
            update_time(currTime);
        }

        // Only print at most once per update and only if data needed updating
        if (printFlag) print_status();

        struct timespec ts;
        ts.tv_sec = 0;
        ts.tv_nsec = 1000000000L / UPDATE_RATE;
        nanosleep(&ts, NULL);
    }
    return 0;
}

void print_status(void) {
    char execString[256];
    sprintf(execString, "xsetroot -name \"%s%s%s%s%s%s\"",
        weatherString, cpuString, memoryString, networkString, batteryString, timeString);
    system(execString);
}

void update_news(void) {

}

void update_pacman(void) {

}

void update_torrent(void) {

}

void update_weather(void) {

}

void update_cpu(time_t currTime) {
    // Usage module
    long double cpuData[4];
    FILE* filePtr = fopen("/proc/stat","r");
    fscanf(filePtr,"%*s %Lf %Lf %Lf %Lf",&cpuData[0],&cpuData[1],&cpuData[2],&cpuData[3]);
    fclose(filePtr);

    long currCpuUsage = cpuData[0]+cpuData[1]+cpuData[2];
    double cpuUsagePercent = (currCpuUsage - prevCpuUsage) * 100. / (currCpuUsage+cpuData[3] - prevCpuUsage-prevCpuIdle);

    prevCpuUsage = currCpuUsage;
    prevCpuIdle = cpuData[3];

    // Temperature module
    float cpuTemp;
    FILE* pipePtr = popen("sensors -A -u", "r");
    fscanf(pipePtr, "%*[^\n]\nPackage id 0:\n temp1_input: %f", &cpuTemp);
    pclose(pipePtr);

    sprintf(cpuString, "|💻%%%.1f@%.1f°C", cpuUsagePercent, cpuTemp);
    printFlag = 1;
}

// use `free -h`
void update_memory(void) {

}

void update_network(void) {

}

void update_battery(void) {

}

void update_time(time_t currTime) {
    strftime(timeString, 26, "|📅%Y-%m-%d %H:%M:%S", localtime(&currTime));
    printFlag = 1;
}

void record_sighand(int signal) {

}

void music_sighand(int signal) {

}

void torrent_sighand(int signal) {

}

void volume_sighand(int signal) {

}

void brightness_sighand(int signal) {

}