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

int main(int argc, char *argv[]) {
    if (argc < 2) {
        if (write(2, "Insufficient Arguments!!!\n", strlen("Insufficient Arguments!!!\n")) < 0) {
            return -1;
        }
        if (write(2, "Please use \"program-name file-name\" format.\n",
                  strlen("Please use \"program-name file-name\" format.\n")) < 0) {
            return -1;
        }
        return -1;
    }
    int gradesFile = open("grades.csv", O_CREAT | O_RDWR, 0666);
    if (gradesFile < 0) {
        if (write(2, "Error in: open\n", strlen("Error in: open\n")) < 0) {
            return -1;
        }
        return -1;
    }
    int errorsFile = open("errors.txt", O_CREAT | O_RDWR, 0666);
    if (errorsFile < 0) {
        if (write(2, "Error in: open\n", strlen("Error in: open\n")) < 0) {
            return -1;
        }
        return -1;
    }
    dup2(errorsFile, 2);
    int confFile = open(argv[1], 'r');
    if (confFile < 0) {
        if (write(2, "Error in: open\n", strlen("Error in: open\n")) < 0) {
            close(gradesFile);
            return -1;
        }
        close(gradesFile);
        return -1;
    }
    char buff[450];
    int readConf = read(confFile, buff, 450);
    if (readConf < 0) {
        if (write(2,"Error in: read\n",strlen("Error in: read\n")) < 0) {
            close(gradesFile);
            close(confFile);
            return -1;
        }
        close(gradesFile);
        close(confFile);
        return -1;
    }
    close(confFile);
    char confLines[3][150];
    char *token = strtok(buff, "\n");
    strcpy(confLines[0], token);
    for (int i = 1; i < 3; i++) {
        token = strtok(NULL, "\n");
        strcpy(confLines[i], token);
    }
    DIR *pMainDir;
    struct dirent *pMainDirent;
    if ((pMainDir = opendir(confLines[0])) == NULL) {
        if (write(2, "Not a valid directory\n", strlen("Not a valid directory\n")) < 0) {
            close(gradesFile);
            return -1;
        }
        close(gradesFile);
        return -1;
    }
    char mainFolder[150];
    while ((pMainDirent = readdir(pMainDir)) != NULL) {
        strcpy(mainFolder, confLines[0]);
        if ((pMainDirent->d_type == DT_DIR) && (strcmp(pMainDirent->d_name, ".")) &&
            (strcmp(pMainDirent->d_name, ".."))) {
            strcat(mainFolder, "/");
            strcat(mainFolder, pMainDirent->d_name);
            DIR *pSubDir;
            struct dirent *pSubDirent;
            if ((pSubDir = opendir(mainFolder)) == NULL) {
                closedir(pMainDir);
                close(gradesFile);
                exit(-1);
            }
            char userName[150];
            strcpy(userName, pMainDirent->d_name);
            int isCFileExist = 0;
            while ((pSubDirent = readdir(pSubDir)) != NULL) {
                int size = strlen(pSubDirent->d_name);
                if ((pSubDirent->d_type == DT_REG) && (pSubDirent->d_name[size - 2] == '.') &&
                (pSubDirent->d_name[size - 1] == 'c')) {
                    isCFileExist = 1;
                    strcat(mainFolder, "/");
                    strcat(mainFolder, pSubDirent->d_name);
                    char exeName[150];
                    int i;
                    for (i = 0; i < (size - 2); i++) {
                        exeName[i] = pSubDirent->d_name[i];
                    }
                    exeName[i] = '\0';
                    strcat(exeName, ".out");
                    pid_t pid = fork();
                    if(pid == -1) {
                        perror("fork failed");
                    }
                    else if (pid == 0) {
                        char *gccArgs[] = {"gcc", "-o", exeName, mainFolder, NULL};
                        if(execvp(gccArgs[0], gccArgs)) {
                            perror("execvp failed");
                        }
                    }
                    int status;
                    if(waitpid(pid, &status, 0) < 0) {
                        perror("waitpid failed");
                    }
                    if (WEXITSTATUS(status) != 0) {
                        char *compErrorTemp = ",10,COMPILATION_ERROR\n";
                        strcat(userName, compErrorTemp);
                        write(gradesFile, userName, strlen(userName));
                        continue;
                    }
                    int inputFile = open(confLines[1], 'r');
                    if (inputFile < 0) {
                        if (write(2, "Error in: open\n", strlen("Error in: open\n")) < 0) {
                            return -1;
                        }
                        return -1;
                    }
                    int outputFile = open("output.txt", O_CREAT | O_RDWR, 0666);
                    if (outputFile < 0) {
                        if (write(2, "Error in: open\n", strlen("Error in: open\n")) < 0) {
                            return -1;
                        }
                        return -1;
                    }
                    pid = fork();
                    if(pid == -1) {
                        perror("fork failed");
                    }
                    else if (pid == 0) {
                        dup2(inputFile, 0);
                        dup2(outputFile, 1);
                        char runExeFile[150];
                        strcpy(runExeFile, "./");
                        strcat(runExeFile, exeName);
                        char *runExeArgs[] = {runExeFile, NULL};
                        if(execvp(runExeArgs[0], runExeArgs)) {
                            perror("execvp failed");
                        }
                    }
                    close(inputFile);
                    close(outputFile);
                    if(waitpid(pid, &status, 0) < 0) {
                        perror("waitpid failed");
                    }
                    pid = fork();
                    if(pid == -1) {
                        perror("fork failed");
                    }
                    else if (pid == 0) {
                        char *deleteExeFile[] = {"rm", exeName, NULL};
                        if(execvp(deleteExeFile[0], deleteExeFile)) {
                            perror("execvp failed");
                        }
                    }
                    if(waitpid(pid, &status, 0) < 0) {
                        perror("waitpid failed");
                    }
                    pid = fork();
                    if(pid == -1) {
                        perror("fork failed");
                    }
                    else if (pid == 0) {
                        char *compOutput[] = {"../comp.out", "output.txt", confLines[2], NULL};
                        if(execvp(compOutput[0], compOutput)) {
                            perror("execvp failed");
                        }
                    }
                    if(waitpid(pid, &status, 0) < 0) {
                        perror("waitpid failed");
                    }
                    switch (WEXITSTATUS(status)) {
                        case 1:
                            strcat(userName, ",100,EXCELLENT\n");
                            write(gradesFile, userName, strlen(userName));
                            break;
                        case 2:
                            strcat(userName, ",50,WRONG\n");
                            write(gradesFile, userName, strlen(userName));
                            break;
                        case 3:
                            strcat(userName, ",75,SIMILAR\n");
                            write(gradesFile, userName, strlen(userName));
                            break;
                        default:
                            break;
                    }
                    pid = fork();
                    if(pid == -1) {
                        perror("fork failed");
                    }
                    else if (pid == 0) {
                        char *deleteOutput[] = {"rm", "output.txt", NULL};
                        if(execvp(deleteOutput[0], deleteOutput)) {
                            perror("execvp failed");
                        }
                    }
                    if(waitpid(pid, &status, 0) < 0) {
                        perror("waitpid failed");
                    }
                }
            }
            if(isCFileExist == 0) {
                strcat(userName, ",0,NO_C_FILE\n");
                write(gradesFile, userName, strlen(userName));
            }
            closedir(pSubDir);
        }
    }
    closedir(pMainDir);
    close(errorsFile);
    close(gradesFile);
}

