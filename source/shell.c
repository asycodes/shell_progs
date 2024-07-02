#include <unistd.h>
#include <fcntl.h>

// Include the shell header file for necessary constants and function declarations
#include "shell.h"

// Function to read a command from the user input
void read_command(char **cmd)
{
  // Define a character array to store the command line input
  char line[MAX_LINE];
  // Initialize count to keep track of the number of characters read
  int count = 0, i = 0;
  // Array to hold pointers to the parsed command arguments
  char *array[MAX_ARGS], *command_token;

  // Infinite loop to read characters until a newline or maximum line length is reached
  for (;;)
  {
    // Read a single character from standard input
    int current_char = fgetc(stdin);
    // Store the character in the line array and increment count
    line[count++] = (char)current_char;
    // If a newline character is encountered, break out of the loop
    if (current_char == '\n')
      break;
    // If the command exceeds the maximum length, print an error and exit
    if (count >= MAX_LINE)
    {
      printf("Command is too long, unable to process\n");
      exit(1);
    }
  }
  // Null-terminate the command line string
  line[count] = '\0';

  // If only the newline character was entered, return without processing
  if (count == 1)
    return;

  // Use strtok to parse the first token (word) of the command
  command_token = strtok(line, " \n");

  // Continue parsing the line into words and store them in the array
  while (command_token != NULL)
  {
    // printf("command_token: %s\n", command_token);
    array[i++] = strdup(command_token);  // Duplicate the token and store it
    command_token = strtok(NULL, " \n"); // Get the next token
  }

  // Copy the parsed command and its parameters to the cmd array
  for (int j = 0; j < i; j++)
  {
    // printf("command_token: %s\n", array[j]);
    cmd[j] = array[j];
  }
  // Null-terminate the cmd array to mark the end of arguments
  cmd[i] = NULL;
}

// Function to display the shell prompt
void type_prompt(char* cwd)
{
  fflush(stdout); // Flush the output buffer
  printf("(╯°□°)╯%s> ", cwd); // Print the shell prompt
  // printf("$$ + %s \n");  // Print the shell prompt
}

int shell_cd(char **args)
{
  if (args[1] == NULL)
  {
    // If no argument is provided, change to the user's home directory
    chdir(getenv("HOME"));
  }
  else
  {
    // Change to the specified directory
    if (chdir(args[1]) != 0)
    {
      // Print an error message if the directory change fails
      perror("dc");
    }
  }
}

int shell_help(char **args) {
  printf("Welcome to the shell help guide!\n");
  printf("The following commands are built in:\n");
  for (int i = 0; i < num_builtin_functions(); i++)
  {
    printf("  %s\n", builtin_commands[i]);
  }
  return 0;
}

int shell_exit(char **args) {
  exit(0);
}

int shell_usage(char **args) {
  // printf("UNIMPLEMENTED!!!!\n");
 
  // if the argument is empty --> not valid instruction
  if (args[1] == NULL) {
    printf("Command not given. Type usage <command>.\n");
    return 1;
  }

  // show basic instruction for the specified command
  if (strcmp(args[1], "cd") == 0) {
    printf("Type: cd directory_name to change the current working directory of the shell\n");
  } else if (strcmp(args[1], "help") == 0) {
    printf("Type: help for supported commands\n");
  } else if (strcmp(args[1], "exit") == 0) {
    printf("Type: exit to terminate the shell gracefully\n");
  } else if (strcmp(args[1], "usage") == 0) {
    printf("Type: usage cd/help/exit\n");
  } else if (strcmp(args[1], "env") == 0) {
    printf("Type: env to list all registered env variables\n");
  } else if (strcmp(args[1], "setenv") == 0) {
    printf("Type: setenv ENV=VALUE to set a new env variable\n");
  } else if (strcmp(args[1], "unsetenv") == 0) {
    printf("Type: unsetenv ENV to remove this env from the list of env variables\n");
  } else {
    printf("The command you gave: %s, is not part of CSEShell's builtin command\n", args[1]);
  }
  return 1;
}

int list_env(char **args) {
  extern char **environ;
  // Loop through the environment variables and print them
  for (char **env = environ; *env != NULL; env++) {
    printf("%s\n", *env);
  }
}

int set_env_var(char **args) {
  // Set the environment variable with the provided key and value
  if (args[1] != NULL && args[2] != NULL) {
    if (setenv(args[1], args[2], 1) != 0) {
      perror("setenv");
    }
  } else {
    printf("Invalid arguments. Usage: set_env <key> <value>\n");
  }
}

int unset_env_var(char **args) {
  unsetenv(args[1]);
}

char* getCwdPath() {
  char* cwd = malloc(PATH_MAX);
  if (getcwd(cwd, PATH_MAX) == NULL)
  {
    perror("getcwd");
  }
  // printf("cwd: %s\n", cwd);
  return cwd;
}

char* getCwdBinPath() {
  char* cwd = getCwdPath();
  char* bin_path = malloc(PATH_MAX);
  char* current_path = getenv("PATH");
  // the ordering is important. cwd commands supercede other commands.
  snprintf(bin_path, PATH_MAX, "%s/bin:%s", cwd, current_path);
  // snprintf(bin_path, PATH_MAX, "%s/bin", cwd);
  return bin_path;
}

