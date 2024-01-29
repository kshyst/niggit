#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <time.h> 
#include <sys/types.h>
#include <sys/stat.h>

#define stagesCurrentAddress ".niggit/.stages/stages-current"
#define latestStageTextFile ".niggit/.stages/stages-latest.txt"
#define stageCount ".niggit/.stages/stages-count.txt"
#define stageForCommit ".niggit/.stages/stages-for-commit"
#define unstageCommands ".niggit/.stages/unstage-commands.txt"
//branch
#define branchesAddress ".niggit/branches"
#define masterAddress ".niggit/branches/master"
#define branchesTextFile ".niggit/branches/branches.txt"
#define currentBranchTextFile ".niggit/branches/current-branch.txt"
#define currentBranchName ".niggit/branches/current-branch-name.txt"
#define totalCommitCount ".niggit/branches/commitCount.txt"
#define globalCommitList ".niggit/branches/commit-list.txt"
#define commitShortcuts ".niggit/branches/commit-shortcuts.txt"
#define canCommit ".niggit/branches/can-commit.txt"
#define headAddress ".niggit/branches/head.txt"
#define theLatestCommit ".niggit/branches/latest-commit.txt"
#define currentCommit ".niggit/branches/current-commit.txt"
//configs
#define globalSettingAddress "/home/kshyst/.niggit-settings"
#define localSettingAddress ".niggit/configs"
#define globalAliasAddress "/home/kshyst/.niggit-settings/global-alias.txt"
#define localAliasAddress ".niggit-settings/local-alias.txt"
#define globalUserName "/home/kshyst/.niggit-settings/global-username.txt"
#define globalUserEmail "/home/kshyst/.niggit-settings/global-userEmail.txt"
#define localUserName ".niggit-settings/local-username.txt"
#define localUserEmail ".niggit-settings/local-userEmail.txt"
//phase2 bullshits
#define tagsTextFile ".niggit/configs/tags.txt"
#define tempTagsTextFile ".niggit/configs/temp-tags.txt"
#define appliedHooks ".niggit/configs/applied-hooks.txt"
#define tempAppliedHooks ".niggit/configs/temp-applied-hooks.txt"
#define hooksTextFile "/home/kshyst/.niggit-settings/hooks.txt"
#define canCommitHookTxt ".niggit/configs/can-commit-hook.txt"
//colors
#define RED "\033[0;31m"
#define GREEN "\033[0;32m"
#define BLUE "\033[0;34m"
#define CYAN "\033[0;36m"
#define YELLOW "\033[0;33m"
#define MAGENTA "\033[0;35m"
#define WHITE "\033[0;37m"
#define BLACK "\033[0;30m"
#define BOLDRED "\033[1;31m"
#define BOLDGREEN "\033[1;32m"
#define BOLDBLUE "\033[1;34m"
#define BOLDCYAN "\033[1;36m"
#define BOLDYELLOW "\033[1;33m"
#define BOLDMAGENTA "\033[1;35m"
#define BOLDWHITE "\033[1;37m"
#define BOLDBLACK "\033[1;30m"
#define BOLD "\033[1m"
#define UNDERLINE "\033[4m"
#define BLINK "\033[5m"
#define REVERSE "\033[7m"
#define HIDDEN "\033[8m"
#define RESET "\033[0m"
//Structs
typedef struct Hook
{
    char hookId[100];
    char fileTypes[100];
    char hookDescription[1000];
    struct Hook* next;
}Hook;
Hook* head = NULL;
//Globals 
int canCommitHook = 1;
//Function Prototypes
void CommandFinder(char **argv);
char* GetTime();
int main(int argc, char **argv)
{
    CommandFinder(argv);
}
//Functions
void remove_orig_files(char* directory)
{
    DIR* dir;
    struct dirent* entry;
    char filepath[1024];

    if ((dir = opendir(directory)) == NULL) {
        perror("Unable to open directory");
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (strstr(entry->d_name, ".orig") != NULL) {
            snprintf(filepath, sizeof(filepath), "%s/%s", directory, entry->d_name);
            if (remove(filepath) == 0) {
                printf("Removed %s\n", filepath);
            } else {
                perror("Unable to remove file");
            }
        }
    }

    closedir(dir);
}
void count_files(char *base_path, int *file_count) 
{
    char path[1000];
    struct dirent *dp;
    DIR *dir = opendir(base_path);

    if (!dir)
        return;

    while ((dp = readdir(dir)) != NULL) {
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0) {
            // Construct new path from our base path
            strcpy(path, base_path);
            strcat(path, "/");
            strcat(path, dp->d_name);

            struct stat path_stat;
            stat(path, &path_stat);

            if (S_ISREG(path_stat.st_mode)) {
                (*file_count)++;
            } else if (S_ISDIR(path_stat.st_mode)) {
                count_files(path, file_count);
            }
        }
    }

    closedir(dir);
}
char* GetTime()
{
    struct tm* local; 
    time_t t = time(NULL); 
  
    local = localtime(&t); 

    char* time = (char*)(malloc(1000));
    sprintf(time , "%d/%d/%d %d:%d:%d" , local->tm_year + 1900 , local->tm_mon + 1 , local->tm_mday , local->tm_hour , local->tm_min , local->tm_sec);
  
    return time;
}
int isFolderEmpty(const char *path) 
{
    DIR *dir = opendir(path);

    if (dir == NULL) {
        perror("Error opening directory");
        exit(EXIT_FAILURE);
    }

    struct dirent *entry;
    int isEmpty = 1; // Assume folder is empty initially

    // Read each entry in the directory
    while ((entry = readdir(dir)) != NULL) {
        // Ignore "." and ".." entries
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            isEmpty = 0; // Folder is not empty
            break;
        }
    }

    closedir(dir);

    return isEmpty;
}
int IsNiggitInitialized()
{
    if (opendir(".niggit") == NULL)
    {
        return 0;
    }
    return 1;
}
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
        if (!opendir(".niggit-settings"))
        {
            system("mkdir .niggit-settings");
        }
        
        configFile = fopen(localUserName , "w");
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
        if (!opendir(".niggit-settings"))
        {
            system("mkdir .niggit-settings");
        }
        
        configFile = fopen(localUserName , "w");
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
        if (!opendir(".niggit-settings"))
        {
            system("mkdir .niggit-settings");
        }
        configFile = fopen(localUserEmail , "w");
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
void ConfigAlias(int isGlobal , char** argv)
{   
    char alias[1000] = "";
    char aliasFor[1000] = "";

    if (isGlobal)
    {
        if ((argv[4] == NULL) || (strstr(argv[3] , ".") == NULL))
        {
            printf("you didn't enter the alias name or the alias for :/\n");
            return;
        }
        char temp[1000] = "";
        sscanf(argv[3] , "alias.%[^\n]%*c" , temp);
        strcat(alias , temp);
        strcat(aliasFor , argv[4]);

        char command[1000] = "";
        strcat(command , globalAliasAddress);
        FILE* configFile = fopen(command , "a");
        if(configFile == NULL)
        {
            printf("Error opening file!\n");
            return;
        }
        fprintf(configFile , "%s-%s\n" , alias , aliasFor);
    }
    else
    {
        if ((argv[3] == NULL) || (strstr(argv[2] , ".") == NULL))
        {
            printf("you didn't enter the alias name or the alias for :/\n");
            return;
        }
        char temp[1000] = "";
        sscanf(argv[2] , "alias.%[^\n]%*c" , temp);
        strcat(alias , temp);
        strcat(aliasFor , argv[3]);

        char command[1000] = "";
        strcat(command , localAliasAddress);
        FILE* configFile = fopen(command , "a");
        if(configFile == NULL)
        {
            printf("Error opening file!\n");
            return;
        }
        fprintf(configFile , "%s-%s\n" , alias , aliasFor);
    }

    printf("alias successfully added ! \n");
}
void Init()
{
    // check if we already have initialized niggit in parent directories UNDONE
    if (!IsNiggitInitialized())
    {
        char currentDir[1000] = "";
        char cwd[1000];
        getcwd(cwd, sizeof(cwd));
        strcpy(currentDir, cwd);

        const char s[2] = "/";
        char *token;

        token = strtok(currentDir, s);

        char currentDir2[1000] = "";

        while( token != NULL ) 
        {
            strcat(currentDir2 , token);
            strcat(currentDir2 , "/");

            char command[1000] = "/";
            strcat(command , currentDir2);
            strcat(command , ".niggit");

            if (opendir(command) != NULL)
            {
                printf("BRUH niggit is already initialized in parent directories :/\n");
                return;
            }

            token = strtok(NULL, s);
        }
    }
    if (opendir(".niggit") != NULL)
    {
        system("rm -r .niggit");

        system("mkdir .niggit");
        system("mkdir .niggit/configs");
        system("mkdir .niggit/branches");
        system("mkdir .niggit/branches/master");
        system("mkdir .niggit/.stages");
        system("mkdir .niggit/.stages/stages-current");
        system("mkdir .niggit/.stages/stages-for-commit");
        system("mkdir .niggit/branches/master/.commits");
        
        FILE *fp = fopen(headAddress , "w");
        fclose(fp);

        FILE *fp2 = fopen(branchesTextFile , "w");
        fprintf(fp2 , "master-.niggit/branches/master\n");
        fclose(fp2);

        FILE *fp3 = fopen(".niggit/.stages/stages-latest.txt" , "w");
        fprintf(fp3 , "-\n");
        fclose(fp3);

        FILE *fp4 = fopen(".niggit/.stages/stages-count.txt" , "w");
        fprintf(fp4 , "0\n");
        fclose(fp4);

        FILE *fp5 = fopen(currentBranchTextFile , "w");
        fprintf(fp5 , ".niggit/branches/master");
        fclose(fp5);

        FILE *fp6 = fopen(".niggit/branches/master/commit-list.txt" , "w");
        fclose(fp6);

        FILE *fp7 = fopen(totalCommitCount , "w");
        fprintf(fp7 , "0");
        fclose(fp7);

        FILE *fp8 = fopen(currentBranchName , "w");
        fprintf(fp8 , "master");
        fclose(fp8);
        
        FILE *fp9 = fopen(commitShortcuts , "w");
        fclose(fp9);

        FILE *fp10 = fopen(canCommit , "w");
        fprintf(fp10 , "%d" , 1);
        fclose(fp10);

        FILE *fp11 = fopen(appliedHooks , "w");
        fclose(fp11);

        printf("niggit reinitialized! :)\n");
    }
    else
    {
        system("mkdir .niggit");
        system("mkdir .niggit/configs");
        system("mkdir .niggit/branches");
        system("mkdir .niggit/branches/master");
        system("mkdir .niggit/.stages");
        system("mkdir .niggit/.stages/stages-current");
        system("mkdir .niggit/.stages/stages-for-commit");
        system("mkdir .niggit/branches/master/.commits");
        
        FILE *fp = fopen(headAddress , "w");
        fclose(fp);

        FILE *fp2 = fopen(branchesTextFile , "w");
        fprintf(fp2 , "master-.niggit/branches/master\n");
        fclose(fp2);

        FILE *fp3 = fopen(".niggit/.stages/stages-latest.txt" , "w");
        fprintf(fp3 , "-\n");
        fclose(fp3);

        FILE *fp4 = fopen(".niggit/.stages/stages-count.txt" , "w");
        fprintf(fp4 , "0\n");
        fclose(fp4);

        FILE *fp5 = fopen(currentBranchTextFile , "w");
        fprintf(fp5 , ".niggit/branches/master");
        fclose(fp5);

        FILE *fp6 = fopen(".niggit/branches/master/commit-list.txt" , "w");
        fclose(fp6);

        FILE *fp7 = fopen(totalCommitCount , "w");
        fprintf(fp7 , "0");
        fclose(fp7);

        FILE *fp8 = fopen(currentBranchName , "w");
        fprintf(fp8 , "master");
        fclose(fp8);
        
        FILE *fp9 = fopen(commitShortcuts , "w");
        fclose(fp9);

        FILE *fp10 = fopen(canCommit , "w");
        fprintf(fp10 , "%d" , 1);
        fclose(fp10);

        FILE *fp11 = fopen(appliedHooks , "w");
        fclose(fp11);

        printf("niggit initialized! :)\n");
    }  
}
void Add(char **argv , int isUndo)
{
    if (IsNiggitInitialized() == 0)
    {
        printf("BRUH niggit is not initialized :/\n");
        return;
    }
    
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
    // redo
    if (!strcmp(argv[2] , "-redo"))
    {
        FILE *fp = fopen(unstageCommands , "r");
        char line[1000];
        while (fgets(line, sizeof(line), fp)) 
        {
            line[strcspn(line, "\n")] = 0;
            char command[1000] = "niggit add \"";
            strcat(command , line);
            strcat(command , "\"");
            system(command);
        }
        FILE *fp2 = fopen(unstageCommands , "w");
        //print
        printf("Redo Done :))\n");
    }
    //multiple file add -f
    else if(!strcmp(argv[2] , "-f"))
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
                const char s[2] = "/";
                char *token;
                char argv2Copy[1000] = "";
                strcat(argv2Copy , argv[ind]);
                token = strtok(argv[ind], s);

                char currentDir[1000] = "";

                while( token != NULL ) 
                {
                    if (strstr(token , ".") != NULL)
                    {
                        break;
                    }

                    strcat(currentDir , token);
                    strcat(currentDir , "/");

                    if (opendir(currentDir) != NULL)
                    {
                        char command[1000] = "mkdir ";
                        strcat(command , "\"");
                        strcat(command , currentStageRepo);
                        strcat(command , "/");
                        strcat(command , currentDir);
                        strcat(command , "\"");
                        strcat(command , " 2> .niggit/error.log");
                        system(command);
                    }

                    token = strtok(NULL, s);
                }

                char command[10000] = "cp ";
                strcat(command , "\"");
                strcat(command , argv2Copy);
                strcat(command , "\"");
                strcat(command , " ");
                strcat(command , "\"");
                strcat(command , currentStageRepo);
                strcat(command , "/");
                strcat(command , currentDir);
                strcat(command , "\"");
                //printf("%s\n" , command);
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
    else if (!strcmp(argv[2], "-n")) 
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

            const char s[2] = "/";
            char *token;
            char argv2Copy[1000] = "";
            strcat(argv2Copy , argv[2]);

            token = strtok(argv[2], s);

            char currentDir[1000] = "";

            while( token != NULL ) 
            {
                if (strstr(token , ".") != NULL)
                {
                    break;
                }
                
                strcat(currentDir , token);
                strcat(currentDir , "/");

                if (opendir(currentDir) != NULL)
                {
                    char command[1000] = "mkdir ";
                    strcat(command , "\"");
                    strcat(command , currentStageRepo);
                    strcat(command , "/");
                    strcat(command , currentDir);
                    strcat(command , "\"");
                    strcat(command , " 2> .niggit/error.log");
                    system(command);
                }

                token = strtok(NULL, s);
            }
            
            char command[10000] = "cp ";
            strcat(command , "\"");
            strcat(command , argv2Copy);
            strcat(command , "\"");
            strcat(command , " ");
            strcat(command , "\"");
            strcat(command , currentStageRepo);
            strcat(command , "/");
            strcat(command , currentDir);
            strcat(command , "\"");
            //printf("%s\n" , command);
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
            printf("Folder Staged :))\n");
        }
    }
    //wildcard add
    else if ((argv[3] == NULL) && doesHaveStar) 
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
        strcat(commandWildcard2, "*.* 2> .niggit/error.log");
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
            if (strstr(path2 , ".niggit") != NULL)
            {
                continue;
            }
            
            path2[strcspn(path2, "\n")] = 0;
            //printf("%s\n", path2);
            const char s[2] = "/";
            char *token;
            char argv2Copy[1000] = "";
            strcat(argv2Copy , path2 + 2);

            token = strtok(path2 + 2, s);

            char currentDir[1000] = "";

            while( token != NULL ) 
            {
                if (strstr(token , ".") != NULL)
                {
                    break;
                }
                
                strcat(currentDir , token);
                strcat(currentDir , "/");

                if (opendir(currentDir) != NULL)
                {
                    char command[1000] = "mkdir ";
                    strcat(command , "\"");
                    strcat(command , currentStageRepo);
                    strcat(command , "/");
                    strcat(command , currentDir);
                    strcat(command , "\"");
                    strcat(command , " 2> .niggit/error.log");
                    system(command);
                }

                token = strtok(NULL, s);
            }
            
            char command[10000] = "cp ";
            strcat(command , "\"");
            strcat(command , argv2Copy);
            strcat(command , "\"");
            strcat(command , " ");
            strcat(command , "\"");
            strcat(command , currentStageRepo);
            strcat(command , "/");
            strcat(command , currentDir);
            strcat(command , "\"");
            //printf("%s\n" , command);
            system(command);
            printf("File Staged :))\n");
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
    // else
    // {
    //     printf("staging failed :/\n");
    // }
}
void Reset(char **argv)
{
    if (IsNiggitInitialized() == 0)
    {
        printf("BRUH niggit is not initialized :/\n");
        return;
    }
    int doesHaveStar = 0;
    for (size_t i = 0; i < strlen(argv[2]); i++)
    {
        if (argv[2][i] == '*')
        {
            doesHaveStar = 1;
            break;
        }
    }
    
    //undo UNDONE
    if (!strcmp(argv[2] , "-undo"))
    {
        //getting stage count
        FILE *StageCount = fopen(stageCount , "r");
        int count ;
        fscanf(StageCount , "%d" , &count);
        fclose(StageCount);

        // getting the list of latest stages
        FILE *latestStages = fopen(latestStageTextFile , "r");
        char command[1000] = "";
        strcat(command , stagesCurrentAddress);
        strcat(command , "/");
        strcat(command , "temp.txt");
        FILE *fpTemp = fopen(command, "w");
        char line[1024];

        // checks if we had any latest stages
        if (count <= 0)
        {
            printf("There is nothing to undo :/\n");
            fclose(fpTemp);
            remove(command);
            return;
        }
        
        // reducing stage count by 1
        StageCount = fopen(stageCount , "w");
        fprintf(StageCount , "%d" , count - 1);
        fclose(StageCount);

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

                // char newLine[1000] = "/";
                // strcat(newLine , line);

                // x[0][strcspn(x[0], "\n")] = '\0';
                
                // char tmp[1000] = "";
                // strcat(tmp , x[0]);
                // //strcat(tmp , "\"");
                line[strcspn(line, "\n")] = '\0';
                strcat(x[2] , line);

                //  printf("%s\n" , x[2]);
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
            //add the command to unstage commands file
            FILE* unstageCommandsFile = fopen(unstageCommands , "a");
            fprintf(unstageCommandsFile , "%s\n" , argv[3+i]);
            fclose(unstageCommandsFile);
            //unstaging
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
        //add the command to unstage commands file
        FILE* unstageCommandsFile = fopen(unstageCommands , "a");
        fprintf(unstageCommandsFile , "%s\n" , argv[2]);
        fclose(unstageCommandsFile);

        //unstaging
        char argvConverted[1000] = "";
        strcat(argvConverted, argv[2]);

        FILE *fp1 = NULL, *fp2 = NULL;
        char path[1000], path2[1000];

        char commandWildcard[1000] = "find ";
        strcat(commandWildcard, stagesCurrentAddress);
        strcat(commandWildcard, " -name ");
        strcat(commandWildcard, argvConverted);
        strcat(commandWildcard, " 2> .niggit/error.log");
        fp1 = popen(commandWildcard, "r");

        char commandWildcard2[1000] = "find ";
        strcat(commandWildcard2, stagesCurrentAddress);
        strcat(commandWildcard2, " -name ");
        strcat(commandWildcard2, argvConverted);
        strcat(commandWildcard2, "*.* 2> .niggit/error.log");
        fp2 = popen(commandWildcard2, "r");

        if ((fp1 == NULL) && (fp2 == NULL)) {
            printf("Awwwww we didn't find any directory with that wildcard:(((\n");
            return;
        }
        
        int isSomethingStaged = 0;

        while (fgets(path, sizeof(path) - 1, fp1) != NULL) 
        {
            if (strstr(path , ".niggit") == NULL)
            {
                continue;
            }
            
            isSomethingStaged = 1;
            path[strcspn(path, "\n")] = 0;
            //printf("%s\n", path);
            char command[10000] = "find ";
            strcat(command, "\"");
            strcat(command, path);
            strcat(command, "\" ");
            strcat(command, " -type f -delete 2> .niggit/error.log");
            system(command);
        }

        while (fgets(path2, sizeof(path2) - 1, fp2) != NULL) 
        {
            if (strstr(path2 , ".niggit") == NULL)
            {
                continue;
            }
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
        //add the command to unstage commands file
        FILE* unstageCommandsFile = fopen(unstageCommands , "a");
        fprintf(unstageCommandsFile , "%s\n" , argv[2]);
        fclose(unstageCommandsFile);

        //unstaging

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
            // printf("%s" , command);
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
    if (IsNiggitInitialized() == 0)
    {
        printf("BRUH niggit is not initialized :/\n");
        return;
    }
    //show branches
    if (argv[2] == NULL)
    {
        FILE *fp2 = fopen(branchesTextFile , "r");
        char line[1000];
        while (fgets(line , sizeof(line) , fp2) != NULL)
        {
            char temp[1000] , temp2[1000];
            sscanf(line , "%[^-]%*c%[^\n]%*c" , temp , temp2);
            printf("%s\n" , temp);
        }
    }
    //create branch
    else
    {
        //checks if we already have a branch with this name
        FILE *fp2 = fopen(branchesTextFile , "r");
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

        //creating a new branch

        char  latestCommitFInd[1000] = "" ;
        FILE *fp = fopen(currentBranchTextFile , "r");
        char currentBranch[1000] = "";
        fgets(currentBranch , sizeof(currentBranch) , fp);
        fclose(fp);
        strcat(latestCommitFInd , currentBranch);
        strcat(latestCommitFInd , "/");
        strcat(latestCommitFInd , "latest-commit.txt");
        fp = fopen(latestCommitFInd , "r");
        char latestCommit[1000] = "";
        fgets(latestCommit , sizeof(latestCommit) , fp);
        fclose(fp);

        char makeFolder[1000] = "mkdir ";
        char newBranchAddress[1000] = "";
        strcat(newBranchAddress , latestCommit);
        strcat(newBranchAddress , "/");
        strcat(newBranchAddress , "branch-");
        strcat(newBranchAddress , argv[2]);

        strcat(makeFolder , "\"");
        strcat(makeFolder , newBranchAddress);
        strcat(makeFolder , "\"");
        system(makeFolder);

        //adding the new branch to branches.txt
        FILE *fp3 = fopen(branchesTextFile , "a");
        fprintf(fp3 , "%s-%s\n" , argv[2] , newBranchAddress );
        fclose(fp3);

        //adding the new branch to current branch
        // FILE *fp4 = fopen(currentBranchTextFile , "w");
        // fprintf(fp4 , "%s" , newBranchAddress);
        // fclose(fp4);

        //adding the new branch to current branch name
        // FILE *fp5 = fopen(currentBranchName , "w");
        // fprintf(fp5 , "%s" , argv[2]);
        // fclose(fp5);

        //creating the .commits folder for branch
        char makeFolder2[1000] = "mkdir ";
        strcat(makeFolder2 , "\"");
        strcat(makeFolder2 , newBranchAddress);
        strcat(makeFolder2 , "/");
        strcat(makeFolder2 , ".commits");
        strcat(makeFolder2 , "\"");
        system(makeFolder2);

        //creating the latest commit list
        char makeFile[1000] = "";
        strcat(makeFile , newBranchAddress);
        strcat(makeFile , "/");
        strcat(makeFile , "latest-commit.txt");
        FILE *fp6 = fopen(makeFile , "w");
        fclose(fp6);

        //creating the commit list
        char makeFile2[1000] = "";
        strcat(makeFile2 , newBranchAddress);
        strcat(makeFile2 , "/");
        strcat(makeFile2 , "commit-list.txt");
        FILE *fp7 = fopen(makeFile2 , "w");
        fclose(fp7);

        printf("Guess What?! branch created :0\n");
    }
}
void Status(char **argv)
{
    if (IsNiggitInitialized() == 0)
    {
        printf("BRUH niggit is not initialized :/\n");
        return;
    }

    // find all files in root
    char rootAd[1000] = "";
    FILE* tempForRootAddress = popen("pwd" , "r");
    fgets(rootAd , sizeof(rootAd) , tempForRootAddress);
    rootAd[strcspn(rootAd, "\n")] = 0;
    pclose(tempForRootAddress);

    char commandForFindAllFilesInRoot[1000] = "find ";
    strcat(commandForFindAllFilesInRoot , rootAd);
    strcat(commandForFindAllFilesInRoot , " -type f 2> .niggit/error.log");
    FILE* tempForFindAllFilesInRoot = popen(commandForFindAllFilesInRoot , "r");
    char line[1000];
    int allFilesInRootCount = 0;
    while (fgets(line , sizeof(line) , tempForFindAllFilesInRoot) != NULL)
    {
        //skip if its in niggit
        if (strstr(line , ".niggit") != NULL)
        {
            continue;
        }
        //check if the file is in stages
        line[strcspn(line, "\n")] = 0;
        int isStaged = 0;
        FILE* tempForFindAllFilesInStages = popen("find .niggit/.stages/stages-current -type f 2> .niggit/error.log" , "r");
        char line2[1000];
        while (fgets(line2 , sizeof(line2) , tempForFindAllFilesInStages) != NULL)
        {
            line2[strcspn(line2, "\n")] = 0;
            if (!strcmp(line + strlen(rootAd) + 1 , line2 + strlen(stagesCurrentAddress) + 1))
            {
                printf("%s" , "+");
                isStaged = 1;
                break;
            }
        }
        if (!isStaged)
        {
            printf("%s" , "-");
        }

        //compare if it is unchanged or deleted or modified or added from last commit
        char theLatestCommitAddress[1000] = "";
        FILE *fp = fopen(theLatestCommit , "r");
        fgets(theLatestCommitAddress , sizeof(theLatestCommitAddress) , fp);
        char commandForFindAllFilesInLastCommit[1000] = "find \"";
        strcat(commandForFindAllFilesInLastCommit , theLatestCommitAddress);
        strcat(commandForFindAllFilesInLastCommit , "\" -type f 2> .niggit/error.log");

        FILE* tempForFindAllFilesInLastCommit = popen(commandForFindAllFilesInLastCommit , "r");
        char line3[1000];
        int isFileInLastCommit = 0;
        while (fgets(line3 , sizeof(line3) , tempForFindAllFilesInLastCommit) != NULL)
        {
            line3[strcspn(line3, "\n")] = 0;
            if (!strcmp(line + strlen(rootAd) + 1 , line3 + strlen(theLatestCommitAddress) + 1))
            {
                isFileInLastCommit = 1;
                // check if the permissions changed
                char comPer1[1000] = "ls -l \"" , comPer2[1000] = "ls -l \"";
                strcat(comPer1 , line);
                strcat(comPer1 , "\"");
                strcat(comPer2 , line3);
                strcat(comPer2 , "\"");
                FILE* per1f = popen(comPer1 , "r");
                FILE* per2f = popen(comPer2 , "r");
                char per1[1000] , per2[1000];
                fgets(per1 , 11 , per1f);
                fgets(per2 , 11 , per2f);
                if (strcmp(per1 , per2))
                {
                    printf("T");
                }
                
                break;
            }
        }
        if (!isFileInLastCommit)
        {
            printf("%s" , "A");
        }
        else
        {
            char commandForCompare[1000] = "cmp -s \"";
            strcat(commandForCompare , line);
            strcat(commandForCompare , "\" \"");
            strcat(commandForCompare , line3);
            strcat(commandForCompare , "\"");
            if (system(commandForCompare) == 0)
            {
                printf("%s" , "U");
            }
            else
            {
                printf("%s" , "M");
            }
        }

        //print the file name
        printf(" File Name = %s\n" , line + strlen(rootAd) + 1);

        allFilesInRootCount++;
    }
    pclose(tempForFindAllFilesInRoot);

    // check if it is deleted or not
    char theLatestCommitAddress[1000] = "";
    FILE *fp = fopen(theLatestCommit , "r");
    fgets(theLatestCommitAddress , sizeof(theLatestCommitAddress) , fp);
    char commandForFindAllFilesInLastCommit[1000] = "find \"";
    strcat(commandForFindAllFilesInLastCommit , theLatestCommitAddress);
    strcat(commandForFindAllFilesInLastCommit , "\" -type f 2> .niggit/error.log");

    FILE* tempForFindAllFilesInLastCommit = popen(commandForFindAllFilesInLastCommit , "r");
    char line3[1000];
    int isFileInLastCommit = 0;
    while (fgets(line3 , sizeof(line3) , tempForFindAllFilesInLastCommit) != NULL)
    {
        line3[strcspn(line3, "\n")] = 0;
        int isFileInRoot = 0;
        FILE* tempForFindAllFilesInRoot = popen(commandForFindAllFilesInRoot , "r");
        char line4[1000];
        while (fgets(line4 , sizeof(line4) , tempForFindAllFilesInRoot) != NULL)
        {
            line4[strcspn(line4, "\n")] = 0;
            if (!strcmp(line3 + strlen(theLatestCommitAddress) + 1 , line4 + strlen(rootAd) + 1))
            {
                isFileInRoot = 1;
                break;
            }
        }
        if (!isFileInRoot)
        {
            printf("-%s" , "D");
            printf(" File Name = %s\n" , line3 + strlen(theLatestCommitAddress) + 1);
        }
    }
}
void SetCommitShortCut(char **argv)
{
    if (IsNiggitInitialized() == 0)
    {
        printf("BRUH niggit is not initialized :/\n");
        return;
    }
    if (strcmp(argv[2] , "-m") || (!argv[2])|| (!argv[3]) || (strcmp(argv[4] , "-s")) || (!argv[5]) || (!argv[4]))
    {
        printf("wrong command for set :/\n");
        return;
    }
    
    FILE *fp = fopen(commitShortcuts , "a");
    fprintf(fp , "%s-%s\n" , argv[3] , argv[5]);
    fclose(fp);

    printf("shortcut set :))\n");
}
void RemoveCommitShortCut(char **argv)
{
    if (IsNiggitInitialized() == 0)
    {
        printf("BRUH niggit is not initialized :/\n");
        return;
    }
    if (strcmp(argv[2] , "-s") || (!argv[2])|| (!argv[3]))
    {
        printf("wrong command for remove :/\n");
        return;
    }
    
    FILE *fp = fopen(commitShortcuts , "r");
    char line[1000];
    char temp[1000] = "";
    int isShortCutFound = 0;
    strcat(temp , argv[3]);
    char temp2[1000] = "";
    while (fgets(line , sizeof(line) , fp) != NULL)
    {
        char shortcutMessage[1000] = "";
        char shortcutShortcut[1000] = "";
        sscanf(line , "%[^-]%*c%[^\n]%*c" , shortcutMessage , shortcutShortcut);
        if (strstr(shortcutShortcut , temp) != NULL)
        {
            isShortCutFound = 1;
            continue;
        }
        strcat(temp2 , line);
    }
    fclose(fp);
    fp = fopen(commitShortcuts , "w");
    fprintf(fp , "%s" , temp2);
    fclose(fp);

    if (!isShortCutFound)
    {
        printf("the shortcut doesn't exists :/\n");
        return;
    }
    printf("shortcut removed :))\n");
    
}
void ReplaceCommitShortCut(char **argv)
{
    if (IsNiggitInitialized() == 0)
    {
        printf("BRUH niggit is not initialized :/\n");
        return;
    }
    if (strcmp(argv[2] , "-m") || (!argv[2])|| (!argv[3]) || (!argv[4]) || (strcmp(argv[4] , "-s") || (!argv[5])))
    {
        printf("wrong command for replace :/\n");
        return;
    }
    
    FILE *fp = fopen(commitShortcuts , "r");
    char line[1000];
    char temp[1000] = "";
    int isShortCutFound = 0;
    strcat(temp , argv[5]);
    char temp2[1000] = "";
    while (fgets(line , sizeof(line) , fp) != NULL)
    {
        char shortcutMessage[1000] = "";
        char shortcutShortcut[1000] = "";
        sscanf(line , "%[^-]%*c%[^\n]%*c" , shortcutMessage , shortcutShortcut);
        if (!strcmp(shortcutShortcut , temp))
        {
            isShortCutFound = 1;
            strcat(temp2 , argv[3]);
            strcat(temp2 , "-");
            strcat(temp2 , argv[5]);
            strcat(temp2 , "\n");
            continue;
        }
        strcat(temp2 , line);
    }
    fclose(fp);
    fp = fopen(commitShortcuts , "w");
    fprintf(fp , "%s" , temp2);
    fclose(fp);

    if (!isShortCutFound)
    {
        printf("the shortcut doesn't exists :/\n");
        return;
    }
    printf("shortcut replaced :))\n");
    

}
void Commit(char **argv)
{
    if (IsNiggitInitialized() == 0)
    {
        printf("BRUH niggit is not initialized :/\n");
        return;
    }
    if(1)
    {
        int canCommiting = 0;
        FILE *fp10 = fopen(canCommit , "r");
        fscanf(fp10 , "%d" , &canCommiting);
        fclose(fp10);

        int canCommitionHook = 0;
        FILE *fp11 = fopen(canCommitHookTxt , "r");
        if (fp11 != NULL)
        {
            fscanf(fp11 , "%d" , &canCommitionHook);
            fclose(fp11);
        }
        
        if (!canCommitionHook)
        {
            printf("you have failed hooks , do you want to proceed with commiting? (y/n)\n");
            char answer[1000] = "";
            scanf("%s" , answer);
            if (strcmp(answer , "y"))
            {
                printf("commiting canceled :/\n");
                return;
            }
        }
        
        
        if (canCommiting == 0)
        {
            printf("You can't cum it on a detached head motherfucker:/\n");
            return;
        }
    }
    if (!strcmp(argv[2] , "-s"))
    {
        if (argv[3] == NULL)
        {
            printf("BRUH you didn't enter a shorcut :/\n");
            return;
        }
        FILE *fp = fopen(commitShortcuts , "r");
        char line[1000];
        char temp[1000] = "";
        int isShortCutFound = 0;
        strcat(temp , argv[3]);
        char temp2[1000] = "";
        while (fgets(line , sizeof(line) , fp) != NULL)
        {
            char shortcutMessage[1000] = "";
            char shortcutShortcut[1000] = "";
            sscanf(line , "%[^-]%*c%[^\n]%*c" , shortcutMessage , shortcutShortcut);
            if (!strcmp(shortcutShortcut , temp))
            {
                isShortCutFound = 1;
                strcat(temp2 , shortcutMessage);
                break;
            }
        }
        fclose(fp);
        if (!isShortCutFound)
        {
            printf("the shortcut doesn't exists :/\n");
            return;
        }
        
        char commad[1000] = "niggit commit -m \"";
        strcat(commad , temp2);
        strcat(commad , "\"");
        system(commad);
        return;
    }
    if (strcmp(argv[2] , "-m") || (argv[2] == NULL))
    {
        printf("BRUH you entered wrong command for commit :/\n");
        return;
    }
    if (argv[3] == NULL)
    {
        printf("BRUH you didn't enter a commit message :/\n");
        return;
    }
    if (isFolderEmpty(stagesCurrentAddress))
    {
        printf("You Didn't fucking stage anything what are you trying to commit ???????\n");
        return;
    }
    if ((fopen(localUserName , "r") == NULL) && (fopen(globalUserName , "r") == NULL))
    {
        printf("you didn't set your username :/\n");
        return;
    }
    //getting the commit message
    char commitMessage[1000] = "";
    strcat(commitMessage , argv[3]);
    if (strlen(argv[3]) > 72)
    {
        printf("commit message tooooo long :(((\n");
        return;
    }

    //put all files from the root into the stages for commit folder
    char rootAddress[1000] = "";
    FILE* temp1 = popen("pwd" , "r");
    fgets(rootAddress , sizeof(rootAddress) , temp1);
    rootAddress[strlen(rootAddress) - 1] = '\0';
    char commandForCpy[1000] = "find \"";
    strcat(commandForCpy , rootAddress);
    strcat(commandForCpy , "\" -maxdepth 1 2>.niggit/error.log");
    FILE* tempForCpy = popen(commandForCpy , "r");
    char line[1000];
    while (fgets(line , sizeof(line) , tempForCpy) != NULL)
    {
        line[strlen(line) - 1] = '\0';
        if (!strcmp(line , rootAddress))
        {
            continue;
        }
        if (strstr(line , ".niggit") == NULL)
        {
            char commandForCpy2[1000] = "cp -r \"";
            strcat(commandForCpy2 , line);
            strcat(commandForCpy2 , "\"");
            strcat(commandForCpy2 , " \"");
            strcat(commandForCpy2 , stageForCommit);
            strcat(commandForCpy2 , "\"");
            system(commandForCpy2);
        }
    }
    //if the file in stageForCommit is the same with the file in stages current replace it with the file in stages current
    
    char commandToReplace[1000] = "find ";
    strcat(commandToReplace , stageForCommit);
    strcat(commandToReplace , " -type f 2> .niggit/error.log");
    FILE* tempForReplace = popen(commandToReplace , "r");
    char line2[1000];
    char commandToFindAllInStageCurrent[1000] = "find ";
    strcat(commandToFindAllInStageCurrent , stagesCurrentAddress);
    strcat(commandToFindAllInStageCurrent , " -type f 2> .niggit/error.log");
    
    while (fgets(line2 , sizeof(line2) , tempForReplace) != NULL)
    {
        line2[strlen(line2) - 1] = '\0';
        //printf("%s\n" , line2 + strlen(stageForCommit) + 1);
        FILE* tempForFindAllInStageCurrent = popen(commandToFindAllInStageCurrent , "r");
        char line3[1000];
        while (fgets(line3 , sizeof(line3) , tempForFindAllInStageCurrent) != NULL)
        {
            line3[strlen(line3) - 1] = '\0';
            // printf("%s\n" , line3);
            // printf("%s\n" , line2);
            // printf("----------\n");
            if (!strcmp(line3 + strlen(stagesCurrentAddress) + 1 , line2 + strlen(stageForCommit) + 1))
            {
                
                char commandToReplace2[1000] = "rm -f \"";
                strcat(commandToReplace2 , line2);
                strcat(commandToReplace2 , "\"");
                system(commandToReplace2);

                int txtLength = 0;
                for (size_t i = strlen(line3)-1; i >=0; i--)
                {
                    if (line3[i] == '/')
                    {
                        break;
                    }
                    txtLength++;
                }
                
                char commandToReplace3[1000] = "cp -r \"";
                strcat(commandToReplace3 , line3);
                strcat(commandToReplace3 , "\"");
                strcat(commandToReplace3 , " \"");
                strcat(commandToReplace3 , line2);
                strcat(commandToReplace3 , "\"");
                system(commandToReplace3);
                break;
            }
        }  
    }

    //if file is deleted in working tree but availble in current stage add it to stage for commit
    char commandToFindAllInStageCurrent1[1000] = "find ";
    strcat(commandToFindAllInStageCurrent1 , stagesCurrentAddress);
    FILE* tempForFindAllInStageCurrent = popen(commandToFindAllInStageCurrent1 , "r");
    char line3[1000];
    while (fgets(line3 , sizeof(line3) , tempForFindAllInStageCurrent) != NULL)
    {
        line3[strlen(line3) - 1] = '\0';
        // printf("%s\n" , line3);
        // printf("%s\n" , line2);
        // printf("----------\n");
        char commandToFindAllInRoot[1000] = "find ";
        strcat(commandToFindAllInRoot , rootAddress);
        FILE* tempForFindAllInRoot = popen(commandToFindAllInRoot , "r");
        char line4[1000];
        int isFileInRoot = 0;
        while (fgets(line4 , sizeof(line4) , tempForFindAllInRoot) != NULL)
        {
            line4[strlen(line4) - 1] = '\0';
            if (!strcmp(line3 + strlen(stagesCurrentAddress) + 1 , line4 + strlen(rootAddress) + 1))
            {
                isFileInRoot = 1;
                break;
            }
        }
        if (!isFileInRoot)
        {
            char commandToReplace2[1000] = "cp -r \"";
            strcat(commandToReplace2 , line3);
            strcat(commandToReplace2 , "\"");
            strcat(commandToReplace2 , " \"");
            strcat(commandToReplace2 , stageForCommit);
            strcat(commandToReplace2 , "\"");
            system(commandToReplace2);
        }
    }

    //finds how many files are there in stage-current
    int fileCount = 0;
    count_files(stageForCommit, &fileCount);
    // if (fileCount == 0)
    // {
    //     printf("You Didn't fucking stage anything what are you trying to commit ???????\n");
    //     return;
    // }
    
    //get the total commit counts till now for creating commit id
    FILE *fp = fopen(totalCommitCount , "r");
    int count ;
    fscanf(fp , "%d" , &count);
    fclose(fp);

    //adding 1 to total commit count

    FILE *fp1 = fopen(totalCommitCount , "w");
    fprintf(fp1 , "%d" , count + 1);
    fclose(fp1);

    //get the current branch to create the folder commit inside it
    FILE *fp2 = fopen(currentBranchTextFile , "r");
    char currentBranch[1000] = "";
    fscanf(fp2 , "%s" , currentBranch);
    fclose(fp2);

    //create commit id
    int commitId = count+1;

    //create commit folder
    char commitFolder[1000] = "";
    strcat(commitFolder , currentBranch);
    strcat(commitFolder , "/.commits/");

    char commitIdString[1000] = "#";
    char temp[1000] = "";
    sprintf(temp , "%d" , commitId);
    strcat(commitIdString , temp);

    char commandToCpyStagesInCommits[1000] = "cp -r \"";
    strcat(commandToCpyStagesInCommits , stageForCommit);
    strcat(commandToCpyStagesInCommits , "\" \"");
    strcat(commandToCpyStagesInCommits , commitFolder);
    strcat(commandToCpyStagesInCommits , "\"");
    system(commandToCpyStagesInCommits);

    char renameTheStageToCommitHash[1000] = "";
    strcat(renameTheStageToCommitHash , commitFolder);
    strcat(renameTheStageToCommitHash , "stages-for-commit");
    char newName[1000] = "";
    strcat(newName , commitFolder);
    strcat(newName , commitIdString);
    rename(renameTheStageToCommitHash , newName);

    //get local username and global username
    char localUserNameString[1000] = "";
    char globalUserNameString[1000] = "";
    
    FILE *fp3 = fopen(localUserName , "r");
    if (fp3 == NULL)
    {
        FILE *fp4 = fopen(globalUserName , "r");
        fgets(globalUserNameString , sizeof(globalUserNameString) , fp4);
        fclose(fp4);
    }
    else
    {
        fgets(localUserNameString , sizeof(localUserNameString) , fp3);
        fclose(fp3);
    }

    char commitUsername[1000] = "";
    if (localUserNameString[0] != '\0')
    {
        strcat(commitUsername , localUserNameString);
    }
    else if (globalUserNameString[0] != '\0')
    {
        strcat(commitUsername , globalUserNameString);
    }
    else
    {
        printf("BRUH you didn't set your username :/\n");
        return;
    }

    //get global and local user email
    char localUserEmailString[1000] = "";
    char globalUserEmailString[1000] = "";

    FILE *fp4 = fopen(localUserEmail , "r");
    if (fp4 == NULL)
    {
        FILE *fp5 = fopen(globalUserEmail , "r");
        fgets(globalUserEmailString , sizeof(globalUserEmailString) , fp5);
        fclose(fp5);
    }
    else
    {
        fgets(localUserEmailString , sizeof(localUserEmailString) , fp4);
        fclose(fp4);
    }

    char commitEmail[1000] = "";
    if (localUserEmailString[0] != '\0')
    {
        strcat(commitEmail , localUserEmailString);
    }
    else if (globalUserEmailString[0] != '\0')
    {
        strcat(commitEmail , globalUserEmailString);
    }
    else
    {
        printf("BRUH you didn't set your email :/\n");
        return;
    }

    // get current branch name
    char currentBranchNameString[1000] = "";
    FILE *fp5 = fopen(currentBranchName , "r");
    fgets(currentBranchNameString , sizeof(currentBranchNameString) , fp5);
    fclose(fp5);

    //deletes stages-current folder and creates a new one
    char commandToDeleteStagesCurrent[1000] = "rm -r \"";
    strcat(commandToDeleteStagesCurrent , stagesCurrentAddress);
    strcat(commandToDeleteStagesCurrent , "\"");
    system(commandToDeleteStagesCurrent);
    char commandToCreateNewStagesCurrent[1000] = "mkdir \"";
    strcat(commandToCreateNewStagesCurrent , stagesCurrentAddress);
    strcat(commandToCreateNewStagesCurrent , "\"");
    system(commandToCreateNewStagesCurrent);

    //delets stages-for-commit folder and creates a new one
    char commandToDeleteStagesForCommit[1000] = "rm -r \"";
    strcat(commandToDeleteStagesForCommit , stageForCommit);
    strcat(commandToDeleteStagesForCommit , "\"");
    system(commandToDeleteStagesForCommit);
    char commandToCreateNewStagesForCommit[1000] = "mkdir \"";
    strcat(commandToCreateNewStagesForCommit , stageForCommit);
    strcat(commandToCreateNewStagesForCommit , "\"");
    system(commandToCreateNewStagesForCommit);

    //add the commit to the commit list of current branch
    char commitList[1000] = "";
    strcat(commitList , currentBranch);
    strcat(commitList , "/commit-list.txt");
    FILE *commitListFp = fopen(commitList , "a");
    fprintf(commitListFp , "%s-%s-%s-%s-%s-%s-%d\n" , commitIdString , commitMessage , GetTime() , currentBranchNameString , commitUsername , commitEmail , fileCount);

    //add commit to global commit list
    FILE *globalCommitListFp = fopen(globalCommitList , "a");
    fprintf(globalCommitListFp , "%s-%s-%s-%s-%s-%s-%d\n" , commitIdString , commitMessage , GetTime() , currentBranchNameString , commitUsername , commitEmail,  fileCount );
    fclose(globalCommitListFp);

    //print every shit about commit

    printf("you just Cummited ! shame...\n");
    printf("commit id : %s\n" , commitIdString);
    printf("commit message : %s\n" , commitMessage);
    printf("commit time : %s\n" , GetTime());
    printf("commit branch : %s\n" , currentBranchNameString);
    printf("commit username : %s\n" , commitUsername);
    printf("commit user email : %s\n" , commitEmail);
    printf("commit file count : %d\n" , fileCount);

    //store the last commit 

    char latestCommitTextFileAddress[1000] = "";
    strcat(latestCommitTextFileAddress , currentBranch);
    strcat(latestCommitTextFileAddress , "/latest-commit.txt");

    FILE* latestCommitTxtFile = fopen(latestCommitTextFileAddress , "w");
    fprintf(latestCommitTxtFile , "%s" , newName);
    fclose(latestCommitTxtFile);

    // store the last commit in global lates-commit.txt
    char latestCommitTextFileAddress2[1000] = "";
    strcat(latestCommitTextFileAddress2 , theLatestCommit);

    FILE* latestCommitTxtFile2 = fopen(latestCommitTextFileAddress2 , "w");
    fprintf(latestCommitTxtFile2 , "%s" , newName);
    fclose(latestCommitTxtFile2);

    // configure the head address
    FILE* headTxt = fopen(headAddress , "w");
    fprintf(headTxt , "%s" , newName);
    fclose(headTxt);

    // set the commit as current commit
    FILE* currentCommitTxt = fopen(currentCommit , "w");
    fprintf(currentCommitTxt , "%s" , newName);
    fclose(currentCommitTxt);
}
void Log(char **argv)
{
    if (IsNiggitInitialized() == 0)
    {
        printf("BRUH niggit is not initialized :/\n");
        return;
    }
    if((argv[2] != NULL) && (!strcmp(argv[2] , "-n") && argv[3] == NULL))
    {
        printf("you didn't enter a number :/\n");
        return;
    }
    //normal log
    else if(argv[2] == NULL)
    {
        FILE *fp = fopen(globalCommitList , "r");
        char lines[1000][1000];
        int count = 0;
        while (fgets(lines[count] , sizeof(lines[count]) , fp) != NULL)
        {
            count++;
        }
        fclose(fp);

        for (int i = count - 1; i >= 0; i--)
        {
            char commitId[1000] , commitMessage[1000] , commitTime[1000] , commitBranch[1000] , commitUsername[1000] , commitFileCount[1000] , commitEmail[1000];
            sscanf(lines[i] , "%[^-]%*c%[^-]%*c%[^-]%*c%[^-]%*c%[^-]%*c%[^-]%*c%[^\n]%*c" , commitId , commitMessage , commitTime , commitBranch , commitUsername , commitEmail , commitFileCount );
            printf(GREEN"commit id : %s\n"RESET , commitId);
            printf("commit message : %s\n" , commitMessage);
            printf("commit time : %s\n" , commitTime);
            printf("commit branch : %s\n" , commitBranch);
            printf("commit username : %s\n" , commitUsername);
            printf("commit user email : %s\n" , commitEmail);
            printf("commit file count : %s\n" , commitFileCount);
            printf("\n");
        }
        return;
    }
    //log with number
    if(!strcmp(argv[2] , "-n") && (argv[3] != NULL))
    {
        int number = atoi(argv[3]);
        FILE *fp = fopen(globalCommitList , "r");
        char lines[1000][1000];
        int count = 0;
        while (fgets(lines[count] , sizeof(lines[count]) , fp) != NULL)
        {
            count++;
        }
        fclose(fp);

        for (int i = count - 1 ; i >= count - number ; i--)
        {
            if (i < 0)
            {
                break;
            }
            
            char commitId[1000] , commitMessage[1000] , commitTime[1000] , commitBranch[1000] , commitUsername[1000] , commitFileCount[1000] , commitEmail[1000];
            sscanf(lines[i] , "%[^-]%*c%[^-]%*c%[^-]%*c%[^-]%*c%[^-]%*c%[^-]%*c%[^\n]%*c" , commitId , commitMessage , commitTime , commitBranch , commitUsername , commitEmail , commitFileCount );
            printf(GREEN"commit id : %s\n"RESET , commitId);
            printf("commit message : %s\n" , commitMessage);
            printf("commit time : %s\n" , commitTime);
            printf("commit branch : %s\n" , commitBranch);
            printf("commit username : %s\n" , commitUsername);
            printf("commit user email : %s\n" , commitEmail);
            printf("commit file count : %s\n" , commitFileCount);
            printf("\n");
        }
    }
    //log with author name
    else if (!strcmp(argv[2] , "-author"))
    {
        if (argv[3] == NULL)
        {
            printf("you didn't enter an author name :/\n");
            return;
        }
        FILE *fp = fopen(globalCommitList , "r");
        char lines[1000][1000];
        int count = 0;
        int isAuthorFound = 0;
        while (fgets(lines[count] , sizeof(lines[count]) , fp) != NULL)
        {
            count++;
        }
        fclose(fp);

        for (int i = count - 1; i >= 0; i--)
        {
            char commitId[1000] , commitMessage[1000] , commitTime[1000] , commitBranch[1000] , commitUsername[1000] , commitFileCount[1000] , commitEmail[1000];
            sscanf(lines[i] , "%[^-]%*c%[^-]%*c%[^-]%*c%[^-]%*c%[^-]%*c%[^-]%*c%[^\n]%*c" , commitId , commitMessage , commitTime , commitBranch , commitUsername , commitEmail , commitFileCount );
            if (!strcmp(commitUsername , argv[3]))
            {
                isAuthorFound = 1;
                printf(GREEN"commit id : %s\n"RESET , commitId);
                printf("commit message : %s\n" , commitMessage);
                printf("commit time : %s\n" , commitTime);
                printf("commit branch : %s\n" , commitBranch);
                printf("commit username : %s\n" , commitUsername);
                printf("commit user email : %s\n" , commitEmail);
                printf("commit file count : %s\n" , commitFileCount);
                printf("\n");
            }
        }

        if (!isAuthorFound)
        {
            printf("we didn't find any commit with this author :/\n");
        }
    }
    //log with branch name
    else if (!strcmp(argv[2] , "-branch"))
    {
        //check if user entered branch name
        if (argv[3] == NULL)
        {
            printf("you didn't enter a branch name :/\n");
            return;
        }
        
        // loop through all of the commits and find the ones with the given branch name
        FILE *fp = fopen(globalCommitList , "r");
        char lines[1000][1000];
        int count = 0;
        int isBranchFound = 0;
        while (fgets(lines[count] , sizeof(lines[count]) , fp) != NULL)
        {
            count++;
        }
        fclose(fp);

        for (int i = count - 1; i >= 0; i--)
        {
            char commitId[1000] , commitMessage[1000] , commitTime[1000] , commitBranch[1000] , commitUsername[1000] , commitFileCount[1000] , commitEmail[1000];
            sscanf(lines[i] , "%[^-]%*c%[^-]%*c%[^-]%*c%[^-]%*c%[^-]%*c%[^-]%*c%[^\n]%*c" , commitId , commitMessage , commitTime , commitBranch , commitUsername , commitEmail , commitFileCount );
            if (!strcmp(commitBranch , argv[3]))
            {
                isBranchFound = 1;
                printf(GREEN"commit id : %s\n"RESET , commitId);
                printf("commit message : %s\n" , commitMessage);
                printf("commit time : %s\n" , commitTime);
                printf("commit branch : %s\n" , commitBranch);
                printf("commit username : %s\n" , commitUsername);
                printf("commit user email : %s\n" , commitEmail);
                printf("commit file count : %s\n" , commitFileCount);
                printf("\n");
            }
        }

        if (!isBranchFound)
        {
            printf("we didn't find any commit with this branch :/\n");
        }
    }
    //log with since
    else if (!strcmp(argv[2] , "-since"))
    {
        if (argv[3] == NULL)
        {
            printf("you didn't enter a date :/\n");
            return;
        }
        int sinceYear , sinceMonth , sinceDay , sinceHour , sinceMinute , sinceSecond;
        sscanf(argv[3] , "%d/%d/%d %d:%d:%d" , &sinceYear , &sinceMonth , &sinceDay , &sinceHour , &sinceMinute , &sinceSecond);

        FILE *fp = fopen(globalCommitList , "r");
        char lines[1000][1000];
        int count = 0;
        int isDateFound = 0;
        while (fgets(lines[count] , sizeof(lines[count]) , fp) != NULL)
        {
            count++;
        }
        fclose(fp);

        for (int i = count - 1; i >= 0; i--)
        {
            char commitId[1000] , commitMessage[1000] , commitTime[1000] , commitBranch[1000] , commitUsername[1000] , commitFileCount[1000] , commitEmail[1000];
            sscanf(lines[i] , "%[^-]%*c%[^-]%*c%[^-]%*c%[^-]%*c%[^-]%*c%[^-]%*c%[^\n]%*c" , commitId , commitMessage , commitTime , commitBranch , commitUsername , commitEmail , commitFileCount );
            int commitYear , commitMonth , commitDay , commitHour , commitMinute , commitSecond;
            sscanf(commitTime , "%d/%d/%d %d:%d:%d" , &commitYear , &commitMonth , &commitDay , &commitHour , &commitMinute , &commitSecond);

            if (commitYear < sinceYear)
            {
                continue;
            }
            else if (commitYear > sinceYear)
            {
                isDateFound = 1;
                printf(GREEN"commit id : %s\n"RESET , commitId);
                printf("commit message : %s\n" , commitMessage);
                printf("commit time : %s\n" , commitTime);
                printf("commit branch : %s\n" , commitBranch);
                printf("commit username : %s\n" , commitUsername);
                printf("commit user email : %s\n" , commitEmail);
                printf("commit file count : %s\n" , commitFileCount);
                printf("\n");
                continue;
            }
            else
            {
                if (commitMonth < sinceMonth)
                {
                    continue;
                }
                else if (commitMonth > sinceMonth)
                {
                    isDateFound = 1;
                    printf(GREEN"commit id : %s\n"RESET , commitId);
                    printf("commit message : %s\n" , commitMessage);
                    printf("commit time : %s\n" , commitTime);
                    printf("commit branch : %s\n" , commitBranch);
                    printf("commit username : %s\n" , commitUsername);
                    printf("commit file count : %s\n" , commitFileCount);
                    printf("\n");
                    continue;
                }
                else
                {
                    if (commitDay < sinceDay)
                    {
                        continue;
                    }
                    else if (commitDay > sinceDay)
                    {
                        isDateFound = 1;
                        printf(GREEN"commit id : %s\n"RESET , commitId);
                        printf("commit message : %s\n" , commitMessage);
                        printf("commit time : %s\n" , commitTime);
                        printf("commit branch : %s\n" , commitBranch);
                        printf("commit username : %s\n" , commitUsername);
                        printf("commit file count : %s\n" , commitFileCount);
                        printf("\n");
                        continue;
                    }
                    else
                    {
                        if (commitHour < sinceHour)
                        {
                            continue;
                        }
                        else if (commitHour > sinceHour)
                        {
                            isDateFound = 1;
                            printf(GREEN"commit id : %s\n"RESET , commitId);
                            printf("commit message : %s\n" , commitMessage);
                            printf("commit time : %s\n" , commitTime);
                            printf("commit branch : %s\n" , commitBranch);
                            printf("commit username : %s\n" , commitUsername);
                            printf("commit file count : %s\n" , commitFileCount);
                            printf("\n");
                            continue;
                        }
                        else
                        {
                            if (commitMinute < sinceMinute)
                            {
                                continue;
                            }
                            else if (commitMinute > sinceMinute)
                            {
                                isDateFound = 1;
                                printf(GREEN"commit id : %s\n"RESET , commitId);
                                printf("commit message : %s\n" , commitMessage);
                                printf("commit time : %s\n" , commitTime);
                                printf("commit branch : %s\n" , commitBranch);
                                printf("commit username : %s\n" , commitUsername);
                                printf("commit file count : %s\n" , commitFileCount);
                                printf("\n");
                                continue;
                            }
                            else
                            {
                                if (commitSecond < sinceSecond)
                                {
                                    continue;
                                }
                                else
                                {
                                    isDateFound = 1;
                                    printf(GREEN"commit id : %s\n"RESET , commitId);
                                    printf("commit message : %s\n" , commitMessage);
                                    printf("commit time : %s\n" , commitTime);
                                    printf("commit branch : %s\n" , commitBranch);
                                    printf("commit username : %s\n" , commitUsername);
                                    printf("commit file count : %s\n" , commitFileCount);
                                    printf("\n");
                                    continue;
                                }
                            }
                        }
                    }
                }
            }
        }    
        if (isDateFound == 0)
        {
            printf("we didn't find any commit since this date :/\n");
        }
    }
    //log with until
    else if (!strcmp(argv[2] , "-before"))
    {
        if (argv[3] == NULL)
        {
            printf("you didn't enter a date :/\n");
            return;
        }
        int sinceYear , sinceMonth , sinceDay , sinceHour , sinceMinute , sinceSecond;
        sscanf(argv[3] , "%d/%d/%d %d:%d:%d" , &sinceYear , &sinceMonth , &sinceDay , &sinceHour , &sinceMinute , &sinceSecond);

        FILE *fp = fopen(globalCommitList , "r");
        char lines[1000][1000];
        int count = 0;
        int isDateFound = 0;
        while (fgets(lines[count] , sizeof(lines[count]) , fp) != NULL)
        {
            count++;
        }
        fclose(fp);

        for (int i = count - 1; i >= 0; i--)
        {
            char commitId[1000] , commitMessage[1000] , commitTime[1000] , commitBranch[1000] , commitUsername[1000] , commitFileCount[1000] , commitEmail[1000];
            sscanf(lines[i] , "%[^-]%*c%[^-]%*c%[^-]%*c%[^-]%*c%[^-]%*c%[^-]%*c%[^\n]%*c" , commitId , commitMessage , commitTime , commitBranch , commitUsername , commitEmail , commitFileCount );
            int commitYear , commitMonth , commitDay , commitHour , commitMinute , commitSecond;
            sscanf(commitTime , "%d/%d/%d %d:%d:%d" , &commitYear , &commitMonth , &commitDay , &commitHour , &commitMinute , &commitSecond);

            if (commitYear > sinceYear)
            {
                continue;
            }
            else if (commitYear < sinceYear)
            {
                isDateFound = 1;
                printf(GREEN"commit id : %s\n"RESET , commitId);
                printf("commit message : %s\n" , commitMessage);
                printf("commit time : %s\n" , commitTime);
                printf("commit branch : %s\n" , commitBranch);
                printf("commit username : %s\n" , commitUsername);
                printf("commit user email : %s\n" , commitEmail);
                printf("commit file count : %s\n" , commitFileCount);
                printf("\n");
                continue;
            }
            else
            {
                if (commitMonth > sinceMonth)
                {
                    continue;
                }
                else if (commitMonth < sinceMonth)
                {
                    isDateFound = 1;
                    printf(GREEN"commit id : %s\n"RESET , commitId);
                    printf("commit message : %s\n" , commitMessage);
                    printf("commit time : %s\n" , commitTime);
                    printf("commit branch : %s\n" , commitBranch);
                    printf("commit username : %s\n" , commitUsername);
                    printf("commit file count : %s\n" , commitFileCount);
                    printf("\n");
                    continue;
                }
                else
                {
                    if (commitDay > sinceDay)
                    {
                        continue;
                    }
                    else if (commitDay < sinceDay)
                    {
                        isDateFound = 1;
                        printf(GREEN"commit id : %s\n"RESET , commitId);
                        printf("commit message : %s\n" , commitMessage);
                        printf("commit time : %s\n" , commitTime);
                        printf("commit branch : %s\n" , commitBranch);
                        printf("commit username : %s\n" , commitUsername);
                        printf("commit file count : %s\n" , commitFileCount);
                        printf("\n");
                        continue;
                    }
                    else
                    {
                        if (commitHour > sinceHour)
                        {
                            continue;
                        }
                        else if (commitHour < sinceHour)
                        {
                            isDateFound = 1;
                            printf(GREEN"commit id : %s\n"RESET , commitId);
                            printf("commit message : %s\n" , commitMessage);
                            printf("commit time : %s\n" , commitTime);
                            printf("commit branch : %s\n" , commitBranch);
                            printf("commit username : %s\n" , commitUsername);
                            printf("commit file count : %s\n" , commitFileCount);
                            printf("\n");
                            continue;
                        }
                        else
                        {
                            if (commitMinute > sinceMinute)
                            {
                                continue;
                            }
                            else if (commitMinute < sinceMinute)
                            {
                                isDateFound = 1;
                                printf(GREEN"commit id : %s\n"RESET , commitId);
                                printf("commit message : %s\n" , commitMessage);
                                printf("commit time : %s\n" , commitTime);
                                printf("commit branch : %s\n" , commitBranch);
                                printf("commit username : %s\n" , commitUsername);
                                printf("commit file count : %s\n" , commitFileCount);
                                printf("\n");
                                continue;
                            }
                            else
                            {
                                if (commitSecond > sinceSecond)
                                {
                                    continue;
                                }
                                else
                                {
                                    isDateFound = 1;
                                    printf(GREEN"commit id : %s\n"RESET , commitId);
                                    printf("commit message : %s\n" , commitMessage);
                                    printf("commit time : %s\n" , commitTime);
                                    printf("commit branch : %s\n" , commitBranch);
                                    printf("commit username : %s\n" , commitUsername);
                                    printf("commit file count : %s\n" , commitFileCount);
                                    printf("\n");
                                    continue;
                                }
                            }
                        }
                    }
                }
            }
        }    
        if (isDateFound == 0)
        {
            printf("we didn't find any commit before this date :/\n");
        }
    }
    //log with a word to search for
    else if (!strcmp(argv[2] , "-search"))
    {
        if (argv[2] == NULL)
        {
            printf("you didn't enter a word to search for :/\n");
            return;
        }
        
        //search with and without wildcard
        
        FILE *fp = fopen(globalCommitList , "r");
        char lines[1000][1000];
        int count = 0;
        int isWordFound = 0;
        while (fgets(lines[count] , sizeof(lines[count]) , fp) != NULL)
        {
            count++;
        }
        fclose(fp);
        for (int i = count - 1; i >= 0; i--)
        {
            char commitId[1000] , commitMessage[1000] , commitTime[1000] , commitBranch[1000] , commitUsername[1000] , commitFileCount[1000] , commitEmail[1000];
            sscanf(lines[i] , "%[^-]%*c%[^-]%*c%[^-]%*c%[^-]%*c%[^-]%*c%[^-]%*c%[^\n]%*c" , commitId , commitMessage , commitTime , commitBranch , commitUsername , commitEmail , commitFileCount );
            int indForArgv = 3;
            while (argv[indForArgv] != NULL)
            {
                if (strstr(argv[indForArgv] , "*"))
                {
                    char tmpArgv[1000] = "";
                    strcat(tmpArgv , argv[indForArgv]);
                    const char s[2] = "*";
                    char *token;
                    token = strtok(tmpArgv, s);
                    int hasAllTheTokens = 1;
                    while(token != NULL) 
                    {
                        if (!strstr(commitMessage , token))
                        {
                            hasAllTheTokens = 0;
                            break;
                        }       
                        token = strtok(NULL, s);
                    }
                    if (hasAllTheTokens)
                    {
                        isWordFound = 1;
                        printf(GREEN"commit id : %s\n"RESET , commitId);
                        printf("commit message : %s\n" , commitMessage);
                        printf("commit time : %s\n" , commitTime);
                        printf("commit branch : %s\n" , commitBranch);
                        printf("commit username : %s\n" , commitUsername);
                        printf("commit user email : %s\n" , commitEmail);
                        printf("commit file count : %s\n" , commitFileCount);
                        printf("\n");
                        break;
                    }
                } 
                else if (strstr(commitMessage , argv[indForArgv]) != NULL)
                {
                    isWordFound = 1;
                    printf(GREEN"commit id : %s\n"RESET , commitId);
                    printf("commit message : %s\n" , commitMessage);
                    printf("commit time : %s\n" , commitTime);
                    printf("commit branch : %s\n" , commitBranch);
                    printf("commit username : %s\n" , commitUsername);
                    printf("commit user email : %s\n" , commitEmail);
                    printf("commit file count : %s\n" , commitFileCount);
                    printf("\n");
                    break;
                }
                indForArgv++;
            }
        }
        if (!isWordFound)
        {
            printf("we didn't find any commit with this(these) word(s) :/\n");
        }         
    }
    else
    {
        printf("wrong command for log!!!\n");
        return;
    }
}
int Alias(char **argv)
{
    FILE* localAliasFile = fopen(localAliasAddress , "r");
    FILE* globalAliasFile = fopen(globalAliasAddress , "r");

    if (globalAliasFile == NULL && localAliasFile == NULL)
    {
        return;
    }
    
    if (globalAliasFile != NULL)
    {
        char line[1000];
        while (fgets(line , sizeof(line) , globalAliasFile) != NULL)
        {
            char alias[1000] , command[1000];
            sscanf(line , "%[^-]%*c%[^\n]%*c" , alias , command);
            if (!strcmp(argv[1] , alias))
            {
                system(command);
                return 1;
            }
        }
        fclose(globalAliasFile);
    }
    if (localAliasFile != NULL)
    {
        char line[1000];
        while (fgets(line , sizeof(line) , localAliasFile) != NULL)
        {
            char alias[1000] , command[1000];
            sscanf(line , "%[^-]%*c%[^\n]%*c" , alias , command);
            if (!strcmp(argv[1] , alias))
            {
                system(command);
                return 1;
            }
        }
        fclose(localAliasFile);
    }
    return 0;
}
void CheckOut(char **argv)
{
    //checkout commit
    if (strstr(argv[2] , "#") != NULL)
    {
        //check out to a commit 
        char commitId[1000] = "";
        strcat(commitId , argv[2]);

        // finding the name of branch of commit
        char branchOfCommit[1000] = "";
        char branchOfCommitAddress[1000] = "";
        FILE* globalCommitListTxtFile = fopen(globalCommitList , "r");
        char gNames[1000];
        int isCommitFound = 0;
        while (fgets(gNames , sizeof(gNames) , globalCommitListTxtFile))
        {
            char id[1000] , message[1000] , time[1000] , branch[1000] , username[1000] , fileCount[1000] , email[1000];
            sscanf(gNames , "%[^-]%*c%[^-]%*c%[^-]%*c%[^-]%*c%[^-]%*c%[^-]%*c%[^\n]%*c" , id , message , time , branch , username , email , fileCount  );
            if (!strcmp(commitId , id))
            {
                strcat(branchOfCommit , branch);
                isCommitFound = 1;
                break;
            }
        }

        if (!isCommitFound)
        {
            printf("BRUH this commit doesn't exists :/\n");
            return;
        }

        // finding the address of the root of the branch
        FILE* branchesTxtFile = fopen(branchesTextFile , "r");
        char bNames[1000];
        int isBranchFound = 0;
        while (fgets(bNames , sizeof(bNames) , branchesTxtFile))
        {
            char name[1000] , address[1000];
            sscanf(bNames , "%[^-]%*c%[^\n]%*c" , name , address );
            if (!strcmp(branchOfCommit , name))
            {
                strcat(branchOfCommitAddress , address);
                isBranchFound = 1;
                break;
            }
        }

        if (!isBranchFound)
        {
            printf("BRUH this branch doesn't exists :/\n");
            return;
        }

        // finding the address of the commit
        char commitAddress[1000] = "";
        strcat(commitAddress , branchOfCommitAddress);
        strcat(commitAddress , "/.commits/");
        strcat(commitAddress , commitId);

        // delete every thing from root execpt .niggit and .niggit-settings
        char rootAddress[1000] = "";
        FILE* temp = popen("pwd" , "r");
        fgets(rootAddress , sizeof(rootAddress) , temp);

        char commandForDelete[1000] = "find ";
        strcat(commandForDelete , rootAddress);
        strcat(commandForDelete , " 2> .niggit/error.log");

        FILE* tempForDelete = popen(commandForDelete , "r");
        char line[1000];
        while (fgets(line , sizeof(line) , tempForDelete) != NULL)
        {
            if (!strcmp(line , rootAddress))
            {
                continue;
            }
            if (strstr(line , ".niggit") == NULL)
            {
                line[strlen(line) - 1] = '\0';
                char commandForDelete2[1000] = "rm -r \"";
                strcat(commandForDelete2 , line);
                strcat(commandForDelete2 , "\" 2> .niggit/error.log");
                system(commandForDelete2);
            }
        }

        // copy every file exept the branch folder into the root next to niggit folder
        char commandForFind[1000] = "find \"";
        strcat(commandForFind, commitAddress);
        strcat(commandForFind , "\"");
        strcat(commandForFind , " -maxdepth 1 2> .niggit/error.log");

        rootAddress[strlen(rootAddress) - 1] = '\0';
        FILE* tempForFind = popen(commandForFind , "r");
        char line2[1000];
        while (fgets(line2 , sizeof(line2) , tempForFind) != NULL)
        {
            line2[strlen(line2) - 1] = '\0';
            if (!strcmp(line2 , commitAddress))
            {
                continue;
            }
            
            if (strstr(line2 + strlen(commitAddress) - 1 , "branch-") == NULL)
            {
                char commandForCopy[1000] = "cp -r \"";
                strcat(commandForCopy , line2);
                strcat(commandForCopy , "\" \"");
                strcat(commandForCopy , rootAddress);
                strcat(commandForCopy , "/\"");
                strcat(commandForCopy , " 2> .niggit/error.log");
                system(commandForCopy);
            }
        }

        // make the commit as checkout dir and disable commiting UNDONE

        FILE *fp10 = fopen(canCommit , "w");
        fprintf(fp10 , "%d" , 0);
        fclose(fp10);

        // make the commit as current commit 
        FILE* currentCommitTxt = fopen(currentCommit , "w");
        fprintf(currentCommitTxt , "%s" , commitAddress);
        fclose(currentCommitTxt);

        //print successful
        printf("you just checked out to a commit !\n");
    }
    //checkout HEAD
    else if (!strcmp(argv[2] , "HEAD"))
    {
        // finding the address of the head
        char commitAddress[1000] = "";
        
        FILE* head = fopen(headAddress , "r");
        fgets(commitAddress , sizeof(commitAddress) , head);
        fclose(head);

        if (commitAddress == NULL)
        {
            printf("You don't have head:/\n");
            return;
        }

        if (commitAddress[strlen(commitAddress) - 1] == '\n')
        {
            commitAddress[strlen(commitAddress) - 1] = '\0';
        }
        
        // delete every thing from root execpt .niggit and .niggit-settings
        char rootAddress[1000] = "";
        FILE* temp = popen("pwd" , "r");
        fgets(rootAddress , sizeof(rootAddress) , temp);

        char commandForDelete[1000] = "find ";
        strcat(commandForDelete , rootAddress);
        strcat(commandForDelete , " 2> .niggit/error.log");

        FILE* tempForDelete = popen(commandForDelete , "r");
        char line[1000];
        while (fgets(line , sizeof(line) , tempForDelete) != NULL)
        {
            if (!strcmp(line , rootAddress))
            {
                continue;
            }
            if (strstr(line , ".niggit") == NULL)
            {
                line[strlen(line) - 1] = '\0';
                char commandForDelete2[1000] = "rm -r \"";
                strcat(commandForDelete2 , line);
                strcat(commandForDelete2 , "\"");
                system(commandForDelete2);
            }
        }

        // copy every file exept the branch folder into the root next to niggit folder
        char commandForFind[1000] = "find \"";
        strcat(commandForFind, commitAddress);
        strcat(commandForFind , "\"");
        strcat(commandForFind , " -maxdepth 1 2> .niggit/error.log");

        rootAddress[strlen(rootAddress) - 1] = '\0';
        FILE* tempForFind = popen(commandForFind , "r");
        char line2[1000];
        while (fgets(line2 , sizeof(line2) , tempForFind) != NULL)
        {
            line2[strlen(line2) - 1] = '\0';
            if (!strcmp(line2 , commitAddress))
            {
                continue;
            }
            
            if (strstr(line2 + strlen(commitAddress) - 1 , "branch-") == NULL)
            {
                char commandForCopy[1000] = "cp -r \"";
                strcat(commandForCopy , line2);
                strcat(commandForCopy , "\" \"");
                strcat(commandForCopy , rootAddress);
                strcat(commandForCopy , "/\"");
                strcat(commandForCopy , " 2> .niggit/error.log");
                system(commandForCopy);
            }
        }

        // make the head as checkout dir and enable commiting UNDONE

        FILE *fp10 = fopen(canCommit , "w");
        fprintf(fp10 , "%d" , 1);
        fclose(fp10);

        // put the head address as current commit
        FILE* currentCommitTxt = fopen(currentCommit , "w");
        fprintf(currentCommitTxt , "%s" , commitAddress);
        fclose(currentCommitTxt);

        //print successful
        printf("you just checked out to HEAD !\n");
    }
    //checkout HEAD-n n commits before head UNDONE
    else if (strstr(argv[2] , "HEAD-"))
    {
        // finding how many commits should we go back from the head
        int n = atoi(argv[2] + 5);

        // finding the address of the head
        char addressOfTheHead[1000] = "";
        FILE* head = fopen(headAddress , "r");
        fgets(addressOfTheHead , sizeof(addressOfTheHead) , head);
        fclose(head);
        if (addressOfTheHead == NULL)
        {
            printf("You don't have head:/\n");
            return;
        }   
        if (addressOfTheHead[strlen(addressOfTheHead) - 1] == '\n')
        {
            addressOfTheHead[strlen(addressOfTheHead) - 1] = '\0';
        }

        // finding the commit list of the branch of the head
        char addressOfCommitList[1000] = "";
        char addressOfRootOfTheBranch[1000] = "";
        int slashCount = 0;
        for (size_t i = strlen(addressOfTheHead) - 1; i >= 0 ; i--)
        {
            if (addressOfTheHead[i] == '/')
            {
                slashCount++;
            }
            if (slashCount == 2)
            {
                addressOfTheHead[i] = '\0';
                break;
            }      
        }
        
        strcat(addressOfCommitList , addressOfTheHead);
        strcat(addressOfCommitList , "/commit-list.txt");

        // finding the address of the commit

        FILE* commitList = fopen(addressOfCommitList , "r");
        char line[1000];
        int count = 0;
        while (fgets(line , sizeof(line) , commitList) != NULL)
        {
            count++;
        }
        fclose(commitList);

        if (n > count)
        {
            printf("you don't have that many commits :/\n");
            return;
        }

        char commitId[1000];
        commitList = fopen(addressOfCommitList , "r");
        for (size_t i = 0; i < n; i++)
        {
            char commitId1[1000] , commitMessage[1000] , commitTime[1000] , commitBranch[1000] , commitUsername[1000] , commitFileCount[1000] , commitEmail[1000];
            fscanf(commitList , "%[^-]%*c%[^-]%*c%[^-]%*c%[^-]%*c%[^-]%*c%[^-]%*c%[^\n]%*c" , commitId1 , commitMessage , commitTime , commitBranch , commitUsername , commitEmail , commitFileCount );
            strcpy(commitId , commitId1);
        }
        fclose(commitList);

        if (commitId[strlen(commitId) - 1] == '\n')
        {
            commitId[strlen(commitId) - 1] = '\0';
        }

        char commitAddress[1000] = "";
        strcat(commitAddress , addressOfTheHead);
        strcat(commitAddress , "/.commits/");
        strcat(commitAddress , commitId);

        // delete every thing from root execpt .niggit and .niggit-settings
        char rootAddress[1000] = "";
        FILE* temp = popen("pwd" , "r");
        fgets(rootAddress , sizeof(rootAddress) , temp);

        char commandForDelete[1000] = "find ";
        strcat(commandForDelete , rootAddress);
        strcat(commandForDelete , " 2> .niggit/error.log");

        FILE* tempForDelete = popen(commandForDelete , "r");
        char line2[1000];
        while (fgets(line2 , sizeof(line2) , tempForDelete) != NULL)
        {
            if (!strcmp(line2 , rootAddress))
            {
                continue;
            }
            if (strstr(line2 , ".niggit") == NULL)
            {
                line2[strlen(line2) - 1] = '\0';
                char commandForDelete2[1000] = "rm -r \"";
                strcat(commandForDelete2 , line2);
                strcat(commandForDelete2 , "\"");
                system(commandForDelete2);
            }
        }

        // copy every file exept the branch folder into the root next to niggit folder

        char commandForFind[1000] = "find \"";
        strcat(commandForFind, commitAddress);
        strcat(commandForFind , "\"");
        strcat(commandForFind , " -maxdepth 1 2> .niggit/error.log");

        rootAddress[strlen(rootAddress) - 1] = '\0';
        FILE* tempForFind = popen(commandForFind , "r");
        char line3[1000];
        while (fgets(line3 , sizeof(line3) , tempForFind) != NULL)
        {
            line3[strlen(line3) - 1] = '\0';
            if (!strcmp(line3 , commitAddress))
            {
                continue;
            }
            
            if (strstr(line3 + strlen(commitAddress) - 1 , "branch-") == NULL)
            {
                char commandForCopy[1000] = "cp -r \"";
                strcat(commandForCopy , line3);
                strcat(commandForCopy , "\" \"");
                strcat(commandForCopy , rootAddress);
                strcat(commandForCopy , "/\"");
                strcat(commandForCopy , " 2> .niggit/error.log");
                system(commandForCopy);
            }
        }        

        //disable commiting
        FILE *fp10 = fopen(canCommit , "w");
        fprintf(fp10 , "%d" , 0);
        fclose(fp10);

        //put the address as current commit
        FILE* currentCommitTxt = fopen(currentCommit , "w");
        fprintf(currentCommitTxt , "%s" , commitAddress);
        fclose(currentCommitTxt);

        //print success
        printf("you just checked out to %d commits before HEAD (commiting is disabled) !\n" , n);
    }
    //checkout branch
    else
    {
        //storing the name of the branch we want
        char branchName[1000] = "";
        strcat(branchName , argv[2]);

        // finding the address of the root of the branch and check if the branch exists
        char branchAddress[1000] = "";
        FILE* branchesTxtFile = fopen(branchesTextFile , "r");
        char bNames[1000];
        int isBranchFound = 0;
        while (fgets(bNames , sizeof(bNames) , branchesTxtFile))
        {
            char name[1000] , address[1000];
            sscanf(bNames , "%[^-]%*c%[^\n]%*c" , name , address );
            if (!strcmp(branchName , name))
            {
                strcat(branchAddress , address);
                isBranchFound = 1;
                break;
            }
        }

        if (!isBranchFound)
        {
            printf("BRUH this branch doesn't exists :/\n");
            return;
        }

        // delete every thing from root execpt .niggit and .niggit-settings
        char rootAddress[1000] = "";
        FILE* temp = popen("pwd" , "r");
        fgets(rootAddress , sizeof(rootAddress) , temp);
        
        char commandForDelete[1000] = "find ";
        strcat(commandForDelete , rootAddress);
        strcat(commandForDelete , " 2> .niggit/error.log");

        FILE* tempForDelete = popen(commandForDelete , "r");
        char line[1000];
        while (fgets(line , sizeof(line) , tempForDelete) != NULL)
        {
            if (!strcmp(line , rootAddress))
            {
                continue;
            }
            if (strstr(line , ".niggit") == NULL)
            {
                line[strlen(line) - 1] = '\0';
                char commandForDelete2[1000] = "rm -r \"";
                strcat(commandForDelete2 , line);
                strcat(commandForDelete2 , "\"");
                strcat(commandForDelete2 , " 2> .niggit/error.log");
                system(commandForDelete2);
            }
        }
        
        //getting the latest commit of the branch
        char latestCommitOfTheBranchAddress[1000] = "";
        char txtAddress[1000] = "";
        strcat(txtAddress , branchAddress);
        strcat(txtAddress , "/latest-commit.txt");
        FILE* latestCommitsTxt = fopen(txtAddress , "r");
        fgets(latestCommitOfTheBranchAddress , sizeof(latestCommitOfTheBranchAddress) , latestCommitsTxt);
        fclose(latestCommitsTxt);
        // make the parent as the checkout dir
        if (!strcmp(latestCommitOfTheBranchAddress , "\n") || !strcmp(latestCommitOfTheBranchAddress , ""))
        {
            char parentAddress[1000] = "";
            strcat(parentAddress , branchAddress);
            for (size_t i = strlen(parentAddress) - 1; i >= 0; i--)
            {
                if (parentAddress[i] == '/')
                {
                    parentAddress[i] = '\0';
                    break;
                }
            }

            // copy every file exept the branch folder into the root next to niggit folder
            char commandForFind[1000] = "find \"";
            strcat(commandForFind, parentAddress);
            strcat(commandForFind , "\"");
            strcat(commandForFind , " -maxdepth 1 2> .niggit/error.log");
            
            rootAddress[strlen(rootAddress) - 1] = '\0';
            FILE* tempForFind = popen(commandForFind , "r");
            char line[1000];
            while (fgets(line , sizeof(line) , tempForFind) != NULL)
            {
                line[strlen(line) - 1] = '\0';
                if (!strcmp(line , parentAddress))
                {
                    continue;
                }
                
                if (strstr(line + strlen(parentAddress) - 1 , "branch-") == NULL)
                {
                    char commandForCopy[1000] = "cp -r \"";
                    strcat(commandForCopy , line);
                    strcat(commandForCopy , "\" \"");
                    strcat(commandForCopy , rootAddress);
                    strcat(commandForCopy , "/\"");
                    strcat(commandForCopy , " 2> .niggit/error.log");
                    system(commandForCopy);
                }
            }
        }
        // make the latest commit as checkout dir
        else
        {
            char parentAddress[1000] = "";
            strcat(parentAddress , latestCommitOfTheBranchAddress);

            // copy every file exept the branch folder into the root next to niggit folder
            char commandForFind[1000] = "find \"";
            strcat(commandForFind, parentAddress);
            strcat(commandForFind , "\"");
            strcat(commandForFind , " -maxdepth 1 2> .niggit/error.log");
            
            rootAddress[strlen(rootAddress) - 1] = '\0';
            FILE* tempForFind = popen(commandForFind , "r");
            char line[1000];
            while (fgets(line , sizeof(line) , tempForFind) != NULL)
            {
                line[strlen(line) - 1] = '\0';
                if (!strcmp(line , parentAddress))
                {
                    continue;
                }
                
                if (strstr(line + strlen(parentAddress) - 1 , "branch-") == NULL)
                {
                    char commandForCopy[1000] = "cp -r \"";
                    strcat(commandForCopy , line);
                    strcat(commandForCopy , "\" \"");
                    strcat(commandForCopy , rootAddress);
                    strcat(commandForCopy , "/\"");
                    strcat(commandForCopy , " 2> .niggit/error.log");
                    system(commandForCopy);
                }
            }
        }

        // change the current branch
        FILE* currentBranchTxt = fopen(currentBranchName , "w");
        fprintf(currentBranchTxt , "%s" , branchName);
        fclose(currentBranchTxt);

        // change the current branch address
        FILE* currentBranchAddressTxt = fopen(currentBranchTextFile , "w");
        fprintf(currentBranchAddressTxt , "%s" , branchAddress);
        fclose(currentBranchAddressTxt);

        // attaching the head
        FILE *fp10 = fopen(canCommit , "w");
        fprintf(fp10 , "%d" , 1);
        fclose(fp10);

        //put the latest commit of the branch address as the head
        char newHeadAddress[1000] = "";
        FILE* latestCommitsTxt2 = fopen(txtAddress , "r");
        fgets(newHeadAddress , sizeof(newHeadAddress) , latestCommitsTxt2);
        fclose(latestCommitsTxt2);

        if (!strcmp(newHeadAddress , "") || !strcmp(newHeadAddress , "\n"))
        {
            char tmp[1000] = "";
            for (int i = 0; i < strlen(branchAddress) - strlen(argv[2]) - 8; i++)
            {
                char tmp2[2] = "";
                tmp2[0] = branchAddress[i];
                strcat(tmp , tmp2);
            }
            
            FILE* headTxt = fopen(headAddress , "w");
            fprintf(headTxt , "%s" , tmp);
            fclose(headTxt);
        }
        else
        {
            FILE* headTxt = fopen(headAddress , "w");
            fprintf(headTxt , "%s" , newHeadAddress);
            fclose(headTxt);
        }

        // put the last commit of the branch as current commit
        FILE* currentCommitTxt = fopen(currentCommit , "w");
        fprintf(currentCommitTxt , "%s" , newHeadAddress);
        fclose(currentCommitTxt);

        //print successful

        printf("you just checked out to %s branch !!!!\n" , branchName);
    }
}
void Merge(char **argv)
{

}
void Tag(char **argv)
{
    if (!IsNiggitInitialized())
    {
        printf("BRUH niggit is not initialized :/\n");
        return;
    }
    // prints all tags with alphabetic order in their names
    if (argv[2] == NULL)
    {
        FILE* tagsTxt = fopen(tagsTextFile , "r");
        char lines[1000][1000];
        int count = 0;
        while (fgets(lines[count] , sizeof(lines[count]) , tagsTxt) != NULL)
        {
            count++;
        }
        fclose(tagsTxt);
        for (int i = 0; i < count; i++)
        {
            for (int j = i + 1; j < count; j++)
            {
                char name1[1000] , name2[1000];
                char  address1[1000] , commitId1[1000] , message1[1000] , username1[1000] , email1[1000]; 
                char  address2[1000] , commitId2[1000] , message2[1000] , username2[1000] , email2[1000]; 
                sscanf(lines[i] , "%[^-]%*c%[^-]%*c%[^-]%*c%[^-]%*c%[^-]%*c%[^\n]%*c" , name1 , address1 , commitId1 , message1 , username1 , email1 );
                sscanf(lines[j] , "%[^-]%*c%[^-]%*c%[^-]%*c%[^-]%*c%[^-]%*c%[^\n]%*c" , name2 , address2 , commitId2 , message2 , username2 , email2);
                if (strcmp(name1 , name2) > 0)
                {
                    char tmp[1000] = "";
                    strcpy(tmp , lines[i]);
                    strcpy(lines[i] , lines[j]);
                    strcpy(lines[j] , tmp);
                }
            }
        }
        for (int i = 0; i < count; i++)
        {
            char name[1000] , address[1000] , commitId[1000] , message[1000] , username[1000] , email[1000]; 
            sscanf(lines[i] , "%[^-]%*c%[^-]%*c%[^-]%*c%[^-]%*c%[^-]%*c%[^\n]%*c" , name , address , commitId , message , username , email );
            printf(GREEN"tag name : %s\n"RESET , name);
            printf("tag commit id : %s\n" , address);
            printf("tag creation date : %s\n" , commitId);
            printf("tag message : %s\n" , message);
            printf("tager username : %s\n" , username);
            printf("tager user email : %s\n" , email);
            printf("\n");
        }
        return;
    }
    //show a tag with given tag name
    if (!strcmp(argv[2] , "show"))
    {
        FILE* tagsTxt = fopen(tagsTextFile , "r");
        char line[1000];
        while (fgets(line , sizeof(line) , tagsTxt) != NULL)
        {
            char name[1000] , address[1000] , commitId[1000] , message[1000] , username[1000] , email[1000]; 
            sscanf(line , "%[^-]%*c%[^-]%*c%[^-]%*c%[^-]%*c%[^-]%*c%[^\n]%*c" , name , address , commitId , message , username , email );
            
            if (!strcmp(name , argv[3]))
            {
                printf(GREEN"tag name : %s\n"RESET , name);
                printf("tag commit id : %s\n" , address);
                printf("tag creation date : %s\n" , commitId);
                printf("tag message : %s\n" , message);
                printf("tager username : %s\n" , username);
                printf("tager user email : %s\n" , email);
                printf("\n");
                break;
            }
        }
        fclose(tagsTxt);
        return;
    }
     
    // making a new temp tags list file
    FILE* tempTagsTxt = fopen(tempTagsTextFile , "a");
    
    //check which options are there in tag and store the index of argv in them
    int doesHaveMessage = -1 , doesHaveCommitId = -1 , doesHaveF = -1;
    int indForArgv = 2;
    while (argv[indForArgv] != NULL)
    {
        if (!strcmp(argv[indForArgv] , "-m"))
        {
            doesHaveMessage = indForArgv;
        }
        else if (!strcmp(argv[indForArgv] , "-c"))
        {
            doesHaveCommitId = indForArgv;
        }
        else if (!strcmp(argv[indForArgv] , "-f"))
        {
            doesHaveF = indForArgv;
        }
        indForArgv++;
    }
    
    //get the commit ID
    char commitId[1000] = "";
    if (doesHaveCommitId != -1)
    {
        strcat(commitId , argv[doesHaveCommitId + 1]);
    }
    else
    {
        //get the current commit
        char currentCommitAddress[1000] = "";
        FILE* currentCommitTxt = fopen(currentCommit , "r");
        fgets(currentCommitAddress , sizeof(currentCommitAddress) , currentCommitTxt);
        fclose(currentCommitTxt);

        if (currentCommitAddress[strlen(currentCommitAddress) - 1] == '\n')
        {
            currentCommitAddress[strlen(currentCommitAddress) - 1] = '\0';
        }

        if (!strcmp(currentCommitAddress , ""))
        {
            printf("you don't have any commit :/\n");
            return;
        }
        
        char commitId1[1000] = "";
        for (size_t i = strlen(currentCommitAddress) - 1; i >= 0; i--)
        {
            if (currentCommitAddress[i] == '/')
            {
                break;
            }
            char tmp[2] = "";
            tmp[0] = currentCommitAddress[i];
            strcat(commitId1 , tmp);
        }
        
        for (int i = strlen(commitId1) - 1; i >= 0; i--)
        {
            char tmp[2] = "";
            tmp[0] = commitId1[i];
            strcat(commitId , tmp);
        }

        if (commitId[strlen(commitId) - 1] == '\n')
        {
            commitId[strlen(commitId) - 1] = '\0';
        }
    }

    //get the message
    char message[1000] = "";
    if (doesHaveMessage != -1)
    {
        strcat(message , argv[doesHaveMessage + 1]);
    }
    else
    {
        strcat(message , "no message");
    }

    //get the tag name
    char tagName[1000] = "";
    strcat(tagName , argv[3]);

    //get local username and global username
    char localUserNameString[1000] = "";
    char globalUserNameString[1000] = "";
    
    FILE *fp3 = fopen(localUserName , "r");
    if (fp3 == NULL)
    {
        FILE *fp4 = fopen(globalUserName , "r");
        fgets(globalUserNameString , sizeof(globalUserNameString) , fp4);
        fclose(fp4);
    }
    else
    {
        fgets(localUserNameString , sizeof(localUserNameString) , fp3);
        fclose(fp3);
    }

    char commitUsername[1000] = "";
    if (localUserNameString[0] != '\0')
    {
        strcat(commitUsername , localUserNameString);
    }
    else if (globalUserNameString[0] != '\0')
    {
        strcat(commitUsername , globalUserNameString);
    }
    else
    {
        printf("BRUH you didn't set your username :/\n");
        return;
    }

    //get global and local user email
    char localUserEmailString[1000] = "";
    char globalUserEmailString[1000] = "";

    FILE *fp4 = fopen(localUserEmail , "r");
    if (fp4 == NULL)
    {
        FILE *fp5 = fopen(globalUserEmail , "r");
        fgets(globalUserEmailString , sizeof(globalUserEmailString) , fp5);
        fclose(fp5);
    }
    else
    {
        fgets(localUserEmailString , sizeof(localUserEmailString) , fp4);
        fclose(fp4);
    }

    char commitEmail[1000] = "";
    if (localUserEmailString[0] != '\0')
    {
        strcat(commitEmail , localUserEmailString);
    }
    else if (globalUserEmailString[0] != '\0')
    {
        strcat(commitEmail , globalUserEmailString);
    }
    else
    {
        printf("BRUH you didn't set your email :/\n");
        return;
    }

    //check if the tag exists if has f then delete it if not then print error
    FILE* tagsTxt = fopen(tagsTextFile , "a+");
    char line[1000];
    while (fgets(line , sizeof(line) , tagsTxt) != NULL)
    {
        char name[1000] , address[1000] , commitId1[1000] , message1[1000] , username[1000] , email[1000]; 
        sscanf(line , "%[^-]%*c%[^-]%*c%[^-]%*c%[^-]%*c%[^-]%*c%[^-]%*c%[^\n]%*c" , name , address , commitId1 , message1 , username , email);
        if (!strcmp(name , tagName))
        {
            if (doesHaveF == -1)
            {
                printf("a tag with this tag name already exists :/\n");
                remove(tempTagsTextFile);
                return;
            }
            continue;
        }
        fprintf(tempTagsTxt , "%s-%s-%s-%s-%s-%s" , name , address , commitId1 , message1 , username , email);
    }
    fclose(tagsTxt);
    
    //rename the temp tags list file to the tags list file
    remove(tagsTextFile);
    rename(tempTagsTextFile , tagsTextFile);
    
    //add the new tag to the temp tags list file
    tagsTxt = fopen(tagsTextFile , "a");
    fprintf(tagsTxt , "%s-%s-%s-%s-%s-%s\n" , tagName , commitId , GetTime() , message , commitUsername , commitEmail);
    fclose(tagsTxt);

    //print successful
    printf("you just tagged the current commit !\n");
}
void Grep(char **argv)
{
    if (!IsNiggitInitialized())
    {
        printf("BRUH niggit is not initialized :/\n");
        return;
    }
    //check for options
    int doesHavelineNumber = -1 , doesHaveCommitId = -1;
    int indForArgv = 2;
    while (argv[indForArgv] != NULL)
    {
        if (!strcmp(argv[indForArgv] , "-n"))
        {
            doesHavelineNumber = indForArgv;
        }
        else if (!strcmp(argv[indForArgv] , "-c"))
        {
            doesHaveCommitId = indForArgv;
        }
        indForArgv++;
    }

    //get the commit ID
    char commitId[1000] = "";
    if (doesHaveCommitId != -1)
    {
        strcat(commitId , argv[doesHaveCommitId + 1]);
    }

    // get the address of search
    char searchAddress[1000] = "";
    
    if (doesHaveCommitId != -1)
    {
        char branchOfCommit[1000] = "";
        char branchOfCommitAddress[1000] = "";
        FILE* globalCommitListTxtFile = fopen(globalCommitList , "r");
        char gNames[1000];
        int isCommitFound = 0;
        while (fgets(gNames , sizeof(gNames) , globalCommitListTxtFile))
        {
            char id[1000] , message[1000] , time[1000] , branch[1000] , username[1000] , fileCount[1000] , email[1000];
            sscanf(gNames , "%[^-]%*c%[^-]%*c%[^-]%*c%[^-]%*c%[^-]%*c%[^-]%*c%[^\n]%*c" , id , message , time , branch , username , email , fileCount  );
            if (!strcmp(commitId , id))
            {
                strcat(branchOfCommit , branch);
                isCommitFound = 1;
                break;
            }
        }

        if (!isCommitFound)
        {
            printf("BRUH this commit doesn't exists :/\n");
            return;
        }

        // finding the address of the root of the branch
        FILE* branchesTxtFile = fopen(branchesTextFile , "r");
        char bNames[1000];
        int isBranchFound = 0;
        while (fgets(bNames , sizeof(bNames) , branchesTxtFile))
        {
            char name[1000] , address[1000];
            sscanf(bNames , "%[^-]%*c%[^\n]%*c" , name , address );
            if (!strcmp(branchOfCommit , name))
            {
                strcat(branchOfCommitAddress , address);
                isBranchFound = 1;
                break;
            }
        }

        if (!isBranchFound)
        {
            printf("BRUH this branch doesn't exists :/\n");
            return;
        }

        // finding the address of the commit
        char commitAddress[1000] = "";
        strcat(commitAddress , branchOfCommitAddress);
        strcat(commitAddress , "/.commits/");
        strcat(commitAddress , commitId);

        // finding the address of the search
        strcat(searchAddress , commitAddress);
    }
    else
    {
        FILE* currentDir = popen("pwd" , "r");
        fgets(searchAddress , sizeof(searchAddress) , currentDir);
        searchAddress[strcspn(searchAddress , "\n")] = '\0';
        fclose(currentDir);
    }

    //get the search word
    char searchWord[1000] = "";
    strcat(searchWord , argv[5]);
 
    //tokenize the search word
    char tmpSearchWord[1000] = "";
    strcat(tmpSearchWord , argv[5]);
    char searchWordTokenized[100][1000] ;
    int count = 0;
    if (strstr(searchWord , "*") != NULL)
    {
        char* token = strtok(tmpSearchWord , "*");
        while (token != NULL)
        {
            token[strcspn(token , "\n")] = '\0';
            strcpy(searchWordTokenized[count] , token);
            count++;
            token = strtok(NULL , "*");
        }
    }

    //get the search file
    char searchFile[1000] = "";
    strcat(searchFile , argv[3]);

    //check if the file exists
    char commandForFind[1000] = "find \"";
    strcat(commandForFind , searchAddress);
    strcat(commandForFind , "\" -name \"");
    strcat(commandForFind , searchFile);
    strcat(commandForFind , "\" -type f 2> .niggit/error.log");

    FILE* tempForFind = popen(commandForFind , "r");
    char line[1000];
    int isWordFound = 0;
    while (fgets(line , sizeof(line) , tempForFind) != NULL)
    {
        line[strcspn(line , "\n")] = '\0';
        if ((doesHaveCommitId == -1) && (strstr(line , ".niggit") != NULL))
        {
            continue;
        }
        
        else if (strstr(line , searchAddress) != NULL)
        {
            //search in the file but the searchWord is wildcard
            if (strstr(searchWord , "*") != NULL)
            {
                FILE* file = fopen(line , "r");
                char line2[1000];
                int lineNumber = 1;
                while (fgets(line2 , sizeof(line2) , file) != NULL)
                {
                    int isWordFoundInThisLine = 1;
                    for (size_t i = 0; i < count; i++)
                    {
                        if (strstr(line2 , searchWordTokenized[i]) == NULL)
                        {
                            isWordFoundInThisLine = 0;
                            break;
                        }
                    }

                    if (isWordFoundInThisLine)
                    {
                        
                        int shouldPrint = 1;
                        char tempLine[1000] = "";
                        char theFuckingWord[1000] = "";
                        char tempLine2[1000] = "";
                        
                        const char s[2] = " ";
                        char *token;

                        token = strtok(line2, s);

                        while( token != NULL ) 
                        {
                            token[strcspn(token , "\n")] = '\0';
                            int isTokenSpecialWord = 1;
                            int indexToSearchAfter = 0;
                            if (searchWord[0] != '*')
                            {
                                if (searchWord[0] != token[0])
                                {
                                    isTokenSpecialWord = 0;
                                }
                            }
                            if (searchWord[strlen(searchWord) - 1] != '*')
                            {
                                if (searchWord[strlen(searchWord) - 1] != token[strlen(token) - 1])
                                {
                                    isTokenSpecialWord = 0;
                                }
                            }
                            
                            for (int i = 0; i < count; i++)
                            {
                                if (strstr(token + indexToSearchAfter , searchWordTokenized[i]) == NULL)
                                {
                                    isTokenSpecialWord = 0;
                                    break;
                                }
                                else
                                {
                                    indexToSearchAfter = strstr(token , searchWordTokenized[i]) - token + strlen(searchWordTokenized[i]);
                                }
                            }
                            
                            if (!isTokenSpecialWord)
                            {
                                strcat(tempLine , token);
                            }
                            else
                            {
                                strcat(tempLine , CYAN);
                                strcat(tempLine , token);
                                strcat(tempLine , RESET);
                                isWordFound = 1;
                                shouldPrint = 0;
                            }
                            strcat(tempLine , " ");

                            token = strtok(NULL, s);
                        }
                        
                        if (!shouldPrint)
                        {
                            if (doesHavelineNumber != -1)
                            {
                                printf("%d:" , lineNumber);
                            }
                            printf("%s\n" , tempLine);
                        }
                    }

                    lineNumber++;
                }
            }
            //search in the file
            else
            {
                FILE* file = fopen(line , "r");
                char line2[1000];
                int lineNumber = 1;
                while (fgets(line2 , sizeof(line2) , file) != NULL)
                {
                    if (strstr(line2 , searchWord) != NULL)
                    {
                        isWordFound = 1;
                        if (doesHavelineNumber != -1)
                        {
                            printf("%d:" , lineNumber);
                        }
                        char tmp[1000] = "";
                        int index = strstr(line2 , searchWord) - line2;
                        for (size_t i = 0; i < index; i++)
                        {
                            char tmp2[2] = "";
                            tmp2[0] = line2[i];
                            strcat(tmp , tmp2);
                        }
                        printf("%s" , tmp);
                        printf(CYAN"%s"RESET , searchWord);
                        strcpy(tmp , "");
                        for (size_t i = index + strlen(searchWord); i < strlen(line2); i++)
                        {
                            char tmp2[2] = "";
                            tmp2[0] = line2[i];
                            strcat(tmp , tmp2);
                        }
                        printf("%s" , tmp);
                    }
                    lineNumber++;
                }
            }
        }
    }
    if (!isWordFound)
    {
        printf("the word you searched for doesn't exist in the file :/\n");
    }

}
void ApplyHookOnFile(char fileAddress[1000] , char hookId[1000] , char fileTypes[1000] , char hookDescription[1000])
{
    //check if the file type is correct , the format_check hook runs in this
    if (1)
    {
        char fileType[1000] = "";
        for (size_t i = strlen(fileAddress) - 1; i >= 0; i--)
        {
            if (fileAddress[i] == '.')
            {
                break;
            }
            char tmp[2] = "";
            tmp[0] = fileAddress[i];
            strcat(fileType , tmp);
        }
        char tmp[1000] = "";
        for (int i = strlen(fileType) - 1; i >= 0; i--)
        {
            char tmp2[2] = "";
            tmp2[0] = fileType[i];
            strcat(tmp , tmp2);
        }
        strcpy(fileType , tmp);
        int isFileTypeCorrect = 0;
        char fileTypesTokenized[100][1000];
        int count = 0;
        char tmpFileTypes[1000] = "";
        strcat(tmpFileTypes , fileTypes);
        char* token = strtok(tmpFileTypes , " ");
        while (token != NULL)
        {
            token[strcspn(token , "\n")] = '\0';
            strcpy(fileTypesTokenized[count] , token);
            count++;
            token = strtok(NULL , " ");
        }
        char tmpFileType[1000] = ".";
        strcat(tmpFileType , fileType);
        for (size_t i = 0; i < count; i++)
        {
            if (!strcmp(tmpFileType , fileTypesTokenized[i]))
            {
                isFileTypeCorrect = 1;
                break;
            }
        }
        if(!strcmp(fileTypesTokenized[0] , "all"))
        {
            isFileTypeCorrect = 1;
        }
        if (!isFileTypeCorrect)
        {
            if (!strcmp(hookId , "format_check"))
            {
                printf(RED"FAILED : "RESET);
                printf("%s\n" , hookId);
                canCommitHook = 0;
                return;
            }
            else
            {
                printf(YELLOW"SKIPPED : "RESET);
                printf("%s\n" , hookId);
                return;
            }
        }
        if (!strcmp(hookId , "format_check"))
        {
            printf(GREEN"PASSED : "RESET);
            printf("%s\n" , hookId);
            return;
        }
    }
    //character limit hook
    if (!strcmp(hookId , "character_limit"))
    {
        //check if the file is empty
        FILE* file = fopen(fileAddress , "r");
        char line[1000];
        int isFileEmpty = 1;
        while (fgets(line , sizeof(line) , file) != NULL)
        {
            if (strcmp(line , "\n"))
            {
                isFileEmpty = 0;
                break;
            }
        }
        fclose(file);
        if (isFileEmpty)
        {
            printf(GREEN"PASSED : \n"RESET);
            printf("%s\n" , hookId);
            return;
        }

        //check if the file has more than 20000 characters
        file = fopen(fileAddress , "r");
        int isFileMoreThan20000Characters = 0;
        int count = 0;
        while (fgets(line , sizeof(line) , file) != NULL)
        {
            count += strlen(line);
            if (count > 20000)
            {
                isFileMoreThan20000Characters = 1;
                break;
            }
        }
        fclose(file);
        if (isFileMoreThan20000Characters)
        {
            printf(RED"FAILED : "RESET);
            printf("%s\n" , hookId);
            canCommitHook = 0;
            return;
        }
        else
        {
            printf(GREEN"PASSED : "RESET);
            printf("%s\n" , hookId);
            return;
        }
    }
    //file size check hook
    else if (!strcmp(hookId , "file_size_check"))
    {
        //check if the file is empty
        FILE* file = fopen(fileAddress , "r");
        char line[1000];
        int isFileEmpty = 1;
        while (fgets(line , sizeof(line) , file) != NULL)
        {
            if (strcmp(line , "\n"))
            {
                isFileEmpty = 0;
                break;
            }
        }
        fclose(file);
        if (isFileEmpty)
        {
            printf(GREEN"PASSED : \n"RESET);
            printf("%s\n" , hookId);
            return;
        }

        //check if files size is less than 5MB
        file = fopen(fileAddress , "r");
        int isFileSizeLessThan5MB = 1;
        int count = 0;
        while (fgets(line , sizeof(line) , file) != NULL)
        {
            count += strlen(line);
            if (count > 5000000)
            {
                isFileSizeLessThan5MB = 0;
                break;
            }
        }
        fclose(file);
        if (!isFileSizeLessThan5MB)
        {
            printf(RED"FAILED : "RESET);
            printf("%s\n" , hookId);
            canCommitHook = 0;
            return;
        }
        else
        {
            printf(GREEN"PASSED : "RESET);
            printf("%s\n" , hookId);
            return;
        }
    }
    // check if there is "TODO" phrase in the file
    else if (!strcmp(hookId , "todo_check"))
    {
        //check if the file is empty
        FILE* file = fopen(fileAddress , "r");
        char line[1000];
        int isFileEmpty = 1;
        while (fgets(line , sizeof(line) , file) != NULL)
        {
            if (strcmp(line , "\n"))
            {
                isFileEmpty = 0;
                break;
            }
        }
        fclose(file);
        if (isFileEmpty)
        {
            printf(GREEN"PASSED : \n"RESET);
            printf("%s\n" , hookId);
            return;
        }

        //check if the file has "TODO" phrase
        file = fopen(fileAddress , "r");
        int isFileHasTodoPhrase = 0;
        while (fgets(line , sizeof(line) , file) != NULL)
        {
            if (strstr(line , "TODO") != NULL)
            {
                isFileHasTodoPhrase = 1;
                break;
            }
        }
        fclose(file);
        if (isFileHasTodoPhrase)
        {
            printf(RED"FAILED : "RESET);
            printf("%s\n" , hookId);
            canCommitHook = 0;
            return;
        }
        else
        {
            printf(GREEN"PASSED : "RESET);
            printf("%s\n" , hookId);
            return;
        }
    }
    //for mp4 mp3 and wav checks that the duration wont be more than 10 minutes
    else if (!strcmp(hookId , "time_limit"))
    {
        //check if the file is empty
        FILE* file = fopen(fileAddress , "r");
        char line[1000];
        int isFileEmpty = 1;
        while (fgets(line , sizeof(line) , file) != NULL)
        {
            if (strcmp(line , "\n"))
            {
                isFileEmpty = 0;
                break;
            }
        }
        fclose(file);
        if (isFileEmpty)
        {
            printf(GREEN"PASSED : \n"RESET);
            printf("%s\n" , hookId);
            return;
        }

        //check if the file has more than 10 minutes duration
        file = fopen(fileAddress , "r");
        int isFileMoreThan10Minutes = 0;
        char commandForGettingSeconds[1000] = "ffprobe -i \"";
        strcat(commandForGettingSeconds , fileAddress);
        strcat(commandForGettingSeconds , "\" -show_entries format=duration -v quiet -of csv=\"p=0\" 2> .niggit/error.log");
        FILE* tempForGettingSeconds = popen(commandForGettingSeconds , "r");
        char seconds[1000];
        fgets(seconds , sizeof(seconds) , tempForGettingSeconds);
        fclose(tempForGettingSeconds);
        if (atof(seconds) > 600)
        {
            isFileMoreThan10Minutes = 1;
        }
        fclose(file);
        if (isFileMoreThan10Minutes)
        {
            printf(RED"FAILED : "RESET);
            printf("%s\n" , hookId);
            canCommitHook = 0;
            return;
        }
        else
        {
            printf(GREEN"PASSED : "RESET);
            printf("%s\n" , hookId);
            return;
        }
    }
    //checks if there is whitespace at the end of the file
    else if (!strcmp(hookId , "eof_blank_space"))
    {
        //check if the file is empty
        FILE* file = fopen(fileAddress , "r");
        char line[1000];
        int isFileEmpty = 1;
        while (fgets(line , sizeof(line) , file) != NULL)
        {
            if (strcmp(line , "\n"))
            {
                isFileEmpty = 0;
                break;
            }
        }
        fclose(file);
        if (isFileEmpty)
        {
            printf(GREEN"PASSED : \n"RESET);
            printf("%s\n" , hookId);
            return;
        }

        //check if the file has whitespace at the end
        file = fopen(fileAddress , "r");
        int isFileHasWhitespaceAtTheEnd = 1;
        while (fgets(line , sizeof(line) , file) != NULL){}
        fclose(file);
        for (size_t i = 0; i < strlen(line); i++)
        {
            if (line[i] != ' ' && line[i] != '\t' && line[i] != '\n')
            {
                isFileHasWhitespaceAtTheEnd = 0;
                break;
            }
        }
        
        if (isFileHasWhitespaceAtTheEnd)
        {
            printf(RED"FAILED : "RESET);
            printf("%s\n" , hookId);
            canCommitHook = 0;
            return;
        }
        else
        {
            printf(GREEN"PASSED : "RESET);
            printf("%s\n" , hookId);
            return;
        }
    }
    //checks for compiler error in c and cpp files
    else if (!strcmp(hookId , "static_error_check"))
    {
        //check if the file is empty
        FILE* file = fopen(fileAddress , "r");
        char line[1000];
        int isFileEmpty = 1;
        while (fgets(line , sizeof(line) , file) != NULL)
        {
            if (strcmp(line , "\n"))
            {
                isFileEmpty = 0;
                break;
            }
        }
        fclose(file);
        if (isFileEmpty)
        {
            printf(GREEN"PASSED : \n"RESET);
            printf("%s\n" , hookId);
            return;
        }

        //check if the file has compiler error
        file = fopen(fileAddress , "r");
        int isFileHasCompilerError = 0;
        char commandForCompilerError[1000] = "";
        if (strstr(fileAddress , ".c") != NULL)
        {
            strcat(commandForCompilerError , "gcc ");
            strcat(commandForCompilerError , fileAddress);
            strcat(commandForCompilerError , " 2> .niggit/configs/compiler_error.log");
        }
        else if (strstr(fileAddress , ".cpp") != NULL)
        {
            strcat(commandForCompilerError , "g++ ");
            strcat(commandForCompilerError , fileAddress);
            strcat(commandForCompilerError , " 2> .niggit/configs/compiler_error.log");
        }
        FILE* tempForCompilerError = popen(commandForCompilerError , "r");
        char error[1000];
        fgets(error , sizeof(error) , tempForCompilerError);
        fclose(tempForCompilerError);
        fclose(file);
        FILE* fp = fopen(".niggit/configs/compiler_error.log" , "r");
        char line2[1000];
        while (fgets(line2 , sizeof(line2) , fp) != NULL)
        {
            if (!strstr(line2 , "error") || !strstr(line2 , "terminated"))
            {
                isFileHasCompilerError = 1;
                break;
            }          
        }
        if (isFileHasCompilerError)
        {
            printf(RED"FAILED : "RESET);
            printf("%s\n" , hookId);
            canCommitHook = 0;
            return;
        }
        else
        {
            printf(GREEN"PASSED : "RESET);
            printf("%s\n" , hookId);
            return;
        }
    }
    //checks if braces open and close counts are equal
    else if (!strcmp(hookId , "balance_braces"))
    {
        //check if the file is empty
        FILE* file = fopen(fileAddress , "r");
        char line[1000];
        int isFileEmpty = 1;
        while (fgets(line , sizeof(line) , file) != NULL)
        {
            if (strcmp(line , "\n"))
            {
                isFileEmpty = 0;
                break;
            }
        }
        fclose(file);
        if (isFileEmpty)
        {
            printf(GREEN"PASSED : \n"RESET);
            printf("%s\n" , hookId);
            return;
        }

        //check if the file has equal open and close braces
        file = fopen(fileAddress , "r");
        int openBracesCount = 0;
        int closeBracesCount = 0;
        while (fgets(line , sizeof(line) , file) != NULL)
        {
            for (size_t i = 0; i < strlen(line); i++)
            {
                if (line[i] == '{')
                {
                    openBracesCount++;
                }
                else if (line[i] == '}')
                {
                    closeBracesCount++;
                }
            }
        }
        fclose(file);
        if (openBracesCount != closeBracesCount)
        {
            printf(RED"FAILED : "RESET);
            printf("%s\n" , hookId);
            canCommitHook = 0;
            return;
        }
        else
        {
            printf(GREEN"PASSED : "RESET);
            printf("%s\n" , hookId);
            return;
        }
    }
    //checks if indentation is allman or k&r
    else if (!strcmp(hookId , "indentation_check"))
    {
        //check if the file is empty
        FILE* file = fopen(fileAddress , "r");
        char line[1000];
        int isFileEmpty = 1;
        while (fgets(line , sizeof(line) , file) != NULL)
        {
            if (strcmp(line , "\n"))
            {
                isFileEmpty = 0;
                break;
            }
        }
        fclose(file);
        if (isFileEmpty)
        {
            printf(GREEN"PASSED : \n"RESET);
            printf("%s\n" , hookId);
            return;
        }

        //make a temp copy of the file
        char commandForCopy[1000] = "cp ";
        strcat(commandForCopy , fileAddress);
        strcat(commandForCopy , " ");
        strcat(commandForCopy , fileAddress);
        strcat(commandForCopy , ".temp");
        FILE* tempForCopy = popen(commandForCopy , "r");
        fclose(tempForCopy);

        //check if the file has allman or k&r indentation
        int isFileHasAllmanIndentation = 0;
        int isFileHasKRIndentation = 0;
        
        //check if the file has allman or k&r indentation
        char commandForCheckAllmanIndentation[1000] = "astyle --style=allman ";
        strcat(commandForCheckAllmanIndentation , fileAddress);
        strcat(commandForCheckAllmanIndentation , " 2> .niggit/error.log");
        char commandForCheckKRIndentation[1000] = "astyle --style=kr ";
        strcat(commandForCheckKRIndentation , fileAddress);
        strcat(commandForCheckKRIndentation , " 2> .niggit/error.log");

        FILE* tempForCheckAllmanIndentation = popen(commandForCheckAllmanIndentation , "r");
        FILE* tempForCheckKRIndentation = popen(commandForCheckKRIndentation , "r");
        char line2[1000];
        while (fgets(line2 , sizeof(line2) , tempForCheckAllmanIndentation) != NULL)
        {
            if (strstr(line2 , "Formatted") != NULL)
            {
                isFileHasAllmanIndentation = 0;
            }
            else if (strstr(line2 , "Unchanged") != NULL)
            {
                isFileHasAllmanIndentation = 1;
                break; 
            }
        }
        while (fgets(line2 , sizeof(line2) , tempForCheckKRIndentation) != NULL)
        {
            if (strstr(line2 , "Formatted") != NULL)
            {
                isFileHasKRIndentation = 0;
            }
            else if (strstr(line2 , "Unchanged") != NULL)
            {
                isFileHasKRIndentation = 1;
                break; 
            }
        }
        fclose(tempForCheckAllmanIndentation);
        fclose(tempForCheckKRIndentation);  

        //remove the temp copy of the file
        char commandForRemove[1000] = "rm ";
        strcat(commandForRemove , fileAddress);
        FILE* tempForRemove = popen(commandForRemove , "r");
        fclose(tempForRemove);
        //remove .orig file
        char commandForRemoveOrig[1000] = "rm ";
        strcat(commandForRemoveOrig , fileAddress);
        strcat(commandForRemoveOrig , ".orig");
        FILE* tempForRemoveOrig = popen(commandForRemoveOrig , "r");
        fclose(tempForRemoveOrig);
        //rename .temp to main file address
        char commandForRename[1000] = "mv ";
        strcat(commandForRename , fileAddress);
        strcat(commandForRename , ".temp ");
        strcat(commandForRename , fileAddress);
        FILE* tempForRename = popen(commandForRename , "r");
        fclose(tempForRename);

        if (!isFileHasAllmanIndentation && !isFileHasKRIndentation)
        {
            printf(RED"FAILED : "RESET);
            printf("%s\n" , hookId);
            canCommitHook = 0;
            return;
        }
        else
        {
            printf(GREEN"PASSED : "RESET);
            printf("%s\n" , hookId);
            return;
        }
    }
}
void PreCommit(char **argv)
{
    //check if niggit is initialized
    if (!IsNiggitInitialized())
    {
        printf("BRUH niggit is not initialized :/\n");
        return;
    }
    
    // make the hooks linklist
    struct Hook* head = NULL;
    struct Hook* current = NULL;
    FILE* hooksTxt = fopen(hooksTextFile , "r");
    char line[1000];
    while (fgets(line , sizeof(line) , hooksTxt) != NULL)
    {
        char hookId[1000] , fileTypes[1000] , description[1000];
        sscanf(line , "%[^-]%*c%[^-]%*c%[^\n]%*c" , hookId , fileTypes , description);
        struct Hook* temp = (struct Hook*)malloc(sizeof(struct Hook));
        strcpy(temp->hookId , hookId);
        strcpy(temp->fileTypes , fileTypes);
        strcpy(temp->hookDescription , description);
        temp->next = NULL;
        if (head == NULL)
        {
            head = temp;
            current = temp;
        }
        else
        {
            current->next = temp;
            current = temp;
        }
    }
    fclose(hooksTxt);

    //do the pre commit on all of the files in stages-current folder
    if (argv[2] == NULL)
    {
        //check if there is any applied hooks
        FILE* appliedHooksTxt = fopen(appliedHooks , "r");
        if (appliedHooksTxt == NULL)
        {
            printf("BRUH you don't have any applied hooks :/\n");
            return;
        }
        fclose(appliedHooksTxt);
        
        //find all of the files in stage current folder
        char commandForFind[1000] = "find ";
        strcat(commandForFind , stagesCurrentAddress);
        strcat(commandForFind , " -type f 2> .niggit/error.log");
        FILE* tempForFind = popen(commandForFind , "r");
        char line[1000];
        
        //apply the hooks on all of the files
        while (fgets(line , sizeof(line) , tempForFind) != NULL)
        {
            line[strcspn(line , "\n")] = '\0';
            printf(CYAN"applying hook on :%s\n"RESET , line);
            FILE* appliedHooksTxt = fopen(appliedHooks , "r");
            char line2[1000];
            while (fgets(line2 , sizeof(line2) , appliedHooksTxt) != NULL)
            {
                char hookId[1000] , fileTypes[1000] , description[1000];
                sscanf(line2 , "%[^-]%*c%[^-]%*c%[^\n]%*c" , hookId , fileTypes , description);
                ApplyHookOnFile(line , hookId , fileTypes , description);
            }
            printf(BLUE"____________________________________________________\n"RESET);
            fclose(appliedHooksTxt);
        }
    }
    //do the pre commit on given files
    else if (!strcmp(argv[2] , "-f"))
    {
        //check if there is any applied hooks
        FILE* appliedHooksTxt = fopen(appliedHooks , "r");
        if (appliedHooksTxt == NULL)
        {
            printf("BRUH you don't have any applied hooks :/\n");
            return;
        }
        fclose(appliedHooksTxt);
        
        //find all of the files in stage current folder
        char commandForFind[1000] = "find ";
        strcat(commandForFind , stagesCurrentAddress);
        strcat(commandForFind , " -type f 2> .niggit/error.log");
        FILE* tempForFind = popen(commandForFind , "r");
        char line[1000];
        //apply the hooks on all of the files that are entered
        while (fgets(line , sizeof(line) , tempForFind) != NULL)
        {
            line[strcspn(line , "\n")] = '\0';
            int indForArgv = 3;
            int isFileFound = 0;
            while (argv[indForArgv] != NULL)
            {
                if (strstr(line , argv[indForArgv]))
                {
                    isFileFound = 1;
                    printf(CYAN"applying hook on :%s\n"RESET , line);
                    FILE* appliedHooksTxt = fopen(appliedHooks , "r");
                    char line2[1000];
                    while (fgets(line2 , sizeof(line2) , appliedHooksTxt) != NULL)
                    {
                        printf("%s\n" , line2);
                        char hookId[1000] , fileTypes[1000] , description[1000];
                        sscanf(line2 , "%[^-]%*c%[^-]%*c%[^\n]%*c" , hookId , fileTypes , description);
                        ApplyHookOnFile(line , hookId , fileTypes , description);
                    }
                    printf(BLUE"____________________________________________________\n"RESET);
                    fclose(appliedHooksTxt);
                    break;
                }
                indForArgv++;
            }
        }
    }
    //fix the failed hooks if it has the -u flag
    else if (!strcmp(argv[2] , "-u"))
    {
        //check if there is any applied hooks
        FILE* appliedHooksTxt = fopen(appliedHooks , "r");
        if (appliedHooksTxt == NULL)
        {
            printf("BRUH you don't have any applied hooks :/\n");
            return;
        }
        fclose(appliedHooksTxt);
        
        //find all of the files in stage current folder and check for bad things
        char commandForFind[1000] = "find ";
        strcat(commandForFind , stagesCurrentAddress);
        strcat(commandForFind , " -type f 2> .niggit/error.log");
        FILE* tempForFind = popen(commandForFind , "r");
        char line[1000];
        while (fgets(line , sizeof(line) , tempForFind) != NULL)
        {
            line[strcspn(line , "\n")] = '\0';
            //correct the indentation
            char commandForCheckAllmanIndentation[1000] = "astyle --style=allman ";
            strcat(commandForCheckAllmanIndentation , line);
            strcat(commandForCheckAllmanIndentation , " 2> .niggit/error.log");
            FILE* fileNIGGER = popen(commandForCheckAllmanIndentation , "r");
            fclose(fileNIGGER);

            // char fileAddressWithOrig[1000] = "";
            // strcat(fileAddressWithOrig , line);
            // strcat(fileAddressWithOrig , ".orig");
            // char commandForDeleteOrigFile[1000] = "rm ";
            // strcat(commandForDeleteOrigFile , fileAddressWithOrig);
            // strcat(commandForDeleteOrigFile , " 2> .niggit/error.log");
            // system(commandForDeleteOrigFile);

            //delete null space at the end of the file
            FILE* appliedHooksTxt = fopen(line , "r");
            char line2[1000][1000];
            int count = 0;
            while (fgets(line2[count] , sizeof(line2[count]) , appliedHooksTxt) != NULL)
            {
                count++;
            }
            for (size_t i = 0; i < strlen(line2[count - 1]); i++)
            {
                if (strlen(line2[count - 1]) != ' ' && strlen(line2[count - 1]) != '\t' && strlen(line2[count - 1]) != '\n')
                {
                    count --;
                    break;
                }
            }
            fclose(appliedHooksTxt);
            
            FILE* fpToOverWrite = fopen(line , "w");
            for (size_t i = 0; i < count; i++)
            {
                fprintf(fpToOverWrite , "%s" , line2[i]);
            }
        }
    }
    //print hooks list
    else if (!strcmp(argv[2] , "hooks") && !strcmp(argv[3] , "list") && argv[4] == NULL)
    {
        struct Hook* temp = head;
        while (temp != NULL)
        {
            printf(GREEN"hook id : %s\n"RESET , temp->hookId);
            printf("file types : %s\n" , temp->fileTypes);
            printf("hook description : %s\n" , temp->hookDescription);
            printf(BLUE"____________________________________________________\n"RESET);
            temp = temp->next;
        }
        return;
    }
    //add hooks to applied hooks by there id
    else if (!strcmp(argv[2] , "add") && !strcmp(argv[3] , "hook"))
    {
        //check if the hook exists
        int isHookFound = 0;
        struct Hook* temp = head;
        while (temp != NULL)
        {
            if (!strcmp(temp->hookId , argv[4]))
            {
                isHookFound = 1;
                break;
            }
            temp = temp->next;
        }
        if (!isHookFound)
        {
            printf("BRUH this hook doesn't exists :/\n");
            return;
        }

        //check if the hook is already applied
        FILE* appliedHooksTxt = fopen(appliedHooks , "r");
        if (appliedHooksTxt == NULL)
        {
            appliedHooksTxt = fopen(appliedHooks , "w");
            fclose(appliedHooksTxt);
            appliedHooksTxt = fopen(appliedHooks , "r");
        }
        char line[1000];
        while (fgets(line , sizeof(line) , appliedHooksTxt) != NULL)
        {
            char hookId[1000];
            sscanf(line , "%[^-]%*c" , hookId);
            if (!strcmp(hookId , argv[4]))
            {
                printf("this hook is already applied :/\n");
                return;
            }
        }
        fclose(appliedHooksTxt);

        //add the hook to applied hooks
        appliedHooksTxt = fopen(appliedHooks , "a");
        fprintf(appliedHooksTxt , "%s-%s-%s\n" , argv[4] , temp->fileTypes , temp->hookDescription);
        fclose(appliedHooksTxt);

        //print successful
        printf("you just added the hook to applied hooks !\n");
        return;
    }
    // remove hooks by hooks id
    else if (!strcmp(argv[2] , "remove") && !strcmp(argv[3] , "hook"))
    {
        //check if the hook exists
        int isHookFound = 0;
        struct Hook* temp = head;
        while (temp != NULL)
        {
            if (!strcmp(temp->hookId , argv[4]))
            {
                isHookFound = 1;
                break;
            }
            temp = temp->next;
        }
        if (!isHookFound)
        {
            printf("BRUH this hook doesn't exists :/\n");
            return;
        }

        //check if the hook is already applied
        FILE* appliedHooksTxt = fopen(appliedHooks , "r");
        if (appliedHooksTxt == NULL)
        {
            printf("BRUH you don't have any applied hooks :/\n");
            return;
        }
        char line[1000];
        while (fgets(line , sizeof(line) , appliedHooksTxt) != NULL)
        {
            char hookId[1000];
            sscanf(line , "%[^-]%*c" , hookId);
            if (!strcmp(hookId , argv[4]))
            {
                break;
            }
        }
        if (feof(appliedHooksTxt))
        {
            printf("BRUH this hook is not applied :/\n");
            return;
        }
        fclose(appliedHooksTxt);

        //remove the hook from applied hooks
        appliedHooksTxt = fopen(appliedHooks , "r");
        FILE* tempAppliedHooksTxt = fopen(tempAppliedHooks , "w");
        while (fgets(line , sizeof(line) , appliedHooksTxt) != NULL)
        {
            char hookId[1000];
            sscanf(line , "%[^-]%*c" , hookId);
            if (strcmp(hookId , argv[4]))
            {
                fprintf(tempAppliedHooksTxt , "%s" , line);
            }
        }
        fclose(appliedHooksTxt);
        fclose(tempAppliedHooksTxt);
        remove(appliedHooks);
        rename(tempAppliedHooks , appliedHooks);

        //print successful
        printf("you just removed the hook from applied hooks !\n");
        return;
    }
    //show applied hooks
    else if (!strcmp(argv[2] , "applied") && !strcmp(argv[3] , "hooks") && argv[4] == NULL)
    {
        FILE* appliedHooksTxt = fopen(appliedHooks , "r");
        char line[1000];
        while (fgets(line , sizeof(line) , appliedHooksTxt) != NULL)
        {
            char hookId[1000] , fileTypes[1000] , description[1000];
            sscanf(line , "%[^-]%*c%[^-]%*c%[^\n]%*c" , hookId , fileTypes , description);
            printf(GREEN"hook id : %s\n"RESET , hookId);
            printf("file types : %s\n" , fileTypes);
            printf("hook description : %s\n" , description);
            printf(BLUE"____________________________________________________\n"RESET);
        }
        fclose(appliedHooksTxt);
        return;
    }
    //disable commiting cause of failed hooks and enabling if successful
    if (!canCommitHook)
    {
        FILE* canCommitHookFile = fopen(canCommitHookTxt , "w");
        fprintf(canCommitHookFile , "0");
        fclose(canCommitHookFile);
    }
    else
    {
        FILE* canCommitHookFile = fopen(canCommitHookTxt , "w");
        fprintf(canCommitHookFile , "1");
        fclose(canCommitHookFile);
    }
}
void Revert(char **argv)
{
    if (!IsNiggitInitialized())
    {
        printf("BRUH niggit is not initialized :/\n");
        return;
    }
    // check for flags
    int doesHaveM = 0 , doesHaveE = 0;
    int indForArgv = 2;
    while (argv[indForArgv] != NULL)
    {
        if (!strcmp(argv[indForArgv] , "-m"))
        {
            doesHaveM = 1;
        }
        else if (!strcmp(argv[indForArgv] , "-e"))
        {
            doesHaveE = 1;
        }
        indForArgv++;
    }
    //normal revert


}
void Diff(char **argv)
{
    if (!IsNiggitInitialized())
    {
        printf("BRUH niggit is not initialized :/\n");
        return;
    }

    if (!strcmp(argv[2] , "-f"))
    {
        //get the file addresses
        char fileAddress1[1000] = "" , fileAddress2[1000] = "";
        strcat(fileAddress1 , argv[3]);
        strcat(fileAddress2 , argv[4]);
        // get the begin and end line for both files
        int beginLine1 = 0 , endLine1 = 0 , beginLine2 = 0 , endLine2 = 0;
        int indexForArgv = 4;
        while (argv[indexForArgv] != NULL)
        {
            if (!strcmp(argv[indexForArgv] , "-line1"))
            {
                int begin , end;
                sscanf(argv[indexForArgv + 1] , "%d-%d" , &begin , &end);
                beginLine1 = begin;
                endLine1 = end;
            }
            else if (!strcmp(argv[indexForArgv] , "-line2"))
            {
                int begin , end;
                sscanf(argv[indexForArgv + 1] , "%d-%d" , &begin , &end);
                beginLine2 = begin;
                endLine2 = end;
            }  
            indexForArgv++;
        }

        if (endLine1 == 0)
        {
            endLine1 = 1000000000;
        }
        if (endLine2 == 0)
        {
            endLine2 = 1000000000;
        }    

        //go through eachline of both file and compare lines
        FILE* file1 = fopen(fileAddress1 , "r");
        FILE* file2 = fopen(fileAddress2 , "r");
        if (file1 == NULL || file2 == NULL)
        {
            printf("one of the files doesn't exist :/\n");
            return;
        }

        char line1[1000] , line2[1000];
        int lineNumberFile1 = 1;
        int lineNumberFile2 = 1;
        fgets(line2 , sizeof(line2) , file2);
        fgets(line1 , sizeof(line1) , file1);
        int isFile1Ended = 0;
        int isFile2Ended = 0;
        int didFoundInEquailty = 0;
        while (1)
        {
            //checks if the current line is in the range of begin and end line
            if (lineNumberFile1 >= beginLine1 && lineNumberFile1 <= endLine1 && lineNumberFile2 >= beginLine2 && lineNumberFile2 <= endLine2)
            {
                //check if current line is null space
                while (1)
                {
                    //check line file 1
                    int isLine1NullSpace = 1;
                    for (size_t i = 0; i < strlen(line1); i++)
                    {
                        if (line1[i] != ' ' && line1[i] != '\t' && line1[i] != '\n')
                        {
                            isLine1NullSpace = 0;
                            break;
                        }
                    }
                    if (isLine1NullSpace)
                    {
                        if (fgets(line1 , sizeof(line1) , file1) == NULL)
                        {
                            break;
                        }
                        lineNumberFile1++;
                    }

                    //check line file 2
                    int isLine2NullSpace = 1;
                    for (size_t i = 0; i < strlen(line2); i++)
                    {
                        if (line2[i] != ' ' && line2[i] != '\t' && line2[i] != '\n')
                        {
                            isLine2NullSpace = 0;
                            break;
                        }
                    }
                    if (isLine2NullSpace)
                    {
                        if (fgets(line2 , sizeof(line2) , file2) == NULL)
                        {
                            break;
                        }
                        lineNumberFile2++;
                    }

                    //break if both lines are not null space
                    if (!isLine1NullSpace && !isLine2NullSpace)
                    {
                        break;
                    }
                }

                //checks if the lines are equal
                if (strcmp(line1 , line2))
                {
                    printf("<<<<<<< \n");
                    printf(BOLDYELLOW"file address : %s\n" , fileAddress1);
                    printf("line %d : " , lineNumberFile1);
                    printf("%s"RESET , line1);
                    printf("=======\n");
                    printf(BOLDMAGENTA"file address : %s\n" , fileAddress2);
                    printf("line %d : " , lineNumberFile2);
                    printf("%s"RESET , line2);
                    printf(">>>>>>> \n");
                    didFoundInEquailty = 1;
                }
            }

            //break if both files are finished
            if (fgets(line1 , sizeof(line1) , file1))
            {
                lineNumberFile1++;
            }
            else
            {
                isFile1Ended = 1;
            }

            if (fgets(line2 , sizeof(line2) , file2))
            {
                lineNumberFile2++;
            }
            else
            {
                isFile2Ended = 1;
            }     

            if (isFile1Ended && isFile2Ended)
            {
                break;
            }
        }

        //print if there is no equality
        if (!didFoundInEquailty)
        {
            printf("there is no difference between these files :)\n");
        }
    }
    if (!strcmp(argv[2] , "-c"))
    {
        
    }
    
}
void Debug(char **argv)
{
    char command[1000] = "ls -l 2.txt";
    FILE* tmp = popen(command , "r");
    char permission[1000];
    fgets(permission , 11 , tmp);
    printf("%s" , permission);
}
void CommandFinder(char **argv)
{
    if (argv[1] == NULL)
    {
        printf("you didn't enter a command :/\n");
        return;
    }
    else if (!strcmp(argv[1] , "help"))
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
    else if (!strcmp(argv[1] , "config"))
    {
        if (!strcmp(argv[2] , "user.name"))
        {
            ConfigUserName(0 , argv[3]);
        }
        else if(!strcmp(argv[3] , "user.name"))
        {
            ConfigUserName(1 , argv[4]);
        }
        else if (!strcmp(argv[2] , "user.email"))
        {
            ConfigUserEmail(0 , argv[3]);
        }
        else if(!strcmp(argv[3] , "user.email"))
        {
            ConfigUserEmail(1 , argv[4]);
        }
        else if (strstr(argv[2] , "alias"))
        {
            ConfigAlias(0 , argv);
        }
        else if (strstr(argv[3] , "alias"))
        {
            ConfigAlias(1 , argv);
        }
        else
        {
            printf("BRUH you entered a wrong command for config :/\n");
        }
        
    }
    else if (!strcmp(argv[1] , "init"))
    {
        Init();
    }
    else if (!strcmp(argv[1] , "add"))
    {
        Add(argv , 0);
    }
    else if (!strcmp(argv[1] , "reset"))
    {
        Reset(argv);
    }
    else if (!strcmp(argv[1] , "branch"))
    {
        Branch(argv);
    }
    else if (!strcmp(argv[1] , "status"))
    {
        Status(argv);
    }
    else if (!strcmp(argv[1] , "set"))
    {
        SetCommitShortCut(argv);
    }
    else if (!strcmp(argv[1] , "remove"))
    {
        RemoveCommitShortCut(argv);
    }
    else if (!strcmp(argv[1] , "replace"))
    {
        ReplaceCommitShortCut(argv);
    }
    else if (!strcmp(argv[1] , "commit"))
    {
        Commit(argv);
    }
    else if (!strcmp(argv[1] , "log"))
    {
        Log(argv);
    }
    else if (!strcmp(argv[1] , "checkout"))
    {
        CheckOut(argv);
    }
    else if (!strcmp(argv[1] , "merge"))
    {
        Merge(argv);
    }
    else if (!strcmp(argv[1] , "tag"))
    {
        Tag(argv);
    }
    else if (!strcmp(argv[1] , "grep"))
    {
        Grep(argv);
    }
    else if (!strcmp(argv[1] , "pre-commit"))
    {
        PreCommit(argv);
    }
    else if (!strcmp(argv[1] , "revert"))
    {
        Revert(argv);
    }
    else if (!strcmp(argv[1] , "diff"))
    {
        Diff(argv);
    }
    else if (!strcmp(argv[1] , "debug"))
    {
        Debug(argv);
    }
    else
    {
        if (!Alias(argv))
        {
            printf("BRUH you entered a wrong command :/\n");
        }
    }   
}