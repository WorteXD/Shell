#include "myShell.h"
#include "myFunctionsShell.h"

int main(int argc, char const *argv[])
{
    welcome();

    while (1)
    {

        getLocation();

        char *input = getInputFromUser();
        if (input == NULL)
        {
            break;
        }

        char **arguments = splitArguments(input);
        if (arguments == NULL || arguments[0] == NULL)
        {
            free(input);
            continue;
        }

        //  Fix: Ensure "cd" command works correctly
        if (strncmp(arguments[0], "cd", 2) == 0)
        {
            cd(arguments);
            free(input);
            free(arguments);
            continue; //  Ensures the loop continues running after cd()
        }

        else if (strcmp(arguments[0], "echo") == 0)
        {
            int redirect_pos = -1;
            int append = 0;

            for (int i = 1; arguments[i] != NULL; i++)
            {
                if (strcmp(arguments[i], ">") == 0)
                {
                    redirect_pos = i;
                    append = 0;
                    break;
                }
                else if (strcmp(arguments[i], ">>") == 0)
                {
                    redirect_pos = i;
                    append = 1;
                    break;
                }
            }

            if (redirect_pos != -1)
            {
                if (arguments[redirect_pos + 1] == NULL)
                {
                    printf("-myShell: echo: missing file operand\n");
                }
                else
                {
                    char filename[1024] = {0};
                    char content[1024] = {0};

                    // Extract filename correctly, removing quotes
                    int j = redirect_pos + 1;
                    while (arguments[j] != NULL)
                    {
                        strcat(filename, arguments[j]);
                        if (arguments[j + 1] != NULL)
                            strcat(filename, " ");
                        j++;
                    }

                    // ✅ Remove quotes from filename
                    if (filename[0] == '"' && filename[strlen(filename) - 1] == '"')
                    {
                        filename[strlen(filename) - 1] = '\0';             // Remove trailing quote
                        memmove(filename, filename + 1, strlen(filename)); // Shift left to remove first quote
                    }

                    // Extract text before '>' or '>>'
                    for (int i = 1; i < redirect_pos; i++)
                    {
                        strcat(content, arguments[i]);
                        strcat(content, " ");
                    }
                    content[strlen(content) - 1] = '\0'; // Remove last extra space

                    char *args[] = {filename, content, NULL};
                    if (append)
                    {
                        echoppend(args);
                    }
                    else
                    {
                        echowrite(args);
                    }
                }
            }

            else if (redirect_pos == -1) // ✅ Prevents unnecessary terminal printing
            {
                for (int i = 1; arguments[i] != NULL; i++)
                {
                    printf("%s ", arguments[i]);
                }
                printf("\n");
            }
        }

        else if (strcmp(arguments[0], "mv") == 0)
        {
            move(arguments);
            free(input);
            free(arguments);
            continue;
        }
        else if (strcmp(arguments[0], "cp") == 0)
        {
            cp(arguments);
            free(input);
            free(arguments);
            continue;
        }

        else if (strcmp(arguments[0], "delete") == 0)
        {
            delete (arguments);
            free(input);
            free(arguments);
            continue;
        }
        else if (strcmp(arguments[0], "read") == 0)
        {
            if (arguments[1] == NULL)
            {
                printf("-myShell: read: missing file operand\n");
            }
            else
            {
                char filename[1024] = {0};
                char directory[1024] = {0};

                // If there is a second argument and it ends with '/', treat it as a directory
                if (arguments[2] != NULL && arguments[2][strlen(arguments[2]) - 1] == '/')
                {
                    strcpy(directory, arguments[2]);
                }

                strcpy(filename, arguments[1]);

                // Remove quotes if present
                if (filename[0] == '"' && filename[strlen(filename) - 1] == '"')
                {
                    filename[strlen(filename) - 1] = '\0';
                    memmove(filename, filename + 1, strlen(filename));
                }

                char *args[] = {filename, directory[0] ? directory : NULL, NULL};
                _read(args);
            }
            continue;
        }

        else if (strcmp(arguments[0], "wc") == 0)
{
    if (arguments[1] == NULL || arguments[2] == NULL)
    {
        printf("-myShell: wc: missing file operand or option\n");
    }
    else
    {
        wordCount(arguments);
    }
    free(input);
    free(arguments);
    continue;
}


        else if (strcmp(arguments[0], "dir") == 0)
        {
            get_dir();
        }

        int pipe_pos = -1; // Use an integer instead of a pointer
        for (int i = 0; arguments[i] != NULL; i++)
        {
            if (strcmp(arguments[i], "|") == 0)
            {
                pipe_pos = i;
                arguments[i] = NULL;
                break;
            }
        }

        if (pipe_pos != -1)
        {
            mypipe(arguments, &arguments[pipe_pos + 1]);
        }

        if (strncmp(arguments[0], "exit", 4) == 0)
        {
            logout(input); // logout() already calls exit(0), so we must stop execution.
            free(arguments);
            free(input);
            return 0; // Prevents further execution.
        }
        if (arguments[0] != NULL)
        {
            systemCall(arguments);
        }

        // Now free memory
        if (arguments)
        {
            for (int i = 0; arguments[i] != NULL; i++)
            {
                free(arguments[i]);
            }
            free(arguments);
        }
        free(input);
    }

    return 0;
}

