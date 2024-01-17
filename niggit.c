#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>

//Function Prototypes
void CommandFinder(char **argv);
int main(int argc, char **argv)
{
    CommandFinder(argv);
}

//Functions
int SearchInArgv(char** argv , char* word)
{
    for (int i = 0; i < 10000; i++)
    {
        if (!strcmp(argv[i] , word))
        {
            return 1;
        }
        if (argv[i] == NULL)
        {
            break;
        }
    }
    return 0;
}
void ConfigUserName(int isGlobal , char* userName)
{

}
void ConfigUserEmail(int isGlobal , char* userEmail)
{

}
void Init()
{
    if (opendir(".niggit") != NULL)
    {
        system("rm -r .niggit");

        system("mkdir .niggit");
        system("mkdir .niggit/stage-files-current");
        system("mkdir .niggit/stage-files-latest");
        system("mkdir .niggit/commited-files");
        fopen(".niggit/settings.txt" , "w");
        printf("niggit reinitialized! :)\n");
    }
    else
    {
        system("mkdir .niggit");
        system("mkdir .niggit/stage-files-current");
        system("mkdir .niggit/stage-files-latest");
        system("mkdir .niggit/commited-files");
        fopen(".niggit/settings.txt" , "w");
        printf("niggit initialized! :)\n");
    }  
}
void Add(char **argv)
{
    int doesHaveStar = 0;
    for (size_t i = 0; i < strlen(argv[2]); i++)
    {
        if (argv[2][i] == '*')
        {
            doesHaveStar = 1;
            break;
        }
    }
    
    //Normal add
    if ((argv[3] == NULL) && !doesHaveStar)
    {
        int dotCount = 0;
        for (size_t i = 0; i < strlen(argv[2]); i++)
        {
            if (argv[2][i] == '.')
            {
                dotCount++;
            }
        }
        if (dotCount == 1)
        {
            FILE* temp = fopen(argv[2] , "r");
            if (temp == NULL)
            {
                printf("BRUH the file doesn't exists :/\n");
                return;
            }
            fclose(temp);

            char command[10000] = "cp ";
            strcat(command , "\"");
            strcat(command , argv[2]);
            strcat(command , "\"");
            strcat(command , " ");
            strcat(command , ".niggit/stage-files-current");

            system(command);
            printf("File Staged :))\n");
        }
        else
        {
            if (opendir(argv[2]) == NULL)
            {
                printf("BRUH the folder doesn't exists :/\n");
                return;
            }
            
            char command[10000] = "cp -r ";
            strcat(command , "\"");
            strcat(command , argv[2]);
            strcat(command , "\"");
            strcat(command , " ");
            strcat(command , ".niggit/stage-files-current");

            system(command);
            printf("File Staged :))\n");
        }
    }
    //wildcard add
    else if ((argv[3] == NULL) && !doesHaveStar)
    {
        
    }
    //multiple file add -f
    else if (!strcmp(argv[2] , "-f"))
    {
        int ind = 3;
        while (argv[ind] != NULL)
        {
            int dotCount = 0;
            for (size_t i = 0; i < strlen(argv[ind]); i++)
            {
                if (argv[ind][i] == '.')
                {
                    dotCount++;
                }
            }
            if (dotCount == 1)
            {
                FILE* temp = fopen(argv[ind] , "r");
                if (temp == NULL)
                {
                    printf("BRUH the file doesn't exists :/\n");
                    return;
                }
                fclose(temp);

                char command[10000] = "cp ";
                strcat(command , "\"");
                strcat(command , argv[ind]);
                strcat(command , "\"");
                strcat(command , " ");
                strcat(command , ".niggit/stage-files-current");

                system(command);
                printf("File Staged :))\n");
            }
            else
            {
                if (opendir(argv[ind]) == NULL)
                {
                    printf("BRUH the folder doesn't exists :/\n");
                    return;
                }

                char command[10000] = "cp -r ";
                strcat(command , "\"");
                strcat(command , argv[ind]);
                strcat(command , "\"");
                strcat(command , " ");
                strcat(command , ".niggit/stage-files-current");

                system(command);
                printf("File Staged :))\n");
            }
            ind++;
        }
    }
}
void CommandFinder(char **argv)
{
    if (!strcmp(argv[1] , "config"))
    {
        if (SearchInArgv(argv , "user.name"))
        {
            if (SearchInArgv(argv , "-global"))
            {
                ConfigUserName(1 , argv[4]);
            }
            else
            {
                ConfigUserName(0 , argv[3]);
            }    
        }
        if (SearchInArgv(argv , "user.email"))
        {
            if (SearchInArgv(argv , "-global"))
            {
                ConfigUserEmail(1 , argv[4]);
            }
            else
            {
                ConfigUserEmail(0 , argv[3]);
            }    
        }
    }
    if (!strcmp(argv[1] , "init"))
    {
        Init();
    }
    if (!strcmp(argv[1] , "add"))
    {
        Add(argv);
    }
    
    
}