#include "myShell.h"
#include "myFunctionsShell.h"

int main(int argc, char const *argv[])
{
    

    welcome();
    while (1)
    {
        int isPipe = 2;
        int isEchoWrith = 0;
        int isEchoPpend = 0;

        getLocation();

        char *input = getInputFromUser();
        puts(input);
        
        if (strncmp(input, "exit",4) == 0)
        {
            puts("Exit");
            logout(input);
        }

        char **arguments = splitArguments(input);
        
        if (strcmp(input, "cd") == 0)
        {
            cd(arguments);
        }
        else if (strncmp(input, "echo",4) == 0)
        {
            if (isEchoWrith)
                echowrite(arguments);
            else if (isEchoPpend)
                echoppend(arguments);
            else
                echo(arguments);
        }
        else if (strcmp(input, "cp") == 0)
        {
            cp(arguments);
        }
        else if (strcmp(input, "delete") == 0)
        {
            delete(arguments);
        }
        else if (strcmp(input, "dir") == 0)
        {
            get_dir();
        }
        else if (isPipe)
        {
            mypipe(arguments, arguments + isPipe + 1);
        }
        else
        {
            systemCall(arguments);
            wait(NULL);
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