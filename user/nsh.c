#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"

#define MAXARGS 10


char whitespace[] = " \t\r\n\v";
char symbols[] = "<|>&;()";

void
panic(char *s)
{
  fprintf(2, "%s\n", s);
  exit(-1);
}

int
fork1(void)
{
  int pid;

  pid = fork();
  if(pid == -1)
    panic("fork");
  return pid;
}

int
getcmd(char *buf, int nbuf)
{
  fprintf(2, "@ ");
  memset(buf, 0, nbuf);
  gets(buf, nbuf);
  if(buf[0] == 0) // EOF
    return -1;
  return 0;
}

void 
redirt(char *re, char *file){
  if(*re == '<'){
    close(0);
    if (open(file, 0) < 0){
      fprintf(2, "open %s error\n", file);
      exit(1);
    }
  }else{
    close(1);
    if (open(file, O_CREATE|O_WRONLY) != 1){
      fprintf(2, "open %s error\n", file);
      exit(1);
    }
  }
  return;
}


//
void
runcmd(int argc, char *argv[])
{
  for(int i=0;i<argc;i++){
    if(!strcmp("|", argv[i])){
      int fd[2];
      if(pipe(fd) < 0){
        
      }
      argv[i] = '\0';
      if(fork1() > 0){
        close(1);
        dup(fd[1]);
        close(fd[0]);
        close(fd[1]);
        runcmd(i, argv);
        wait(0);
      }else{
        close(0);
        dup(fd[0]);
        close(fd[0]);
        close(fd[1]);
        runcmd(argc-i-1, argv+i+1);
      }
      return;
    }
  }
  for(int i=0;i<argc;i++){
    if(!strcmp(argv[i], "<") || !strcmp(argv[i], ">")){
      if(i >= argc - 1){
        fprintf(2, "Error:redirect need input or output\n");
        exit(1);
      }
      redirt(argv[i], argv[i+1]);
      argv[i] = 0;
    }
  }

  if(argv[0] == 0)
    exit(-1);
  exec(argv[0], argv);
  fprintf(2, "exec failed\n");
  return;
}

// 
void setargs(char *cmd, char *argv[], int *argc){
  int i = 0, j =0;
  while(i < MAXARGS &&  cmd[j] != '\n' && cmd[j] != '\r' && cmd[j] != '\0'){
      while(cmd[j] != '\0' && strchr(whitespace, cmd[j])){
        j++;
      }
      argv[i++] = cmd + j;
      while(cmd[j] != '\0' && !strchr(whitespace, cmd[j])){
        j++;
      }
      cmd[j++] = 0;
  }
  argv[i] = 0;
  *argc = i;
}

int
main(void)
{
  static char buf[100];
  int fd;

  // Ensure that three file descriptors are open.
  while((fd = open("console", O_RDWR)) >= 0){
    if(fd >= 3){
      close(fd);
      break;
    }
  }

  // Read and run input commands.
  while(getcmd(buf, sizeof(buf)) >= 0){
    if(buf[0] == 'c' && buf[1] == 'd' && buf[2] == ' '){
      // Chdir must be called by the parent, not the child.
      buf[strlen(buf)-1] = 0;  // chop \n
      if(chdir(buf+3) < 0)
        fprintf(2, "cannot cd %s\n", buf+3);
      continue;
    }
    if(fork1() == 0){
      char *argv[MAXARGS];
      int argc = 0;
      setargs(buf, argv, &argc);
      runcmd(argc, argv);
      //runcmd(parsecmd(buf));
    }
    wait(0);
  }
  exit(0);
}