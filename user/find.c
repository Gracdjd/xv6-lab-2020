#include "kernel/types.h" 
#include "user/user.h"

#include "kernel/stat.h"  
#include "kernel/fs.h"

void match(const char* path, const char* name){
    //printf("%s %s", path, name);
    int pp = 0;
    int pa = 0;
    while(path[pp] != 0){
        pa = 0;
        int np = pp;
        while(name[pa] != 0){
            if (name[pa] == path[np]){
                pa++;
                np++;
            }
            else
                break;
        }
        if(name[pa] == 0){
            printf("%s\n", path);
            return;
        }
        pp++;
    }
}


void find(char* path, char* file)
{
    
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;
    // printf("begin\n");
    if((fd = open(path, 0)) < 0)
    {
        fprintf(2, "find: connot open %s\n", path);
        return;
    }

    if(fstat(fd, &st) < 0)
    {
        fprintf(2, "find: cannot stat %s\n", path); 
        close(fd);
        return;
    }
    char *p1, *p2;
    switch(st.type)
    {
        case T_FILE:
        
        p1 = path + strlen(path);
        p2 = file + strlen(file);

        while(*p1 != '/' && p1 >= path)
        {
            if(*p1 != *p2)
            {
                // printf("%s", *p1);
                   close(fd);
                    return;
            }
            --p1;
            --p2;
        }
        // match(path, file);
        printf("%s\n", path);
        break;
        case T_DIR:
            // printf("dir");
            if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf)
            {
                printf("find: path too long\n");
                break;
            }
            strcpy(buf, path);
            p = buf+strlen(buf);
            *p++ = '/';
            while(read(fd, &de, sizeof(de)) == sizeof(de))
            {

                if(de.inum == 0)
                    continue;
            
                if(strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
                {
                    continue;
                }
         
                memmove(p, de.name, DIRSIZ);
                
                p[DIRSIZ] = 0;

                if(stat(buf, &st) < 0){
                printf("ls: cannot stat %s\n", buf);
                continue;
                }
                find(buf, file);                
            }
            // printf("end");
         
    }
    close(fd);
    return;
    
}

int main(int argc, char* argv[])
{

    if(argc == 2)
    {

        find(".", argv[1]);
    
        exit(0);
    }

    find(argv[1], argv[2]);

    
    exit(0);
}