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

void getLocation() 
{
    char cwd[PATH_MAX];  
    char hostname[HOST_NAME_MAX];  
    struct passwd *pw;  
    char *username;

    // Get the current working directory
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        perror("getcwd error");
        return;
    }

    // Get the hostname
    if (gethostname(hostname, sizeof(hostname)) != 0) {
        perror("gethostname error");
        return;
    }

    // Get the username
    pw = getpwuid(getuid());
    if (pw) {
        username = pw->pw_name;
    } else {
        username = "unknown";
    }

    // Print the formatted prompt, mimicking the original terminal style
    printf("\033[1;32m%s\033[0m@\033[1;36m%s\033[0m:\033[34m%s\033[0m$ ", username, hostname, cwd);
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
void cd(char **arguments)
{

    if (strncmp(arguments[1], "\"", 1) != 0 && arguments[2] != NULL)
        printf("-myShell: cd: too many arguments\n");

    else if (chdir(arguments[1]) != 0)
        printf("-myShell: cd: %s: No such file or directory\n", arguments[1]);
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
