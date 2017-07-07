#include<stdio.h>
#include<mpi.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#define _GNU_SOURCE

#include <sched.h>
#include <sys/resource.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>

#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


int min(int a,int b){ return (a<b)?a:b;}
int max(int a,int b){ return (a>b)?a:b;}

int main(int argc,char *argv[])
{
    printf("hello sandbox\n");
    int uid,gid;
    printf("uid is %d\n",geteuid());
    printf("gid is %d\n",getegid());
    int fd;
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    char *file1 = "file1";
    char *file2 = "file2";
    fd = open(file1, O_WRONLY | O_CREAT | O_TRUNC, mode);
    if (fd==-1)
    {
        printf("process can't open files\n");
    }
    else
    {
        printf("process can open files\n");
        close(fd);
    }
    int tmp;
    tmp = fork();
    if(tmp==-1)
    {
        printf("process can't fork \n");
    }
    if(tmp==0)
    {
        return 0;
    }
    if(tmp>0)
    {
        printf("process can fork \n");
    }

    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if(listenfd==-1)
    {
        printf("process can't open socket\n");
    }
    else
    {
        printf("process can open socket\n");
        close(listenfd);
    }
    for(int i=0;i<=argc-1;i++)
    {
        printf("%s\n",argv[i]);
    }



    return 0;
}
