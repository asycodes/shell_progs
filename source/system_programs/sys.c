#include "system_program.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/utsname.h>
#include <unistd.h>
#include <string.h>

void printOSInfo() {
    // get the name and information about the current operating system. 
    struct utsname unameData;
    uname(&unameData);
    printf("Operating System: %s\n", unameData.sysname);
    printf("Kernel Version: %s\n", unameData.release);
}

void printTotalMemory() {
    FILE* file = fopen("/proc/meminfo", "r");
    char line[256];
    long totalMemory = 0;
    long freeMemory = 0;
    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "MemTotal:", 9) == 0) {
            sscanf(line + 10, "%ld", &totalMemory);
        } else if (strncmp(line, "MemFree:", 8) == 0) {
            sscanf(line + 10, "%ld", &freeMemory);
        }
    }
    fclose(file);

    long usedMemory = totalMemory - freeMemory;
    double usedMemoryMB = usedMemory / 1024.0;
    double totalMemoryMB = totalMemory / 1024.0;

    printf("Memory Usage: %.2f MB / %.2f MB\n", usedMemoryMB, totalMemoryMB);
}

void printCurrentUser() {
    char* user = getenv("USER");
    if (user == NULL) {
        perror("getenv() error");
    } else {
        printf("Current User: %s\n", user);
    }
}

void printCPUName() {
    FILE* file = fopen("/proc/cpuinfo", "r");
    char line[256];

    if (file != NULL) {
        while (fgets(line, sizeof(line), file)) {
            if (strncmp(line, "model name", 10) == 0) {
                char* modelName = strchr(line, ':') + 2; // Skip past the ": "
                printf("CPU Name: %s", modelName);
                break;
            }
        }
        fclose(file);
    } else {
        perror("/proc/cpuinfo");
    }
}

void printCPUCount() {
    long num_cores;

    num_cores = sysconf(_SC_NPROCESSORS_ONLN); // Get the number of online (available) cores
    if (num_cores > 0) {
        printf("Number of CPU cores available: %ld\n", num_cores);
    } else {
        perror("sysconf");
    }
}

void printCPUUsage() {
    FILE* file;
    long double a[4], b[4], loadavg;
    file = fopen("/proc/stat","r");
    fscanf(file,"%*s %Lf %Lf %Lf %Lf",&a[0],&a[1],&a[2],&a[3]);
    fclose(file);
    sleep(1);

    file = fopen("/proc/stat","r");
    fscanf(file,"%*s %Lf %Lf %Lf %Lf",&b[0],&b[1],&b[2],&b[3]);
    fclose(file);

    loadavg = ((b[0]+b[1]+b[2]) - (a[0]+a[1]+a[2])) / ((b[0]+b[1]+b[2]+b[3]) - (a[0]+a[1]+a[2]+a[3]));
    printf("CPU Usage (1 sec avg): %.2Lf%%\n",loadavg*100);
}

void printGPUName() {
    FILE* pipe = popen("lspci", "r");
    char line[256];

    if (pipe != NULL) {
        while (fgets(line, sizeof(line), pipe)) {
            if (strstr(line, "VGA") || strstr(line, "3D")) {
                char* gpuName = strchr(line, ':') + 2; // Skip past the ": "
                printf("GPU Name: %s", gpuName);
                break;
            }
        }
        pclose(pipe);
    } else {
        perror("lspci");
    }
}

int main() {
    printf("\n");
    printOSInfo();
    printTotalMemory();
    printCurrentUser();
    printCPUName();
    printCPUCount();
    printCPUUsage();
    printGPUName();
    printf("\n");
    return 0;
}