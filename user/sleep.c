#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]){
    int time;
    if (argc != 2){
        fprintf(2, "Error:need an argument\n");
        exit(1);
    }

    if((time = atoi(argv[1])) == 0){
        fprintf(2, "Error:illegal time\n");
        exit(1);
    }
    sleep(time);
    exit(0);
}