#include "myFunctionsShell.h"
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <limits.h>
#include <pwd.h>       // Required for getpwuid()
#include <sys/types.h> // Required for user ID functions
#include <sys/stat.h> // Required for checking if a path is a directory
#include <string.h>
#include <stdlib.h>


char *getInputFromUser()
{

    char ch;
    int len = 0;
    char *input = (char *)malloc(sizeof(char) * (len + 1));
    *input = '\0';
    while ((ch = getchar()) != '\n')
    {
        *(input + len) = ch;
        input = (char *)realloc(input, (++len + 1));
    }
    *(input + len) = '\0';

    return input;
}

char **splitArguments(char *str) {
    if (str == NULL)
        return NULL;

    int size = 10;
    int index = 0;
    char **args = malloc(size * sizeof(char *));
    if (!args) {
        perror("malloc failed");
        return NULL;
    }

    char *token = strtok(str, " ");
    while (token != NULL) {
        if (token[0] == '"') { 
            // Handle quoted arguments
            char temp[1024] = {0}; 
            strcat(temp, token); // Start with the first token

            while (token[strlen(token) - 1] != '"' && (token = strtok(NULL, " ")) != NULL) {
                strcat(temp, " ");
                strcat(temp, token);
            }

            // Remove the quotes
            if (temp[0] == '"' && temp[strlen(temp) - 1] == '"') {
                temp[strlen(temp) - 1] = '\0';
                args[index++] = strdup(temp + 1);
            } else {
                args[index++] = strdup(temp);
            }
        } else {
            args[index++] = strdup(token);
        }

        if (index >= size - 1) {
            size *= 2;
            args = realloc(args, size * sizeof(char *));
        }

        token = strtok(NULL, " ");
    }

    args[index] = NULL; // Ensure NULL termination
    return args;
}


void getLocation()
{
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) == NULL)
    {
        printf("Error getting current directory\n");
        return;
    }

    char hostname[1024];
    gethostname(hostname, sizeof(hostname));

    char *username = getenv("USER");

    //  Color Fix: Make the username cyan and the path yellow
    printf("\033[1;36m%s@%s:\033[1;33m%s\033[0m$ ",
           username, hostname, cwd);
    fflush(stdout);
}

void systemCall(char **arguments)
{
    if (arguments == NULL || arguments[0] == NULL)
    {
        printf("-myShell: No command entered.\n");
        return;
    }

    // Ensure arguments array is NULL-terminated
    int i = 0;
    while (arguments[i] != NULL)
    {
        i++;
    }
    arguments[i] = NULL; // Explicitly terminate with NULL

    pid_t pid = fork();
    if (pid == -1)
    {
        perror("-myShell: fork");
        return;
    }

    if (pid == 0)
    { // Child process
        execvp(arguments[0], arguments);
        perror("-myShell: command not found"); // Print error if execvp fails
        exit(EXIT_FAILURE);
    }
    else
    { // Parent process
        int status;
        waitpid(pid, &status, 0);
    }
}

void mypipe(char **args1, char **args2)
{
    int fd[2];
    if (pipe(fd) == -1)
    {
        perror("-myShell: pipe");
        return;
    }

    pid_t pid1 = fork();
    if (pid1 == -1)
    {
        perror("-myShell: fork");
        return;
    }

    if (pid1 == 0)
    {                               // First child process
        close(fd[0]);               // Close unused read end
        dup2(fd[1], STDOUT_FILENO); // Redirect stdout to pipe write end
        close(fd[1]);               // Close write end after dup2

        if (execvp(args1[0], args1) == -1)
        {
            perror("-myShell: execvp");
            exit(EXIT_FAILURE);
        }
    }

    pid_t pid2 = fork();
    if (pid2 == -1)
    {
        perror("-myShell: fork");
        return;
    }

    if (pid2 == 0)
    {                              // Second child process
        close(fd[1]);              // Close unused write end
        dup2(fd[0], STDIN_FILENO); // Redirect stdin to pipe read end
        close(fd[0]);              // Close read end after dup2

        if (execvp(args2[0], args2) == -1)
        {
            perror("-myShell: execvp");
            exit(EXIT_FAILURE);
        }
    }

    close(fd[0]);
    close(fd[1]);

    int status1, status2;
    waitpid(pid1, &status1, 0);
    waitpid(pid2, &status2, 0);

    if (WIFEXITED(status1) && WEXITSTATUS(status1) != 0)
    {
        printf("-myShell: First command in pipe failed\n");
    }
    if (WIFEXITED(status2) && WEXITSTATUS(status2) != 0)
    {
        printf("-myShell: Second command in pipe failed\n");
    }
    wait(NULL);
    dup2(STDOUT_FILENO, fileno(stdout)); //  Restore stdout
}

