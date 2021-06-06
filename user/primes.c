#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void prime(int input_fd);

int main(int argc, char const *argv[])
{
    int parent_fd[2];
    pipe(parent_fd);
    if (fork())
    {
        close(parent_fd[0]);
        int i;
        for (i = 2; i < 36; i++)
        {
            write(parent_fd[1], &i, sizeof(int));
        }
        close(parent_fd[1]);
    }
    else
    {
        close(parent_fd[1]);
        prime(parent_fd[0]);
    }
    wait(0);
    exit(0);
}

void prime(int input_fd)
{
    int base;
    /* Exit if last child */
    if (read(input_fd, &base, sizeof(int)) == 0)
    {
        exit(1);
    }
    printf("prime %d\n", base);

    /* Create new child if not last */
    int p[2];
    pipe(p);
    if (fork() == 0)
    {
        close(p[1]);
        prime(p[0]);
    }
    else
    {
        close(p[0]);
        int n;
        int eof;
        do
        {
            eof = read(input_fd, &n, sizeof(int));
            if (n % base != 0)
            {
                write(p[1], &n, sizeof(int));
            }
        } while (eof);

        close(p[1]);
    }
    wait(0);
    exit(1);
}

// #include "kernel/types.h"
// #include "kernel/stat.h"
// #include "kernel/param.h"
// #include "user/user.h"

// void primes(int input_fd){
//     int fd[2];
//     int base,rec;
//     if(read(input_fd, &base, sizeof(int)) <= 0){
//         fprintf(2,"error\n");
//         exit(1);
//     }
//     printf("prime %d\n", base);
//     pipe(fd);
//     if(fork() > 0){
//         close(fd[0]);
//         while(read(input_fd, &rec, sizeof(int)) > 0){
//             if(rec % base != 0){
//                 write(fd[1], &rec, sizeof(int));
//             }
//         }
//         close(fd[1]);
//         wait(0);
//     }else{
//         close(fd[1]);
//         primes(fd[0]);
//     }
//     exit(0);
// }
// int main(){
//     int fd[2];
//     pipe(fd);
//     if(fork() > 0){
//         close(fd[0]);
//         for(int i=2;i<=35;i++){
//             write(fd[1], &i, sizeof(int));
//         }
//         close(fd[1]);
//         wait(0);
//     }else{
//         close(fd[1]);
//         primes(fd[0]);
//     }
//     exit(0);
// }