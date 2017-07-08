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
#include <sys/types.h>
#include <sys/wait.h>
#include <pwd.h>
#include <time.h>

#define	getit(name)	pr_limits(#name, name)
#define	doit(name,lim)	set_limits(#name, name,lim)

static void	pr_limits(char *, int);
static void set_limits(char *, int, unsigned long long int);
void setup_sandbox(int flag);
void status_sandbox();
void test_sandbox();
void redirect_output();
int get_extension(char *filename);
int execute_cfile(int filetype);
int execute_pyfile();
int create_user();
void delete_user(int uid);
int rand_interval(unsigned int min, unsigned int max);
void get_running_dir();

unsigned long long int cpu=5,mem=1572864000,file=0,out=500000,proc=1;
char exe_path[1024];


int main(int argc,char *argv[])
{

    if(argc!=2)
    {
        fprintf(stderr, "usage: ./a.out input_file\n");
        return 0;
    }
    
    get_running_dir();
    int filetype = get_extension(argv[1]);
    if(filetype<0)
        fprintf(stderr,"unrecognized file\n");
    if(filetype==1||filetype==2)
    {
        fprintf(stderr,"input file is c or c++\n");
        execute_cfile(filetype);
    }
    if(filetype==3)
    {
        fprintf(stderr,"input file is python\n");
        execute_pyfile();
    }
    if(filetype==4)
    {
        fprintf(stderr,"input file is java\n");
    }
    //test_sandbox();
    //flag 1 for c and c++
    //setup_sandbox(flag);
    //test_sandbox();
	exit(0);
}

void setup_io()
{
    int exe_dir = open(exe_path,O_RDONLY);
    //read stdin from input.txt
    //write stdout to output.txt
    close(0);
    close(1);
    //close(3);
    char *infile = "input.txt";
    char *outfile = "output.txt";
    int fd0,fd1,fd2;
    //fd0,fd1,fd2 will be 0,1,2 respectively
    fd0 = openat(exe_dir,infile, O_RDONLY);
    fd1 = openat(exe_dir,outfile, O_WRONLY|O_CREAT,S_IROTH|S_IWOTH);
    //fd2 = openat(exe_dir,outfile, O_WRONLY|O_CREAT,S_IROTH|S_IWOTH);
}

int get_extension(char *filename)
{
    /*
    c  1
    cpp  2
    py  3
    java  4
    error  -1
    unrecognized -2
    */
    fprintf(stderr, "filename is %s\n", filename);
    char str[50];
    const char dot = '.';
    if(strlen(filename)>49)
    {
        fprintf(stderr,"file name too long\n");
        exit(0);
    }
    for(int i=0;i<strlen(filename);i++)
    {
        str[i]=filename[i];
    }
    char *ext = strrchr(str, dot);
    if (!ext) 
    {
        return -1;
    } 
    else 
    {
        int loc=ext-str;
        int len=strlen(filename);
        //fprintf(stderr,"dot location at %d\n",loc);
        //fprintf(stderr,"%d\n",len-loc);
        if((len-loc==2)&&filename[loc+1]=='c')
            return 1;
        if((len-loc==4)&&filename[loc+1]=='c'&&filename[loc+2]=='p'&&
                        filename[loc+3]=='p')
            return 2;
        if((len-loc==3)&&filename[loc+1]=='p'&&filename[loc+2]=='y')
            return 3;
        if((len-loc==5)&&filename[loc+1]=='j'&&filename[loc+2]=='a'&&
                filename[loc+3]=='v'&&filename[loc+4]=='a')
            return 4;
        return -2;
    }
}

void status_sandbox()
{
	getit(RLIMIT_AS);
	getit(RLIMIT_CPU);
	getit(RLIMIT_FSIZE);
	getit(RLIMIT_NOFILE);
	getit(RLIMIT_NPROC);
}

void setup_sandbox(int flag)
{
    /*
    if flag is 1 limit the file descriptors and child process we
    can only do this for statically compiled files(c and c++)

    RLIMIT_CPU : limits cpu time;
    RLIMIT_AS: limits virtual memory;
    RLIMIT_FSIZE: limits output filesize;
    RLIMIT_NOFILE: limits process from creating more file descriptors;
    RLIMIT_NPROC: limits number of child processes ;
    CLONE_NEWNET: will limit the process from network
    */
    fprintf(stderr, "setting sandbox\n");
    doit(RLIMIT_CPU,cpu);
    doit(RLIMIT_AS,mem);
    doit(RLIMIT_FSIZE,out);
    if(unshare(CLONE_NEWNET)==-1)
    {
        fprintf(stderr,"network isolation using unshare failed\n");
        //exit(0);
    }
    if(flag==1)
    {
        doit(RLIMIT_NOFILE,file);
        doit(RLIMIT_NPROC,proc);
    }
}

