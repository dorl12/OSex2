// Dor Levy 313547085
#define _GNU_SOURCE
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>

//int isDir(const char* fileName)
//{
//    struct stat path;
//    stat(fileName, &path);
//    return S_ISREG(path.st_mode);
//}

int main(int argc, char *argv[]) {
//    if (argc < 2) {
//
//    }
    int gradesFile = open("grades.csv", O_CREAT, S_IRUSR | S_IWUSR);
    if (gradesFile < 0) {
//        printf("Error in: open\n");
//        return -1;
    }
    int confFile = open(argv[1], 'r');
    if (confFile < 0) {
//        printf("Error in: open\n");
//        return -1;
    }
    char buff[450];
    int readConf = read(confFile, buff, 450);
    if (readConf < 0) {
//        printf("Error in: read\n");
//        return -1;
    }
    close(confFile);
    char confLines[3][150];
    char* token = strtok(buff, "\n");
    strcpy(confLines[0], token);
    for (int i = 1; i < 3; i++) {
        token = strtok(NULL, "\n");
        strcpy(confLines[i], token);
    }
//    if (isDir(confLines[0])) {
//        printf("Not a valid directory\n");
//        return -1;
//    }
    DIR *pMainDir;
    struct dirent *pMainDirent;
    if ((pMainDir = opendir(confLines[0])) == NULL) {
//        exit(1);
    }
    chdir(confLines[0]);
    // looping through the directory, printing the directory entry name
    while ((pMainDirent = readdir(pMainDir)) != NULL ) {
//        printf( "%s\n", pDirent ->d_name );
        if ((pMainDirent->d_type == DT_DIR) && (strcmp(pMainDirent->d_name, ".")) && (strcmp(pMainDirent->d_name, ".."))) {
            //printf( "%s\n", pDirent ->d_name );
            DIR *pSubDir;
            struct dirent *pSubDirent;
            if ((pSubDir = opendir(pMainDirent->d_name)) == NULL) {
                exit(1);
            }
            while ((pSubDirent = readdir(pSubDir)) != NULL) {
                printf("%s\n", pSubDirent->d_name);
            }
//        exit(1);
            }
        }
//        pid_t pid = fork();
//        if(pid == -1) {
////            perror("fork failed");
//        }
//        else if (pid == 0) {
//            if(execvp(gcc, commandArray)) {
////                perror("execvp failed");
//            }
//            exit(0);
//        }
//        else {
//            if(wait(NULL) < 0) {
////                perror("wait failed");
//            }
//        }
    closedir(pMainDir);
    close(gradesFile);
    }