int executeCommand(char** cmd) {

  ///----------------- BUILT IN COMMANDS -----------------///
  int builtin = 0;
  // Loop through our command list and check if the commands exist in the builtin command list
  for (int command_index = 0; command_index < num_builtin_functions(); command_index++)
  {
    if (strcmp(cmd[0], builtin_commands[command_index]) == 0) // Assume args[0] contains the first word of the command
    {
      // printf("Command found: %s\n", cmd[0]);
      // We will create new process to run the function with the specific command except for builtin commands.
      // These have to be done by the shell process. 
      (*builtin_command_func[command_index])(cmd);
      builtin = 1;
      break;
    }
  }
  if (builtin) {
    return 0;
  }

  ///----------------- NON BUILT IN COMMANDS -----------------///

  pid_t child_pid = fork();

  if (child_pid < 0) {
    fprintf(stderr, "Fork has failed. Exiting now");
    return 1; // exit error
  }
  else if (child_pid == 0) {
    // within child process, the MVP COMMAND!!!
    int status = execvp(cmd[0], cmd);
    if (status == -1) {
      _exit(-1);
    }
  }
  else {
    // within parent process
    int status;
    // wait for this child pid to change state.
    pid_t wpid = waitpid(child_pid, &status, 0); 

    // -1 value indicates that waitpid has failed abnormally. No child process, invalid argument, sig interrupt.
    if (wpid == -1) {
      // send descriptive error message to stderr
      perror("waitpid");
      exit(EXIT_FAILURE);
    }

    // check if child process terminated normally by checking the exit status. Non-zero value means normal termination.
    if (WIFEXITED(status)) {
      // 0 means succeed, 1 means find command didn't find, -1 means execv didn't work
      int child_exit_status = WEXITSTATUS(status);

      // -1 from child becomes 255 for some reason. Unsigned integer?
      if (child_exit_status == 255) {
        // the following lines are all error handling code if execv returns, meaning it failed.
        printf("Command %s not found\n", cmd[0]);
        // Free the allocated memory for the command arguments before exiting
        for (int i = 0; cmd[i] != NULL; i++)
        {
          free(cmd[i]);
        }
        // memset(cwd, '\0', sizeof(cwd)); // clear the cwd array
      }
    }
    
  }
}

void shellIntro() {
  printf("_    ___  ____ _  _ . ___    _ _ _ ____ _  _ ___    ___  ____\n");
  printf("|    |  \\ |  | |\\ | '  |     | | | |__| |\\ |  |     |  \\ |  |\n");            
  printf("|    |__/ |__| | \\|    |     |_|_| |  | | \\|  |     |__/ |__|\n");
  printf("\n");
  printf("(╯°□°）╯︵ ┻━┻  ...  ┬─┬ノ( º _ ºノ)  ...  ノಠ益ಠ)ノ彡┻━┻\n");           
  printf("____ _  _ ____ _    _       ____ _  _ _   _ _  _ ____ ____ ____   /  /  /\n");
  printf("[__  |__| |___ |    |       |__| |\\ |  \\_/  |\\/| |  | |__/ |___  /  /  /\n");
  printf("___] |  | |___ |___ |___    |  | | \\|   |   |  | |__| |  \\ |___ .  .  .  \n");
  printf("\n");
}

void execRC() {
    char *cmd[MAX_ARGS];
  FILE *fptr;
  fptr = fopen(".cseshellrc", "ra");
  if (fptr) {
    fputs("\n", fptr);
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), fptr) != NULL) {
      // printf("line: %s\n", line);
      // DO NOT CHECK FOR END OF FILE POINTER! WILL CAUSE THE LAST LINE TO BE IGNORED
      if (line[0] == '\n') {
        continue;
      }

      // Compare the first 4 bytes for PATH. If present, then set the PATH.
      if (strncmp(line, "PATH", 4) == 0) {
        char *path = strtok(line, "=");
        path = strtok(NULL, "="); // get the next token, which is the actual path
        // char final_path[PATH_MAX];
        // char* cwdBinPath = getCwdBinPath();
        // snprintf(final_path, sizeof(final_path), "%s:%s", path, cwdBinPath);
        setenv("PATH", path, 1);
      }

      else {
        // execute the line as a command
        char* command = strdup(line);
        char* command_args[MAX_ARGS];
        int i = 0;
        char* command_token = strtok(command, " \n");
        while (command_token != NULL)
        {
          // printf("command_token: %s\n", command_token);
          command_args[i++] = strdup(command_token);  // Duplicate the token and store it
          command_token = strtok(NULL, " \n"); // Get the next token
        }
        command_args[i] = NULL;
        executeCommand(command_args);
      }
    }
    fclose(fptr);
  }
}

// The main function where the shell's execution begins
int main(void)
{
  // Clear the screen
  #ifdef _WIN32
    system("cls"); // Windows command to clear screen
  #else
    system("clear"); // UNIX/Linux command to clear screen
  #endif

  shellIntro();

  setenv("PATH", getCwdBinPath(), 1);
  execRC();

    // Define an array to hold the command and its arguments
  char *cmd[MAX_ARGS];

  // Infinite loop to keep the shell running
  while(1) {
    type_prompt(getCwdPath());     // Display the prompt
    read_command(cmd); // Read a command from the user
    executeCommand(cmd); // Execute the command
  }
}
