#include "kernel/param.h"  

#include "kernel/types.h" 
#include "kernel/stat.h"
#include "user/user.h"

char buf[512];

char* readLine()
{
    char *b = malloc(32);

    for(int i = 0;;++i)
    {
        int r = read(0, &b[i], 1);
        if(b[i] == '\n')
        {

            // b[i] = 0;
            break;
        }
        if(r == 0)
        {
            // b[i - 1] = 0;
            free(b);
            return 0;
        }
    }
    return b;

}

int main(int argc, char* argv[])    
{
   
    int f = fork();
    if(f == 0)
    {
        char* mArgs[MAXARG];
        int n = 0;

        if(strcmp(argv[1], "-n") == 0)
        {
            // char *o[1];
            

             for(int i = 3; i < argc; ++i)
            {
                mArgs[n] = argv[i];
                ++n;
            }
            int te = n;
            for(; te < MAXARG ; ++te)
            {
                
               mArgs[n] = readLine();

                if(mArgs[n] == 0)
                {
                    exit(0);
                    
                    // mArgs[n] = "\n";   
                }
               
                mArgs[n][strlen(mArgs[n]) - 1] = 0;
                if(fork() == 0)
                {
                    // printf("%s\n", mArgs[n]);
                    exec(argv[3], mArgs);
                    
                }
                else
                {
                    wait(0);
                }

            }

        }
        else
        {
            for(int i = 1; i < argc; ++i)
            {
                mArgs[n] = argv[i];
                ++n;
            }
            for(; n < MAXARG && (mArgs[n] = readLine()) != 0; ++n)
            {
                // if(mArgs[n] !)
                // printf("%s", mArgs[n]);
                mArgs[n][strlen(mArgs[n]) - 1] = 0;
            }
            
            // mArgs[n - 1][strlen(mArgs[n -1]) - 1] = 0;


            exec(argv[1], mArgs);
        }

    }
    else
    {
        wait(0);
    }
    exit(0);
}