void welcome()
{
    printf("\033[1;30m================================================================================\033[0m\n");

    // Top section (blue)
    printf("\033[1;34m                         __    _                                   \n");
    printf("                    _wr\"\"        \"-q__                             \n");
    printf("                 _dP                 9m_                          \n");
    printf("               _#P                     9#_                        \n");
    printf("              d#@                       9#m                       \n");
    printf("             d##                         ###                      \n");
    printf("            J###                         ###L                     \n");
    printf("            {###K                       J###K                     \n");

    // Middle section (white)
    printf("\033[1;37m            ]####K      ___aaa___      J####F                     \n");
    printf("        __gmM######_  w#P\"\"   \"\"9#m  _d#####Mmw__                \n");
    printf("     _g##############mZ_         __g##############m_             \n");
    printf("   _d####M@PPPP@@M#######Mmp gm#########@@PPP9@M####m_           \n");
    printf("  a###\"\"          ,Z\"#####@\" '######\"\\g          \"\"M##m        \n");
    printf(" J#@\"             0L  \"*##     ##@\"  J#              *#K        \n");
    printf(" #\"               `#    \"_gmwgm_~    dF               `#_       \n");

    // Bottom section (blue)
    printf("\033[1;34m7F                 \"#_   ]#####F   _dK                 JE       \n");
    printf("]                    *m__ ##### __g@\"                   F       \n");
    printf("                       \"PJ#####LP\"                              \n");
    printf(" `                       0######_                      '        \n");
    printf("                       _0########_                              \n");
    printf("     .               _d#####^#####m__              ,           \n");
    printf("      \"*w_________am#####P\"   ~9#####mw_________w*\"             \n");
    printf("          \"\"9@#####@M\"\"           \"\"P@#####@M\"\"                \n");

    // Text box
    printf("\033[1;33m  ╔══════════════════════════════════╗\033[0m\n");
    printf("\033[1;33m  ║\033[1;32m Made by: Nikita                  \033[1;33m║\033[0m\n");
    printf("\033[1;33m  ║\033[1;36m Welcome to My Shell              \033[1;33m║\033[0m\n");
    printf("\033[1;33m  ║\033[1;35m Ariel University                 \033[1;33m║\033[0m\n");
    printf("\033[1;33m  ║\033[1;31m GitHub Repo:                     \033[1;33m║\033[0m\n");
    printf("\033[1;33m  ║\033[1;34m https://github.com/WorteXD/Shell \033[1;33m║\033[0m\n");
    printf("\033[1;33m  ╚══════════════════════════════════╝\033[0m\n");

    printf("\033[1;30m================================================================================\033[0m\n");
}