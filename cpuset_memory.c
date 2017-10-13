#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <malloc.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/mount.h>
#include <sys/types.h>

#define MEMORY_LIMIT_BYTES 1048576 // 1M

int create_folder(const char* path)
{
    int beginCmpPath;
    int endCmpPath;
    int fullPathLen;
    int pathLen = strlen(path);
    char currentPath[128] = {0};
    char fullPath[128] = {0};

    // relative path
    if('/' != path[0])
    {
        // get current path
        getcwd(currentPath, sizeof(currentPath));
        strcat(currentPath, "/");
        beginCmpPath = strlen(currentPath);
        strcat(currentPath, path);
        if(path[pathLen] != '/')
        {
            strcat(currentPath, "/");
        }
        endCmpPath = strlen(currentPath);
    }
    else
    {
        // absolute path
        int pathLen = strlen(path);
        strcpy(currentPath, path);
        if(path[pathLen] != '/')
        {
            strcat(currentPath, "/");
        }
        beginCmpPath = 1;
        endCmpPath = strlen(currentPath);
    }
    // create directory at all levels
    for(int i = beginCmpPath; i < endCmpPath ; i++ )
    {
        if('/' == currentPath[i])
        {
            currentPath[i] = '\0';
            if(access(currentPath, 0) != 0)
            {
                if(mkdir(currentPath, 0755) == -1)
                {
                    fprintf(stderr, "Could not create folder %s: %s\n", currentPath, strerror (errno));
                    return -1;
                }
            }
            currentPath[i] = '/';
        }
    }

    return 0;
}

int mount_folder(const char *source, const char *target, const char *filesystemtype,const void *data)
{
    if(mount(source,target,filesystemtype,0,data) != 0)
    {
        fprintf(stderr, "Could not mount %s: %s\n",source, strerror (errno));
        return -1;
    }

    return 0;
}

int write_data(const char *source, const char *data)
{
    FILE *fp;
    fp = fopen(source,"w");

    if(fp)
    {
        fprintf(fp,"%s",data);
        fclose(fp);
    }
    else
    {
        fprintf(stderr, "Could not open %s: %s\n",source, strerror (errno));
        fclose(fp);
        return -1;
    }

    return 0;
}

int main(int argc, char *argv[])
{
    if(argc < 3)
    {
        fprintf(stderr,"Too few parameters!\n");
        fprintf(stderr,"Usage: [memory_limit 1M=1] [cpu_cpus 0-1] [cpu_mems 0]\n");
        return -1;
    }

    pid_t pid;
    pid = getpid();

    // get data from command line
    unsigned int memory_limit_in_bytes = atoi(argv[1]) * MEMORY_LIMIT_BYTES;
    char* cpu_number = argv[2];
    
    char* cpu_mems = NULL;
    if(argv[3] != NULL) cpu_mems = argv[3];
    else cpu_mems = "0";
    

    // convert format from int to char
    char meomry_limit[10] = {0};
    sprintf(meomry_limit,"%d",memory_limit_in_bytes);

    char pid_num[10] = {0};
    sprintf(pid_num,"%d",pid);

    // 1. create /cgroup/cpu and /cgroup/memory
    create_folder("/tmp/cgroup/cpuset");
    create_folder("/tmp/cgroup/memory");

    // 2. mount cpu and memory to /cgroup
    mount_folder("cpuset","/tmp/cgroup/cpuset","cgroup","cpuset");
    mount_folder("memory","/tmp/cgroup/memory","cgroup","memory");

    // 3. create new cgroup to limit process
    create_folder("/tmp/cgroup/cpuset/test");
    create_folder("/tmp/cgroup/memory/test");

    // 4.write limit data
    // 4.1 memory limit
    write_data("/tmp/cgroup/memory/test/memory.limit_in_bytes",meomry_limit);
    // 4.2 cpu number limit
    write_data("/tmp/cgroup/cpuset/test/cpuset.cpus",cpu_number);
    write_data("/tmp/cgroup/cpuset/test/cpuset.mems",cpu_mems);

    // 5.bind process number
    write_data("/tmp/cgroup/memory/test/tasks",pid_num);
    write_data("/tmp/cgroup/cpuset/test/tasks",pid_num);

    // test this project
    int i = 99;
    char *tmp_ptr = NULL;
    int mallocCnt = 0;
    int mallocMB = 1 * 1024 * 1024; // 1 MB
    while(1)
    {
        i = i*i;
        tmp_ptr = (char*)malloc(mallocMB);
        memset(tmp_ptr, mallocCnt, mallocMB);
        printf("Malloc %d * 1MB\n", ++mallocCnt);
    }

    return 0;
}
