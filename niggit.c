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
//branch
#define branchesAddress ".niggit/branches"
#define masterAddress ".niggit/branches/master"
#define branchesTextFile ".niggit/branches/branches.txt"
#define currentBranchTextFile ".niggit/branches/current-branch.txt"
#define currentBranchName ".niggit/branches/current-branch-name.txt"
#define totalCommitCount ".niggit/branches/commitCount.txt"
#define globalCommitList ".niggit/commit-list.txt"
//configs
#define globalSettingAddress "/home/kshyst/.niggit-settings"
#define localSettingAddress ".niggit/configs"
#define globalAliasAddress "/home/kshyst/.niggit-settings/global-alias.txt"
#define localAliasAddress ".niggit/configs/local-alias.txt"
#define globalUserName "/home/kshyst/.niggit-settings/global-username.txt"
#define globalUserEmail "/home/kshyst/.niggit-settings/global-userEmail.txt"
#define localUserName ".niggit-settings/local-username.txt"
#define localUserEmail ".niggit-settings/local-userEmail.txt"
//colors
#define RED "\033[0;31m"
#define GREEN "\033[0;32m"
#define RESET "\033[0m"

//Function Prototypes
void CommandFinder(char **argv);
char* GetTime();
int main(int argc, char **argv)
{
    CommandFinder(argv);
}
//Functions
void count_files(char *base_path, int *file_count) {
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
int isFolderEmpty(const char *path) {
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
        char temp[1000] = "";
        sscanf(argv[3] , "alias.%[^\n]%*c" , temp);
        strcat(alias , temp);
        strcat(aliasFor , argv[4]);

        char command[1000] = "";
        strcat(command , globalSettingAddress);
        strcat(command , "/global-alias.txt");
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
        char temp[1000] = "";
        sscanf(argv[2] , "alias.%[^\n]%*c" , temp);
        strcat(alias , temp);
        strcat(aliasFor , argv[3]);

        char command[1000] = "";
        strcat(command , localSettingAddress);
        strcat(command , "/local-alias.txt");
        FILE* configFile = fopen(command , "a");
        if(configFile == NULL)
        {
            printf("Error opening file!\n");
            return;
        }
        fprintf(configFile , "%s-%s\n" , alias , aliasFor);
    }


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
        system("mkdir .niggit/branches/master/.commits");
        
        FILE *fp = fopen(".niggit/head.txt" , "w");
        fprintf(fp , "HEAD=.niggit/branches/master\n");  //UNDONE
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

        fprintf(fp , "HEAD=.niggit/branches/master\n");
        fclose(fp);

        FILE *fp2 = fopen(".niggit/branches.txt" , "w");
        fprintf(fp2 , "master-.niggit/branches/master\n");
        fclose(fp2);

        FILE *fp3 = fopen(".niggit/.stages/stages-latest.txt" , "w");
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
        FILE *fp2 = fopen(".niggit/branches.txt" , "r");
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

        char dirStageCommand[1000] = "" , dirMakeCommand[1000] = "mkdir " , dirCommitCommand[1000] =""
            ,dirStageCurrent[1000] = "" , dirStageLatest[1000] = "";
        
        strcat(dirMakeCommand , GetHead());
        strcat(dirMakeCommand , "/");
        strcat(dirMakeCommand , argv[2]);

        char newHeadDir[1000] = "";
        strcat(newHeadDir , GetHead());
        strcat(newHeadDir , "/");
        strcat(newHeadDir , argv[2]);

        FILE *branchesFp = fopen(branchesTextFile , "a");
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
void Status(char **argv)
{
    if (IsNiggitInitialized() == 0)
    {
        printf("BRUH niggit is not initialized :/\n");
        return;
    }
}
void Commit(char **argv)
{
    if (IsNiggitInitialized() == 0)
    {
        printf("BRUH niggit is not initialized :/\n");
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

    //finds how many files are there in stage-current
    int fileCount = 0;
    count_files(stagesCurrentAddress, &fileCount);
    if (fileCount == 0)
    {
        printf("You Didn't fucking stage anything what are you trying to commit ???????\n");
        return;
    }
    
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
    strcat(commandToCpyStagesInCommits , stagesCurrentAddress);
    strcat(commandToCpyStagesInCommits , "\" \"");
    strcat(commandToCpyStagesInCommits , commitFolder);
    strcat(commandToCpyStagesInCommits , "\"");
    system(commandToCpyStagesInCommits);

    char renameTheStageToCommitHash[1000] = "";
    strcat(renameTheStageToCommitHash , commitFolder);
    strcat(renameTheStageToCommitHash , "stages-current");
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

    //add the commit to the commit list of current branch
    char commitList[1000] = "";
    strcat(commitList , currentBranch);
    strcat(commitList , "/commit-list.txt");
    FILE *commitListFp = fopen(commitList , "a");
    fprintf(commitListFp , "%s-%s-%s-%s-%s-%d\n" , commitIdString , commitMessage , GetTime() , currentBranchNameString , commitUsername , fileCount);

    //add commit to global commit list
    FILE *globalCommitListFp = fopen(globalCommitList , "a");
    fprintf(globalCommitListFp , "%s-%s-%s-%s-%s-%d\n" , commitIdString , commitMessage , GetTime() , currentBranchNameString , commitUsername , fileCount);
    fclose(globalCommitListFp);

    //print every shit about commit

    printf("you just Cummited ! shame...\n");
    printf("commit id : %s\n" , commitIdString);
    printf("commit message : %s\n" , commitMessage);
    printf("commit time : %s\n" , GetTime());
    printf("commit branch : %s\n" , currentBranchNameString);
    printf("commit username : %s\n" , commitUsername);
    printf("commit file count : %d\n" , fileCount);

    //store the last commit 


}
void Log(char **argv)
{
    if (IsNiggitInitialized() == 0)
    {
        printf("BRUH niggit is not initialized :/\n");
        return;
    }
    if(!strcmp(argv[2] , "-n") && argv[3] == NULL)
    {
        printf("you didn't enter a number :/\n");
        return;
    }
    //normal log
    if(argv[2] == NULL)
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
            char commitId[1000] , commitMessage[1000] , commitTime[1000] , commitBranch[1000] , commitUsername[1000] , commitFileCount[1000];
            sscanf(lines[i] , "%[^-]%*c%[^-]%*c%[^-]%*c%[^-]%*c%[^-]%*c%[^\n]%*c" , commitId , commitMessage , commitTime , commitBranch , commitUsername , commitFileCount);
            printf(GREEN"commit id : %s\n"RESET , commitId);
            printf("commit message : %s\n" , commitMessage);
            printf("commit time : %s\n" , commitTime);
            printf("commit branch : %s\n" , commitBranch);
            printf("commit username : %s\n" , commitUsername);
            printf("commit file count : %s\n" , commitFileCount);
            printf("\n");
        }
    }
    //log with number
    if(!strcmp(argv[2] , "-n") && argv[3] != NULL)
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

        for (int i = count - 1 ; i >= number; i--)
        {
            char commitId[1000] , commitMessage[1000] , commitTime[1000] , commitBranch[1000] , commitUsername[1000] , commitFileCount[1000];
            sscanf(lines[i] , "%[^-]%*c%[^-]%*c%[^-]%*c%[^-]%*c%[^-]%*c%[^\n]%*c" , commitId , commitMessage , commitTime , commitBranch , commitUsername , commitFileCount);
            printf(GREEN"commit id : %s\n"RESET , commitId);
            printf("commit message : %s\n" , commitMessage);
            printf("commit time : %s\n" , commitTime);
            printf("commit branch : %s\n" , commitBranch);
            printf("commit username : %s\n" , commitUsername);
            printf("commit file count : %s\n" , commitFileCount);
            printf("\n");
        }
    }
    //log with author name
    if (!strcmp(argv[2] , "-author"))
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
            char commitId[1000] , commitMessage[1000] , commitTime[1000] , commitBranch[1000] , commitUsername[1000] , commitFileCount[1000];
            sscanf(lines[i] , "%[^-]%*c%[^-]%*c%[^-]%*c%[^-]%*c%[^-]%*c%[^\n]%*c" , commitId , commitMessage , commitTime , commitBranch , commitUsername , commitFileCount);
            if (!strcmp(commitUsername , argv[3]))
            {
                isAuthorFound = 1;
                printf(GREEN"commit id : %s\n"RESET , commitId);
                printf("commit message : %s\n" , commitMessage);
                printf("commit time : %s\n" , commitTime);
                printf("commit branch : %s\n" , commitBranch);
                printf("commit username : %s\n" , commitUsername);
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
    //log with since
    if (!strcmp(argv[2] , "-since"))
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
            char commitId[1000] , commitMessage[1000] , commitTime[1000] , commitBranch[1000] , commitUsername[1000] , commitFileCount[1000];
            sscanf(lines[i] , "%[^-]%*c%[^-]%*c%[^-]%*c%[^-]%*c%[^-]%*c%[^\n]%*c" , commitId , commitMessage , commitTime , commitBranch , commitUsername , commitFileCount);
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
    if (!strcmp(argv[2] , "-before"))
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
            char commitId[1000] , commitMessage[1000] , commitTime[1000] , commitBranch[1000] , commitUsername[1000] , commitFileCount[1000];
            sscanf(lines[i] , "%[^-]%*c%[^-]%*c%[^-]%*c%[^-]%*c%[^-]%*c%[^\n]%*c" , commitId , commitMessage , commitTime , commitBranch , commitUsername , commitFileCount);
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
    if (!strcmp(argv[2] , "-search"))
    {
        if (argv[2] == NULL)
        {
            printf("you didn't enter a word to search for :/\n");
            return;
        }
        
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
            char commitId[1000] , commitMessage[1000] , commitTime[1000] , commitBranch[1000] , commitUsername[1000] , commitFileCount[1000];
            sscanf(lines[i] , "%[^-]%*c%[^-]%*c%[^-]%*c%[^-]%*c%[^-]%*c%[^\n]%*c" , commitId , commitMessage , commitTime , commitBranch , commitUsername , commitFileCount);
            
            int indForArgv = 3;
            while (argv[indForArgv] != NULL)
            {
                if (strstr(commitMessage , argv[indForArgv]) != NULL)
                {
                    isWordFound = 1;
                    printf(GREEN"commit id : %s\n"RESET , commitId);
                    printf("commit message : %s\n" , commitMessage);
                    printf("commit time : %s\n" , commitTime);
                    printf("commit branch : %s\n" , commitBranch);
                    printf("commit username : %s\n" , commitUsername);
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
    else if (!strcmp(argv[1] , "commit"))
    {
        Commit(argv);
    }
    else if (!strcmp(argv[1] , "log"))
    {
        Log(argv);
    }
    //search for alias UNDONE
    else
    {
        printf("BRUH you entered a wrong command :/\n");
    }   
}