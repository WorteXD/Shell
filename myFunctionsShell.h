#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/wait.h>
#define SIZE_BUFF 256
char *getInputFromUser();
/**
 * Splits a string into individual tokens based on whitespace.
 *
 * This function takes a string as input and tokenizes it based on whitespace
 * characters (space, tab, newline). It dynamically allocates memory for an
 * array of strings to store the tokens. The caller is responsible for freeing
 * the memory allocated for the array and its elements when it's no longer needed.
 *
 * @param input A pointer to the string to be split.
 *
 * @return A dynamically allocated array of strings (char**) containing the tokens.
 *         The last element of the array is NULL. Returns NULL if input is NULL
 *         or if memory allocation fails.
 *
 * @note The input string will be modified (tokenized) by this function.
 *       Make sure to use a copy of the original string if preservation
 *       of the original string is required.
 *
 * @warning The caller is responsible for freeing the memory allocated for
 *          the returned array and its elements using free().
 */
char **splitArguments(char *);
/**
 * Retrieves and displays the current working directory, username, and hostname.
 *
 * This function replicates the behavior of a standard Linux terminal prompt.
 * It displays:
 * - The username in **green**.
 * - The hostname in **cyan**.
 * - The current working directory in **blue**.
 * - A `$` symbol at the end.
 *
 * @note This function should be called whenever the shell needs to display a prompt.
 *       It does not return any value but directly prints the formatted prompt.
 */
void getLocation();
/**
 * Exits the shell properly.
 *
 * This function handles the "exit" command and ensures the shell terminates correctly.
 * It trims unnecessary spaces, checks if the command is valid, prints a farewell message,
 * and then exits the shell using exit(0).
 *
 * @param input A pointer to the string entered by the user.
 *
 * @note If the input contains extra text after "exit", an error message is displayed.
 */
void logout(char *input);
void echo(char **arguments);
/**
 * Changes the current working directory of the shell.
 *
 * This function updates the working directory of MyShell based on the user input.
 * It correctly handles directories with spaces and absolute/relative paths.
 *
 * @param args A pointer to an array of strings containing the directory path.
 *             The first element is "cd", and the second element is the target path.
 *
 * @note If no directory is specified, it defaults to the home directory.
 * @note If the directory does not exist, an error message is displayed.
 * 
 * @warning Ensure the input is valid to prevent errors.
 */
void cd(char **args);
/**
 * Copies a file from a source path to a destination path.
 *
 * This function takes two arguments: the source file path and the destination file path.
 * It reads the contents of the source file and writes them into the destination file.
 * If the destination file already exists, it will be overwritten.
 *
 * @param args A pointer to an array of strings (char**), where:
 *             args[1] is the source file path
 *             args[2] is the destination file path
 *
 * @return void
 *
 * @note If the source file does not exist, an error message is printed.
 *       If the user lacks the necessary permissions, an error message is printed.
 *       Proper file handling ensures no memory leaks.
 */
void cp(char **args);
/**
 * Lists the contents of the current directory.
 *
 * This function opens the current directory and prints each file
 * and folder name in a single line. It uses `opendir()` to open
 * the directory and `readdir()` to read its contents.
 *
 * @note Hidden files (starting with `.`) will also be displayed.
 * @warning Ensure the directory exists before calling this function.
 */
void get_dir();
/**
 * Deletes a specified file from the system.
 *
 * This function attempts to delete a file using the `remove()` system call.
 * If successful, it prints a confirmation message. If the file does not exist
 * or another error occurs, it displays an error message.
 *
 * @param args A pointer to the array of command arguments.
 *             The second element (`args[1]`) should contain the filename.
 *
 * @note This function does not support deleting directories.
 * @warning Ensure the correct file is specified to prevent unintended deletion.
 */
void delete(char **args);
void systemCall(char **arguments);
/**
 * Executes two commands with a pipe between them.
 *
 * This function creates a pipe, forks two child processes, and redirects
 * the output of the first command to the input of the second command.
 * The two commands are executed using `execvp()`.
 *
 * @param args1 First command and its arguments (NULL-terminated array)
 * @param args2 Second command and its arguments (NULL-terminated array)
 *
 * @note The function handles `pipe()`, `fork()`, `dup2()`, and `execvp()`.
 *       Both child processes will be waited on using `wait()`.
 */
void mypipe(char **args1, char **args2);
/**
 * Moves or renames a file or directory.
 *
 * This function allows renaming a file or directory or moving it to a different location.
 * It takes two arguments: the source path and the destination path.
 * The function utilizes `rename()` to perform the move operation.
 *
 * @param args A pointer to an array of strings (char**), where:
 *             args[1] is the source file/directory path.
 *             args[2] is the destination file/directory path.
 *
 * @note If the source or destination path is missing, an error message is displayed.
 *       If the operation fails due to invalid paths or permissions, an appropriate error is printed.
 *       The function ensures proper handling of paths with spaces.
 *
 * @warning Ensure correct paths are provided to avoid unintended file movements or renaming.
 */
void move(char **args);
/**
 * @brief Appends text to a file.
 * 
 * This function takes a filename and appends text to it. The text does not require quotes.
 * If the file does not exist, an error message is displayed.
 * 
 * @param args The command-line arguments, where args[1] is the filename and args[2+] is the text to append.
 */
void echoppend(char **args);
/**
 * Writes content to a file, overwriting any existing content.
 *
 * @param args The command arguments. Expected format: echowrite <filename> <text>
 *             - args[1]: The file to write to.
 *             - args[2+]: The text to be written to the file.
 *
 * Function behavior:
 * - Opens the specified file in write mode (creates the file if it does not exist).
 * - Writes the provided text into the file.
 * - Replaces existing content in the file.
 * - Handles errors such as missing arguments or inability to open the file.
 */
void echowrite(char **args);
/**
 * Reads the contents of a file and prints them to the standard output.
 *
 * @param args The command arguments. Expected format: read <filename>
 *             - args[1]: The file to read from.
 *
 * Function behavior:
 * - Opens the specified file in read mode.
 * - Reads and prints its content line by line.
 * - Handles errors such as missing arguments or inability to open the file.
 */
void _read(char **args);
/**
 * wordCount - Counts words, lines, or bytes in a file.
 * @args: Command arguments. Requires:
 *        args[1] - File path
 *        args[2] - Option: "-w" for words, "-l" for lines, "-c" for bytes
 * 
 * If no file is provided, prints an error.
 * If no option is provided, prints an error.
 * If an invalid option is given, prints an error.
 */
void wordCount(char **args);