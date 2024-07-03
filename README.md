[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-24ddc0f5d75046c5622901739e7c5dd533143b0c8e959d652212380cedb1ea36.svg)](https://classroom.github.com/a/u16ttUuk)
# CSEShell

CSEShell is a simple, custom shell for Unix-based systems, designed to provide an interface for executing system programs. This project includes a basic shell implementation, a set of system programs (`find`, `ld`, `ldr`), and some test files.

## Directory Structure

The project is organized as follows:

- `bin/` - Contains compiled executables for system programs.
  - `find` - Program to find files.
  - `ld` - Program for listing the contents of the current directory.
  - `ldr` - Program for listing the contents of the current directory recursively.
- `cseshell` - The main executable for the CSEShell.
- `files/` - Contains various test files used with the shell and system programs.
  - `combined.txt`, `file1.txt`, `file2.txt`, ... - Test text files.
  - `notes.pdf` - A PDF file for testing.
  - `ss.png` - An image file.
  - `user_config` - A text file to track user's preference (language and prompt emoji)
- `makefile` - Makefile for building the CSEShell and system programs.
- `source/` - Source code for the shell and system programs.
  - `shell.c` and `shell.h` - Source and header files for the shell.
  - `system_programs/` - Source code and header for the system programs.
    - `language.c` - Allows user to change language responses
    - `vibe.c` - Allows user to change the emoji of the prompt
    - `backup.c ` - Zips directory
    - `sys.c` - Print out basic info of user's OS
    - `dspawn.c` - Summons daemon process
    - `dcheck.c` - Count of Live Daemon
    - `find.c` - Searches for files in a directory.
    - `ld.c` - List the contents of the curent directory.
    - `ldr.c` - List the contents of the current directory recursively.

## Building the Project

To build the CSEShell and system programs, run the following command in the root directory of the project:

```bash
make
```

This will compile the source code and place the executable files in the appropriate directories, where the source code will be placed into the bin directory

## Running CSEShell

After building, you can start the shell by running:

```bash
./cseshell
```

From there, you can execute built-in commands and any of the included system programs (e.g., `find`, `ld`, `ldr` etc).

## System Programs

System Programs available are as seen under the System Program directory list above!

## Additional features supported

- Multi Language Supported (English and Malay), however its limited to 2 system programs (help and usage)
- Improved Prompt Details, time, user.
- User Profile, with the user_config file, the shell is able to be manipulated such that it fits to the user's preferences.
- Better UI control for user, user can change text emoji as seen in the prompt (happy,tired,angry)

## Inclusivity
- With multi language support, those (malay-speaking) with limited knowledge of english will be able to comprehend responses made by the shell.


## Sustainability
- With user preferences feature, users are able to edit the shell's appearance and experience, increasing the shell's usability, discouraging them from consuming other packages/libraries/shells that might be less energy-friendly or storage-friendly
