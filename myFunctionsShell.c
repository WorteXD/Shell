#include "myFunctionsShell.h"
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <limits.h>
#include <pwd.h>        // Required for getpwuid()
#include <sys/types.h>  // Required for user ID functions
#include <sys/stat.h>


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

char **splitArguments(char *str)
{
    if (str == NULL) return NULL;

    int size = 10;  // Initial size of the array
    int index = 0;
    char **args = malloc(size * sizeof(char *));
    if (!args) {
        perror("malloc failed");
        return NULL;
    }

    char *token = strtok(str, " ");
    while (token) {
        if (index >= size - 1) {
            size *= 2;  // Double the size if needed
            args = realloc(args, size * sizeof(char *));
            if (!args) {
                perror("realloc failed");
                return NULL;
            }
        }

        args[index] = strdup(token); // Copy token into allocated memory
        index++;
        token = strtok(NULL, " ");
    }
    args[index] = NULL; // Null-terminate the array

    return args;
}

void getLocation() {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        printf("Error getting current directory\n");
        return;
    }

    char hostname[1024];
    gethostname(hostname, sizeof(hostname));

    char *username = getenv("USER");

    // ✅ Color Fix: Make the username cyan and the path yellow
    printf("\033[1;36m%s@%s:\033[1;33m%s\033[0m$ ", 
           username, hostname, cwd);
    fflush(stdout);
}




void systemCall(char **arguments) {
    if (arguments == NULL || arguments[0] == NULL) {
        return;
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("-myShell: fork");
        return;
    }

    if (pid == 0) {
        execvp(arguments[0], arguments);
        perror("-myShell: execvp");
        exit(EXIT_FAILURE);
    } else {
        wait(NULL);
    }
}


void mypipe(char **args1, char **args2) {
    int fd[2];
    if (pipe(fd) == -1) {
        perror("-myShell: pipe");
        return;
    }

    pid_t pid1 = fork();
    if (pid1 == -1) {
        perror("-myShell: fork");
        return;
    }

    if (pid1 == 0) {  // First child process
        close(fd[0]);  // Close unused read end
        dup2(fd[1], STDOUT_FILENO);  // Redirect stdout to pipe write end
        close(fd[1]);  // Close write end after dup2

        if (execvp(args1[0], args1) == -1) {
            perror("-myShell: execvp");
            exit(EXIT_FAILURE);
        }
    }

    pid_t pid2 = fork();
    if (pid2 == -1) {
        perror("-myShell: fork");
        return;
    }

    if (pid2 == 0) {  // Second child process
        close(fd[1]);  // Close unused write end
        dup2(fd[0], STDIN_FILENO);  // Redirect stdin to pipe read end
        close(fd[0]);  // Close read end after dup2

        if (execvp(args2[0], args2) == -1) {
            perror("-myShell: execvp");
            exit(EXIT_FAILURE);
        }
    }

    close(fd[0]);
    close(fd[1]);
    
    wait(NULL);
    dup2(STDOUT_FILENO, fileno(stdout)); // ✅ Restore stdout

}

void move(char **args) {
    if (args[1] == NULL || args[2] == NULL) {
        printf("-myShell: mv: missing file operand\n");
        return;
    }

    char source[256] = "";
    char destination[256] = "";

    // Merge arguments if the filename has spaces
    int i = 1;
    while (args[i + 1] != NULL) {
        strcat(source, args[i]);
        if (args[i + 2] != NULL) strcat(source, " ");
        i++;
    }
    strcpy(destination, args[i]);

    // Perform rename
    if (rename(source, destination) != 0) {
        perror("-myShell: mv");
    }
}

void echoppend(char **args) {
    if (args[1] == NULL || args[2] == NULL) {
        perror("-myShell: echoppend: missing arguments");
        return;
    }

    int fd = open(args[1], O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd == -1) {
        perror("-myShell: echoppend: error opening file");
        return;
    }

    for (int i = 2; args[i] != NULL; i++) {
        write(fd, args[i], strlen(args[i]));
        if (args[i + 1] != NULL) {
            write(fd, " ", 1);  // Add space between words
        }
    }
    write(fd, "\n", 1); // Append a newline at the end
    close(fd);
}
void echowrite(char **args) {}
void _read(char **args) {}
void wordCount(char **args) {}

