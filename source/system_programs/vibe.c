#include "system_program.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//must validate the emotion user picked
int is_valid_emotion(const char *emotion) {
    return strcmp(emotion, "happy") == 0 ||
           strcmp(emotion, "angry") == 0 ||
           strcmp(emotion, "tired") == 0;
}


void change_emotion(const char *new_emotion) {
    if (!is_valid_emotion(new_emotion)) {
        fprintf(stderr, "Invalid emotion: %s. Valid emotions are 'happy', 'angry', or 'tired'.\n", new_emotion);
        return;
    }

    FILE *file = fopen("files/user_config.txt", "r");
    if (file == NULL) {
        perror("Failed to open config file for reading");
        return;
    }

    char buffer[256];
    char *lang = NULL;
    while (fgets(buffer, sizeof(buffer), file)) {
        if (strncmp(buffer, "lang=", 5) == 0) {
            lang = strdup(buffer);
        }
    }
    fclose(file);


    file = fopen("files/user_config.txt", "w");
    if (file == NULL) {
        perror("Failed to open config file for writing");
        free(lang);
        return;
    }
    fprintf(file, "prompt_emotion=%s\n", new_emotion);
    if (lang) {
        fprintf(file, "%s", lang);
        free(lang);
    }
    fclose(file);

    printf("Emotion changed to %s\n", new_emotion);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <new_emotion>, try happy, tired or angry\n", argv[0]);
        return 1;
    }

    change_emotion(argv[1]);
    return 0;
}
