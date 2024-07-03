#include "system_program.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//check language inputted   
int is_valid_language(const char *language) {
    return strcmp(language, "english") == 0 || strcmp(language, "malay") == 0;
}


void change_language(const char *new_language) {
    if (!is_valid_language(new_language)) {
        fprintf(stderr, "Invalid language: %s. Valid languages are 'english' or 'malay'.\n", new_language);
        return;
    }

    FILE *file = fopen("files/user_config.txt", "r");
    if (file == NULL) {
        perror("Failed to open config file for reading");
        return;
    }

    char buffer[256];
    char *emotion = NULL;
    while (fgets(buffer, sizeof(buffer), file)) {
        if (strncmp(buffer, "prompt_emotion=", 15) == 0) {
            emotion = strdup(buffer);
        }
    }
    fclose(file);

    file = fopen("files/user_config.txt", "w");
    if (file == NULL) {
        perror("Failed to open config file for writing");
        free(emotion);
        return;
    }
    if (emotion) {
        fprintf(file, "%s", emotion);
        free(emotion);
    }
    fprintf(file, "lang=%s\n", new_language);
    fclose(file);

    printf("Language changed to %s\n", new_language);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <new_language>, try english or malay\n", argv[0]);
        return 1;
    }

    change_language(argv[1]);
    return 0;
}