void echo(char **arguments)
{
    while (*(++arguments))
        printf("%s ", *arguments);

    puts("");
}
void cd(char **arguments) {
    if (arguments[1] == NULL) {
        char *home = getenv("HOME");
        if (home == NULL) {
            printf("-myShell: cd: HOME not set\n");
            return;
        }
        if (chdir(home) != 0) {
            printf("-myShell: cd: %s: %s\n", home, strerror(errno));
        }
        return;
    }

    //  Join arguments into a valid path using `/`
    char path[1024] = "";
    for (int i = 1; arguments[i] != NULL; i++) {
        strcat(path, arguments[i]);
        if (arguments[i + 1] != NULL) {
            strcat(path, "/");  //  Join folders with `/`
        }
    }

        //  Try to change directory
    if (chdir(path) != 0) {
        printf("-myShell: cd: %s: %s\n", path, strerror(errno));    
    }
}
void cp(char **args) {
    if (args[1] == NULL || args[2] == NULL) {
        printf("-myShell: cp: missing file operand\n");
        return;
    }

    FILE *source = fopen(args[1], "rb");
    if (source == NULL) {
        printf("-myShell: cp: cannot open source file '%s'\n", args[1]);
        return;
    }

    FILE *destination = fopen(args[2], "wb");
    if (destination == NULL) {
        printf("-myShell: cp: cannot open destination file '%s'\n", args[2]);
        fclose(source);
        return;
    }

    char buffer[1024];
    size_t bytesRead;
    while ((bytesRead = fread(buffer, 1, sizeof(buffer), source)) > 0) {
        fwrite(buffer, 1, bytesRead, destination);
    }

    fclose(source);
    fclose(destination);

    printf("-myShell: cp: successfully copied '%s' to '%s'\n", args[1], args[2]);
}
void get_dir() 
{
    DIR *d;
    struct dirent *dir;
    d = opendir(".");
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            printf("%s  ", dir->d_name);
        }
        printf("\n");
        closedir(d);
    } else {
        perror("-myShell: dir");
    }
}
void delete(char **args) {
    if (args[1] == NULL) 
    {
        printf("-myShell: delete: missing file operand\n");
        return;
    }
    
    for (int i = 2; args[i] != NULL; i++) {
        struct stat path_stat;
        stat(args[i], &path_stat);
        if (S_ISDIR(path_stat.st_mode)) {
            printf("-myShell: delete: cannot remove '%s': Is a directory\n", args[i]);
            continue;
        }
    
        if (remove(args[i]) == 0) {
            printf("Deleted successfully: %s\n", args[i]);
        } else {
            perror("-myShell: delete");
        }
    }
    
    struct stat path_stat;
    stat(args[1], &path_stat);
    if (S_ISDIR(path_stat.st_mode)) 
    {
        printf("-myShell: delete: cannot remove '%s': Is a directory\n", args[1]);
        return;
    }
    
    if (remove(args[1]) == 0) {
        printf("Deleted successfully: %s\n", args[1]);
    } else {
        perror("-myShell: delete");
    }
    
}
void logout(char *input) {
    char **args = splitArguments(input);  //  Use splitArguments() for proper parsing

    if (args[0] && strcmp(args[0], "exit") == 0) {
        if (args[1] == NULL) {  //  No extra words after "exit", so exit normally
            printf("\033[1;31mExiting MyShell... Goodbye!\033[0m\n");
            fflush(stdout);

            //  Free allocated memory before exiting
            for (int i = 0; args[i] != NULL; i++) {
                free(args[i]);
            }
            free(args);

            exit(0);
        } else {
            printf("\033[1;33m[ERROR] Invalid usage: Use 'exit' without extra arguments.\033[0m\n");
            fflush(stdout);
        }
    }

    //  Free memory allocated by splitArguments()
    for (int i = 0; args[i] != NULL; i++) {
        free(args[i]);
    }
    free(args);
}