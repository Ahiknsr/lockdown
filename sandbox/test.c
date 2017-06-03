#include <sys/resource.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#define	getit(name)	pr_limits(#name, name)
#define	doit(name,lim)	set_limits(#name, name,lim)

static void	pr_limits(char *, int);
static void set_limits(char *, int, unsigned long long int);

int main(int argc,char *argv[])
{
    /*
    RLIMIT_CPU : limits cpu time;
    RLIMIT_AS: limits virtual memory;
    RLIMIT_FSIZE: limits output filesize;
    RLIMIT_NOFILE: limits process from creating more file descriptors;
    RLIMIT_NPROC: limits number of child processes ;
    */
    if(argc!=2)
    {
        printf("usage: ./a.out execfile");
        return 0;
    }
    unsigned long long int cpu=5,mem=1572864000,file=0,out=500000,proc=0;
	getit(RLIMIT_AS);
	getit(RLIMIT_CPU);
	getit(RLIMIT_FSIZE);
	getit(RLIMIT_NOFILE);
	getit(RLIMIT_NPROC);
    printf("\n\n");
    
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


    printf("limiting process\n");
    doit(RLIMIT_CPU,cpu);
    doit(RLIMIT_AS,mem);
    doit(RLIMIT_FSIZE,out);
    doit(RLIMIT_NOFILE,file);
    doit(RLIMIT_NPROC,proc);

    fd = open(file2, O_WRONLY | O_CREAT | O_TRUNC, mode);
    if (fd==-1)
    {
        printf("process can't open files\n");
    }
    else
    {
        printf("process can open files\n");
        close(fd);
    }
    tmp = fork();
    if(tmp==-1)
    {
        printf("process can't fork\n");
    }
    if(tmp==0)
    {
        return 0;
    }
    if(tmp>0)
    {
        printf("process can fork \n");
    }
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if(listenfd==-1)
    {
        printf("process can't open socket\n");
    }
    else
    {
        printf("process can open socket\n");
        close(listenfd);
    }


    printf("\n\n");
	getit(RLIMIT_AS);
	getit(RLIMIT_CPU);
	getit(RLIMIT_FSIZE);
	getit(RLIMIT_NOFILE);
	getit(RLIMIT_NPROC);
    char *exec_argv[0];
    exec_argv[0]=NULL;
    int ret=execvp(argv[1],exec_argv);
    printf("%d\n",ret);
	exit(0);
}

static void pr_limits(char *name, int resource)
{
	struct rlimit		limit;
	unsigned long long	lim;

	if (getrlimit(resource, &limit) < 0)
		printf("getrlimit error for %s", name);
	printf("%-14s  ", name);
	if (limit.rlim_cur == RLIM_INFINITY) {
		printf("(infinite)  ");
	} else {
		lim = limit.rlim_cur;
		printf("%10lld  ", lim);
	}
	if (limit.rlim_max == RLIM_INFINITY) {
		printf("(infinite)");
	} else {
		lim = limit.rlim_max;
		printf("%10lld", lim);
	}
	putchar((int)'\n');
}

static void set_limits(char *name, int resource,unsigned long long int lim)
{
    struct rlimit limit;
    limit.rlim_cur = lim;
    limit.rlim_max = lim;
    setrlimit(resource,&limit);
}
