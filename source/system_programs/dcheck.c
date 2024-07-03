#include "system_program.h"


int main() {
    int status;
    char buffer[256];
    FILE *file;
    int count = 0;
    const char *term = "dspawn";
    //printf("dcheck running...\n");

    // system function allows to make shell commands
    status = system("ps -ef > files/output_dcheck.txt"); //try to put test file within the appropriate place, and replace the content within
    if (status == -1) {
        perror("system");
        return 1;
    } else {
        file = fopen("files/output_dcheck.txt","r");
        if (file == NULL){
            perror("File open failed");
            return 1;
        }
        while(fgets(buffer,sizeof(buffer),file) != NULL){
            // we check line by line !
            if(strstr(buffer,term) != NULL){
                count++;
            }
        }
        fclose(file);
        printf("Live daemon count: %d\n", count);
    }

    //printf("dcheck completed.\n");

    return 0;
}