void move(char **args)
{
    if (args[1] == NULL || args[2] == NULL)
    {
        printf("-myShell: mv: missing file operand\n");
        return;
    }

    char source[1024] = {0};
    char destination[1024] = {0};

    // Extract and handle source path
    strcpy(source, args[1]);

    // Handle quotes in source filename
    if (source[0] == '"' && source[strlen(source) - 1] == '"')
    {
        source[strlen(source) - 1] = '\0';
        memmove(source, source + 1, strlen(source));
    }

    // Check if the destination is a directory
    struct stat destStat;
    int is_directory = (stat(args[2], &destStat) == 0 && S_ISDIR(destStat.st_mode));

    if (is_directory)
    {
        // Extract filename from source
        char *filename = strrchr(source, '/');
        if (filename)
        {
            filename++; // Skip the '/'
        }
        else
        {
            filename = source; // No '/' found, source is just the filename
        }

        // Construct full destination path (moving into directory)
        snprintf(destination, sizeof(destination), "%s/%s", args[2], filename);
    }
    else
    {
        // Normal file rename (no directory involved)
        strcpy(destination, args[2]);
    }

    // Handle quotes in destination filename
    if (destination[0] == '"' && destination[strlen(destination) - 1] == '"')
    {
        destination[strlen(destination) - 1] = '\0';
        memmove(destination, destination + 1, strlen(destination));
    }

    // Perform the move operation
    if (rename(source, destination) == 0)
    {
        if (is_directory)
        {
            printf("-myShell: mv: successfully moved '%s' to '%s'\n", source, destination);
        }
        else
        {
            printf("-myShell: mv: successfully renamed '%s' to '%s'\n", source, destination);
        }
    }
    else
    {
        perror("-myShell: mv");
    }

    return; // Ensure the function exits properly
}



void echoppend(char **args)
{
    if (args[0] == NULL || args[1] == NULL)
    {
        printf("-myShell: echoppend: missing file operand or text\n");
        return;
    }

    int fd = open(args[0], O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd == -1)
    {
        perror("-myShell: echoppend: error opening file");
        return;
    }

    write(fd, args[1], strlen(args[1]));
    write(fd, "\n", 1);

    close(fd);
}
void echowrite(char **args)
{
    if (args[0] == NULL || args[1] == NULL)
    {
        printf("-myShell: echowrite: missing file operand or text\n");
        return;
    }

    int fd = open(args[0], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1)
    {
        perror("-myShell: echowrite: error opening file");
        return;
    }

    // Ensure only the text is written once without extra spaces or quotes
    char *text = args[1];

    if (text[0] == '"' && text[strlen(text) - 1] == '"')
    {
        text[strlen(text) - 1] = '\0'; // Remove trailing quote
        text++;                        // Move past leading quote
    }

    dprintf(fd, "%s\n", text); // Print only once with a newline

    close(fd);
}


void _read(char **args) {
    if (args[0] == NULL) {
        printf("-myShell: read: missing file operand\n");
        return;
    }

    char filepath[1024] = {0};

    // If there's a second argument and it ends with '/', it's a directory
    if (args[1] != NULL && args[1][strlen(args[1]) - 1] == '/') {
        snprintf(filepath, sizeof(filepath), "%s%s", args[1], args[0]); 
    } else {
        strcpy(filepath, args[0]);
    }

    FILE *file = fopen(filepath, "r");
    if (file == NULL) {
        printf("-myShell: read: cannot open file '%s': No such file or directory\n", filepath);
        return;
    }

    char line[1024];
    while (fgets(line, sizeof(line), file)) {
        printf("%s", line);
    }

    fclose(file);
}

void wordCount(char **args)
{
    if (args[1] == NULL || args[2] == NULL)
    {
        printf("-myShell: wc: missing file operand or option\n");
        return;
    }

    char option[10];
    char filename[1024] = {0};

    // Extract and handle the option
    strcpy(option, args[1]);

    // Extract and handle filename (remove quotes)
    strcpy(filename, args[2]);
    if (filename[0] == '"' && filename[strlen(filename) - 1] == '"')
    {
        filename[strlen(filename) - 1] = '\0';
        memmove(filename, filename + 1, strlen(filename));
    }

    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("-myShell: wc: cannot open file '%s': No such file or directory\n", filename);
        return;
    }

    int bytes = 0, words = 0, lines = 0;
    char c, prev = '\0';

    while ((c = fgetc(file)) != EOF)
    {
        bytes++;
        if (c == '\n')
            lines++;
        if ((c == ' ' || c == '\n' || c == '\t') && (prev != ' ' && prev != '\n' && prev != '\t'))
        {
            words++;
        }
        prev = c;
    }
    fclose(file);

    if (strcmp(option, "-c") == 0)
    {
        printf("Bytes: %d\n", bytes);
    }
    else if (strcmp(option, "-l") == 0)
    {
        printf("Lines: %d\n", lines);
    }
    else if (strcmp(option, "-w") == 0)
    {
        printf("Words: %d\n", words);
    }
    else
    {
        printf("-myShell: wc: invalid option '%s'. Use '-c', '-l', or '-w'.\n", option);
    }
}

