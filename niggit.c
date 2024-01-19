#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>

#define branchesAddress ".niggit/branches"
#define masterAddress ".niggit/branches/master"

#define RED "\033[0;31m"
#define GREEN "\033[0;32m"
#define RESET "\033[0m"

//Function Prototypes
void CommandFinder(char **argv);
int main(int argc, char **argv)
{
    CommandFinder(argv);
}

//Functions
char* GetHead()
{
    char currentDir[1000] = "";
    FILE *fp = fopen(".niggit/head.txt" , "r");
    fgets(currentDir , sizeof(currentDir) , fp);
    char *currentDirTokenized = (char*)(malloc(1000));
    sscanf(currentDir , "HEAD=%[^\n]%*c" , currentDirTokenized);
    return currentDirTokenized;
}
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
int Search(char* argv , char* word)
{
    if (!strcmp(argv , word))
    {
        return 1;
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
        system("mkdir .niggit/branches");
        system("mkdir .niggit/branches/master");
        system("mkdir .niggit/branches/master/.stages");
        system("mkdir .niggit/branches/master/.stages/stages-current");
        system("mkdir .niggit/branches/master/.stages/stages-latest");
        system("mkdir .niggit/branches/master/.commits");
        
        FILE *fp = fopen(".niggit/head.txt" , "w");
        fprintf(fp , "HEAD=.niggit/branches/master\n");
        fclose(fp);

        FILE *fp2 = fopen(".niggit/branches.txt" , "w");
        fprintf(fp2 , "master-.niggit/branches/master\n");
        fclose(fp2);

        printf("niggit reinitialized! :)\n");
    }
    else
    {
        system("mkdir .niggit");
        system("mkdir .niggit/branches");
        system("mkdir .niggit/branches/master");
        system("mkdir .niggit/branches/master/.stages");
        system("mkdir .niggit/branches/master/.stages/stages-current");
        system("mkdir .niggit/branches/master/.stages/stages-latest");
        FILE *fp = fopen(".niggit/head.txt" , "w");
        //fopen(".niggit/latestStages.txt" , "w");
        printf("niggit reinitialized! :)\n");

        fprintf(fp , "HEAD=.niggit/branches/master\n");
        fclose(fp);
    }  
}
void Add(char **argv)
{
    //printf("%s\n" , argv[2]);
    int doesHaveStar = 0;
    for (size_t i = 0; i < strlen(argv[2]); i++)
    {
        if (argv[2][i] == '*')
        {
            doesHaveStar = 1;
            break;
        }
    }

    char currentStageRepo[1000] = "";
    strcat(currentStageRepo , GetHead());
    strcat(currentStageRepo , "/.stages/stages-current");
    
    //multiple file add -f
    if(!strcmp(argv[2] , "-f"))
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
                strcat(command , currentStageRepo);

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
                strcat(command , currentStageRepo);
                system(command);
                printf("File Staged :))\n");
            }
            ind++;
        }
    }
    //depth
    if (!strcmp(argv[2], "-n")) 
    {
        FILE *fp, *fpStage;
        char path[10000], pathStage[10000];

        char findCommand[1000] = "find -maxdepth ";
        strcat(findCommand , argv[3]);
        fp = popen(findCommand, "r");

        int isFirstTimeInLoop = 0;
        while (fgets(path, sizeof(path), fp) != NULL) 
        {
            if (isFirstTimeInLoop == 0)
            {
                isFirstTimeInLoop = 1;
                continue;
            }
            
            path[strcspn(path, "\n")] = '\0';

            int isStaged = 0;
            char findCommandStage[1000] = "find";
            strcat(findCommandStage , " ");
            strcat(findCommandStage , currentStageRepo);
            fpStage = popen(findCommandStage, "r");

            while (fgets(pathStage, sizeof(pathStage), fpStage) != NULL) 
            {
                pathStage[strcspn(pathStage, "\n")] = '\0';
                //printf("--%s\n" , path );

                if (strstr(pathStage , path + 1) != NULL) 
                {
                    isStaged = 1;
                    break;
                }
            }

            if (strstr(path , ".niggit") == NULL && path + 1 != NULL)
            {
                printf("%s", path + 1);
                if (isStaged) 
                {
                    printf(GREEN"(This is staged!!!!)\n"RESET);
                }
                else
                {
                    printf(RED"(This is not staged :/ )\n"RESET);
                }
            }
            pclose(fpStage);
        }

        pclose(fp);
    }
    //Normal add
    else if ((argv[3] == NULL) && !doesHaveStar)
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
            strcat(command , currentStageRepo);

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
            strcat(command , currentStageRepo);

            system(command);
            printf("File Staged :))\n");
        }
    }
    //wildcard add
    if ((argv[3] == NULL) && doesHaveStar) 
    {
        char argvConverted[1000] = "";
        strcat(argvConverted, argv[2]);

        FILE *fp1 = NULL, *fp2 = NULL;
        char path[1000], path2[1000];

        char commandWildcard[1000] = "find . -type d -name '";
        strcat(commandWildcard, argvConverted);
        strcat(commandWildcard, "' 2> .niggit/error.log");
        //printf("%s\n", commandWildcard);
        fp1 = popen(commandWildcard, "r");

        char commandWildcard2[1000] = "find . -name '";
        strcat(commandWildcard2, argvConverted);
        strcat(commandWildcard2, "*.txt' 2> .niggit/error.log");
        fp2 = popen(commandWildcard2, "r");

        if ((fp1 == NULL) && (fp2 == NULL)) {
            printf("Awwwww we didn't find any directory with that wildcard:(((\n");
            return;
        }
        
        int isSomethingStaged = 0;

        while (fgets(path, sizeof(path) - 1, fp1) != NULL) 
        {
            isSomethingStaged = 1;
            path[strcspn(path, "\n")] = 0;
            //printf("%s\n", path);
            char command[10000] = "cp -r ";
            strcat(command, "\"");
            strcat(command, path);
            strcat(command, "\" ");
            strcat(command, currentStageRepo);
            system(command);
        }

        while (fgets(path2, sizeof(path2) - 1, fp2) != NULL) 
        {
            isSomethingStaged = 1;
            path2[strcspn(path2, "\n")] = 0;
            //printf("%s\n", path2);
            char command[10000] = "cp ";
            strcat(command, "\"");
            strcat(command, path2);
            strcat(command, "\" ");
            strcat(command, currentStageRepo);
            system(command);
        }
    
        if (!isSomethingStaged) 
        {
            printf("Awwwww we didn't find any directory with that wildcard:(((\n");
            return;
        }

        printf("Files with the given wildcard Staged :))\n");

        pclose(fp1);
        pclose(fp2);
    }
}
void Reset(char **argv)
{
    if (!strcmp(argv[2] , "-undo"))
    {
        
    }
    else
    {

    }
    
    
}
void Branch(char **argv)
{
    if (argv[2] == NULL)
    {
        FILE *fp2 = fopen(".niggit/branches.txt" , "r");
        char line[1000];
        while (fgets(line , sizeof(line) , fp2) != NULL)
        {
            char temp[1000] , temp2[1000];
            sscanf(line , "%[^-]%*c%[^\n]%*c" , temp , temp2);
            printf("%s\n" , temp);
        }
    }
    else
    {
        FILE *fp2 = fopen(".niggit/branches.txt" , "r");
        char line[1000];
        while (fgets(line , sizeof(line) , fp2) != NULL)
        {
            char temp[1000] , temp2[1000];
            sscanf(line , "%[^-]%*c%[^\n]%*c" , temp , temp2);
            
            if (!strcmp(temp , argv[2]))
            {
                printf("Hmmm Looks like a branch with given name already exists\n");
                return;
            }
            
        }
        
        fclose(fp2);

        char dirStageCommand[1000] = "" , dirMakeCommand[1000] = "mkdir " , dirCommitCommand[1000] =""
            ,dirStageCurrent[1000] = "" , dirStageLatest[1000] = "";
        
        strcat(dirMakeCommand , GetHead());
        strcat(dirMakeCommand , "/");
        strcat(dirMakeCommand , argv[2]);

        char newHeadDir[1000] = "";
        strcat(newHeadDir , GetHead());
        strcat(newHeadDir , "/");
        strcat(newHeadDir , argv[2]);

        FILE *branchesFp = fopen(".niggit/branches.txt" , "a");
        char newBranchName[1000]="";
        strcat(newBranchName , argv[2]);
        strcat(newBranchName , "-");
        strcat(newBranchName , newHeadDir);
        fprintf(branchesFp , "%s\n" , newBranchName);

        FILE *fp = fopen(".niggit/head.txt" , "w");
        fprintf(fp , "HEAD=%s" , newHeadDir);
        
        strcat(dirStageCommand , dirMakeCommand);
        strcat(dirStageCommand , "/.stages");
        strcat(dirCommitCommand , dirMakeCommand);
        strcat(dirCommitCommand , "/.commits");
        strcat(dirStageCurrent , dirStageCommand);
        strcat(dirStageCurrent , "/stages-current");
        strcat(dirStageLatest , dirStageCommand);
        strcat(dirStageLatest , "/stages-latest");

        system(dirMakeCommand);
        system(dirStageCommand);
        system(dirCommitCommand);
        system(dirStageCurrent);
        system(dirStageLatest);

        printf("Guess What?! branch created :0\n");
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
    if (!strcmp(argv[1] , "reset"))
    {
        Reset(argv);
    }
    if (!strcmp(argv[1] , "branch"))
    {
        Branch(argv);
    }
    
    
}