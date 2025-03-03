#include "myShell.h"
#include "myFunctionsShell.h"

int main(int argc, char const *argv[])
{
    welcome();

    char *input;
    char **arguments = NULL; //  Ensure arguments is declared before use

    while (1)
    {
        int isPipe = 2;
        int isEchoWrith = 0;
        int isEchoPpend = 0;
        ;

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

        else if (strcmp(arguments[0], "echowrite") == 0)
        {
            echowrite(arguments);
        }
        else if (strcmp(arguments[0], "echoppend") == 0)
        {
            echoppend(arguments);
        }
        else if (strcmp(arguments[0], "echo") == 0)
        {
            echo(arguments);
        }

        else if (strcmp(arguments[0], "mv") == 0)
        {
            move(arguments);
        }
        else if (strncmp(arguments[0], "cp", 2) == 0)
        {
            cp(arguments);
        }
        else if (strcmp(arguments[0], "delete") == 0)
        {
            delete (arguments);
        }
        else if (strcmp(arguments[0], "read") == 0) {
            _read(arguments);
        }
        else if (strcmp(arguments[0], "wc") == 0) {
            wordCount(arguments);
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

        //  Fix: Free memory properly after execution
        if (arguments)
        {
            for (int i = 0; arguments[i] != NULL; i++)
            {
                free(arguments[i]);
            }
            free(arguments);
        }

        free(input); //  Free input only once per loop
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