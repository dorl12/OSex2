// Dor Levy 313547085
#define _GNU_SOURCE
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>

void checkArgs(int argc) {
    if (argc < 2) {
        if (write(2, "Insufficient Arguments!!!\n", strlen("Insufficient Arguments!!!\n")) < 0) {
            exit(-1);
        }
        if (write(2, "Please use \"program-name file-name\" format.\n",
                  strlen("Please use \"program-name file-name\" format.\n")) < 0) {
            exit(-1);
        }
        exit(-1);
    }
}

int openGradesFile() {
    int gradesFile = open("results.csv", O_CREAT | O_RDWR, 0666);
    if (gradesFile < 0) {
        if (write(2, "Error in: open\n", strlen("Error in: open\n")) < 0) {
            exit(-1);
        }
        exit(-1);
    }
    return gradesFile;
}

int openErrorsFile(int gradesFile) {
    int errorsFile = open("errors.txt", O_CREAT | O_RDWR, 0666);
    if (errorsFile < 0) {
        if (write(2, "Error in: open\n", strlen("Error in: open\n")) < 0) {
            if (close(gradesFile) < 0) {
                if (write(2,"Error in: close\n",strlen("Error in: close\n")) < 0) {
                    exit(-1);
                }
                exit(-1);
            }
            exit(-1);
        }
        if (close(gradesFile) < 0) {
            if (write(2,"Error in: close\n",strlen("Error in: close\n")) < 0) {
                exit(-1);
            }
            exit(-1);
        }
        exit(-1);
    }
    return errorsFile;
}

void closeFiles(int gradesFile, int errorsFile) {
    if (close(gradesFile) < 0) {
        if (write(2,"Error in: close\n",strlen("Error in: close\n")) < 0) {
            exit(-1);
        }
        exit(-1);
    }
    if (close(errorsFile) < 0) {
        if (write(2,"Error in: close\n",strlen("Error in: close\n")) < 0) {
            exit(-1);
        }
        exit(-1);
    }
}

void parseConfFile(char *argv[], int confFile, int gradesFile, int errorsFile, char confLines[3][150]) {
    confFile = open(argv[1], 'r');
    if (confFile < 0) {
        if (write(2, "Error in: open\n", strlen("Error in: open\n")) < 0) {
            closeFiles(gradesFile, errorsFile);
            exit(-1);
        }
        close(gradesFile);
        exit(-1);
    }
    char buff[450];
    int readConf = read(confFile, buff, 450);
    if (readConf < 0) {
        if (write(2,"Error in: read\n",strlen("Error in: read\n")) < 0) {
            closeFiles(gradesFile, errorsFile);
            exit(-1);
        }
        closeFiles(gradesFile, errorsFile);
        exit(-1);
    }
    close(confFile);
    char *token = strtok(buff, "\n");
    strcpy(confLines[0], token);
    int i;
    for(i = 1; i < 3; i++) {
        token = strtok(NULL, "\n");
        strcpy(confLines[i], token);
    }
}

int isFile(char* path) {
    struct stat stat_p;
    if (stat(path, &stat_p) != 0) {
        return 0;
    }
    return S_ISREG(stat_p.st_mode);
}

int isDirectory(char* path) {
    struct stat stat_p;
    if (stat(path, &stat_p) != 0) {
        return 0;
    }
    return S_ISDIR(stat_p.st_mode);
}

void checkConfFileLines(int gradesFile, int errorsFile, char confLines[3][150]) {
    if (!isDirectory(confLines[0])) {
        write(STDOUT_FILENO, "Not a valid directory\n", strlen("Not a valid directory\n"));
        closeFiles(gradesFile, errorsFile);
        exit(-1);
    }
    if (!isFile(confLines[1])) {
        write(STDOUT_FILENO, "Input file not exist\n", strlen("Input file not exist\n"));
        closeFiles(gradesFile, errorsFile);
        exit(-1);
    }
    if (!isFile(confLines[2])) {
        write(STDOUT_FILENO, "Output file not exist\n", strlen("Output file not exist\n"));
        closeFiles(gradesFile, errorsFile);
        exit(-1);
    }
}

