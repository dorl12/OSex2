// Dor Levy 313547085
#define _GNU_SOURCE
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
//#include<sys/wait.h>
//#include<sys/types.h>
//#include <sys/stat.h>
#include <fcntl.h>

void closeFiles(int fd1, int fd2) {
    if (close(fd1) < 0) {
        if (write(2,"Error in: close\n",strlen("Error in: close\n")) < 0) {
            exit(-1);
        }
        exit(-1);
    }
    if (close(fd2) < 0) {
        if (write(2,"Error in: close\n",strlen("Error in: close\n")) < 0) {
            exit(-1);
        }
        exit(-1);
    }
}

int main(int argc, char *argv[]) {
    int fd1;
    int fd2;
    char ch1;
    char ch2;
    int x1;
    int x2;
    if (argc < 3) {
        if (write(2,"Insufficient Arguments!!!\n",strlen("Insufficient Arguments!!!\n")) < 0) {
            return -1;
        }
        if (write(2,"Please use \"program-name file-name1 file-name2\" format.\n",
                  strlen("Please use \"program-name file-name1 file-name2\" format.\n")) < 0) {
            return -1;
        }
        return -1;
    }
    fd1 = open(argv[1], 'r');
    if (fd1 < 0) {
        if (write(2,"Error in: open\n",strlen("Error in: open\n")) < 0) {
            return -1;
        }
        return -1;
    }
    fd2 = open(argv[2], 'r');
    if (fd2 < 0) {
        if (write(2,"Error in: open\n",strlen("Error in: open\n")) < 0) {
            return -1;
        }
        if (close(fd1) < 0) {
            if (write(2,"Error in: close\n",strlen("Error in: close\n")) < 0) {
                return -1;
            }
            return -1;
        }
        return -1;
    }
    while (1) {
        x1 = read(fd1, &ch1, 1);
        if (x1 < 0) {
            if (write(2,"Error in: read\n",strlen("Error in: read\n")) < 0) {
                return -1;
            }
            closeFiles(fd1, fd2);
            return -1;
        }
        x2 = read(fd2, &ch2, 1);
        if (x2 < 0) {
            if (write(2,"Error in: read\n",strlen("Error in: read\n")) < 0) {
                return -1;
            }
            closeFiles(fd1, fd2);
            return -1;
        }
        if (x1 == 0 && x2 == 0) {
            closeFiles(fd1, fd2);
            return 1;
        }
        if (x1 == 0 || x2 == 0) {
            break;
        }
        if (ch1 != ch2) {
            break;
        }
    }
    closeFiles(fd1, fd2);
    fd1 = open(argv[1], 'r');
    if (fd1 < 0) {
        if (write(2,"Error in: open\n",strlen("Error in: open\n")) < 0) {
            return -1;
        }
        return -1;
    }
    fd2 = open(argv[2], 'r');
    if (fd2 < 0) {
        if (write(2,"Error in: open\n",strlen("Error in: open\n")) < 0) {
            return -1;
        }
        if (close(fd1) < 0) {
            if (write(2,"Error in: close\n",strlen("Error in: close\n")) < 0) {
                return -1;
            }
            return -1;
        }
        return -1;
    }
    while (1) {
        x1 = read(fd1, &ch1, 1);
        if (x1 < 0) {
            if (write(2,"Error in: read\n",strlen("Error in: read\n")) < 0) {
                return -1;
            }
            closeFiles(fd1, fd2);
            return -1;
        }
        x2 = read(fd2, &ch2, 1);
        if (x2 < 0) {
            if (write(2,"Error in: read\n",strlen("Error in: read\n")) < 0) {
                return -1;
            }
            closeFiles(fd1, fd2);
            return -1;
        }
        while ((ch1 == ' ') || (ch1 == '\n') || (ch1 == '\r')) {
            x1 = read(fd1, &ch1, 1);
            if (x1 < 0) {
                if (write(2,"Error in: read\n",strlen("Error in: read\n")) < 0) {
                    return -1;
                }
                closeFiles(fd1, fd2);
                return -1;
            }
            if (x1 == 0) {
                break;
            }
        }
        while ((ch2 == ' ') || (ch2 == '\n') || (ch2 == '\r')) {
            x2 = read(fd2, &ch2, 1);
            if (x2 < 0) {
                if (write(2,"Error in: read\n",strlen("Error in: read\n")) < 0) {
                    return -1;
                }
                closeFiles(fd1, fd2);
                return -1;
            }
            if (x2 == 0) {
                break;
            }
        }
        if (x1 == 0 && x2 == 0) {
            closeFiles(fd1, fd2);
            return 3;
        }
        if (x1 == 0 || x2 == 0) {
            closeFiles(fd1, fd2);
            return 2;
        }
        if (ch1 == ch2) {
            continue;
        }
        if ((ch1 >= 65 && ch1 <= 90) && (ch2 >= 97 && ch2 <= 122)) {
            if ((ch1 + 32) == ch2) {
                continue;
            }
            closeFiles(fd1, fd2);
            return 2;
        }
        if ((ch2 >= 65 && ch2 <= 90) && (ch1 >= 97 && ch1 <= 122)) {
            if ((ch2 + 32) == ch1) {
                continue;
            }
            closeFiles(fd1, fd2);
            return 2;
        }
        if(ch1 != ch2) {
            closeFiles(fd1, fd2);
            return 2;
        }
    }
}


