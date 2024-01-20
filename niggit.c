#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>

#define branchesAddress ".niggit/branches"
#define masterAddress ".niggit/branches/master"
#define stagesCurrentAddress ".niggit/.stages/stages-current"
#define latestStageTextFile ".niggit/.stages/stages-latest.txt"
#define stageCount ".niggit/.stages/stages-count.txt"

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
    FILE* configFile;
    if (!isGlobal)
    {
        configFile = fopen(".niggit/configs/local-username.txt" , "w");
        if(configFile == NULL)
        {
            printf("Error opening file!\n");
            return;
        }
        fprintf(configFile , "%s" , userName);
        printf("local username successfully changed ! \n");
    }
    else
    {
        configFile = fopen("/home/kshyst/.niggit-settings/global-username.txt" , "w");
        if(configFile == NULL)
        {
            printf("Error opening file!\n");
            return;
        }
        fprintf(configFile , "%s" , userName);
        printf("global username successfully changed ! \n");
    }
    fclose(configFile);
}
void ConfigUserEmail(int isGlobal , char* userEmail)
{
    FILE* configFile;
    if (!isGlobal)
    {
        configFile = fopen(".niggit/configs/local-userEmail.txt" , "w");
        if(configFile == NULL)
        {
            printf("Error opening file!\n");
            return;
        }
        fprintf(configFile , "%s" , userEmail);
        printf("local userEmail successfully changed ! \n");
    }
    else
    {
        configFile = fopen("/home/kshyst/.niggit-settings/global-userEmail.txt" , "w");
        if(configFile == NULL)
        {
            printf("Error opening file!\n");
            return;
        }
        fprintf(configFile , "%s" , userEmail);
        printf("global userEmail successfully changed ! \n");
    }
    fclose(configFile);
}
void Init()
{
    if (opendir(".niggit") != NULL)
    {
        system("rm -r .niggit");

        system("mkdir .niggit");
        system("mkdir .niggit/configs");
        system("mkdir .niggit/branches");
        system("mkdir .niggit/branches/master");
        system("mkdir .niggit/.stages");
        system("mkdir .niggit/.stages/stages-current");
        system("mkdir .niggit/branches/master/.commits");
        
        FILE *fp = fopen(".niggit/head.txt" , "w");
        fprintf(fp , "HEAD=.niggit/branches/master\n");
        fclose(fp);

        FILE *fp2 = fopen(".niggit/branches.txt" , "w");
        fprintf(fp2 , "master-.niggit/branches/master\n");
        fclose(fp2);

        FILE *fp3 = fopen(".niggit/.stages/stages-latest.txt" , "w");
        fprintf(fp3 , "-\n");
        fclose(fp3);

        FILE *fp4 = fopen(".niggit/.stages/stages-count.txt" , "w");
        fprintf(fp4 , "0\n");
        fclose(fp4);

        printf("niggit reinitialized! :)\n");
    }
    else
    {
        system("mkdir .niggit");
        system("mkdir .niggit/branches");
        system("mkdir .niggit/branches/master");
        system("mkdir .niggit/.stages");
        system("mkdir .niggit/.stages/stages-current");
        system("mkdir .niggit/.stages/stages-latest");
        FILE *fp = fopen(".niggit/head.txt" , "w");
        //fopen(".niggit/latestStages.txt" , "w");
        printf("niggit reinitialized! :)\n");

        fprintf(fp , "HEAD=.niggit/branches/master\n");
        fclose(fp);

        FILE *fp2 = fopen(".niggit/branches.txt" , "w");
        fprintf(fp2 , "master-.niggit/branches/master\n");
        fclose(fp2);

        FILE *fp3 = fopen(".niggit/.stages/stages-latest.txt" , "w");
        fclose(fp3);
    }  
}
void Add(char **argv , int isUndo)
{
    int isAnythingStaged = 0;
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
    strcat(currentStageRepo , stagesCurrentAddress);
    
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

                if (!isUndo)
                {
                    FILE *latestStage = fopen(latestStageTextFile , "a");
                    fprintf(latestStage , "%s\n" , argv[ind]);
                    fclose(latestStage);
                    isAnythingStaged = 1;
                }
                

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

                if (!isUndo)
                {
                    FILE *latestStage = fopen(latestStageTextFile , "a");
                    fprintf(latestStage , "%s\n" , argv[ind]);
                    fclose(latestStage);
                    isAnythingStaged = 1;
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
        if (!isUndo)
        {
            FILE *StageCount = fopen(stageCount , "r");
            int count ;
            fscanf(StageCount , "%d" , &count);
            fclose(StageCount);
            StageCount = fopen(stageCount , "w");
            fprintf(StageCount , "%d" , count + 1);
            fclose(StageCount);
        }
        
    }
    //depth
    if (!strcmp(argv[2], "-n")) 
    {
        FILE *fp, *fpStage;
        char path[10000], pathStage[10000];

        char findCommand[1000] = "find -maxdepth ";
        if (argv[3] == NULL)
        {
            strcat(findCommand , "100");
        }
        else
        {
            strcat(findCommand , argv[3]);
        }
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

            if (!isUndo)
            {
                FILE *latestStage = fopen(latestStageTextFile , "a");
                fprintf(latestStage , "%s\n" , argv[2]);
                fclose(latestStage);
                isAnythingStaged = 1;

                FILE *StageCount = fopen(stageCount , "r");
                int count ;
                fscanf(StageCount , "%d" , &count);
                fclose(StageCount);
                StageCount = fopen(stageCount , "w");
                fprintf(StageCount , "%d" , count + 1);
                fclose(StageCount);
            }

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

            if (!isUndo)
            {
                FILE *latestStage = fopen(latestStageTextFile , "a");
                fprintf(latestStage , "%s\n" , argv[2]);
                fclose(latestStage);
                isAnythingStaged = 1;

                FILE *StageCount = fopen(stageCount , "r");
                int count ;
                fscanf(StageCount , "%d" , &count);
                fclose(StageCount);
                StageCount = fopen(stageCount , "w");
                fprintf(StageCount , "%d" , count + 1);
                fclose(StageCount);
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

        char commandWildcard[1000] = "find . -type d -name ";
        strcat(commandWildcard, argvConverted);
        strcat(commandWildcard, " 2> .niggit/error.log");
        //printf("%s\n", commandWildcard);
        fp1 = popen(commandWildcard, "r");

        char commandWildcard2[1000] = "find . -name ";
        strcat(commandWildcard2, argvConverted);
        strcat(commandWildcard2, "*.txt 2> .niggit/error.log");
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
            strcat(command, " 2> .niggit/error.log");
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
            strcat(command, " 2> .niggit/error.log");
            system(command);
        }
    
        if (!isSomethingStaged) 
        {
            printf("Awwwww we didn't find any directory with that wildcard:(((\n");
            return;
        }
        
        if (!isUndo)
        {
            FILE *latestStage = fopen(latestStageTextFile , "a");
            fprintf(latestStage , "%s\n" , argv[2]);
            fclose(latestStage);
            isAnythingStaged = 1;
            FILE *StageCount = fopen(stageCount , "r");
            int count ;
            fscanf(StageCount , "%d" , &count);
            fclose(StageCount);
            StageCount = fopen(stageCount , "w");
            fprintf(StageCount , "%d" , count + 1);
            fclose(StageCount);
        }

        if (isSomethingStaged)
        {     
            printf("Files with the given wildcard Staged :))\n");
        }
        

        pclose(fp1);
        pclose(fp2);
    }
    //append a -
    if (isAnythingStaged)
    {
        FILE *latestStages = fopen(latestStageTextFile , "a");
        fprintf(latestStages , "%s\n" , "-");
        fclose(latestStages);
    }
}
void Reset(char **argv)
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
    
    //undo
    if (!strcmp(argv[2] , "-undo"))
    {
        FILE *StageCount = fopen(stageCount , "r");
        int count ;
        fscanf(StageCount , "%d" , &count);
        fclose(StageCount);
        StageCount = fopen(stageCount , "w");
        fprintf(StageCount , "%d" , count - 1);
        fclose(StageCount);

        FILE *latestStages = fopen(latestStageTextFile , "r");
        char command[1000] = "";
        strcat(command , stagesCurrentAddress);
        strcat(command , "/");
        strcat(command , "temp.txt");
        FILE *fpTemp = fopen(command, "w");
        char line[1024];

        if (count == 0)
        {
            printf("There is nothing to undo :/\n");
            fclose(fpTemp);
            remove(command);
            return;
        }
        

        while (fgets(line, sizeof(line), latestStages)) 
        {
            if (!strcmp(line , "-\n") && (count > 0))
            {
                count--;
                fputs(line, fpTemp);
            } 
            else if (count == 0)
            {
                char **x = (char**)malloc(1 * sizeof(char*));
                x[2] = (char*)malloc(1000 * sizeof(char));
                x[1] = (char*)malloc(1000 * sizeof(char));
                x[0] = (char*)malloc(1000 * sizeof(char));

                char newLine[1000] = "/";
                strcat(newLine , line);

                for (size_t i = 0; i < strlen(newLine); i++)
                {
                    if (newLine[i] == '/')
                    {
                        char temp[1000] = "";
                        strcpy(temp , newLine + i + 1);
                        strcpy(x[0] , temp);
                    }
                }

                x[0][strcspn(x[0], "\n")] = '\0';
                
                char tmp[1000] = "";
                strcat(tmp , x[0]);
                //strcat(tmp , "\"");
                strcpy(x[2] , tmp);

                //printf("%s\n" , x[2]);
                if (strcmp(x[2] , "-"))
                {
                    Reset(x);
                }
                
            }
            else 
            {
                fputs(line, fpTemp);
            }
        }

        fclose(latestStages);
        fclose(fpTemp);

        remove(latestStageTextFile);
        rename(command, latestStageTextFile);

        printf("Last Staged File Unstaged :))\n");
    }
    //reset multiple
    else if (!strcmp(argv[2] , "-f"))
    {
        int i = 0;
        while (argv[3+i] != NULL)
        {
            int dotCount = 0;
            for (size_t j = 0; j < strlen(argv[3+i]); j++)
            {
                if (argv[3+i][j] == '.')
                {
                    dotCount++;
                }
            }
            if (dotCount != 0)
            {

                char command[1000] = "rm -r ";
                strcat(command , "\"");
                strcat(command , stagesCurrentAddress);
                strcat(command , "/");
                strcat(command , argv[3+i]);
                strcat(command , "\" 2> .niggit/error.log");
                system(command);
                printf("File Unstaged :))\n");
            }
            else
            {
                char folderToUnstage[1000] = "\"";
                strcat(folderToUnstage , stagesCurrentAddress);
                strcat(folderToUnstage , "/");
                strcat(folderToUnstage , argv[3+i]);
                strcat(folderToUnstage , "\"");

                char command[1000] = "find ";
                strcat(command , folderToUnstage);
                strcat(command , " -type f -delete 2> .niggit/error.log");
                system(command);
                //printf("%s\n" , command);
                printf("Folder Unstaged :))\n");
            }
            i++;
        }
        
    }
    // reset wild card
    else if ((argv[2] != NULL) && (doesHaveStar == 1))
    {
        char argvConverted[1000] = "";
        strcat(argvConverted, argv[2]);

        FILE *fp1 = NULL, *fp2 = NULL;
        char path[1000], path2[1000];

        char commandWildcard[1000] = "find ";
        strcat(commandWildcard, stagesCurrentAddress);
        strcat(commandWildcard, " -name ");
        strcat(commandWildcard, argvConverted);
        strcat(commandWildcard, " 2> .niggit/error.log");
        //printf("%s\n", commandWildcard);
        fp1 = popen(commandWildcard, "r");

        char commandWildcard2[1000] = "find ";
        strcat(commandWildcard2, stagesCurrentAddress);
        strcat(commandWildcard2, " -name ");
        strcat(commandWildcard2, argvConverted);
        strcat(commandWildcard2, "*.txt 2> .niggit/error.log");
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
            printf("%s\n", path);
            char command[10000] = "find ";
            strcat(command, "\"");
            strcat(command, path);
            strcat(command, "\" ");
            strcat(command, " -type f -delete 2> .niggit/error.log");
            system(command);
        }

        while (fgets(path2, sizeof(path2) - 1, fp2) != NULL) 
        {
            isSomethingStaged = 1;
            path2[strcspn(path2, "\n")] = 0;
            //printf("%s\n", path2);
            char command[10000] = "rm -r ";
            strcat(command, "\"");
            strcat(command, path2);
            strcat(command, "\" ");
            strcat(command, " 2> .niggit/error.log");
            system(command);
        }
    
        if (!isSomethingStaged) 
        {
            printf("Awwwww we didn't find any directory with that wildcard:(((\n");
            return;
        }

        printf("Files with the given wildcard Unstaged :))\n");

        pclose(fp1);
        pclose(fp2);
    }
    //normal reset
    else
    {
        int doesHaveDot = 0;
        for (size_t i = 0; i < strlen(argv[2]); i++)
        {
            if (argv[2][i] == '.')
            {
                doesHaveDot = 1;
                break;
            }
        }
        
        if (doesHaveDot != 0)
        {
            char currentStageRepo[1000] = "";
            strcat(currentStageRepo , stagesCurrentAddress);
            char command[1000] = "rm -r \"";
            strcat(command , currentStageRepo);
            strcat(command , "/");
            strcat(command , argv[2]);
            strcat(command , "\" 2> .niggit/error.log");
            system(command);
            printf("File Unstaged :))\n");
        }
        else
        {
            char folderToUnstage[1000] = "\"";
            strcat(folderToUnstage , stagesCurrentAddress);
            strcat(folderToUnstage , "/");
            strcat(folderToUnstage , argv[2]);
            strcat(folderToUnstage , "\"");

            char command[1000] = "find ";
            strcat(command , folderToUnstage);
            strcat(command , " -type f -delete 2> .niggit/error.log");
            system(command);
            //printf("%s\n" , command);
            printf("Folder Unstaged :))\n");
        }
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
        //system(dirStageCommand);
        system(dirCommitCommand);
        //system(dirStageCurrent);
        //system(dirStageLatest);

        printf("Guess What?! branch created :0\n");
    }
}
void CommandFinder(char **argv)
{
    if (!strcmp(argv[1] , "help"))
    {
        printf("niggit init\n");
        printf("niggit add <file>\n");
        printf("niggit add -f <file1> <file2> ...\n");
        printf("niggit add -n <depth>\n");
        printf("niggit add \"<wildcard>\"\n");
        printf("niggit reset <file>\n");
        printf("niggit reset -undo\n");
        printf("niggit branch\n");
        printf("niggit branch <branch-name>\n");
        // printf("niggit checkout <branch-name>\n");
        // printf("niggit checkout -b <branch-name>\n");
        // printf("niggit checkout -f <branch-name>\n");
        // printf("niggit checkout -f -b <branch-name>\n");
        // printf("niggit merge <branch-name>\n");
        // printf("niggit merge -f <branch-
    }
    if (!strcmp(argv[1] , "config"))
    {
        if (!strcmp(argv[2] , "user.name"))
        {
            ConfigUserName(0 , argv[3]);
        }
        else if(!strcmp(argv[3] , "user.name") && (!strcmp(argv[3] , "user.name")))
        {
            ConfigUserName(1 , argv[4]);
        }
        else if (!strcmp(argv[2] , "user.email"))
        {
            ConfigUserEmail(0 , argv[3]);
        }
        else if(!strcmp(argv[3] , "user.email") && (!strcmp(argv[3] , "user.email")))
        {
            ConfigUserEmail(1 , argv[4]);
        }
        else
        {
            printf("BRUH you entered a wrong command for config :/\n");
        }
        
    }
    if (!strcmp(argv[1] , "init"))
    {
        Init();
    }
    if (!strcmp(argv[1] , "add"))
    {
        Add(argv , 0);
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