int compileCFile(pid_t pid, char exeName[150], char mainFolder[150], char userName[150], int gradesFile,
                 int errorsFile) {
    pid = fork();
    if(pid == -1) {
        if (write(2,"Error in: fork\n",strlen("Error in: fork\n")) < 0) {
            closeFiles(gradesFile, errorsFile);
            exit(-1);
        }
        exit(-1);
    }
    else if (pid == 0) {
        char *gccArgs[] = {"gcc", "-o", exeName, mainFolder, NULL};
        if(execvp(gccArgs[0], gccArgs)) {
            if (write(2,"execvp 1 failed\n",strlen("execvp failed\n")) < 0) {
                closeFiles(gradesFile, errorsFile);
                exit(-1);
            }
        }
    }
    int status;
    if(waitpid(pid, &status, 0) < 0) {
        if (write(2,"Error in: waitpid\n",strlen("Error in: waitpid\n")) < 0) {
            closeFiles(gradesFile, errorsFile);
            exit(-1);
        }
        exit(-1);
    }
    if (WEXITSTATUS(status) != 0) {
        char *compErrorTemp = ",10,COMPILATION_ERROR\n";
        strcat(userName, compErrorTemp);
        write(gradesFile, userName, strlen(userName));
        return 0;
    }
    return 1;
}

void runExeFile(pid_t pid, char confLines[3][150], char exeName[150], int gradesFile, int errorsFile) {
    int inputFile = open(confLines[1], 'r');
    if (inputFile < 0) {
        if (write(2, "Error in: open\n", strlen("Error in: open\n")) < 0) {
            closeFiles(gradesFile, errorsFile);
            exit(-1);
        }
        closeFiles(gradesFile, errorsFile);
        exit(-1);
    }
    int outputFile = open("output.txt", O_CREAT | O_RDWR, 0666);
    if (outputFile < 0) {
        if (write(2, "Error in: open\n", strlen("Error in: open\n")) < 0) {
            closeFiles(gradesFile, errorsFile);
            exit(-1);
        }
        closeFiles(gradesFile, errorsFile);
        exit(-1);
    }
    pid = fork();
    if(pid == -1) {
        if (write(2,"Error in: fork\n",strlen("Error in: fork\n")) < 0) {
            closeFiles(gradesFile, errorsFile);
            exit(-1);
        }
        closeFiles(gradesFile, errorsFile);
        exit(-1);
    }
    else if (pid == 0) {
        dup2(inputFile, 0);
        dup2(outputFile, 1);
        char runExeFile[150];
        strcpy(runExeFile, "./");
        strcat(runExeFile, exeName);
        char *runExeArgs[] = {runExeFile, NULL};
        if(execvp(runExeArgs[0], runExeArgs)) {
            if (write(2,"execvp 2 failed\n",strlen("execvp failed\n")) < 0) {
                closeFiles(gradesFile, errorsFile);
                exit(-1);
            }
        }
    }
    close(inputFile);
    close(outputFile);
    int status;
    if(waitpid(pid, &status, 0) < 0) {
        if (write(2,"Error in: waitpid\n",strlen("Error in: waitpid\n")) < 0) {
            closeFiles(gradesFile, errorsFile);
            exit(-1);
        }
        closeFiles(gradesFile, errorsFile);
        exit(-1);
    }
}

void deleteExeFiles(pid_t pid, char exeName[150], int gradesFile, int errorsFile) {
    pid = fork();
    if(pid == -1) {
        if (write(2,"Error in: fork\n",strlen("Error in: fork\n")) < 0) {
            closeFiles(gradesFile, errorsFile);
            exit(-1);
        }
        closeFiles(gradesFile, errorsFile);
        exit(-1);
    }
    else if (pid == 0) {
        char *deleteExeFile[] = {"rm", exeName, NULL};
        if(execvp(deleteExeFile[0], deleteExeFile)) {
            if (write(2,"execvp 3 failed\n",strlen("execvp failed\n")) < 0) {
                closeFiles(gradesFile, errorsFile);
                exit(-1);
            }
        }
    }
    int status;
    if(waitpid(pid, &status, 0) < 0) {
        if (write(2,"Error in: waitpid\n",strlen("Error in: waitpid\n")) < 0) {
            closeFiles(gradesFile, errorsFile);
            exit(-1);
        }
        closeFiles(gradesFile, errorsFile);
        exit(-1);
    }
}

