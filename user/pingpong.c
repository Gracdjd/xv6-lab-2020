#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main()
{
    int p1[2];
    int p2[2];
    
    pipe(p1);
    pipe(p2);
    char c[1] = {'p'};
    char temp = 0;
    int f = fork();
    if(f != 0)
    {
        close (p1[0]);
        close (p2[1]);
        write(p1[1], c, 1);
        
        wait(0);

        int r = read(p2[0], &temp, 1);

        if(r == 1)
        {
            int t = getpid();
            printf("%d: received pong\n", t);
        }
        else
        {
            printf("error\n");
        }
        
    }
    else
    {
        close(p1[1]);
        close(p2[0]);
        int r = read(p1[0], &temp, 1);
        
        
        if(r == 1)
        {
            int t = getpid();
            printf("%d: received ping\n", t);
            write(p2[1], c, 1);
        }
        else
        {
            printf("error\n");
        }


    }
    exit(0);
}