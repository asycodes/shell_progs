#include "system_program.h"

char output_file_path[PATH_MAX];

static int daemon_work()
{
    // put your full PROJECT_DIR path here  
    // strcpy(output_file_path, "[PROJECT_DIR]/dspawn.log"); 

    int num = 0;
    FILE *fptr;
    char *cwd;
    char buffer[1024];

    // write PID of daemon in the beginning
    fptr = fopen(output_file_path, "a");
    if (fptr == NULL)
    {
        return EXIT_FAILURE;
    }

    fprintf(fptr, "Daemon process running with PID: %d, PPID: %d, opening logfile with FD %d\n", getpid(), getppid(), fileno(fptr));

    // then write cwd
    cwd = getcwd(buffer, sizeof(buffer));
    if (cwd == NULL)
    {
        perror("getcwd() error");
        return 1;
    }

    fprintf(fptr, "Current working directory: %s\n", cwd);
    fclose(fptr);

    while (1)
    {

        // use appropriate location if you are using MacOS or Linux
        fptr = fopen(output_file_path, "a");

        if (fptr == NULL)
        {
            return EXIT_FAILURE;
        }

        fprintf(fptr, "PID %d Daemon writing line %d to the file.  \n", getpid(), num);
        num++;

        fclose(fptr);

        sleep(10);

        if (num == 10) // we just let this process terminate after 10 counts
            break;
    }

    return EXIT_SUCCESS;
}

int main(int argc, char **args) {
    // Setup path
    if (getcwd(output_file_path, sizeof(output_file_path)) == NULL)
    {
        perror("getcwd() error, exiting now.");
        return 1;
    }
    // printf("%s\n", output_file_path);
    strcat(output_file_path, "/dspawn.log"); 


    // make sure control is immediately returned to the shell
    pid_t intermediatePID = fork();

    if (intermediatePID < 0) {
        fprintf(stderr, "Dspawn intermediate fork has failed. Exiting now");
        return 1; // exit error
    }
    else if (intermediatePID == 0) {
        setsid(); // make the child the session leader to lose the controlling tty
        signal(SIGCHLD, SIG_IGN); // ignore SIGCHLD signal. Ensure that daemon spawn of this intermediate process will not become zombie process since this intermediate process will not wait for them.
        signal(SIGHUP, SIG_IGN); // ignore SIGHUP signal. Killing this intermediate process will not kill the daemon spawn.

        pid_t daemonPID = fork();

        if (daemonPID < 0) {
            fprintf(stderr, "Dspawn daemon fork has failed. Exiting now");
            return 1; // exit error
        }
        else if (daemonPID == 0) {
            // daemon process
            umask(0); // cause all files to be created as permission 0777 or world-RW & executable
            chdir("/"); // change the current working directory to root directory. Daemons prevent unmounting of their cwd, so change the cwd to /, which won't get unmounted.

            /* Close all open file descriptors */
            int x;
            for (x = sysconf(_SC_OPEN_MAX); x>=0; x--)
            {
                close (x);
            }
            /*
            * Attach file descriptors 0, 1, and 2 to /dev/null. */
            int fd0 = open("/dev/null", O_RDWR);
            int fd1 = dup(0);
            int fd2 = dup(0);

            daemon_work();
        }
        // exit intermediate process
        _exit(0);
    }
    // exit parent process
    // printf("parent exiting\n");
}