void compFiles(pid_t pid, char confLines[3][150], int gradesFile, int errorsFile, char userName[150]) {
    pid = fork();
    if(pid == -1) {
        if (write(2,"Error in: fork\n",strlen("Error in: fork\n")) < 0) {
            closeFiles(gradesFile, errorsFile);
            exit(-1);
        }
        closeFiles(gradesFile, errorsFile);
        exit(-1);
    }
    else if (pid == 0) {
        char *compOutput[] = {"./comp.out", "./output.txt", confLines[2], NULL};
        if(execvp(compOutput[0], compOutput)) {
            if (write(2,"execvp 4 failed\n",strlen("execvp 4 failed\n")) < 0) {
                closeFiles(gradesFile, errorsFile);
                exit(-1);
            }
        }
    }
    int status;
    if(waitpid(pid, &status, 0) < 0) {
        if (write(2,"Error in: waitpid 4\n",strlen("Error in: waitpid 4\n")) < 0) {
            closeFiles(gradesFile, errorsFile);
            exit(-1);
        }
        closeFiles(gradesFile, errorsFile);
        exit(-1);
    }
    switch (WEXITSTATUS(status)) {
        case 1:
            strcat(userName, ",100,EXCELLENT\n");
            if (write(gradesFile, userName, strlen(userName)) < 0) {
                closeFiles(gradesFile, errorsFile);
                exit(-1);
            }
            break;
        case 2:
            strcat(userName, ",50,WRONG\n");
            if (write(gradesFile, userName, strlen(userName)) < 0) {
                closeFiles(gradesFile, errorsFile);
                exit(-1);
            }
            break;
        case 3:
            strcat(userName, ",75,SIMILAR\n");
            if (write(gradesFile, userName, strlen(userName)) < 0) {
                closeFiles(gradesFile, errorsFile);
                exit(-1);
            }
            break;
        default:
            break;
    }
}

void deleteOutputFile(pid_t pid, int gradesFile, int errorsFile) {
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
    int status;
    if(waitpid(pid, &status, 0) < 0) {
        perror("waitpid failed");
    }
}

void iterateSubDir(int gradesFile, int errorsFile, char confLines[3][150], DIR *pMainDir, struct dirent *pMainDirent,
        char mainFolder[150] ) {
    DIR *pSubDir;
    struct dirent *pSubDirent;
    if ((pSubDir = opendir(mainFolder)) == NULL) {
        closedir(pMainDir);
        closeFiles(gradesFile, errorsFile);
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
            pid_t pid;
            if (compileCFile(pid, exeName, mainFolder, userName, gradesFile, errorsFile) == 0) {
                continue;
            }
            runExeFile(pid, confLines, exeName, gradesFile, errorsFile);
            deleteExeFiles(pid, exeName, gradesFile, errorsFile);
            compFiles(pid, confLines, gradesFile, errorsFile, userName);
            deleteOutputFile(pid, gradesFile, errorsFile);
        }
    }
    if(isCFileExist == 0) {
        strcat(userName, ",0,NO_C_FILE\n");
        if (write(gradesFile, userName, strlen(userName)) < 0) {
            closeFiles(gradesFile, errorsFile);
            exit(-1);
        }
    }
    closedir(pSubDir);
}

void iterateMainDir(int gradesFile, int errorsFile, char confLines[3][150]) {
    DIR *pMainDir;
    struct dirent *pMainDirent;
    if ((pMainDir = opendir(confLines[0])) == NULL) {
        closeFiles(gradesFile, errorsFile);
        exit(-1);
    }
    char mainFolder[150];
    while ((pMainDirent = readdir(pMainDir)) != NULL) {
        strcpy(mainFolder, confLines[0]);
        if ((pMainDirent->d_type == DT_DIR) && (strcmp(pMainDirent->d_name, ".")) &&
            (strcmp(pMainDirent->d_name, ".."))) {
            strcat(mainFolder, "/");
            strcat(mainFolder, pMainDirent->d_name);
            iterateSubDir(gradesFile, errorsFile, confLines, pMainDir, pMainDirent, mainFolder);
        }
    }
    closedir(pMainDir);
}

int main(int argc, char *argv[]) {
    checkArgs(argc);
    int gradesFile = openGradesFile();
    int errorsFile = openErrorsFile(gradesFile);
    dup2(errorsFile, 2);
    int confFile;
    char confLines[3][150];
    parseConfFile(argv, confFile, gradesFile, errorsFile, confLines);
    checkConfFileLines(gradesFile, errorsFile, confLines);
    iterateMainDir(gradesFile, errorsFile, confLines);
    closeFiles(gradesFile, errorsFile);
}
