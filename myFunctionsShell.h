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
void mypipe(char **, char **);
void move(char **args);
void echoppend(char **args);
void echowrite(char **args);
void _read(char **args);
void wordCount(char **args);