#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/resource.h>
#include <limits.h>
#include <signal.h>
#include <wordexp.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <arpa/inet.h>
#include <net/if.h>

#define WEATHER_API ""

// Frequency per second to check for updates and signals
#define UPDATE_RATE 10

// Interval in seconds to auto-update modules, other modules are updated manually using signal
#define NEWS_INTERVAL 300
#define PKGUP_INTERVAL 600
#define TORRENT_INTERVAL 10
#define WEATHER_INTERVAL 600
#define CPU_INTERVAL 10
#define MEMORY_INTERVAL 10
#define VOLUME_INTERVAL 10
#define BRIGHTNESS_INTERVAL 10
#define NETWORK_INTERVAL 10
#define BATTERY_INTERVAL 60
#define TIME_INTERVAL 60

// Usage:
//  kill -s [SIGNAL_CODES] $(pidof dwm-status)
//  static const char *volupcmd[]  = { "sh", "-c", "pulsemixer --change-volume 5; kill -s 38 $(pidof dwm-status)", NULL };
//  Signal codes. Use 34 and higher to avoid interfearing with critical signals
#define RECORD_SIGNAL 34
#define MUSIC_SIGNAL 35
#define PKGUP_SIGNAL 36
#define TORRENT_SIGNAL 37
#define VOLUME_SIGNAL 38
#define BRIGHTNESS_SIGNAL 39

int printFlag = 0;

long prevCpuUsage = 0L;
long prevCpuIdle = 0L;

char recordString[16] = {0};
char musicString[32] = {0};
char newsString[16] = {0};
char pkgupString[16] = {0};
char torrentString[16] = {0};
char weatherString[24] = {0};
char cpuString[32] = {0};
char memoryString[32] = {0};
char volumeString[32] = {0};
char brightnessString[16] = {0};
char networkString[128] = {0};
char batteryString[16] = {0};
char timeString[24] = {0}; 

void print_status(void);
int expand_path(char*, char*);

void update_news(void);
void update_pkgup(void);
void update_torrent(void);
void update_weather(void);
void update_cpu(void);
void update_memory(void);
void update_volume(void);
void update_brightness(void);
void update_network(void);
void update_battery(void);
void update_time(time_t);

// Signal handlers
void record_sighand(int);
void music_sighand(int);
void pkgup_sighand(int);
void torrent_sighand(int);
void volume_sighand(int);
void brightness_sighand(int);

