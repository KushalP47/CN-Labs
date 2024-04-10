#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<time.h>
#include<assert.h>

void getTime(){
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    char s[64];
    size_t ret = strftime(s, sizeof(s), "%c", tm);
    assert(ret);
    printf("%s\n", s);
}

void getFolderContents(){
    char *command = "ls";
    char *arguments[] = {"ls", NULL, NULL};
    execvp(command, arguments);
}

int main(){

    pid_t pid;
    int c = 1;
    pid = fork();
    // printf("pid: %d\n", pid);
    if(pid == 0){
            printf("Child process 1\n");
            printf("Child process 1 id: %d\n", getpid());
            printf("Parent process of child 1 id: %d\n", getppid());
            getTime();
            wait(NULL);
            
    }
    else{
        printf("Parent process\n");
        printf("Parent process id: %d\n", getpid());
        printf("Child process id: %d\n", pid);
        pid = fork();
        // printf("pid: %d\n", pid);
        if(pid == 0){
            printf("Child process 2\n");
            printf("Child process 2 id: %d\n", getpid());
            printf("Parent process of child 2 id: %d\n", getppid());
            getFolderContents();
        }
    }
    return 0;
}