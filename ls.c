#include <stdio.h>
#include <unistd.h>

int main() {
    char buf[100];
    int rc;
    int fd[2];
    pipe(fd);
    if (fork() == 0) {
        dup2(fd[1], STDOUT_FILENO);
        close(fd[1]);
        close(fd[0]);
        execlp("ls", "ls", NULL);
        perror("ls");
        exit(1);
    }
    close(fd[1]);
    while((rc = read(fd[0], buf, sizeof(buf)))>0) {
        write(STDOUT_FILENO, buf, sizeof(buf));
    }    
    wait(NULL);
    return 0;
}