void cd(char **args)
{
    if (args[1] == NULL)
    {
        const char *home = getenv("HOME");
        if (home == NULL)
        {
            printf("-myShell: cd: HOME not set\n");
        }
        else
        {
            chdir(home);
        }
    }
    else
    {
        if (chdir(args[1]) != 0)
        {
            perror("-myShell: cd");
        }
    }
}

void cp(char **args) {
    if (args[1] == NULL || args[2] == NULL) {
        printf("-myShell: cp: missing file operand\n");
        return;
    }

    char source[1024] = {0};
    char destination[1024] = {0};

    // Extract the source path
    strcpy(source, args[1]);

    // Check if the destination is a directory
    struct stat destStat;
    if (stat(args[2], &destStat) == 0 && S_ISDIR(destStat.st_mode)) {
        // Extract filename from source
        char *filename = strrchr(source, '/');
        if (filename) {
            filename++; // Skip the '/'
        } else {
            filename = source; // No '/' found, source is just the filename
        }

        // Construct full destination path
        snprintf(destination, sizeof(destination), "%s/%s", args[2], filename);
    } else {
        // Normal file-to-file copy
        strcpy(destination, args[2]);
    }

    // Open source file
    FILE *sourceFile = fopen(source, "rb");
    if (sourceFile == NULL) {
        printf("-myShell: cp: cannot open source file '%s'\n", source);
        return;
    }

    // Open destination file
    FILE *destinationFile = fopen(destination, "wb");
    if (destinationFile == NULL) {
        printf("-myShell: cp: cannot open destination file '%s'\n", destination);
        fclose(sourceFile);
        return;
    }

    // Copy file contents
    char buffer[1024];
    size_t bytesRead;
    while ((bytesRead = fread(buffer, 1, sizeof(buffer), sourceFile)) > 0) {
        fwrite(buffer, 1, bytesRead, destinationFile);
    }

    fclose(sourceFile);
    fclose(destinationFile);

    printf("-myShell: cp: successfully copied '%s' to '%s'\n", source, destination);
}

void get_dir()
{
    DIR *d;
    struct dirent *dir;
    d = opendir(".");
    if (d)
    {
        while ((dir = readdir(d)) != NULL)
        {
            printf("%s  ", dir->d_name);
        }
        printf("\n");
        closedir(d);
    }
    else
    {
        perror("-myShell: dir");
    }
}
void makeDir(char **args) {
    if (args[1] == NULL) {
        printf("-myShell: mkdir: missing directory operand\n");
        return;
    }

    if (mkdir(args[1], 0777) == -1) {
        perror("-myShell: mkdir");
    } else {
        printf("-myShell: mkdir: created directory '%s'\n", args[1]);
    }
}
void delete(char **args) {
    if (args[1] == NULL) {
        printf("-myShell: delete: missing file operand\n");
        return;
    }

    for (int i = 1; args[i] != NULL; i++) {
        struct stat path_stat;
        stat(args[i], &path_stat);

        // Prevent deletion of directories
        if (S_ISDIR(path_stat.st_mode)) {
            printf("-myShell: delete: cannot remove '%s': Is a directory\n", args[i]);
            continue;
        }

        // Attempt to delete the file
        if (remove(args[i]) == 0) {
            printf("Deleted successfully: %s\n", args[i]);
        } else {
            perror("-myShell: delete");
        }
    }

    return; // Explicitly ensure function exits cleanly
}

void logout(char *input)
{
    char **args = splitArguments(input); //  Use splitArguments() for proper parsing

    if (args[0] && strcmp(args[0], "exit") == 0)
    {
        if (args[1] == NULL)
        { //  No extra words after "exit", so exit normally
            printf("\033[1;31mExiting MyShell... Goodbye!\033[0m\n");
            fflush(stdout);

            //  Free allocated memory before exiting
            for (int i = 0; args[i] != NULL; i++)
            {
                free(args[i]);
            }
            free(args);

            exit(0);
        }
        else
        {
            printf("\033[1;33m[ERROR] Invalid usage: Use 'exit' without extra arguments.\033[0m\n");
            fflush(stdout);
        }
    }

    //  Free memory allocated by splitArguments()
    for (int i = 0; args[i] != NULL; i++)
    {
        free(args[i]);
    }
    free(args);
}