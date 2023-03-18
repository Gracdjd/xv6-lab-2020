#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#define REC 0
#define SED 1

// 逻辑：第一个进程筛选到不合适的，记录prime,fork,
int newChild(int p)
{
    int pc[2];
    pipe(pc);

    int pri; 
    read(p, &pri, 4);
    
    printf("prime %d\n", pri);
    int temp;
    int test = read(p, &temp, 4);
    do
    {
        if(test == 0)
        {
            // printf("1111");
            exit(0);
        }
        if(temp % pri != 0)
        {
            if(fork() == 0)
            {
                close(pc[1]);
                newChild(pc[0]);
            }
            write(pc[1],  &temp, 4);
            close(pc[0]);
        }
    } while (read(p, &temp, 4));

    while(read(p, &temp, 4))
    {
        if(temp % pri != 0)
            write(pc[1], &temp, 4);
    }
    close(p);
    close(pc[1]);
    wait(0);    
    exit(0);
}

int main()
{
    int prime = 2;
    int p[2];
    pipe(p);
    int i;
    printf("prime 2 \n");

    if(fork() == 0)
    {
        close(p[SED]);
        newChild(p[REC]);
    }
    close(p[REC]);
    // i
    for(i = 2; i <= 35; ++i)
    {
        if(i % prime != 0)
        {
            write(p[SED], &i, 4);
        }
    }
    close(p[SED]);
    wait(0);
    exit(0);
}