#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>

#define BUFFER_SIZE 1024

void zip_directory(const char *dir, const char *zip_filename) {
    char command[BUFFER_SIZE * 2];

    // ensures that any spaces or special characters in the paths are handled correctly by the shell
    snprintf(command, sizeof(command), "zip -r \"%s\" \"%s\"", zip_filename, dir);
    int result = system(command);
    if (result != 0) {
        fprintf(stderr, "Failed to create zip archive: %s\n", zip_filename);
        exit(EXIT_FAILURE);
    }
}

int main() {
    char *backup_dir = getenv("BACKUP_DIR");
    if (!backup_dir) {
        fprintf(stderr, "Error: BACKUP_DIR environment variable is not set.\n");
        return EXIT_FAILURE;
    }

    char project_dir[BUFFER_SIZE];
    if (getcwd(project_dir, sizeof(project_dir)) == NULL) {
        perror("getcwd() error");
        return EXIT_FAILURE;
    }

    char archive_dir[BUFFER_SIZE];
    snprintf(archive_dir, sizeof(archive_dir), "%s/archive", project_dir);
    if (mkdir(archive_dir, 0755) == -1 && errno != EEXIST) {
        perror("mkdir() error");
        return EXIT_FAILURE;
    }

    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char datetime[BUFFER_SIZE];
    strftime(datetime, sizeof(datetime), "%Y%m%d%H%M%S", t);

    char backup_filename[BUFFER_SIZE];
    snprintf(backup_filename, sizeof(backup_filename), "%s/backup-%s.zip", archive_dir, datetime);

    printf("Creating backup of '%s' at '%s'\n", backup_dir, backup_filename);
    zip_directory(backup_dir, backup_filename);

    printf("Backup created successfully.\n");

    return 0;
}