void test_sandbox()
{

    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    char *file1 = "file1";
    char *file2 = "file2";
    int fd = open(file2, O_WRONLY | O_CREAT | O_TRUNC, mode);
    if (fd==-1)
    {
        fprintf(stderr, "process can't open files\n");
    }
    else
    {
        fprintf(stderr, "process can open files\n");
        close(fd);
    }
    int tmp = fork();
    if(tmp==-1)
    {
        fprintf(stderr, "process can't fork\n");
    }
    if(tmp==0)
    {
        _exit(0);
    }
    if(tmp>0)
    {
        fprintf(stderr, "process can fork \n");
    }
    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if(listenfd==-1)
    {
        fprintf(stderr, "process can't open socket\n");
    }
    else
    {
        fprintf(stderr, "process can open socket\n");
        close(listenfd);
    }

    fprintf(stderr, "\n\n");

}

static void pr_limits(char *name, int resource)
{
	struct rlimit		limit;
	unsigned long long	lim;

	if (getrlimit(resource, &limit) < 0)
		fprintf(stderr, "getrlimit error for %s", name);
	fprintf(stderr, "%-14s  ", name);
	if (limit.rlim_cur == RLIM_INFINITY) {
		fprintf(stderr, "(infinite)  ");
	} else {
		lim = limit.rlim_cur;
		fprintf(stderr, "%10lld  ", lim);
	}
	if (limit.rlim_max == RLIM_INFINITY) {
		fprintf(stderr, "(infinite)");
	} else {
		lim = limit.rlim_max;
		fprintf(stderr, "%10lld", lim);
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


int execute_cfile(int filetype)
{
    //remove previous files
    system("rm output.txt >/dev/null 2>&1");
    char cd[1024]= "cd ";
    strcat(cd,exe_path);
    //compile the source with static flag so that
    //shared libraries are not needed
    if(filetype==1)
    {
        char compile[99] = "&&gcc input.c -o input -static >output.txt 2>&1";
        strcat(cd,compile);
        system(cd);
        //system("gcc input.c -o input -static >output.txt 2>&1 ");
    }
    if(filetype==2)
    {
        char compile[99] = "&&g++ input.cpp -o input -static >output.txt 2>&1";
        strcat(cd,compile);
        system(cd);
        system("g++ input.cpp -o input -static >output.txt 2>&1 ");
    }

    int tmp_uid=create_user();
    int cpid=fork();
    if(cpid==-1)
    {
        fprintf(stderr,"fork failed\n");
    }
    if(cpid>0)
    {
        int ret;
        waitpid(cpid,&ret,WUNTRACED | WCONTINUED);
        delete_user(tmp_uid);
    }
    if(cpid==0)
    {

        setup_io();
        setup_sandbox(1);
        //execute input using exevp
        //since system(3) will not work due to sandbox
        setgid(tmp_uid);
        setuid(tmp_uid);
        char *exec_argv[0];
        exec_argv[0]=NULL;
        int ret=execvp("./input",exec_argv);
        exit(ret);
    }
}


int execute_pyfile()
{
    //remove previous files
    system("rm output.txt input >/dev/null 2>&1");
    int tmp_uid=create_user();
    int cpid=fork();
    if(cpid==-1)
    {
        fprintf(stderr,"fork failed\n");
    }
    if(cpid>0)
    {
        int ret;
        waitpid(cpid,&ret,WUNTRACED | WCONTINUED);
        delete_user(tmp_uid);
    }
    if(cpid==0)
    {
        setup_io();
        setup_sandbox(0);
        //execute input using exevp
        //since system(3) will not work due to sandbox
        setgid(tmp_uid);
        setuid(tmp_uid);
        char *exec_argv[2];
        exec_argv[0]="python2";
        exec_argv[1]="input.py";
        exec_argv[2]=NULL;
        int ret=execvp("python2",exec_argv);
        exit(ret);
    }


}

int create_user()
{
    struct passwd *p;
    uid_t uid=rand_interval(1001,50000);
    while( (p = getpwuid(uid)) != NULL) 
        uid = rand_interval(1001,50000);
    char add[50];
    //useradd -M -u 9999 u9999 -p 123
    sprintf (add,"useradd -M -u %d u%d -p 123 " ,uid, uid);
    fprintf(stderr,add);
    fprintf(stderr,"\n");
    system(add);
    return uid;
}

void delete_user(int uid)
{
    //todo: error handling
    char del[50];
    //userdel u9999
    sprintf(del,"userdel u%d",uid);
    fprintf(stderr,del);
    fprintf(stderr,"\n");
    system(del);
}

//https://stackoverflow.com/a/17554531
int rand_interval(unsigned int min, unsigned int max)
{
    int r;
    const int range = 1 + max - min;
    const int buckets = RAND_MAX / range;
    const int limit = buckets * range;

    /* Create equal size buckets all in a row, then fire randomly towards
     * the buckets until you land in one of them. All buckets are equally
     * likely. If you land off the end of the line of buckets, try again. */

    srand ( time(NULL) );
    do
    {
        r = rand();
    } while (r >= limit);

    return min + (r / buckets);
}

//https://stackoverflow.com/a/198099
void get_running_dir()
{
    char tmp[1024];
    sprintf(tmp, "/proc/%d/exe", getpid());
    readlink(tmp,exe_path,sizeof(exe_path));
    char *ext = strrchr(exe_path, '/');
    *ext='\0';
    fprintf(stderr,"sandbox is located in %s\n",exe_path);
    //chdir is needed because of
    //execvp("./input",exec_argv)
    chdir(exe_path);
}