int main(void) {
    // Set process to lowest priority for less CPU usage, needs privileges?
    setpriority(PRIO_PROCESS, 0, INT_MAX);
    
    music_sighand(0);

    signal(RECORD_SIGNAL, record_sighand);
    signal(MUSIC_SIGNAL, music_sighand);
    signal(PKGUP_SIGNAL, pkgup_sighand);
    signal(TORRENT_SIGNAL, torrent_sighand);
    signal(VOLUME_SIGNAL, volume_sighand);
    signal(BRIGHTNESS_SIGNAL, brightness_sighand);

    int newsNextUpdate = 0;
    int pkgupNextUpdate = 0;
    int torrentNextUpdate = 0;
    int weatherNextUpdate = 0;
    int cpuNextUpdate = 0;
    int memoryNextUpdate = 0;
    int volumeNextUpdate = 0;
    int brightnessNextUpdate = 0;
    int networkNextUpdate = 0;
    int batteryNextUpdate = 0;
    int timeNextUpdate = 0;

    while (1) {
        time_t currTime = time(NULL);

        if (newsNextUpdate <= currTime) {
            newsNextUpdate = currTime + NEWS_INTERVAL;
            update_news();
        } if (pkgupNextUpdate <= currTime) {
            pkgupNextUpdate = currTime + PKGUP_INTERVAL;
            update_pkgup();
        } if (torrentNextUpdate <= currTime) {
            torrentNextUpdate = currTime + TORRENT_INTERVAL;
            update_torrent();
        } if (weatherNextUpdate <= currTime) {
            weatherNextUpdate = currTime + WEATHER_INTERVAL;
            update_weather();
        } if (cpuNextUpdate <= currTime) {
            cpuNextUpdate = currTime + CPU_INTERVAL;
            update_cpu();
        } if (memoryNextUpdate <= currTime) {
            memoryNextUpdate = currTime + MEMORY_INTERVAL;
            update_memory();
        } if (volumeNextUpdate <= currTime) {
            volumeNextUpdate = currTime + VOLUME_INTERVAL;
            update_volume();
        } if (brightnessNextUpdate <= currTime) {
            brightnessNextUpdate = currTime + BRIGHTNESS_INTERVAL;
            update_brightness();
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
        if (printFlag) {
            printFlag = 0;
            print_status();
        }

        struct timespec ts;
        ts.tv_sec = 0;
        ts.tv_nsec = 1000000000L / UPDATE_RATE;
        nanosleep(&ts, NULL);
    }
    return 0;
}

void update_news(void) {

}

void update_pkgup(void) {
    int ch, pkgCount = 0;

    system("pacman -Sy");

    FILE* pipePtr = popen("pacman -Qu", "r");
    do {
        ch = fgetc(pipePtr);
        if(ch == '\n')
            ++pkgCount;
    } while (ch != EOF);
    pclose(pipePtr);

    pipePtr = popen("yay -Qu", "r");
    do {
        ch = fgetc(pipePtr);
        if(ch == '\n')
            ++pkgCount;
    } while (ch != EOF);
    pclose(pipePtr);

    if (pkgCount)
        sprintf(pkgupString, " 📦%d", pkgCount);
    else
        pkgupString[0] = '\0';
    printFlag = 1;
}

void update_torrent(void) {

}

void update_weather(void) {

}

void update_cpu(void) {
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

    sprintf(cpuString, " 💻%.1f%%@%.1f°C", cpuUsagePercent, cpuTemp);
    printFlag = 1;
}

void update_memory(void) {
    char total[8], used[8];
    FILE* pipePtr = popen("free -h", "r");
    fscanf(pipePtr, "%*[^\n]\n%*s %s %s", total, used);
    pclose(pipePtr);

    sprintf(memoryString, " 🧠%s/%s", used, total);
    printFlag = 1;
}

void update_volume(void) {
    int mute, volume;
    char icon[8] = {0}, volVal[8] = {0};

    FILE* pipePtr = popen("pulsemixer --get-mute", "r");
    fscanf(pipePtr, "%d", &mute);
    pclose(pipePtr);

    if(mute)
        strcpy(icon, "🔇");
    else {
        FILE* pipePtr = popen("pulsemixer --get-volume", "r");
        fscanf(pipePtr, "%d", &volume);
        pclose(pipePtr);

        if(volume > 70)
            strcpy(icon, "🔊");
        else if (volume > 30)
            strcpy(icon, "🔉");
        else
            strcpy(icon, "🔈");

        sprintf(volVal, "%d%%", volume);
    }  

    sprintf(volumeString, " %s%s", icon, volVal);
    printFlag = 1;
}

void update_brightness(void) {

}

// TODO: refactor. This code is slow garbage. Store string in buffer and sprintf all at the same time
void update_network(void) {
    // Network strength/status module
    char networkStatus[8] = {0}, pathBuffer[256];

    if(!expand_path(pathBuffer, "/sys/class/net/e*/operstate")) {
        FILE* filePtr = fopen(pathBuffer, "r");
        fscanf(filePtr, "%s", networkStatus);
        fclose(filePtr);
    }

    if (!strcmp(networkStatus, "up"))
        strcpy(networkString, " 🌐");
    else {
        if(!expand_path(pathBuffer, "/sys/class/net/w*/operstate")) {
            FILE* filePtr = fopen(pathBuffer, "r");
            fscanf(filePtr, "%s", networkStatus);
            fclose(filePtr);
        }

        if (!strcmp(networkStatus, "down")) {
            strcpy(networkString, " ⚠️🌐");
            printFlag = 1;
        } else {
            FILE* filePtr = fopen("/proc/net/wireless", "r");
            fscanf(filePtr, "%*[^\n]\n%*[^\n]\n %*s %*s %s", networkStatus);
            fclose(filePtr);

            sprintf(networkString, " 📶%.0f%%", strtof(networkStatus, NULL) * 100 / 70);
        }
    }

    // IP module
    struct ifaddrs *addresses;
    getifaddrs(&addresses);
    struct ifaddrs *addressPtr = addresses;
    int activeInterfaces = 0;

    while (addressPtr) {
        if (addressPtr->ifa_addr &&                     // addressStruct is valid
        addressPtr->ifa_addr->sa_family == AF_INET &&   // interface is of family type INET
        addressPtr->ifa_flags & IFF_RUNNING &&          // interface is up and running
        strcmp(addressPtr->ifa_name, "lo")) {           // not loopback interface
            if(activeInterfaces) strcat(networkString, "+");
            strcat(networkString, inet_ntoa(((struct sockaddr_in *)addressPtr->ifa_addr)->sin_addr));
            ++activeInterfaces;
        }
        addressPtr = addressPtr->ifa_next;
    }

    freeifaddrs(addresses);
    printFlag = 1;
}

void update_battery(void) {

}

void update_time(time_t currTime) {
    strftime(timeString, 26, " 📅%a%d%b 🕓%H:%M ", localtime(&currTime));
    printFlag = 1;
}

void record_sighand(int signal) {

}

void music_sighand(int signal) {

}

void pkgup_sighand(int signal) {
    update_pkgup();
}

void torrent_sighand(int signal) {

}

void volume_sighand(int signal) {
    update_volume();
}

void brightness_sighand(int signal) {

}

// Helper functions

// Prints stringn to xsetroot
void print_status(void) {
    char execString[512];
    sprintf(execString, "xsetroot -name \"%s%s%s%s%s%s%s%s%s%s%s%s%s\"",
        recordString, musicString, newsString, pkgupString, torrentString,
        weatherString, cpuString, memoryString, volumeString, brightnessString,
        networkString, batteryString, timeString);
    system(execString);
}

// Expands path using shell pattern matching expansion
//  pathStringSource | Input path string to be expanded
//  pathStringDest | Make sure it's bigger than inPath string or else smashing will occur
//  returns 0 on success, -1 on failure
int expand_path(char* pathStringDest, char* pathStringSource) {
    wordexp_t path;
    if (wordexp(pathStringSource, &path, 0)) return -1;
    strcpy(pathStringDest, path.we_wordv[0]);
    for(int i = 1; i < path.we_wordc; ++i)
        strcat(pathStringDest, path.we_wordv[i]);
    return 0;
}