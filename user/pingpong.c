#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(){
    int parent_df[2], child_fd[2];
    pipe(parent_df);
    pipe(child_fd);
    char v = 0;
    int pp;
    if(fork() > 0){
        close(parent_df[0]);
        close(child_fd[1]);
        write(parent_df[1], &v, 1);
        read(child_fd[0], &v, 1);
        if(v == 1){
            printf("%d: received pong\n", getpid());
        }
        close(parent_df[1]);
        close(child_fd[0]);
        wait(&pp);
    }else{
        close(parent_df[1]);
        close(child_fd[0]);
        read(parent_df[0], &v, 1);
        if(v == 0){
            printf("%d: received ping\n", getpid());
        }
        v = 1;
        write(child_fd[1], &v, 1);
        close(child_fd[1]);
        close(parent_df[0]);
        exit(0);
    }
    exit(0);
}