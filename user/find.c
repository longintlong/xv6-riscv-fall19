#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"


void find(char *path, char *target, char *cur){
    char buf[512], *p;
    struct dirent de;
    struct stat st;
    int fd;

    if((fd = open(path, 0)) < 0){
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    if(fstat(fd, &st) < 0){
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }
    
    if (st.type != T_DIR){
        if (strcmp(cur, target) == 0)
            printf("%s\n",path);
        close(fd);
        return;
    }

    //if (st.type != T_DIR){
    //    fprintf(2, "find: cannot find in a file %s\n", path);
    //    close(fd);
    //    return;
    //}

    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
        printf("ls: path too long\n");
        close(fd);
        return;
    }

    strcpy(buf, path);
    p = buf+strlen(buf);
    *p++ = '/';
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
        if(de.inum == 0)
            continue;
        if(strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
            continue;
        
        memmove(p, de.name, DIRSIZ);
        p[DIRSIZ] = 0;
        
        if(stat(buf, &st) < 0){
            printf("ls: cannot stat %s\n", buf);
            continue;
        }
        find(buf, target, de.name);
        //printf("%s %d %d %d\n", fmtname(buf), st.type, st.ino, st.size);
    }
    close(fd);
    return;
}

int main(int argc, char *argv[]){
    if(argc != 3){
        fprintf(2, "Error:need 2 arguments!\n");
    }
    find(argv[1], argv[2], "");
    exit(0);
}