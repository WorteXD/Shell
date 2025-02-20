#include "myFunctionsShell.h"
#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <pwd.h>        // Required for getpwuid()
#include <sys/types.h>  // Required for user ID functions
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
    char hostname[HOST_NAME_MAX];
    char cwd[PATH_MAX];
    struct passwd *pw;
    char *username;

    // Get username
    pw = getpwuid(getuid());
    username = pw ? pw->pw_name : "unknown";

    // Get hostname
    gethostname(hostname, sizeof(hostname));

    // Get current working directory
    getcwd(cwd, sizeof(cwd));

    // Print the location in terminal
    printf("\033[1;33m(MyShell) \033[1;32m%s@\033[1;36m%s\033[1;37m:%s$\033[0m ", username, hostname, cwd);
}

void logout(char *input) {
    char **args = splitArguments(input);  // ✅ Use splitArguments() for proper parsing

    if (args[0] && strcmp(args[0], "exit") == 0) {
        if (args[1] == NULL) {  // ✅ No extra words after "exit", so exit normally
            printf("\033[1;31mExiting MyShell... Goodbye!\033[0m\n");
            fflush(stdout);

            // ✅ Free allocated memory before exiting
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

    // ✅ Free memory allocated by splitArguments()
    for (int i = 0; args[i] != NULL; i++) {
        free(args[i]);
    }
    free(args);
}


void systemCall(char **arguments)
{
    puts("systemCall");
    pid_t pid = fork();
    if (pid == -1)
    {
        perror("fork err");
        return;
    }

    if (pid == 0)
    {

        if (execvp(arguments[0], arguments) == -1)
        {
            exit(EXIT_FAILURE);
        }
    }
}

void mypipe(char **argv1, char **argv2)
{

    int fd[2];

    if (fork() == 0)
    {
        pipe(fd);
        if (fork() == 0)
        {

            close(STDOUT_FILENO);
            dup2(fd[1], STDOUT_FILENO);
            close(fd[1]);
            close(fd[0]);
            execvp(argv1[0], argv1);
        }

        close(STDIN_FILENO);
        dup(fd[0]);
        close(fd[1]);
        close(fd[0]);
        execvp(argv2[0], argv2);
    }
}

void move(char **args) {}
void echoppend(char **args) {}
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
        // No arguments: Change to home directory
        char *home = getenv("HOME");
        if (home == NULL) {
            printf("-myShell: cd: HOME not set\n");
        } else {
            chdir(home);
        }
        return;
    }

    // Concatenate all arguments into a single path (fixes "too many arguments")
    char path[PATH_MAX] = "";
    for (int i = 1; arguments[i] != NULL; i++) {
        strcat(path, arguments[i]);
        if (arguments[i + 1] != NULL) {
            strcat(path, " ");  // Preserve spaces in paths
        }
    }

    // Remove surrounding quotes if present
    size_t len = strlen(path);
    if (len > 1 && path[0] == '"' && path[len - 1] == '"') {
        path[len - 1] = '\0';
        memmove(path, path + 1, len - 1);
    }

    // Change directory
    if (chdir(path) != 0) {
        printf("-myShell: cd: %s: No such file or directory\n", path);
    }
}

void cp(char **arguments)
{
    char ch;
    FILE *src, *des;
    if ((src = fopen(arguments[1], "r")) == NULL)
    {
        puts("error");
        return;
    }

    if ((des = fopen(arguments[2], "w")) == NULL)
    {
        puts("error");
        fclose(src);
        return;
    }
    while ((ch = fgetc(src)) != EOF)
        fputc(ch, des);

    fclose(src);
    fclose(des);
}
void get_dir()
{
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir("./")) == NULL)
    {
        perror("");
        return;
    }
    while ((ent = readdir(dir)) != NULL)
        printf("%s ", ent->d_name);
    puts("");
}
void delete(char **arguments)
{
    if (unlink(arguments[1]) != 0)
        printf("-myShell: delete: %s: No such file or directory\n", arguments[1]);
}
